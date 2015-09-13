#include "stm8s.h"
#include "stm8s_it.h"
#include "stm8s_gpio.h"
#include "stm8s_spi.h"
#include "stm8s_exti.h"
#include "stm8s_tim1.h"
#include "stm8s_tim2.h"
#include "stm8s_tim4.h"
#include "stm8s_adc1.h"

#define MCU_SPEED_MHZ 8000000
#define MS_IN_SECOND 1000
#define get_ms(x) (MS_IN_SECOND*x)/(MCU_SPEED_MHZ/(1 << TIM2_PRESCALER_128))

#define ADC_PIN GPIO_PIN_6 // Port D

#define LED0_YELLOW GPIO_PIN_4 // Port B
#define LED1_BLUE   GPIO_PIN_5 // Port B
#define LED2        GPIO_PIN_3 // Port A


#define VOL_PLUS GPIO_PIN_5 // Port C
#define VOL_MINUS GPIO_PIN_6 // Port C
#define NEXT GPIO_PIN_7 // Port C

#define PREV GPIO_PIN_2 // Port D
#define PLAY_TOGGLE GPIO_PIN_3 // Port D

static uint16_t Conversion_Value = 0;
static int first_time = 0;
static uint32_t counter = 0;
static int high = 0;
static uint32_t array[100] = {0};
static int index = 0;

INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
{
  EXTI_Sensitivity_TypeDef sensitivity = EXTI_GetExtIntSensitivity(EXTI_PORT_GPIOA);
  if(sensitivity == EXTI_SENSITIVITY_RISE_ONLY){
    nop();
  } else if (sensitivity == EXTI_SENSITIVITY_FALL_ONLY){
    nop();
  }
}


INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  
  int pins = GPIO_ReadInputData(GPIOB);
  BitStatus pin_led1_blue = (BitStatus)(pins & LED1_BLUE);
  BitStatus pin_led0_yellow = (BitStatus)(pins & LED0_YELLOW);
  
  if (pin_led1_blue == RESET){
    high = counter;
  } else {
    pins = counter - high;
    if (sizeof(array)/sizeof(int) > index){
       // array[index++] = pins;
    }
  }
 
  pins++;
  pin_led0_yellow++;
  pin_led1_blue++;

}


/**
  * @brief Timer2 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
 {
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
   
     if(TIM2_GetITStatus(TIM2_IT_CC1)) 
     {
        TIM2_ClearITPendingBit(TIM2_IT_CC1);
        if (first_time == 0){
          first_time = 1;
        } else {
          int32_t CCR1_Val=TIM2_GetCapture1();
          if (sizeof(array)/sizeof(uint32_t) > index){
            array[index++] = (CCR1_Val*10000)/78125;
          }
        }
        TIM2_SetCounter(0);
     }
      if(TIM2_GetITStatus(TIM2_IT_CC2)) 
     {
        TIM2_ClearITPendingBit(TIM2_IT_CC2);
        int32_t CCR2_Val=TIM2_GetCapture2();
        
        if (sizeof(array)/sizeof(uint32_t) > index){
          array[index++] = (CCR2_Val*10000)/78125; // ((8000000/1024)/(1000))
        }
     }
 }


 INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
 {
  
    FlagStatus status = ADC1_GetFlagStatus(ADC1_FLAG_AWS6);

    Conversion_Value = ADC1_GetConversionValue();
    
    Conversion_Value = ADC1_GetBufferValue(0);
    Conversion_Value = ADC1_GetBufferValue(1);
    Conversion_Value = ADC1_GetBufferValue(2);
    Conversion_Value = ADC1_GetBufferValue(3);
    Conversion_Value = ADC1_GetBufferValue(4);
    Conversion_Value = ADC1_GetBufferValue(5);
    Conversion_Value = ADC1_GetBufferValue(6);
    Conversion_Value = ADC1_GetBufferValue(7);


    
    ADC1_ClearITPendingBit(ADC1_IT_AWS6);
    ADC1_ClearITPendingBit(ADC1_IT_EOC);
    ADC1_ClearFlag(ADC1_FLAG_EOC);
    //ADC1_StartConversion();
 }


static void delay(uint32_t t)
{
  while(t--);
}


void t_delay(int32_t delay_ms){
  for (;delay_ms > 0; delay_ms-=4){
    TIM4_SetCounter(0);
    while(TIM4_GetFlagStatus(TIM4_FLAG_UPDATE) == RESET ){
      counter++;
    }
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  }
}

void play_toggle(){
    GPIO_WriteHigh(GPIOD, PLAY_TOGGLE);
    delay(5000); 
    GPIO_WriteLow(GPIOD, PLAY_TOGGLE);
    delay(1000);
}

/* Long
  GPIO_WriteHigh(GPIOD, PLAY_TOGGLE);
  t_delay(1000);
  GPIO_WriteLow(GPIOD, PLAY_TOGGLE);
  t_delay(1000);
*/


