//***************************************
//
//	Проект весы для Андреевича
//
//	07.05.14
//	29.05.14		СБОРКА МАКЕТА
//  23.09.14
//  16.01.15   пошел SPI добавлен ADC
//  17.01.15    temper 
//  20.01.15    cell
//  23.01.15 структура переменных, 2-я буферизация
//
//
//
//***************************************
//
//    начало компоновки и трассировки
//							НмПВ
//			2000кг 1кг        20   = 2000
//			3000кг 1кг        20   = 3000
//			5000кг 2кг        40   = 2500
//			10000кг 5кг       100  = 2000
//			15000кг 5кг       100  = 3000
//			20000кг 10кг      200  = 2000
//			30000кг 10кг      200  = 3000
//			50000кг 20кг      400  = 2500
//
//
//
//	Модель		НПВ, кг	НмПВ, кг	Цена деления, кг	Высота весов, мм	Масса весов, кг	Цена, руб.
//	МК-2000Д	2000	20			1					550						12			48000
//	МК-3000Д	3000	20			1					620						15			49900
//	МК-5000Д	5000	40			2					750						15			59900
//	МК-10000Д	10000	100			5					970						27			75000
//	МК-15000Д	15000	100			5					1120					47			85000
//	МК-20000Д	20000	200			10					1220					50			95000
//	МК-30000Д	30000	200			10					1400					101			39900
//	МК-50000Д	50000	400			20					1700					160			190000
//
//
/////////////////////////////////////



#include "compiler_defs.h"
#include "c8051F350_defs.h"
#include "F350_FlashPrimitives.h"


//#include <c8051f350.h>                       // SFR declarations
#include <stdio.h>
#include <string.h>
#include <intrins.h> 
#include <math.h>
#include <stdlib.h>
#include <andr.h>

#define Len_ADC_Buf 16   //16  
#define ON              0x01
#define OFF             0x00 
#define pervij		    0x00
#define vtoroi		    0x01
#define AD0ISEL		    0x04
#define AD0VREF		    0x02
#define AD0PL		    0x04
#define AD0BPHE		    0x07
#define AD0BPLE		    0x06
#define temperatura     0x01
#define cell            0x02
#define otl

		
		const char code InitConfigRegsPer[] = {
		/* 0 */				CHIPMODE_STBYMODE | FREQBAND_915 | VCO_TRIM_00,   //0b00110000
		/* 1 */				MODSEL_OOK | DATAMODE_PACKET | IFGAIN_0 | DATAMODE_BUFFERED | OOKTHRESHTYPE_PEAK,          //0b01101100 
		/* 2 */				0x0a,  //b`00001001,										  //0b00001001
		/* 3 */			   0x13,  //0b00010011,										  //0b00010011	
		/* 4 */				OOKFLOORTHRESH_VALUE,							  //0b00001100
		/* 5 */				0x0f,  //0b00001111,                                       //0b00001111 FIFOSIZE_64 | FIFO_THRSHOLD_1
		/* 6 */				153,
		/* 7 */				122,
		/* 8 */				68,
		/* 9 */				0,
		/* 10 0x0a*/			0,
		/* 11 0x0b*/			0,
		/* 12 0x0c*/			DEF_PARAMP | PA_RAMP_23,						  //0b00111000
		/* 13 0x0d*/			IRQ1_TX_TXDONE,                                   //0b00001000        **          
		/* 14 0x0e*/ 			IRQ0_TX_START_FIFONOTEMPTY | IRQ1_PLL_LOCK_PIN_ON,            //0b00010001
		/* 15 0x0f*/ 			RSSIIRQTHRESH_VALUE,														//0b00000000
		/* 16 0x10*/ 			PASSIVEFILT_378 | RXFC_FOPLUS100,                                             //0b10100011  **
		/* 17 0x11*/			DEF_RXPARAM1 | FO_100,														  //0b00111000
		/* 18 0x12*/			DEF_RXPARAM2 | POLYPFILT_ON | SYNC_SIZE_32 | SYNC_ON | SYNC_ERRORS_0,        // 0b10111000  ** 
		/* 19 0x13*/			DEF_RXPARAM3,                   // 0b00000111                                 //0b00000111  **
		/* 20 0x14*/			0,
		/* 21 0x15*/			OOK_THRESH_DECSTEP_000 | OOK_THRESH_DECPERIOD_000 | OOK_THRESH_AVERAGING_00,  //0b00000000
		/* 22 0x16*/ 			0x97, // 1st byte of Sync word,
		/* 23 0x17*/ 			0x37, // 2nd byte of Sync word,
		/* 24 0x18*/ 			0xF1, // 3rd byte of Sync word,
		/* 25 0x19*/ 			0xC3, // 4th byte of Sync word,
		/* 26 0x1a*/ 			FC_200 | TXPOWER_13,                                           //0b01110000 
		/* 27 0x1b*/ 			CLKOUT_OFF | CLKOUT_12800,                                     //0b00000000
		/* 28 0x1c*/ 			MANCHESTER_OFF | 2,										       //0b00000010
		/* 29 0x1d*/ 			NODEADRS_VALUE,                                                //0
		/* 30 0x1e*/ 			PKT_FORMAT_FIXED | PREAMBLE_SIZE_3 | WHITENING_OFF | CRC_ON | ADRSFILT_NONE,  //0b01001000
 		/* 31 0x1f*/ 			FIFO_AUTOCLR_ON | FIFO_STBY_ACCESS_WRITE                      //0b00000000
							};	
			
	const char code InitConfigRegsPri[] = {
		/* 0 */				CHIPMODE_STBYMODE | FREQBAND_915 | VCO_TRIM_00,   //0b00110000
		/* 1 */				MODSEL_OOK | DATAMODE_PACKET | IFGAIN_0 | DATAMODE_BUFFERED | OOKTHRESHTYPE_PEAK,          //0b01101100 
		/* 2 */				0x0a,  //0b00001001,										  //0b00001001
		/* 3 */				0x13,  //0b00010011,										  //0b00010011	
		/* 4 */				OOKFLOORTHRESH_VALUE,							  //0b00001100
		/* 5 */				0x1f,  //0b00001111,                                       //0b00001111 FIFOSIZE_64 | FIFO_THRSHOLD_1
		/* 6 */				153,
		/* 7 */				122,
		/* 8 */				68,
		/* 9 */				0,
		/* 10 0x0a*/			0,
		/* 11 0x0b*/			0,
		/* 12 0x0c*/			DEF_PARAMP | PA_RAMP_23,						  //0b00111000
		/* 13 0x0d*/			IRQ0_RX_STDBY_FIFOEMPTY | IRQ1_RX_STDBY_CRCOK,                                   //0b00001000        **          
		/* 14 0x0e*/ 			IRQ1_PLL_LOCK_PIN_ON,            //0b00010001
		/* 15 0x0f*/ 			RSSIIRQTHRESH_VALUE,														//0b00000000
		/* 16 0x10*/ 			PASSIVEFILT_378 | RXFC_FOPLUS100,                                             //0b10100011  **
		/* 17 0x11*/			DEF_RXPARAM1 | FO_100,														  //0b00111000
		/* 18 0x12*/			DEF_RXPARAM2 | POLYPFILT_ON | SYNC_SIZE_32 | SYNC_ON | SYNC_ERRORS_0,        // 0b10111000  ** 
		/* 19 0x13*/			DEF_RXPARAM3,                   // 0b00000111                                 //0b00000111  **
		/* 20 0x14*/			0,
		/* 21 0x15*/			OOK_THRESH_DECSTEP_000 | OOK_THRESH_DECPERIOD_000 | OOK_THRESH_AVERAGING_00,  //0b00000000
		/* 22 0x16*/ 			0x97, // 1st byte of Sync word,
		/* 23 0x17*/ 			0x37, // 2nd byte of Sync word,
		/* 24 0x18*/ 			0xF1, // 3rd byte of Sync word,
		/* 25 0x19*/ 			0xC3, // 4th byte of Sync word,
		/* 26 0x1a*/ 			FC_200 | TXPOWER_13,                                           //0b01110000 
		/* 27 0x1b*/ 			CLKOUT_OFF | CLKOUT_12800,                                     //0b00000000
		/* 28 0x1c*/ 			MANCHESTER_OFF | 13,										       //0b00000010
		/* 29 0x1d*/ 			NODEADRS_VALUE,                                                //0
		/* 30 0x1e*/ 			PKT_FORMAT_FIXED | PREAMBLE_SIZE_3 | WHITENING_OFF | CRC_ON | ADRSFILT_NONE,  //0b01001000
 		/* 31 0x1f*/ 			FIFO_AUTOCLR_ON | FIFO_STBY_ACCESS_READ                      //0b00000000
							};				
	unsigned char xdata  TxPacket[packetlength];
	unsigned char  xdata RxPacket[packetlength];				
					
								//SBIT (AD0VREF, ADC0CF, 2);   
								//sbit AD0VREF = ADC0CF^4;  
								//SBIT (AD0ISEL, ADC0CF, 4);     
  sbit A00 = P0^0; 
  sbit  A01 = P0^1; 
  sbit IRQ0 = P0^6;    
  sbit IRQ1 = P0^7;  
  sbit reset = P1^0;
  sbit CSCON= P1^2;
  sbit CSDAT = P1^3;
 
  bit cs_con = 1;
  bit cs_dat = 1;
  bit one,two,flag_int0=0,flag_int1=0;
  bit flag_ocifrovka,flag_ocifrovka_temper;
  bit ADC_buf_overflov;
  bit ADC_buf_empty=1; 
							
  FLADDR xdata addr; 
  
  
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 char * ttr_buf;
 unsigned char   *tr_buf;
 unsigned char  	buf[40];			// это передача
 unsigned char 	buf2[40];
 unsigned char 	buf1[40];
 unsigned char 	temp3[40];
	
	
union
	{
  unsigned long Long;
  unsigned char Byte[4];
	} xdata ADC;


	unsigned char xdata A2,A3,i,a5,RxPacketLen;
	unsigned char xdata command,val;
	unsigned char xdata msek,diagnoz[12];

	unsigned long xdata ADC_buf[Len_ADC_Buf];
	unsigned long xdata *start_ADC_buf;
	unsigned long xdata *end_ADC_buf;
unsigned long cell_long;
	unsigned long xdata ADC_srednee,cell_long2;
	unsigned long xdata rrez1=0,rrez1_c;
	unsigned long xdata rrez1_copy=0,rrez2=0,rrez2_copy=0;
	float xdata temper,temper2;
	unsigned char ind,flag_xvost,flag_zanyato2,flag_zanyato1;
	char sel;
	
void ADC_calculate(void);
//*************************************************
//
//   Перечень команда
//
//	1 калибровка
//	6 ноль
//  7 полный вес  
//  2  спать
//  3  подъем 
//  4  измерения
//  5  условный ноль
//  8 команда выполнена
//
//************************************************
	
	
	
//
//
// это пакет принятый от головы
//   команда, вес, температура, напряжение, ведущее число, уровень нуля
//								
	
	//char xdata bu[] ="3,1234567.23,12.34,1234,1234567,1,1";
	
//***************************************************** 
// 
//  пакет калибровки
//
//  команда,ведущее число, уровень нуля, k1, k2
  //char xdata bu[] ="1, 1234, 1234567, 1, 1";	
	//char xdata tm[10];
 struct var 
	 {
		 char k1;
		 char k2;
		 long diskr;
	 };
	 
	// struct var perem;
	 
	 union global2
	 {
		 struct var variable;
		 char yy[sizeof(struct var)];
	 };
	 
	 union global2 xdata BB;	
	 
struct cons
		{ 

		long npv;
		int  nmpv;
		char diskreta;
		char half_diskret;
	 
		}; 

 //struct cons var_cons;


	struct cons2
		{ 
		char cod;		
		struct cons var;
		int vedushee;
		long avtonol;
		
		}; 
	struct cons2 var_cons2;	
 union global 
	{ struct cons2 co1;
	  char con[sizeof(var_cons2)];
	};



// union global xdata AA;
 
struct pac 
	 {
		 char comm;
		 long ves;
		 char temp;
		 float v;
		
	 };
	 
union pack
	{
  struct pac var;
  unsigned char Byte[sizeof(struct pac)];
	} xdata packet,packet2;	 
	 

 void Timer_Init()
{
    TMOD      = 0x20;
    CKCON     = 0x41;
    TH1       = 0x60;
	TMR3CN    = 0x04;
//	TR3 = 1;
}

void UART_Init()
{
    SCON0     = 0x10;
}

	void SPI_Init() 
	{
		SPI0CFG   = 0x40;
		SPI0CN    = 0x01;
		SPI0CKR   = 0x97;		   //  	 0x1D
		IT0 = 1;
		IT1 = 1;
	}

void ADC_Init()
{
  
//вариант измерения температуры	

   
 
	
							//ADC0CN    = 0x10;
	//ADC0CN  |=  (1<<AD0PL);	// биполярный
	 ADC0CN  &= ~(1<<AD0PL);	// однополярном
							//	GPIO |= (1<<dclk);
							
							//  GPIO &= ~(1<< dclk);
	
							//ADC0CF  |= (1<<AD0ISEL);
							//ADC0CF  |=  (1<<AD0VREF);		

							//	AD0PL: Полярность АЦП0. 
							//0: АЦП функционирует в однополярном режиме(результат в прямом двоичном коде). 
							//1: АЦП функционирует в биполярном режиме(результат в дополнительном коде). 


							
	ADC0CF  &= ~ (1<<AD0ISEL);   // ФильтрSINC3.  внутреннееVREF
	ADC0CF  &= ~ (1<<AD0VREF);
	
							/////////ADC0CF    = 0x0;      //0x10;   // быстрый фильтр
							// AD0ISEL=0;   // ФильтрSINC3.
							// AD0ISEL= 1;  //  ФильтрFast. 
							//AD0VREF=0;  // АЦП0 использует внутреннееVREF (2.5В). Сброс этого бита в0 включает внутренний ИОН. 
											// 1: АЦП0 использует внешнееVREF. 
							// AD0VREF=1;  // АЦП0 использует внешнееVREF
							
							
    ADC0MD    = 0x80;  		///Бит7: AD0EN: Бит включения АЦП0. 
							//000: Режим ожидания.
							//001: Полная внутренняя калибровка(калибровка смещения и крутизны).
							//010: Единичное преобразование. 
						
						
    ADC0MUX   = 0x78;    	   // тензодатчик 
	//  ADC0MUX   = 0xFF;     // датчик температуры
		
		
		
		
		
	ADC0BUF   = 0x40;
							// AD0BPHE		    0x07
							// AD0BPLE		    0x06
											// Биты5-4:AD0BPS: Настройка буферов для положительного канала. 
							// 00 = Буферы не используются, т.е. зашунтированы накоротко(по умолчанию). 
							// 01 = Используется«младший» буфер. 
							// 10 = Используется«старший» буфер. 
							// 11 = Зарезервировано.
				
				
	
	//    370 ~ 50Hz 
	ADC0CLK   = 0x09;
	ADC0DECH  = 0x01;
    ADC0DECL  = 0x13;
	
	
}


void Port_IO_Init()
{
   

    // P0.0  -  SCK  (SPI0), Push-Pull,  Digital
    // P0.1  -  MISO (SPI0), Open-Drain, Digital
    // P0.2  -  MOSI (SPI0), Push-Pull,  Digital
    // P0.3  -  Unassigned,  Push-Pull,  Digital
    // P0.4  -  Unassigned,  Push-Pull,  Digital
    // P0.5  -  Unassigned,  Push-Pull,  Digital
    // P0.6  -  Unassigned,  Push-Pull,  Digital   irq1
    // P0.7  -  Unassigned,  Push-Pull,  Digital   irq0

    // P1.0  -  Unassigned,  Push-Pull,  Digital   reset
    // P1.1  -  Unassigned,  Push-Pull,  Digital
    // P1.2  -  Unassigned,  Push-Pull,  Digital   cscon
    // P1.3  -  Unassigned,  Push-Pull,  Digital   csdata
    // P1.4  -  Unassigned,  Push-Pull,  Digital
    // P1.5  -  Unassigned,  Push-Pull,  Digital
    // P1.6  -  Unassigned,  Push-Pull,  Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital

    P0MDOUT   = 0xFD;
    P1MDOUT   = 0x7F;
    XBR0      = 0x02;
    XBR1      = 0xC0;

          //     P0MDOUT   = 0xFB;
    //P1MDOUT   = 0x0D;
    //P0SKIP    = 0x01;
    //XBR0      = 0x02;
    //XBR1      = 0xC0;
    


}

void Oscillator_Init()
{
    OSCICN    = 0x83;
}

void Interrupts_Init()
{
   

								//   EIE1      = 0x08;
							   // IT01CF    = 0xfe;
							   // IE        = 0xE5;
								
	EIE1      = 0x88;  // timer 3
						//EIE1      = 0x08;
    IT01CF    = 0x07;
    IE        = 0xC5;
	
	
	
							   // EIE1      = 0x08;     ??????????
							   // IT01CF    = 0x07;
							   // IE        = 0xC5;


}

 void Timer2_Init (void)
	{
		  
		CKCON     |= 0x10;
		TMR2CN    = 0x04;

	   	

  						//  TMR2CN    = 0x04;
		TMR2L     = 0x06d;  //0x4a;   ///0x3e;
		TMR2H     = 0xf6;  //0xa0;   //0X50;			 // b
		TMR2RLH   = 0xf6;  //0X50;
		TMR2RLL   = 0x6d;  //0X3e;   
	     
	
	}
	
	
	
void init_all(void)

	{  
		char ii;
		for (ii=0;ii<16;ii++)			 // на кой фиг
    		ADC_buf[ii] =0;

		ADC.Long=0;
						//byte_cnt = 0;
						//byte_cnt1 =0;
						//buf1_doubl = 0;   // флаг буфера
						//buf2_doubl = 0;	  // флаг буфера
						//Switch = RXD;	  // 485 на прием
		temper =0.0;
		temper2 = 0.0;
   		ADC0MD=0x81;	  // 7бит вкл/откл АЦП 1бит уалибровка
  						
	}	
	
	
	
// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
	{Oscillator_Init();
    Timer_Init();
																				//  UART_Init();
   
  
    Port_IO_Init();
	SPI_Init();
    ADC_Init();
	Timer2_Init();
    Interrupts_Init();
	one = 0;
	two = 0;
	msek = 0;
	
		
	/*
	 TR2 = 1;           // Timer0 enabled
	// reset
	reset = 1;
	while (!one);
	// 100ucek 1
	reset = 0;
	while (!two);
	//  5 mcek 0
	_nop_();
	 TR2 = 0;*/
	init_all();
	}

//******************************
//
//	 наверное скользящее целое
//
//******************************



void ADC_calculate(void)

	{ 
			
	
		//float tr;
		// static unsigned long sred;
   	ADC.Byte[1]=ADC0H;
   	ADC.Byte[2]=ADC0M;
   	ADC.Byte[3]=ADC0L;

		if (!(ind%0xf))
			{ //sel = temperatura;
			 if (flag_ocifrovka_temper == pervij)
										//if (sel== temperatura)
											{ temper = ((float)(ADC.Long)*2450/0x7fffff-54.300)/.205 ;}
										//else
										//	{ cell_long = ADC_srednee/16;}
									//else
									//	if (sel== temperatura)
											{ temper2 = ((float)(ADC.Long)*2450/0x7fffff-54.300)/.205 ;
											}
										//else
										//	{cell_long2 = ADC_srednee/16;}
										
									 flag_ocifrovka_temper = ~flag_ocifrovka_temper;
			}
			else
			{ //sel =  cell;
			  
			
	
	
	
	
   	if(ADC_buf_empty)						// флаг пустого буфера
       {									// Time_request = 0;
						 start_ADC_buf 		= ADC_buf;		// начало буфера
						 end_ADC_buf 		= ADC_buf;		// конец буфера
						 end_ADC_buf[0]		= ADC.Long;
						 ADC_buf_empty		= OFF;
						 ADC_buf_overflov	= OFF;			// начальная инициализация
						 ADC_srednee		= 0;
       }
    else
       {
					 if(ADC_buf_overflov)
							{
								   ADC_srednee -= start_ADC_buf[0];	         // зачем ?
								   ADC_srednee += ADC.Long;	    // текущее значение АЦП
																// sred = ADC_srednee;
																//  sred/=Len_ADC_Buf;		  // 16 значений в буфере
								
									
									if (flag_ocifrovka == pervij)
										//if (sel== temperatura)
										//	{ temper = ((float)(ADC_srednee/16)*2450/0x7fffff-54.300)/.205 ;}
										//else
											{ cell_long = ADC_srednee/16;}
									else
										//if (sel== temperatura)
										//	{ temper2 = ((float)(ADC_srednee/16)*2450/0x7fffff-54.300)/.205 ;
										//	}
										//else
											{cell_long2 = ADC_srednee/16;}
										
									 flag_ocifrovka = ~flag_ocifrovka;
								  
								   //  окончание оцифровки



							}
					else 
							ADC_srednee += ADC.Long;
					if(start_ADC_buf<=end_ADC_buf)
						{
							   if(++end_ADC_buf == &ADC_buf[Len_ADC_Buf]) 
									{end_ADC_buf=ADC_buf;
									start_ADC_buf++;
									ADC_buf_overflov=ON;}  
						}
					 else
						{ 
								   ++end_ADC_buf;
								   ++start_ADC_buf;
								   if(start_ADC_buf >= &ADC_buf[Len_ADC_Buf]) 
											start_ADC_buf=ADC_buf;
						}
					 end_ADC_buf[0]=ADC.Long;   // 
       }
	   }
    	ind++;
			if (!(ind%0xf))
			{ sel = temperatura;
			  ADC0CN  |=  (1<<AD0PL);	// биполярный
			  ADC0MUX   = 0xff;    	// датчик температуры
			}
			else
			{ sel =  cell;
			  ADC0MUX   = 0x78; // тензодатчик 
			  ADC0CN  &= ~(1<<AD0PL);
			}
	}
	



void write_spi_con(unsigned char A1, unsigned char value)
	{
		static unsigned char t;	
		t = 0;  

		t = _crol_(A1,1);
		while (command != NU);
		cs_con = 0;

		CSCON = 0;
		command = WR1;
		SPI0DAT = t;
		A2 = value;
		while (command != NU);
		CSCON = 1;

	}

unsigned char read_spi_con(unsigned char A1)
	{
			static unsigned char t;

			t = 0;	
			t = _crol_(A1,1);
			t |= 0x40;
			command = RD;
		    cs_con = 0;

			CSCON = 0;
			SPI0DAT = t;
			while (command != NU);
			command = RD;
			SPI0DAT = 0x00;
				//A3 =  SPI0DAT;
				while (command != NU);
			A3 =  SPI0DAT;
		   	CSCON = 1;
		return (A3);
	}


	
	void WriteFIFO(unsigned char Dat)

	{
									//volatile BYTE tmp0RFIE = PHY_IRQ1_En;
			
									//PHY_IRQ1_En = 0;
			CSDAT =0;
			while (command != NU);
			command = WR2;
			SPI0DAT = Dat;
			while (command != NU);
			CSDAT = 1;
									//PHY_IRQ1_En = tmp0RFIE;
			
	}		
	void SetRFMode_my( char mode)
{
	 char	mcparam0_read;
	mcparam0_read = read_spi_con(REG_MCPARAM0);
	switch (mode) {
		case RF_TRANSMITTER:
			write_spi_con(REG_MCPARAM0, (mcparam0_read & 0x1F) | RF_TRANSMITTER);
			RF_Mode = RF_TRANSMITTER;				//RF in TX mode
			break;
		case RF_RECEIVER:
			write_spi_con(REG_MCPARAM0, (mcparam0_read & 0x1F) | RF_RECEIVER);
			RF_Mode = RF_RECEIVER;					//RF in RX mode
			break;
		case RF_SYNTHESIZER:
			write_spi_con(REG_MCPARAM0, (mcparam0_read & 0x1F) | RF_SYNTHESIZER);
			RF_Mode = RF_SYNTHESIZER;				//RF in Synthesizer mode
			break;
		case RF_STANDBY:
			write_spi_con(REG_MCPARAM0, (mcparam0_read & 0x1F) | RF_STANDBY);
			RF_Mode = RF_STANDBY;					//RF in standby mode
			break;
		case RF_SLEEP:
			write_spi_con(REG_MCPARAM0, (mcparam0_read & 0x1F) | RF_SLEEP);
			RF_Mode = RF_SLEEP;						//RF in sleep mode
			break;
	} /* end switch (mode) */

}

 	unsigned char ReadFIFO(void)
	
		{
								//volatile BYTE tmp0RFIE = PHY_IRQ1_En;
			CSDAT =0;
			while (command != NU);
			command = RD;
			SPI0DAT = 0xff;
			while (command != NU);
			A3 =  SPI0DAT;
			CSDAT =1;
								//PHY_IRQ1_En = tmp0RFIE;
			return (A3);
		}


void di_(void)
	{		unsigned char  dat,  a;
			unsigned char i = 0;
					i =read_spi_con(RF_STANDBY);
					a = 0;
					while (a<32)
					{
						dat = ReadFIFO();
						RxPacket[a] = dat;	
						a++;
					}
					SetRFMode_my(RF_RECEIVER);
	}

	void dia(void)
				{
					unsigned char a;
					diagnoz[0] = read_spi_con(0x0); 	
					diagnoz[1] = read_spi_con(0x0d); 
					diagnoz[2] = read_spi_con(0x0e); 
					diagnoz[3] = read_spi_con(0x1d); 
					diagnoz[4] = read_spi_con(0x1c);  // КОЛ-БАЙТ
					diagnoz[5] = read_spi_con(0x14);  //   RSSI Value bits мощность
					diagnoz[6] = read_spi_con(0x1a);  //  bit 3-1 TXOPVAL<2:0>:Transmit Output Power Value bits (1 step 3dB)   000= 13 dBm  
					diagnoz[7] = read_spi_con(0x12);
					diagnoz[8] = read_spi_con(0x1e);
				   diagnoz[9] = read_spi_con(0x04);
				    diagnoz[10] = read_spi_con(0x1f);
					a=0;
											//	di_();
				}

	void init_TX(void)
			{	unsigned char i;
			for(i = 0;i <32;i++)
				TxPacket[i] = i;
			
			}
	
	void init_RX(void)
			{	unsigned char i;
			for(i = 0;i <32;i++)
				RxPacket[i] = 0;
			
			}
	void Send_Packet_my(void)
	{
		unsigned int i;
		unsigned char a,a1,a2,dat;
		SetRFMode_my(RF_STANDBY);
		
		write_spi_con(0x1F, ((InitConfigRegsPer[0x1F] & 0xBF)| FIFO_STBY_ACCESS_WRITE));
		write_spi_con(0x0D, (InitConfigRegsPer[0x0D] | IRQ1_FIFO_OVERRUN_CLEAR ));
		write_spi_con(0x0E, ((InitConfigRegsPer[0x0E]) | 0x02));
		write_spi_con(0x16, 0x97);	//на передачу
		dia();
		//WriteFIFO(TxPacketLen+1);
		WriteFIFO(16);	//Node_adrs
		WriteFIFO(0x23);
		init_RX();
		init_TX();
		for(i=0; i< packetlength; i++)
		{
		WriteFIFO(TxPacket[i]);
		}

		a1 =read_spi_con(0x1f); 
		write_spi_con(0x1F,0x40);
		a2 =read_spi_con(0x1f); 
		a= 0;
			while (a<32)
					{
						dat = ReadFIFO();
						RxPacket[a] = dat;	
						a++;
					}
		write_spi_con(0x1F,a1);



	//	INTCONbits.GIE = 0;    //?
		//до этого момента на ноге прерывания 0
		SetRFMode_my(RF_TRANSMITTER);
		dia();
		for(i=0;i<255;i++)
		{
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
		}

	while(!(flag_int0));
		flag_int0 = 0;
	//здесь должно на ноге irq1 появиться лог 1
	//     IT01CF    = 0xFE;
    //     IE        = 0x85; ( 0x05) EX1 EX0 )
	//    TCON     (IE1   IE0 )  0 - к уровню           1 - к фронту

	//
	//
		{
		_nop_();	//ожидание прерывания будет заменено временной задержкой.
		_nop_();
		_nop_();
		_nop_();
		}

	//	INTCONbits.GIE = 1;   //?
		dia();
		
		
		write_spi_con(0x16, 0x68);
		SetRFMode_my(RF_RECEIVER);	//rfmode = ресивер
		//Reset FIFO  очистить буфер после передачи
		i = read_spi_con(REG_IRQPARAM0);
		write_spi_con(REG_IRQPARAM0, (i | 0x01));

	}	

	/*	
void ReceiveFrame()
{
	unsigned char data, node_adrs;
	unsigned char i = 0;
	while(!(flag_int1));
		{
		RxPacketLen = ReadFIFO();	
		flag_int0 = 0;
		node_adrs = ReadFIFO();
		RxPacketLen = (RxPacketLen-1);

		while(RxPacketLen--)
			{
			flag_int0 = 0;
			data = ReadFIFO();
			RxPacket[i] = data;
			i++;
			};
		RxPacketLen = i;
		}	
	flag_int1 = 0;

	//hasPacket = TRUE;
	//Reset FIFO
	i = RegisterRead(REG_IRQPARAM0);
	RegisterSet(REG_IRQPARAM0, (i | 0x01));
}
*/		
unsigned char ReceiveFrame_my(void)
		{
			unsigned char  dat, node_adrs;
			unsigned char i = 0;
				SetRFMode_my(RF_STANDBY);
			///////////////////////
			write_spi_con(0x1F, ((InitConfigRegsPri[0x1F] & 0xBF)| FIFO_STBY_ACCESS_READ)|FIFO_AUTOCLR_OFF);// 
			write_spi_con(0x0D, (InitConfigRegsPri[0x0D] | IRQ1_FIFO_OVERRUN_CLEAR ));
			write_spi_con(0x0E, ((InitConfigRegsPri[0x0E]) | 0x02));
		//	write_spi_con(0x16, 0x97);		   // 0x97
			SetRFMode_my(RF_RECEIVER);
			init_RX();
			dia();
			//////////////////////////
			
			while(!(flag_int1));
			dia();
				SetRFMode_my(RF_STANDBY);
			RxPacketLen = 16;  // ReadFIFO();	
			flag_int0 = 0;
			node_adrs = ReadFIFO();
			RxPacketLen = (RxPacketLen-1);
			

			while(RxPacketLen--)
			{
			flag_int0 = 0;
			dat = ReadFIFO();
			RxPacket[i] = dat;
			i++;
			};
		RxPacketLen = i;
			
	flag_int1 = 0;

			//write_spi_con(0x0D, (0x0a));	//перезагрузить фифо
				i = read_spi_con(REG_IRQPARAM0);
		        write_spi_con(REG_IRQPARAM0, (i | 0x01));
			
			
			return node_adrs;
		}
	

		void razborka(void)
		{
			
			/*	
			char ii;
			packet.var.temp = -13;
			packet.var.v = 12.34;
			packet.var.comm= 3;
														
														
		for (ii =0; ii < sizeof(packet);ii++)
		    packet2.Byte[ii] = packet.Byte[ii];
	*/	
			
		
		}

		/*
		
			char xdata bu[] ="3,1234567,23,12.34";
	
	
	
struct pac 
	 {
		 char comm;
		 long ves;
		 char temp;
		 float v;
	 };
	 
union pack
	{
  struct pac var;
  unsigned char Byte[sizeof(struct pac)];
	} xdata packet;	 
		char xdata tm[10]; 

		*/
	void raspichivaem(void)
	{
		
	}
    //**********************************************
	//
	//
	//
	//**********************************************
	//    передача мастеру 
    //
    //
	//
	//*****************************************************************************
	//   команда, вес, температура, напряжение, ведущее число, уровень нуля
    //   "3,1234567.23,12.34";

