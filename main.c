#include "stm32f10x.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "main.h"
#include "delay.h"
#include "lcd16x2.h"
#include "LRFID.h"
#include <string.h>
#define Bitband(diachi, bitso) *(unsigned long *)((diachi & 0xF0000000) + 0x2000000 + (diachi & 0xFFFFF) * 32 + bitso * 4)
#define WKUP Bitband((unsigned long)&GPIOA->IDR, 0)
#define KEY0 Bitband((unsigned long)&GPIOC->IDR, 1)
#define KEY1 Bitband((unsigned long)&GPIOC->IDR, 13)
#define LED0 Bitband((unsigned long)&GPIOA->ODR, 8)
#define LED1 Bitband((unsigned long)&GPIOD->ODR, 2)
#define WARN Bitband((unsigned long)&GPIOA->ODR, 1)

uint8_t doce[] = {
	0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00};
uint8_t CAILOA[] = {
	0x01, 0x03, 0x07, 0x1F, 0x1F, 0x07, 0x03, 0x01};
uint8_t DAULAU[] = {
	0x1F, 0x15, 0x1B, 0x1F, 0x0E, 0x0E, 0x00, 0x00};
uint8_t traitim[] = {
	0x00, 0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00};

unsigned short kq[200], n;
unsigned long tong1, tong2;
int tt_ct = 0, nap = 0, at = 0, baothuc = 0;

float nd = 0, LM35 = 0;

char tdo = 25, doam = 50, tt = 0, info[30];
unsigned long dem2 = 0, dem = 0;
unsigned char data[16], serial[5];
uint8_t TimeDisplay = 0;

const unsigned char month_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static unsigned char cal_hour = 14;
static unsigned char cal_minute = 04;
static unsigned char cal_second = 00;
static unsigned char cal_date = 14;
static unsigned char cal_month = 04;
static unsigned int cal_year = 1994;
static unsigned char al_hour = 7;
static unsigned char al_minute = 0;
int main(void)
{
	setup_khoitao();
	kiemtra_RTC();
	khoitaochongtrom();
	while (1) //LAP VO HAN VA NAP THE MOI
	{
		hoatdong_napthe();

		//doc_the(void); //ok
		//doc_the1(void);
		/*-------------------------*/
		/*-------------- thoi gian thuc -----------------*/
		setting();
		if (TimeDisplay == 1) //NGAT 1GIAY, 1GIAY CHO HIEN THOI GIAN 1 LAN
		{
			// Xoa co bao hieu ngat 1s xay ra
			TimeDisplay = 0;
			//xuat gia tri RTC
			get_RTC();
		}
		/*-------------- bao thuc -----------------*/
		if ((cal_hour == al_hour) && (cal_minute == al_minute) && (cal_second <= 10))
		{
			WARN = 1;
			DelayMs(200);
			WARN = 0;
			baothuc = 1;
		}
		else
			baothuc = 0;
		/*------------------ kiem tra gio bao thuc va nhiet do CPU----------------------*/
		if ((!KEY1))
		{
			DelayMs(100);
			if ((!KEY1))
			{
				LED0 = ~LED0;
				lcd16x2_clrscr();
				sprintf(info, "CPU: %1.1f ", nd);
				lcd16x2_gotoxy(0, 0);
				lcd16x2_puts(info);
				lcd16x2_put_custom_char(10, 0, 0);
				lcd16x2_puts("C");

				printf("Bao Thuc: %0.2d:%0.2d \r\n", al_hour, al_minute);
				sprintf(info, "Bao Thuc: %0.2d:%0.2d ", al_hour, al_minute);
				lcd16x2_gotoxy(0, 1);
				lcd16x2_puts(info);
				while ((!KEY1))
					;
			}
		}
	}
	/*---------- END WHILE(1) -------------*/
}

unsigned char check_for_leap_year(unsigned int year)
{
	if (year % 4 == 0)
	{
		if (year % 100 == 0)
		{
			if (year % 400 == 0)
			{
				return 1;
			}

			else
			{
				return 0;
			}
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 0;
	}
}
unsigned int change_value(unsigned char x_pos, unsigned char y_pos,
						  signed int value, signed int value_min,
						  signed int value_max, unsigned char value_type)
{
	tt++;
	if (tt == 1)
	{
		lcd16x2_gotoxy(14, 0);
		lcd16x2_puts("hh");
	}
	else if (tt == 2)
	{
		lcd16x2_gotoxy(14, 0);
		lcd16x2_puts("mm");
	}
	else if (tt == 3)
	{
		lcd16x2_gotoxy(14, 0);
		lcd16x2_puts("ss");
	}
	else if (tt == 4)
	{
		lcd16x2_gotoxy(14, 0);
		lcd16x2_puts("YY");
	}
	else if (tt == 5)
	{
		lcd16x2_gotoxy(14, 0);
		lcd16x2_puts("MM");
	}
	else if (tt == 6)
	{
		lcd16x2_gotoxy(14, 0);
		lcd16x2_puts("DD");
	}
	else if (tt == 7)
	{
		lcd16x2_gotoxy(14, 0);
		lcd16x2_puts("HG");
	}
	else
	{
		lcd16x2_gotoxy(14, 0);
		lcd16x2_puts("HP");
	}
	while (1)
	{
		LED0 = ~LED0; //LED0 bao hieu dang setup thoi gian
		switch (value_type)
		{
		case 1:
		{
			lcd16x2_gotoxy(x_pos, y_pos);
			lcd16x2_puts("    ");
			break;
		}
		default:
		{
			lcd16x2_gotoxy(x_pos, y_pos);
			lcd16x2_puts("  ");
			break;
		}
		}
		DelayMs(120);

		if (!KEY1)
		{
			value++;
		}

		if (value > value_max)
		{
			value = value_min;
		}

		if (!KEY0)
		{
			value--;
		}

		if (value < value_min)
		{
			value = value_max;
		}

		switch (value_type)
		{
		case 1:
		{
			sprintf(info, "%0.2d", ((unsigned int)value));
			lcd16x2_gotoxy(x_pos, y_pos);
			lcd16x2_puts(info);
			break;
		}
		default:
		{
			sprintf(info, "%0.2d", ((unsigned int)value));
			lcd16x2_gotoxy(x_pos, y_pos);
			lcd16x2_puts(info);
			break;
		}
		}
		DelayMs(90);

		if (WKUP)
		{
			while (WKUP)
				;
			DelayMs(100);
			return value;
		}
	};
}

void RCC_Configuration(void)
{
	/* Enable GPIOA B C E and AFIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}
void cauhinhgppio(void)
{
	GPIO_InitTypeDef gp;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 |
							   RCC_APB2Periph_AFIO |
							   RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC |
							   RCC_APB2Periph_GPIOD,
						   ENABLE);
	gp.GPIO_Pin = GPIO_Pin_0;
	gp.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &gp);

	gp.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_1; // CAU HINH LED A8
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &gp);

	gp.GPIO_Pin = GPIO_Pin_2; // CAU HINH LED D2
	gp.GPIO_Speed = GPIO_Speed_50MHz;
	gp.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &gp);

	gp.GPIO_Pin = GPIO_Pin_1; // CAU HINH BUTTON  KEY0
	gp.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &gp);

	gp.GPIO_Pin = GPIO_Pin_13; // CAU HINH BUTTON  KEY1
	gp.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &gp);

	gp.GPIO_Pin = GPIO_Pin_0; // CAU HINH BUTTON  WK UP
	gp.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &gp);
}
void UART_Configuration(void)
{
	/*Cap clock cho USART v� port su dung*/
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

void cauhinhadc(void)
{
	ADC_InitTypeDef ad;

	ADC_DeInit(ADC1);

	ad.ADC_ContinuousConvMode = ENABLE;
	ad.ADC_DataAlign = ADC_DataAlign_Right;
	ad.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ad.ADC_Mode = ADC_Mode_Independent;
	ad.ADC_NbrOfChannel = 2;
	ad.ADC_ScanConvMode = ENABLE;
	ADC_Init(ADC1, &ad);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,
							 ADC_SampleTime_239Cycles5); //cau hinh adc1 kenh 0
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 2,
							 ADC_SampleTime_239Cycles5); //cau hinh adc1 kenh 16
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1))
		;
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1))
		;
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void cauhinhdma(void)
{
	DMA_InitTypeDef dm;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel1);
	dm.DMA_MemoryBaseAddr = (unsigned long)&kq;
	dm.DMA_PeripheralBaseAddr = (unsigned long)&ADC1->DR;
	dm.DMA_DIR = DMA_DIR_PeripheralSRC;
	dm.DMA_BufferSize = 200;
	dm.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dm.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dm.DMA_MemoryInc = DMA_MemoryInc_Enable;		  //tang dia chi bo nho
	dm.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //k tang dia chi ngoai vi ADC
	dm.DMA_M2M = DMA_M2M_Disable;
	dm.DMA_Mode = DMA_Mode_Circular;
	dm.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel1, &dm);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
}
void cauhinhNVIC(void)
{
	NVIC_InitTypeDef nv;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	nv.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	nv.NVIC_IRQChannelCmd = ENABLE;
	nv.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nv);
	// cau hinh uu tien ngat la 1 bit pre-emption priority va 3 bit subpriority
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	// cau hinh ngat 1s
	nv.NVIC_IRQChannel = RTC_IRQn;
	nv.NVIC_IRQChannelPreemptionPriority = 1;
	nv.NVIC_IRQChannelCmd = ENABLE;
	nv.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&nv);
}

