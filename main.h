#include "stm32f10x.h"

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
/* KHAI BAO */
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef UART_InitStructure;			
/* Private function prototypes -----------------------------------------------*/
unsigned int change_value(unsigned char x_pos, unsigned char y_pos, \
						signed int value, signed int value_min,\
						signed int value_max, unsigned char value_type);
unsigned char check_for_leap_year(unsigned int year);
void cauhinhgppio(void);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void UART_Configuration (void);
void Delay(__IO uint32_t num);
void Delay_us(uint32_t us);
void unless_loop(void);
void setup_khoitao(void);
void set_RTC(unsigned int year, unsigned char month, unsigned char date,\
				unsigned char hour, unsigned char minute, unsigned char second);
void get_RTC(void);
void setting(void);	
void kiemtra_RTC(void);
void khoitaochongtrom(void);
void hoatdong_napthe(void);
void doc_the(void);
void doc_the1(void);
unsigned char CardID[5] ;
unsigned char MyID[5] = {0xA0, 0x5B, 0x9F, 0xA4, 0xC0};	//My card on my keys
unsigned char MyID0[5] = {1,2,3,4,5};
	

unsigned char keyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };

char canhbao[30] = "Co canh bao dot nhap du lieu!";
char hople[30] = "Truy cap thiet bi hop le!";
int i, j, pos,t;

void Delay(__IO uint32_t num)
{
	__IO uint32_t index = 0;

	/* default system clock is 72MHz */
	for(index = (720000 * num); index != 0; index--)
	{
	}
}
 
/**
  * @brief  	Make Delay using Loop CPU 
  * @param  	uint32_t Delay in milisecond
  * @retval 	None
  */
void Delay_us(uint32_t us){
	 uint32_t temp;
	 temp = us * 6; // cho 24 MHZ
	 while(temp > 0)
	 {
		 temp --;
	 }
}  
void unless_loop()
{
  while(1)
	 {
		 Delay(20);
	 }
}	




