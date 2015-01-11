
/////////////////////////////////////
//  Проект весы для Андреевича     //
//  процессор F314
//  03.01.15 старт
//  04.01.15 флэш 
//  09.01.15 KN
//  10.01.16 kalibr
//           двойная буферизация
//
//
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
#include "C8051F310_defs.h"
#include <stdio.h>
#include <string.h>
#include <intrins.h> 
#include <math.h>
#include <stdlib.h>
#include "F350_FlashPrimitives.h"
#include <andr.h>

#define ON              0x01
#define OFF             0x00 
#define pervij		    0x00
#define vtoroi		    0x01

// Peripheral specific initialization functions,
// Called from the Init_Device() function


#define diskret 4

bit left_old,left_new,leftchench;
bit right_old,right_new,rightchench;
bit tara_old,tara_new,tarachench;
bit null_old,null_new,nullchench;
bit flag_sek,flag_sekunda,flag_k;


  bit cs_con = 1;
  bit cs_dat = 1;
  bit one,two,flag_int0=0,flag_int1=0;


char code baza[9][5]= {
		{'0','2','0','0','0'},
		{'0','3','0','0','0'},
		{'0','5','0','0','0'},
		{'1','0','0','0','0'},
		{'1','5','0','0','0'},
		{'2','0','0','0','0'},
		{'3','0','0','0','0'},
		{'5','0','0','0','0'},
		{'0','0','0','0','0'}
	};
char code dis[10][5]={
	    {'0','0','0','1','0'},
		{'0','0','0','2','0'},
		{'0','0','0','3','0'},
		{'0','0','0','4','0'},
		{'0','0','0','5','0'},
		{'0','0','0','6','0'},
		{'0','0','0','7','0'},
		{'0','0','0','8','0'},
		{'0','0','0','9','0'},
		{'0','0','1','0','0'}
	};
	
unsigned int xdata pr,tmp3;
unsigned char xdata tmp2,half,k2,selekt,k1;
char xdata index;
char xdata msek,sek;
char xdata xvost[5];

sbit P13 = P1^3;
sbit P14 = P1^4;
sbit P15 = P1^5;
sbit P16 = P1^6;
sbit P17 = P1^7;

sbit P20 = P2^0;
sbit P21 = P2^1;
sbit P22 = P2^2;
sbit P23 = P2^3;
sbit P24 = P2^4;
sbit P25 = P2^5;
sbit P26 = P2^6;
sbit P27 = P2^7;

sbit right = P3^1;
sbit left  = P3^2;
sbit tara  = P3^3;
sbit nul   = P3^4;



union
	{
  unsigned long Long;
  unsigned char Byte[4];
	} xdata ADC;
	
bit null_5,null_4,null_3,null_2;	
bit flag_ocifrovka;
bit ADC_buf_overflov;
bit ADC_buf_empty=1; 
							 //  for ADC