void DMA1_Channel1_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_TC1))
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);
		/*------------- cho phep chuyen doi ----------------------*/
		tt_ct++; //bien tt_ct giup han che viec doc ADC lam gia tri bi nhieu loan tren LCD
		if (tt_ct > 5000)
			tt_ct = 0;
		if (tt_ct % 250 == 0) // khi maf tt_ct chia het cho 250 thi moi cho phep chuyen doi
		{
			for (n = 0, tong1 = 0, tong2 = 0; n < 200; n += 2)
			{
				tong1 += kq[n + 0];
				tong2 += kq[n + 1];
			}
			tong1 /= 100;
			tong2 /= 100;

			nd = (1.43 - (tong2 * 3.3 / 4096)) / 0.0043 + 25;
			LM35 = tong1;
			LM35 = LM35 / 12.413 - 2;
		}
	}
}
void RTC_IRQHandler(void)
{
	RTC_ClearITPendingBit(RTC_IT_SEC);
	LED1 = ~LED1;
	TimeDisplay = 1;
	RTC_WaitForLastTask();
	if (at == 0)
	{
		dem++;
		if (dem >= 1000)
			dem = 0;
	}
}
void RTC_Configuration2()
{
	// cap clock cho PWR va BKP
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	// cho phep truy cap den BKP Domain
	PWR_BackupAccessCmd(ENABLE);

	//Cho phep thach anh ngoai (LSE)*/
	RCC_LSEConfig(RCC_LSE_ON);
	// cho den khi nao LSE san sang
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}
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
void RTC_Configuration()
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
	{
	}
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