/* Double click

#define HIGH 400
#define LOW (HIGH/2)
  
  GPIO_WriteHigh(GPIOD, PLAY_TOGGLE);
  t_delay(HIGH);
  GPIO_WriteLow(GPIOD, PLAY_TOGGLE);
  t_delay(LOW);
  GPIO_WriteHigh(GPIOD, PLAY_TOGGLE);
  t_delay(HIGH);
  GPIO_WriteLow(GPIOD, PLAY_TOGGLE);
  t_delay(LOW);

*/

/* Short Click

#define HIGH 400
#define LOW (HIGH/2)
  GPIO_WriteHigh(GPIOD, PLAY_TOGGLE);
  t_delay(HIGH);
  GPIO_WriteLow(GPIOD, PLAY_TOGGLE);
  t_delay(LOW);

*/

void init_gpio(){
  
  GPIO_Init(GPIOC, VOL_PLUS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, VOL_MINUS, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, NEXT, GPIO_MODE_OUT_PP_LOW_FAST);
  
  GPIO_Init(GPIOD, PREV, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, PLAY_TOGGLE, GPIO_MODE_OUT_PP_LOW_FAST);
  
  //EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_LOW);
  
  GPIO_DeInit(GPIOB);
  GPIO_DeInit(GPIOA);
  GPIO_Init(GPIOB, LED0_YELLOW, GPIO_MODE_IN_FL_IT);
  GPIO_Init(GPIOB, LED1_BLUE, GPIO_MODE_IN_FL_IT);
  GPIO_Init(GPIOA, LED2, GPIO_MODE_IN_FL_IT);
  
  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);
  
  GPIO_Init(GPIOD, ADC_PIN, GPIO_MODE_IN_FL_NO_IT);
  
}

void init_tim2(){

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
  TIM2_TimeBaseInit(TIM2_PRESCALER_1024, 0xFFFF);
  TIM2_PWMIConfig(TIM2_CHANNEL_1, TIM2_ICPOLARITY_FALLING, TIM2_ICSELECTION_DIRECTTI, TIM2_ICPSC_DIV1, 0xF);
  TIM2_ITConfig(TIM2_IT_CC1, ENABLE);
  TIM2_ITConfig(TIM2_IT_CC2, ENABLE);  
  TIM2_CCxCmd(TIM2_CHANNEL_1, ENABLE);
  TIM2_Cmd(ENABLE);

}

void init_adc(){

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
  
   ADC1_DeInit();
  
  /* Enable conversion data buffering */
  //ADC1_DataBufferCmd(ENABLE);
    

  /* Enable scan mode conversion */
  ADC1_ScanModeCmd(ENABLE);
  
  /* ADC1 Channel 6 */
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_6, ADC1_PRESSEL_FCPU_D12, \
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_ALL,\
            DISABLE);   
  
  
  ADC1_StartConversion();
  while (ADC1_GetFlagStatus(ADC1_FLAG_EOC) == RESET);
  ADC1_ClearFlag(ADC1_FLAG_EOC);
  
   Conversion_Value = ADC1_GetConversionValue();
  
   while(1){
      ADC1_StartConversion();
      while (ADC1_GetFlagStatus(ADC1_FLAG_EOC) == RESET);
      ADC1_ClearFlag(ADC1_FLAG_EOC);
      Conversion_Value = ADC1_GetConversionValue();
      
   }
  
  
}

void init_tim4(){

  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
  TIM4_DeInit();
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 250);
  TIM4_SelectOnePulseMode(TIM4_OPMODE_REPETITIVE);
  TIM4_Cmd(ENABLE);
  
}

int main( void )
{
  CLK_DeInit();
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV2);
  //CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
  
  init_gpio();
  
  init_tim2();  
  
  init_tim4();
  
  init_adc();
  
  disableInterrupts();
  EXTI_DeInit();
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_RISE_FALL); // EXTI_SENSITIVITY_RISE_FALL
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA, EXTI_SENSITIVITY_RISE_FALL); 
  enableInterrupts();      

/*
  GPIO_WriteHigh(GPIOD, PLAY_TOGGLE);
  t_delay(250);
  GPIO_WriteLow(GPIOD, PLAY_TOGGLE);
  t_delay(1000);
*/

#define HIGH 400
#define LOW (HIGH/2)
  
  GPIO_WriteHigh(GPIOD, PLAY_TOGGLE);
  t_delay(HIGH);
  GPIO_WriteLow(GPIOD, PLAY_TOGGLE);
  t_delay(LOW);
  
  while(1){
    
    counter++;
  }
   
}



#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */

u8* file_;
void assert_failed(u8* file, u32 line)
{ 
  file_ = file;
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif