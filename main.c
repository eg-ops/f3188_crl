#include "stm8s.h"
#include "stm8s_it.h"
#include "stm8s_gpio.h"
#include "stm8s_spi.h"
#include "stm8s_exti.h"

#define LED0 GPIO_PIN_5 // Port B
#define LED1 GPIO_PIN_4 // Port B
#define LED2 GPIO_PIN_3 // Port A


#define VOL_PLUS GPIO_PIN_5 // Port C
#define VOL_MINUS GPIO_PIN_6 // Port C
#define NEXT GPIO_PIN_7 // Port C

#define PREV GPIO_PIN_2 // Port D
#define PLAY_TOGGLE GPIO_PIN_3 // Port D



INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
    nop();
}


INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  nop();
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
  //GPIO_Init(GPIOB, LED0, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOB, LED1, GPIO_MODE_IN_PU_IT);
  
  disableInterrupts();
  EXTI_DeInit();
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_RISE_ONLY); // EXTI_SENSITIVITY_RISE_FALL
  enableInterrupts();      
  
  while(1){
    GPIO_WriteHigh(GPIOC, NEXT);
    delay(500);
    GPIO_WriteLow(GPIOC, NEXT);
    delay(0x2000);
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