void setup_khoitao()
{
	//setup he thong va ngoai vi
	SystemInit();
	cauhinhgppio();
	cauhinhadc();
	cauhinhdma();
	cauhinhNVIC();
	RCC_Configuration();
	UART_Configuration();
	// khoi lao ham delay
	DelayInit();
	// khoi tao lcd- phai cam lcd vao thi moi khoi tao dc
	lcd16x2_init(LCD16X2_DISPLAY_ON_CURSOR_OFF_BLINK_OFF);
	lcd16x2_clrscr();
	//----RFID-----
	TM_MFRC522_Init();
	printf(" Tien hanh quet RFID Card...! \r\n");
	//khoi tao trang thai led
	LED0 = LED1 = 1;
	WKUP = WARN = 0;
	al_hour = BKP_ReadBackupRegister(BKP_DR8);
	al_minute = BKP_ReadBackupRegister(BKP_DR7);
	// Ctao ky tu dac biet
	lcd16x2_create_custom_char(0, doce);
	lcd16x2_create_custom_char(1, CAILOA);
	/*-------------- KHOI TAO MAN HINH HIEN THI TEN -----------------*/
	lcd16x2_gotoxy(0, 0);
	lcd16x2_puts("NGUYEN DUY BANG");
	lcd16x2_gotoxy(0, 1);
	lcd16x2_puts("---Protecting---");
	DelayMs(1000);
	lcd16x2_clrscr();
}
void setting(void)
{
	if (WKUP)
	{
		DelayMs(100);
		if (WKUP)
		{
			tt = 0;
			printf("SetUp Time! \r\n");
			RCC_RTCCLKCmd(DISABLE);
			// cho bo RTC registers duoc dong bo
			RTC_WaitForSynchro();
			// Cho qua trinh ghi data vao bo RTC duoc ket thuc
			RTC_WaitForLastTask();
			// cho phep ngat 1s
			RTC_ITConfig(RTC_IT_SEC, DISABLE);
			// cho ghi xong data cho bo RTC
			RTC_WaitForLastTask();
			printf("SetUp hour! \r\n");
			cal_hour = change_value(6, 0, cal_hour, 0, 23, 0);
			printf("Chon gio: %0.2d  -> SetUp minute! \r\n", cal_hour);
			cal_minute = change_value(9, 0, cal_minute, 0, 59, 0);
			printf("Chon phut: %0.2d -> SetUp second! \r\n", cal_minute);
			cal_second = change_value(12, 0, cal_second, 0, 59, 0);
			printf("Chon giay: %0.2d -> SetUp year! \r\n", cal_second);
			//vi co nam nhuan nen setup nam>>thang>>ngay
			cal_year = change_value(12, 1, cal_year, 1994, 2094, 1);
			printf("Chon nam: %0.2d -> SetUp month! \r\n", cal_year);
			cal_month = change_value(9, 1, cal_month, 1, 12, 0);
			printf("Chon thang: %0.2d -> SetUp date! \r\n", cal_month);
			if ((cal_month == 2) && (check_for_leap_year(cal_year) == 1))
				cal_date = change_value(6, 1, cal_date, 1, 29, 0);
			else if (cal_month == 2)
				cal_date = change_value(6, 1, cal_date, 1, 28, 0);
			else
				cal_date = change_value(6, 1, cal_date, 1, month_table[cal_month - 1], 0);

			/*----------- chinh hen gio---------------*/
			printf("Chon ngay: %0.2d -> SetUp bao thuc! \r\n", cal_date);
			al_hour = change_value(6, 0, al_hour, 0, 23, 0);
			printf("Chon gio bao thuc: %0.2d -> Chon phut bao thuc! \r\n", al_hour);
			al_minute = change_value(9, 0, al_minute, 0, 59, 0);
			printf("Chon phut bao thuc: %0.2d \r\n", al_minute);
			printf("Hoan Tat! \r\n");

			/*---------------------------*/
			LED0 = ~LED0;
			RTC_Configuration2();
			BKP_WriteBackupRegister(BKP_DR7, al_minute);
			BKP_WriteBackupRegister(BKP_DR8, al_hour);
			//THANHGHI = CardID;
			al_hour = BKP_ReadBackupRegister(BKP_DR8);
			al_minute = BKP_ReadBackupRegister(BKP_DR7);
			printf("Setup Bao Thuc: %0.2d:%0.2d \r\n", al_hour, al_minute);
			printf("Setup Time: %0.2d:%0.2d:%0.2d \r\n", cal_hour, cal_minute, cal_month);
			printf("Setup Date: %0.2d/%0.2d/%0.2d \r\n", cal_date, cal_month, cal_year);
			/*-----------------------------------*/
			set_RTC(cal_year, cal_month, cal_date, cal_hour, cal_minute, cal_second);
			lcd16x2_gotoxy(14, 0);
			lcd16x2_puts("SU");
			printf("SetUp! \r\n");
			DelayMs(1000);
			LED0 = 1;
			while (WKUP)
				;
		}
		lcd16x2_gotoxy(14, 0);
		lcd16x2_puts("Ok");
		printf("SetUp thanh cong! \r\n");
		DelayMs(1300);
		lcd16x2_gotoxy(14, 0);
		lcd16x2_puts("  ");
	}
}
void set_RTC(unsigned int year, unsigned char month, unsigned char date,
			 unsigned char hour, unsigned char minute, unsigned char second)
{
	unsigned int i = 0;
	unsigned long counts = 0;
	RTC_WaitForLastTask();
	if (year > 2094)
	{
		year = 2094;
	}

	if (year < 1994)
	{
		year = 1994;
	}

	for (i = 1994; i < year; i++)
	{
		if (check_for_leap_year(i) == 1)
		{
			counts += 31622400;
		}
		else
		{
			counts += 31536000;
		}
	}

	month -= 1;

	for (i = 0; i < month; i++)
	{
		counts += (((unsigned long)month_table[i]) * 86400);
	}

	if (check_for_leap_year(year) == 1 && month > 1) // 1 � because step before decrementing month
	{
		counts += 86400;
	}

	counts += ((unsigned long)(date - 1) * 86400);
	counts += ((unsigned long)hour * 3600);
	counts += ((unsigned long)minute * 60);
	counts += second;
	RTC_SetCounter(counts); //nap vao thanh ghi
	RTC_WaitForLastTask();
}

