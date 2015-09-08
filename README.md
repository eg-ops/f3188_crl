# f3188_crl

  // Timer 2, F=2Hz
  // ARR=Fsck/Prescale/Fuser
  // ARR=16 000 000 / 128 / 2 = 62500
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2,ENABLE);
  TIM2_TimeBaseInit(TIM2_Prescaler_128,TIM2_CounterMode_Up,62500);
  // Interrupt
  TIM2_ITConfig(TIM2_IT_Update,ENABLE);
  // Start TIM2
  TIM2_Cmd(ENABLE);
