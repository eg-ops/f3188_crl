#include "stm8s.h"
#include "stm8s_it.h"
#include "stm8s_gpio.h"
#include "stm8s_spi.h"
#include "stm8s_exti.h"


#define CE GPIO_PIN_3 // Port C
#define CSN GPIO_PIN_4 // Port C
#define SCK GPIO_PIN_5 // Port C
#define MOSI GPIO_PIN_6 // Port C
#define MISO GPIO_PIN_7 // Port C

#define IRQ GPIO_PIN_2 // Port D






INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
    
}



static void delay(uint32_t t)
{
  while(t--);
}

int main( void )
{
  CLK_DeInit();
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
  //CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);
  
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
  
  
  GPIO_Init(GPIOD, GPIO_PIN_4 | GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
 
  
  GPIO_Init(GPIOC, MOSI, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, MISO, GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIOC, CSN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, CE, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, SCK, GPIO_MODE_OUT_PP_LOW_FAST);
  
  GPIO_Init(GPIOD, IRQ, GPIO_MODE_IN_FL_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_FALL_LOW);
  
  
  
  
  
  
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);
  
  
  while(1){
    delay(2000);
  }
  
  return 0; 
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