void get_RTC(void)
{
	unsigned int temp1 = 0;
	static unsigned int day_count;

	unsigned long temp = 0;
	unsigned long counts = 0;
	counts = RTC->CNTH;
	counts <<= 16;
	counts += RTC->CNTL;
	temp = (counts / 86400);

	if (day_count != temp)
	{
		day_count = temp;
		temp1 = 1994;

		while (temp >= 365)
		{
			if (check_for_leap_year(temp1) == 1)
			{
				if (temp >= 366)
				{
					temp -= 366;
				}

				else
				{
					break;
				}
			}

			else
			{
				temp -= 365;
			}

			temp1++;
		};

		cal_year = temp1;

		temp1 = 0;
		while (temp >= 28)
		{
			if ((temp1 == 1) && (check_for_leap_year(cal_year) == 1))
			{
				if (temp >= 29)
				{
					temp -= 29;
				}

				else
				{
					break;
				}
			}

			else
			{
				if (temp >= month_table[temp1])
				{
					temp -= ((unsigned long)month_table[temp1]);
				}

				else
				{
					break;
				}
			}

			temp1++;
		};

		cal_month = (temp1 + 1);
		cal_date = (temp + 1);
	}

	temp = (counts % 86400);

	cal_hour = (temp / 3600);
	cal_minute = ((temp % 3600) / 60);
	cal_second = ((temp % 3600) % 60);

	sprintf(info, "Time: %0.2d:%0.2d:%0.2d", cal_hour, cal_minute, cal_second);
	printf("Time: %0.2d:%0.2d:%0.2d \r\n", cal_hour, cal_minute, cal_second);
	lcd16x2_gotoxy(0, 0);
	lcd16x2_puts(info);
	printf("Bao Thuc: %0.2d:%0.2d \r\n", al_hour, al_minute);

	if (baothuc == 0)
	{
		sprintf(info, "Date: %0.2d/%0.2d/%0.2d", cal_date, cal_month, cal_year);
		printf("Date: %0.2d/%0.2d/%0.2d \r\n", cal_date, cal_month, cal_year);
		lcd16x2_gotoxy(0, 1);
		lcd16x2_puts(info);
	}
	else
	{
		lcd16x2_put_custom_char(15, 0, 1);
		sprintf(info, "Bao Thuc: %0.2d:%0.2d ", al_hour, al_minute);
		lcd16x2_gotoxy(0, 1);
		lcd16x2_puts(info);
	}
}

void kiemtra_RTC()
{ // kiem tra RTC da duoc cau hinh chua bang cach kiem tra thanh ghi backup data
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xBABA)
	{
		printf("RTC chua duoc cau hinh....\r\n");
		RTC_Configuration();
		printf("RTC da duoc cau hinh....\r\n ");
		set_RTC(cal_year, cal_month, cal_date, cal_hour, cal_minute, cal_second);
		BKP_WriteBackupRegister(BKP_DR1, 0xBABA);
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
		// Xoa man hinh
		lcd16x2_clrscr();
	}
}