#define Len_ADC_Buf 16       //16
	//unsigned long xdata ADC_buf[Len_ADC_Buf];
	//unsigned long xdata *start_ADC_buf;
	//unsigned long xdata *end_ADC_buf;

	unsigned long xdata ADC_srednee;
	unsigned long xdata rrez1=0,rrez1_copy=0,rrez2=0,rrez2_copy=0;


	unsigned int xdata max_weight,cell_weight;
	long xdata sred;
	union zap
		{   char ch[4];
			long lo;
		};
	int xdata tara_var;
 
 
	 union  Crr
	 {
	   unsigned int Int;
	   unsigned char Char[2];
	   
	 };
	 union  Crr2
	 {
	   int Int;
	   char Char[2];
	   
	 };
	 union Crr xdata Crc_send,Crc1_send;
	 unsigned char xdata tek_ves1[5],granica;

	 struct var 
	 {
		 char k1;
		 char k2;
		 long diskr;
	 };
	 
	 struct var perem;
	 
	 union global2
	 {
		 struct var variable;
		 char yy[sizeof(perem)];
	 };
	 
	 union global2 xdata BB;
	 
	struct cons
		{ 
		char cod;
		long npv;
		int  nmpv;
		char diskreta;
		char half_diskret;
	 
		}; 
   
   struct cons code vesy[8]= {
		{2000,10,1,0},
		{3000,20,1,0},
		{5000,40,2.1},
		{10000,10,5,2},
		{15000,100,5,2},
		{20000,200,10,5},
		{30000,200,10,5},
		{50000,400,20,10}
   };
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
							
							
  
 struct cons var_cons;


 union global 
	{ struct cons co1;
	  char con[sizeof(var_cons)];
	};


 union global xdata AA;

 FLADDR xdata addr; 
 unsigned char  idata ii,jj;
 unsigned char idata yt;
 //unsigned char code shift[8] =  {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
 unsigned char code codtabl[18]={~0xf6,~0x30,~0xe5,~0xf1,~0x33,~0xd3,~0xd7,~0x70,
                                ~0xf7,~0xf3,~0x02,~0x40,~0x20,~0x10,~0x80,~0x04,
                                ~0x01,~0x08 };

 unsigned char  pdata  regen[5];// = {'1','2','3','4','5'};
 
 void init_flash(void);
 void copi_kalibr_ves(void);
	
	

//******************************
//
//	 наверное скользящее целое
//
//******************************


/*
void ADC_calculate(void)

	{ // static unsigned long sred;
   	ADC.Byte[1]=1;//ADC0H;
   	ADC.Byte[2]=2;//ADC0M;
   	ADC.Byte[3]=3;//ADC0L;

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
										{ rrez1 = ADC_srednee;				}
									else
										{ rrez2 = ADC_srednee;
										}
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
	*/
void Timer_Init()
{
	
	TMOD      = 0x02;
    CKCON     = 0x01;
    TMR2CN    = 0x04;
	TR2 = 0; 
	
}

void SPI_Init()
{
    SPI0CFG   = 0x40;
    SPI0CN    = 0x01;
    SPI0CKR   = 0x1D;
}

void Port_IO_Init()
{
											// P0.0  -  SCK  (SPI0), Open-Drain, Digital
											// P0.1  -  MISO (SPI0), Open-Drain, Digital
											// P0.2  -  MOSI (SPI0), Open-Drain, Digital
											// P0.3  -  Unassigned,  Open-Drain, Digital
											// P0.4  -  Unassigned,  Open-Drain, Digital
											// P0.5  -  Unassigned,  Open-Drain, Digital
											// P0.6  -  Skipped,     Open-Drain, Digital
											// P0.7  -  Skipped,     Open-Drain, Digital

											// P1.0  -  Unassigned,  Open-Drain, Digital
											// P1.1  -  Unassigned,  Open-Drain, Digital
											// P1.2  -  Unassigned,  Open-Drain, Digital
											// P1.3  -  Unassigned,  Open-Drain, Digital
											// P1.4  -  Unassigned,  Open-Drain, Digital
											// P1.5  -  Unassigned,  Open-Drain, Digital
											// P1.6  -  Unassigned,  Open-Drain, Digital
											// P1.7  -  Unassigned,  Open-Drain, Digital
											// P2.0  -  Unassigned,  Open-Drain, Digital
											// P2.1  -  Unassigned,  Open-Drain, Digital
											// P2.2  -  Unassigned,  Open-Drain, Digital
											// P2.3  -  Unassigned,  Open-Drain, Digital

    P0MDOUT   = 0xFF;
    P1MDOUT   = 0xFF;
    P2MDOUT   = 0xFF;
											//  P3MDOUT   = 0x1F; 
	P3MDOUT   = 0x01;
    P0SKIP    = 0xC0;
    XBR0      = 0x02;
    XBR1      = 0xC0;

}

	void Oscillator_Init()
	{
		OSCICN    = 0x83;
	}

	void Interrupts_Init()
	{
										
		IT01CF    = 0x76;
		IE        = 0xA2;

	}
	void start_timer0(void)

    {
									// TMOD |=2;
									// TH0 = 0x10;
      TR0=1;
      ET0=1;
									// EA=1;
    }
// Initialization function for device,
// Call Init_Device() from your main program

	void Init_Device(void)
	{
	
	jj= 1;
	ii = 0;
    Timer_Init();
    SPI_Init();
    Port_IO_Init();
    Oscillator_Init();
	start_timer0();

    Interrupts_Init();
	} 
	
	//********************************
	//
	//	  запись FLASH
	//
	//********************************

	void init_write(void)
		{  
		char I;
		
															//	AA.co1.npv =20000;
															//	AA.co1.nmpv =100;
															//	AA.co1.diskreta =10;
																
																//BB.variable.k1 =2;
																//BB.variable.k2 =5;
																//BB.variable.diskr =12345;
		FLASH_PageErase((FLADDR) addr);
		for (I=0;I<sizeof(AA);I++)
				FLASH_ByteWrite ((FLADDR)addr+I,AA.con[I]);
																//addr += 8;
		for (I=0;I<sizeof(BB);I++)
				FLASH_ByteWrite ((FLADDR)addr+I+sizeof(AA),BB.yy[I]);	
		}	
	
	
	//**********************
	//
	//	 чтение FLASH
	//
	//**********************
	
	
	void init_read(void)
		{
		char i;
		//	int y;
			
		for (i=0;i<sizeof(AA);i++)
   				AA.con[i]=FLASH_ByteRead ((FLADDR)addr+i);
			
			
		for (i=0;i<sizeof(BB);i++)
			{
			
				
   				BB.yy[i]=FLASH_ByteRead ((FLADDR)addr+i+sizeof(AA));	
			}
		if (AA.co1.cod !=  0x55)
			init_flash();	
		}
	
	//*****************************
	//
	//  Прописываем Flash первый
	//  раз
	//
	//*****************************
		
		
		
	void init_flash(void)
	{
			AA.co1.cod  = 0x55;	
			AA.co1.npv  = 20000;
			AA.co1.nmpv = 100;
			AA.co1.diskreta = 10;
		
			BB.variable.k1 = 0;
			BB.variable.k2 = 0;
			BB.variable.diskr = 12345;
			init_write();
			init_read();
	}
	
	//**********************
	//
	//	 хождение по дискрету
	//
	//**********************
	
	
	unsigned int zn(unsigned int izm)
		{
		unsigned char tmp;
			tmp = izm % diskret;
			if  (tmp >= half)
				return ( izm + diskret - tmp);
			else 
				 return	( izm - tmp); 
		}
		
	void start(char bukva)
		{
		unsigned char i;
		for (i= 0;i<5;i++)
			regen[i] =  bukva; 
		}
		
	void delay(int cnt)  {
		int i,ii;
		for (i=0;i<cnt;i++)
			for (ii=0;ii<20000;ii++)
				_nop_();
		}

	//********************************
	//
	//  Вывод тестирования индикатора
	//
	//********************************
	
	void test(void)
		{
			char i,j=0x3a;
			null_5 = 1;
			null_4 = 1;
			null_3 = 1;
			null_2 = 1;
			for (i=0;i<8;i++)
					{
						start(j+i);
						delay(40);
						if (!tara)
						{
							tara_old = 0;
							tara_new = 0;
							flag_k=1;
							break;
						}
					}
					
			if (!flag_k)
				{
					regen[0] ='1';
					regen[1] ='2';
					regen[2] ='3';
					regen[3] ='4';
					regen[4] ='5';
				}
			
			
		}
	
	//*****************************
	//
	//  Вывод 5 нулей
	//
	//*****************************
	
	
	void copi_null(void)
	{
		char i;
			null_5 = 1;
			null_4 = 1;
			null_3 = 1;
			null_2 = 1;
			for(i=0;i<5;i++)
			{
				regen[i] =baza[8][i];
					
			}
	
	}
	
	
	//*****************************
	//
	//  Вывод веса калибровки .
	//
	//*****************************
	
	void copi_kalibr_ves(void)
	{
		float a;
		char i;
		for(i=0;i<5;i++)
		xvost[i] =baza[BB.variable.k1][i];
		a = (BB.variable.k2+1)/10;
		
		a = a*atoi(xvost);
		sprintf(regen,"%0.5u",(int)a); 
		null_5 = 1;
			null_4 = 1;
			null_3 = 1;
			null_2 = 1;
			null_5 = (regen[0] != '0');
		if (!null_5)
			null_4 = (regen[1] != '0');
	}
	
	//*****************************
	//
	//  Вывод таблицы весов
	//
	//*****************************
	
	void copi(char nu)
	{
		char i;
			null_5 = 1;
			null_4 = 1;
			null_3 = 1;
			null_2 = 1;
			for(i=0;i<5;i++)
			{
				regen[i] =baza[nu][i];
					if (i==0)
						null_5 = (regen[i] != '0');
			}
	
	}
	
	//*****************************
	//
	//  Вывод таблицы % веса
	//
	//*****************************
	
	void copi_dis(char nu)
	{
		char i;
			null_5 = 0;
			null_4 = 0;
			null_3 = 1;
			null_2 = 1;
			//null_1 = 1;
			for(i=0;i<5;i++)
			{
				regen[i] =dis[nu][i];
					if (i==2)
						null_3 = (regen[i] != '0');
			}
	
	}
	
	void sekunda(void)
	{
			//null_5 = 0;
			//null_4 = 0;
			//null_3 = 0;
			//null_2 = 0;
			
			{
				if (flag_sek)
					{
						flag_sek = 0;
						sprintf(regen,"%0.5u",(int)sek); 
						null_2 = (regen[3] != '0');
					}		
			}
	}
	
	
	//*****************************
	//
	//  Инициализация параметров
	//
	//*****************************
	
	
	
	void init_param(void)
		{null_4 = 1;
		null_3 = 1;
		null_2  = 1;
													//copi(3);
		index =0;
													//regen[0] ='1';
													//regen[1] ='2';
													//regen[2] ='3';
													//regen[3] ='4';
													//regen[4] ='5';
		P2= 0xff;		P13 = 1;
		P14 = 0;
		P15 = 0;
		P16 = 0;
		P17 = 0;
		Init_Device();
		addr=0x1c00;  
															//	init_write();
		//addr=0x1c00;  
		//BB.variable.k1 =0;
		//BB.variable.k2 =0;
		//BB.variable.diskr =0;
		init_read();	
		//AA.co1.npv =0;
		//AA.co1.nmpv =0;
		//AA.co1.diskreta =0;
		left_old=1;left_new=1;leftchench=0;
		right_old=1;right_new=1;rightchench=0;
		tara_old=1;tara_new=1;tarachench=0;
		null_old=1;null_new=1;nullchench=0;
		index = 0;
		k2 = 0;
		selekt = 1;
		flag_sek = 0;
		msek = 0;
		sek= 0;
		flag_sekunda = 0;
		flag_k =0;
		}	
		
		
		
	void tes(char k1,char sel)
		{
			right_new = right;
			if (right_old != right_new) 
				{
					if (!right_new)
						{
							if (++index > k1 )          //7
								index = 0;
							if (sel == 2)
							{	
								BB.variable.k2 =index;	
								copi_dis(index);
							}
							else
							{
								BB.variable.k1 =index;	
								copi(index);
							}								
						}
						right_old = right_new;
				}
			left_new = left;
			if (left_old != left_new) 
				{
					if (!left_new)
						{
							if (--index < 0 )
								index = k1;           //
							if (sel == 2)
							{	
								BB.variable.k2 =index;	
								copi_dis(index);
							}
							else
							{
								BB.variable.k1 =index;	
								copi(index);
							}			
																//BB.variable.k2 =index;
																//copi_dis(index);
						}
						left_old = left_new;
				}	
		}
	void kalib_1(void)
	{
		
		switch (k2)
						{
							case 0:
								init_read();
								index = BB.variable.k1;
								k1 =  7;
								copi(index);
								break;
							case 1:
							    BB.variable.k1= index;
								init_write();
								init_read();
								index = BB.variable.k2;
								k1 =9;
								copi_dis(index);
								selekt = 2;
								break;
							case 2:
								 BB.variable.k2= index;
								init_write();
								init_read();
								copi_null();
								
								break;
							case 3:
								null_5 = 0;
								null_4 = 0;
								null_3 = 0;
								null_2 = 0;
								flag_sekunda = 1;
								TR2 = 1;
								break;
							case 4:
								sek = 0;
								msek = 0;
								copi_kalibr_ves();
								break;
							case 5:
							// запомнить ведущее число
								TR2 = 1;
								null_5 = 0;
								null_4 = 0;
								null_3 = 0;
								null_2 = 0;
								flag_sekunda = 1;
								break;
							case 6:
								flag_k = 0;
								// это конец калибровки
								break;
						}	
	}
	

	
			
	void kalib(void)
	{
		if (!flag_sekunda)
		{
		tara_new = tara;
			if (tara_old != tara_new) 
				{
					if (!tara_new)
						{
							k2++;         //7
							kalib_1();
						}
						tara_old = tara_new;
						
					}	
		}			
		if (flag_sekunda)				
			sekunda();
						
				
			
	}
	
	
void main(void)
	{
		PCA0MD &= ~0x40;
		_nop_();
		init_param();
		//addr = 0x1c00;
		//FLASH_PageErase (addr);
		test();
		if (flag_k)
			{
			copi(index);
			kalib_1();
			while (flag_k)
				{
					kalib();
					tes(k1,selekt);
					_nop_();
				}
			null_5 = 1;
			null_4 = 1;
			null_3 = 1;
			null_2 = 1;
			
			regen[0] ='1';
			regen[1] ='2';
			regen[2] ='3';
			regen[3] ='4';
			regen[4] ='5';
			}
			while (1);
			
	}
	
	//*****************************
	//
	//  Регенерация видеопамяти
	//
	//*****************************
	
	void Timer0 (void) interrupt 1

		{   
				
			 TR0=0;
			 P2 = 0xff;
			 if (P13)
			 {
				P13 = 0;
				P14 = 1;
				if (null_4)
					P2= codtabl[regen[1] - 0x30];
				else
					P2 = 0xff;
			 }
			else if (P14)
			{
				P14 = 0;
				P15 = 1;
				if (null_3)
					P2= codtabl[regen[2] - 0x30];
				else
					P2 = 0xff;
			}
			else if (P15)
			{
				P15 = 0;
				P16 = 1;
				if (null_2)
					P2= codtabl[regen[3] - 0x30];
				else
					P2 = 0xff;
			}
			else if (P16)
			{
				P16 = 0;
				P17 = 1;
				P2= codtabl[regen[4] - 0x30];
				
			}
			else if (P17)
			{
				P17 = 0;
				P13 = 1;
				if (null_5)
					P2= codtabl[regen[0] - 0x30];
				else	
					P2 = 0xff;
			}
			
			TR0=1;

		}
		
	
	//*****************************
	//
	//  Секундная метка
	//
	//*****************************
		
		
	void Timer2 (void) interrupt 5
	{
		TF2H = 0;
		msek++;
		if (msek > 30)
		{
			msek = 0;
			sek++;
			flag_sek = 1;
		}
		if (sek > 20)
			{
			flag_sekunda = 0;
			k2++;
			kalib_1();	
			TR2 = 0;
			}
		_nop_();
	}
	