#ifdef otl														
														
 void otv(void)
		{
			char ii;
				EA = 0;
				if (flag_ocifrovka == pervij)
					{
						
						packet.var.ves=cell_long2;
					}
				else
					{
						
						packet.var.ves=cell_long;
					}
				if (flag_ocifrovka_temper == pervij)
					{
						packet.var.temp=temper2;
						
					}
				else
					{
						packet.var.temp = temper;
						
					}	
				EA = 1;
			packet.var.comm = 3;	
			packet.var.v = 0.0;
			for (ii =0; ii < sizeof(packet);ii++)
		        TxPacket[ii] = packet.Byte[ii];   // ??????
					
			//TxPacket[i]
			
		}
#endif		
		
void main(void)
	{
		PCA0MD &= ~0x40; 
		Init_Device();
		flag_int0 = 0;
		flag_int1 = 0;
			while (1)
				otv();
	

		IE0 = 0;
		IE1 = 0;
	    val = read_spi_con(0x01);
		while (1)
		
			{
				i =0;
												//	val = read_spi_con(0);
												//	__delay_ms(2);
												//	val = read_spi_con(2);
				while (i < 0x20)
					{
					//регистры выставлены
								write_spi_con(i,InitConfigRegsPri[i]);
								val = read_spi_con(i); 
								a5 = InitConfigRegsPri[i];
								i++;
					}
						i = 0;
								
						dia();
						
						val = read_spi_con(0x0e);
						write_spi_con(0x0e,(val | 0x02));
						val = read_spi_con(0x0e);
						val = read_spi_con(0x00);
						write_spi_con(0x00,((val & 0x1F) | RF_SYNTHESIZER));
						val = read_spi_con(0x00);
						do
							{
								val = read_spi_con(0x0e);

							}
						while (!(val & 0x02));
						dia();
						
						
						while (1)
						{	
						//	Send_Packet_my();
							ReceiveFrame_my();
						i++;
						}
			}


	}

     void spi_int(void) interrupt 6
	 	{


			  SPIF = 0;  
			  if (command == WR1)
			  	{
					SPI0DAT = A2;
					command = WR2;
				}                      // Clear the SPIF flag
			  else if (command == WR2)
			   		{command = NU;
						if   (!cs_con )
							{cs_con = 1;
						  // CSCON = 1;
							}
					}
			  else if (command == RD)
			  		{command = NU;
					
					}
		}

	  void Timer2_ISR (void) interrupt 5

	{
 	TF2H = 0;
 //	if (start)
	{															//	TR2=0;
	msek++;
		one = 1;
	if (msek==51)
		two = 1;
	}
	
		
 
 	}

	void irq0_int(void) interrupt 0
	 	{


			//  IE0 = 0;                        // Clear the SPIF flag
			  flag_int0 = 1;
		}
	void irq1_int(void) interrupt 2
	 	{


			//  IE1 = 0;                        // Clear the SPIF flag
			  flag_int1 = 1;
		}
	//*********************************
	//
	//
	//
	//*********************************

	void  ADC_convert(void) interrupt 10

		{
			ADC_calculate();
		
			AD0INT =0;	 		// если флаг установлен то преобразование закончено
								// его нужно сбросить программно
			ADC0MD=0x82;		// пока не пришел запрос продолжаем оцифровку			 
						 
							
		}
	
	void Timer3_int (void) interrupt INTERRUPT_TIMER3
		{
			TMR3CN &= ~0x80; 
			//flag_peredacha = 1;
		}