void khoitaochongtrom()
{
	//ghi du lieu backup
	MyID0[0] = BKP_ReadBackupRegister(BKP_DR2);
	MyID0[1] = BKP_ReadBackupRegister(BKP_DR3);
	MyID0[2] = BKP_ReadBackupRegister(BKP_DR4);
	MyID0[3] = BKP_ReadBackupRegister(BKP_DR5);
	MyID0[4] = BKP_ReadBackupRegister(BKP_DR6);
	while (at == 0) //KIEM TRA MO KHOA
	{
		lcd16x2_gotoxy(0, 0);
		lcd16x2_puts("Chong trom xe!");
		sprintf(info, "%lu s mo khoa!", dem);
		lcd16x2_gotoxy(0, 1);
		lcd16x2_puts(info);
		printf("Con %lu s mo khoa! \r\n", dem);
		Delay(4);

		if (TM_MFRC522_Check(CardID) == MI_OK)
		{
			WARN = 1;
			DelayMs(250);
			WARN = 0;
			printf(" ID the: [0x%02X-0x%02X-0x%02X-0x%02X-0x%02X] \r\n",
				   CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
		}
		if ((dem <= 10) && ((TM_MFRC522_Compare(CardID, MyID) == MI_OK) || (TM_MFRC522_Compare(CardID, MyID0) == MI_OK)))
		{
			printf(" (^_^) Chao mung!\r\n");
			printf(" %s \r\n Card ID= [0x%02X-0x%02X-0x%02X-0x%02X-0x%02X] \r\n", hople,
				   CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
			at = 1;
			dem = 0;
		}
		else if (dem > 10)
		{
			WARN = 1; //coi hu
			if ((TM_MFRC522_Compare(CardID, MyID) == MI_OK) || (TM_MFRC522_Compare(CardID, MyID0) == MI_OK))
			{
				printf(" (^_^) Chao mung!\r\n");
				printf(" %s \r\n Card ID= [0x%02X-0x%02X-0x%02X-0x%02X-0x%02X] \r\n", hople,
					   CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
				WARN = 0;
				at = 1;
				dem = 0;
			}
		}
	}
	lcd16x2_clrscr();
	lcd16x2_gotoxy(0, 0);
	lcd16x2_puts("Da Mo Khoa Xe!");
	lcd16x2_gotoxy(0, 1);
	lcd16x2_puts("Good luck!");
	Delay(7); //tri hoan 5s de tranh nap the vao thanh ghi
	lcd16x2_clrscr();
}

void hoatdong_napthe()
{
	//-----------------------------RFID Analayzer------------------------------------------
	/*---------- test the & bo sung the moi----------------*/
	if (TM_MFRC522_Check(CardID) == MI_OK)
	{
		WARN = 1;
		DelayMs(250);
		WARN = 0; // keu bao tit' tit'
		printf("Co ID the: [0x%02X-0x%02X-0x%02X-0x%02X-0x%02X] \r\n",
			   CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
		if (TM_MFRC522_Compare(CardID, MyID) == MI_OK)
		{
			lcd16x2_clrscr();
			lcd16x2_gotoxy(0, 0);
			lcd16x2_puts("Xin phep nap ID!");
			lcd16x2_gotoxy(0, 1);
			lcd16x2_puts("Giu nguyen the!!");
			printf(" Xin phep nap the! \r\n Cho 8s!! \r\n");
			Delay(30);
			if (TM_MFRC522_Check(CardID) == MI_OK)
			{
				if (TM_MFRC522_Compare(CardID, MyID) == MI_OK)
				{
					lcd16x2_clrscr();
					lcd16x2_gotoxy(0, 0);
					lcd16x2_puts("Cho phep nap ID!");
					lcd16x2_gotoxy(0, 1);
					lcd16x2_puts("Dua the vao nap!");
					printf(" Cho phep nap the! \r\n");
					printf(" Dua the vao nap! \r\n");
					nap = 1; //bien nay cho phep nap the moi
					WARN = 1;
					DelayMs(250);
					WARN = 0;
					Delay(20);
				}
				else if (TM_MFRC522_Compare(CardID, MyID) == MI_ERR)
				{
					printf(" Gia mao the Master!  \r\n");
					lcd16x2_clrscr();
					lcd16x2_gotoxy(0, 0);
					lcd16x2_puts("NOT MASTER ID!");
					Delay(5);
					lcd16x2_clrscr();
				}
			}
			else
			{
				printf(" Khong du 8s!\r\n Khong nap the nua! \r\n");
				lcd16x2_clrscr();
				lcd16x2_gotoxy(0, 0);
				lcd16x2_puts("Khong du tgian!");
				lcd16x2_gotoxy(0, 1);
				lcd16x2_puts("Huy lenh nap!");
				Delay(5);
			}
		}
		else
		{
			printf(" XIN MASTER RFID Card...! \r\n");
			lcd16x2_clrscr();
			lcd16x2_gotoxy(1, 0);
			lcd16x2_puts("Dung Pha Phach!");
			lcd16x2_gotoxy(3, 1);
			lcd16x2_puts("Good luck!");
			printf(" %s \r\n Card ID= [0x%02X-0x%02X-0x%02X-0x%02X-0x%02X] \r\n", canhbao,
				   CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
			Delay(5);
			lcd16x2_clrscr();
		}
	}
	/************** tien hanh nap *********************/
	if (nap == 1)
	{
		lcd16x2_clrscr();
		lcd16x2_gotoxy(1, 0);
		lcd16x2_puts("Cho 3s!");
		WARN = 1;
		DelayMs(250);
		WARN = 0;
		Delay(5);
		if (TM_MFRC522_Check(CardID) == MI_OK)
		{
			printf(" ID the moi: [0x%02X-0x%02X-0x%02X-0x%02X-0x%02X] \r\n",
				   CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
			printf(" Tien hanh nap the! \r\n");
			lcd16x2_clrscr();
			lcd16x2_gotoxy(0, 0);
			lcd16x2_puts("Tien hanh nap the!");
			Delay(10);
			//kiem tra ma id card
			RTC_Configuration2();
			BKP_WriteBackupRegister(BKP_DR2, CardID[0]);
			BKP_WriteBackupRegister(BKP_DR3, CardID[1]);
			BKP_WriteBackupRegister(BKP_DR4, CardID[2]);
			BKP_WriteBackupRegister(BKP_DR5, CardID[3]);
			BKP_WriteBackupRegister(BKP_DR6, CardID[4]);
			//THANHGHI = CardID;
			MyID0[0] = BKP_ReadBackupRegister(BKP_DR2);
			MyID0[1] = BKP_ReadBackupRegister(BKP_DR3);
			MyID0[2] = BKP_ReadBackupRegister(BKP_DR4);
			MyID0[3] = BKP_ReadBackupRegister(BKP_DR5);
			MyID0[4] = BKP_ReadBackupRegister(BKP_DR6);
			printf(" Hoc ID the: [0x%02X-0x%02X-0x%02X-0x%02X-0x%02X] \r\n",
				   CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
			printf(" Hoc the thanh cong! \r\n");
			lcd16x2_clrscr();
			lcd16x2_gotoxy(0, 0);
			lcd16x2_puts("Hoc the thanh cong!");
			nap = 0; //reset bien nap ve
			Delay(15);
			lcd16x2_clrscr();
		}
		WARN = 1;
		DelayMs(250);
		WARN = 0;
	}
}

void doc_the()
{
	if (TM_MFRC522_Request(PICC_REQIDL, data) == MI_OK)
	{
		printf(" ID serial the: [0x%02X-0x%02X] \r\n", data[0], data[1]);
		TM_MFRC522_Anticoll(data);
		memcpy(serial, data, 5);
		printf(" ID serial the: [0x%02X-0x%02X-0x%02X-0x%02X-0x%02X] \r\n", serial[0], serial[1], serial[2], serial[3], serial[4]);
		TM_MFRC522_SelectTag(serial);
		for (i = 0; i < 64; i++)
		{
			if (TM_MFRC522_Auth(PICC_AUTHENT1A, i, keyA, serial) == MI_OK)
			{
				printf("Authenticated block nb. - 0x%02X with key A. \r\n", i);
				if (TM_MFRC522_Read(i, data) == MI_OK)
				{
					for (j = 0; j < 15; j++)
					{
						printf("0x%02X ", data[j]);
					}
					printf("0x%02X \r\n", data[15]);
				}
				else
				{
					printf("Read failed. \r\n");
				}
			}
			else
			{
				printf("NO Authenticated block with key A. \r\n");
				Delay(5);
				break;
			}
		}
		TM_MFRC522_Halt();
	}
}

void doc_the1()
{
	if (TM_MFRC522_Check(CardID) == MI_OK)
	{
		WARN = 1;
		DelayMs(250);
		WARN = 0;
		printf(" ID data the: [0x%02X-0x%02X-0x%02X-0x%02X-0x%02X] \r\n",
			   CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
		TM_MFRC522_SelectTag(CardID);
		for (i = 0; i < 64; i++)
		{
			if (TM_MFRC522_Auth(PICC_AUTHENT1A, i, keyA, CardID) == MI_OK)
			{
				if (TM_MFRC522_Read(i, data) == MI_OK)
				{
					for (j = 0; j < 15; j++)
					{
						printf("0x%02X ", data[j]);
					}
					printf("0x%02X \r\n", data[15]);
				}
				else
				{
					printf("Loi tai block %d \r\n", i);
					Delay(3);
				}
			}
			else
			{
				printf("auth sai r may \r\n");
				Delay(3);
				break;
			}
		}
		TM_MFRC522_Halt();
	}
	else
		WARN = 0;
}
