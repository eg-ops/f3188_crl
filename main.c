#include "stm8s.h"
#include "stm8s_it.h"
#include "stm8s_gpio.h"
#include "stm8s_spi.h"
#include "stm8s_exti.h"

#define LED0_YELLOW GPIO_PIN_4 // Port B
#define LED1_BLUE   GPIO_PIN_5 // Port B
#define LED2        GPIO_PIN_3 // Port A


#define VOL_PLUS GPIO_PIN_5 // Port C
#define VOL_MINUS GPIO_PIN_6 // Port C
#define NEXT GPIO_PIN_7 // Port C

#define PREV GPIO_PIN_2 // Port D
#define PLAY_TOGGLE GPIO_PIN_3 // Port D

static int counter = 0;
static int high = 0;
static unsigned int array[100] = {0};
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
        array[index++] = pins;
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
        int CCR1_Val=TIM2_GetCapture1();
        CCR1_Val++;
        CCR1_Val++;
        TIM2_SetCounter(0);
     }
      if(TIM2_GetITStatus(TIM2_IT_CC2)) 
     {
        TIM2_ClearITPendingBit(TIM2_IT_CC2);
        int CCR2_Val=TIM2_GetCapture2();
        
        if (sizeof(array)/sizeof(int) > index){
          array[index++] = CCR2_Val;
        }
        
        CCR2_Val++;
        CCR2_Val++;
        
     }
 }


static void delay(uint32_t t)
{
  while(t--);
}

void play_toggle(){
    GPIO_WriteHigh(GPIOD, PLAY_TOGGLE);
    delay(5000);
    GPIO_WriteLow(GPIOD, PLAY_TOGGLE);
    delay(1000);
}

int main( void )
{
  CLK_DeInit();
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
  //CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
  
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
  
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
  
  TIM2_TimeBaseInit(TIM2_PRESCALER_16, 0xFFFF);
  
  TIM2_PWMIConfig(TIM2_CHANNEL_1, TIM2_ICPOLARITY_FALLING, TIM2_ICSELECTION_DIRECTTI, TIM2_ICPSC_DIV2, 0xF);
  
  TIM2_ITConfig(TIM2_IT_CC1, ENABLE);
  TIM2_ITConfig(TIM2_IT_CC2, ENABLE);
  
  TIM2_CCxCmd(TIM2_CHANNEL_1, ENABLE);
   
  TIM2_Cmd(ENABLE);
  
  disableInterrupts();
  EXTI_DeInit();
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_RISE_FALL); // EXTI_SENSITIVITY_RISE_FALL
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA, EXTI_SENSITIVITY_RISE_FALL); 
  enableInterrupts();      
  
  while(1){
    /*
    GPIO_WriteHigh(GPIOD, PREV);
    delay(12500);
    GPIO_WriteLow(GPIOD, PREV);
    delay(0x2000);
    */
    delay(100);
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