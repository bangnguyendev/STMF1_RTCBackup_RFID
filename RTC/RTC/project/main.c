#include "stm32f10x.h"
#include "stdio.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
	
PUTCHAR_PROTOTYPE
{
	/* Place your implementation of fputc here */
  	/* e.g. write a character to the LCD */
	//lcd_Data_Write((u8)ch);
	USART_SendData(USART2,(u8)ch);
	
	/*Loop until the end of transmission */
	while (USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET)
	{}

  	return ch;
}
	uint8_t TimeDisplay = 0;
  uint32_t Tmp_HH = 0, Tmp_MM = 0, Tmp_SS = 0;
  uint32_t THH = 0, TMM = 0, TSS = 0;

GPIO_InitTypeDef 					GPIO_InitStructure;
USART_InitTypeDef					UART_InitStructure;

void GPIO_Configuration(void);
void Delay_ms(uint16_t time);
void RTC_Configuration(void);
void NVIC_Configuration(void);
void Time_Adjust(void);
void Time_Display(uint32_t TimeVar);
void UART_Configuration (void);

int main(void)
{
	SysTick_Config(SystemCoreClock/1000);
	GPIO_Configuration();
	UART_Configuration();
  NVIC_Configuration();
	// kiem tra RTC da duoc cau hinh chua bang cach kiem tra thanh ghi backup data
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    printf("RTC chua duoc cau hinh....\r\n");
    RTC_Configuration();
    printf("RTC da duoc cau hinh....\r\n ");
    Time_Adjust();
    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
    // Kiem ra nguyen nhan cua reset la do k cap nguon
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("Nguon da duoc reset....\r\n");
    }
     // Kiem ra nguyen nhan cua reset la do nhan nut reset
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("Reset ngoai da xay ra....\r\n");
    }
    printf("Khong can cau hinh lai RTC....\r\n");
    // cho bo RTC on dinh
    RTC_WaitForSynchro();
    // Cho phep ngat moi 1s
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    // Cho cau hinh xong
    RTC_WaitForLastTask();
		 // Xoa co reset bo RTC
		RCC_ClearFlag();
  }
  while (1)
  {
		  // Cu 1s in thoi gian 1 lan
    if (TimeDisplay == 1)
    {
      Time_Display(RTC_GetCounter());		// doc va hien thi gioi gian qua UART
      TimeDisplay = 0;									// Xoa co bao hieu ngat 1s xay ra
		}
  }
}

void GPIO_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void Delay_ms(uint16_t time)
{
uint32_t time_n=time*12000;
	while(time_n!=0){time_n--;}

}
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	
 // cau hinh uu tien ngat la 1 bit pre-emption priority va 3 bit subpriority
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  // cau hinh ngat 1s
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void RTC_Configuration(void)
{
  // cap clock cho PWR va BKP
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  // cho phep truy cap den BKP Domain 
  PWR_BackupAccessCmd(ENABLE);
  // Reset Backup Domain 
  BKP_DeInit();
  //Cho phep thach anh ngoai (LSE)*/
  RCC_LSEConfig(RCC_LSE_ON);
  // cho den khi nao LSE san sang
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}
  // cho phep LSE la clock cu bo RTC
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  // cho phep RTC Clock 
  RCC_RTCCLKCmd(ENABLE);
  // cho bo RTC registers duoc dong bo 
  RTC_WaitForSynchro();
  // Cho qua trinh ghi data vao bo RTC duoc ket thuc
  RTC_WaitForLastTask();
  // cho phep ngat 1s
  RTC_ITConfig(RTC_IT_SEC, ENABLE);
  // cho ghi xong data cho bo RTC
  RTC_WaitForLastTask();
  // cau hinh chia clock cho bo RTC
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
  // cho ghi xong data cho bo RTC
  RTC_WaitForLastTask();
}

void Time_Adjust(void)
{
  RTC_WaitForLastTask();
  // Set gia tri thoi gian ban dau
  RTC_SetCounter((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
  // cho cho qua trinh ghi data vao thanh ghi dem hoan tat
  RTC_WaitForLastTask();
}
void Time_Display(uint32_t TimeVar)
{
  // Reset khi thanh ghi dem tran
  if (RTC_GetCounter() == 0x0001517F)
  {
     RTC_SetCounter(0x0);
   // cho cho qua trinh reset thanh ghi dem hoan tat
     RTC_WaitForLastTask();
  }
  // tinh toan gio, phut, giay tu thanh ghi dem
  THH = TimeVar / 3600;
  TMM = (TimeVar % 3600) / 60;
  TSS = (TimeVar % 3600) % 60;
  printf("Time: %0.2d:%0.2d:%0.2d\r\n", THH, TMM, TSS);
}

void UART_Configuration (void)
	{
		/*Cap clock cho USART và port su dung*/
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		
			/* Cau Tx mode AF_PP, Rx mode FLOATING  */
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
		
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
		/*Cau hinh USART*/
			UART_InitStructure.USART_BaudRate = 115200;
			UART_InitStructure.USART_WordLength = USART_WordLength_8b;
			UART_InitStructure.USART_StopBits = USART_StopBits_1;
			UART_InitStructure.USART_Parity = USART_Parity_No;
			UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
			UART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
			USART_Init(USART2, &UART_InitStructure);
				
			/* Cho phep UART hoat dong */
			USART_Cmd(USART2, ENABLE);
	
	}
	