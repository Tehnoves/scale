
/////////////////////////////////////
//
//  Проект весы для Андреевича     //
//  процессор F314
//
//  03.01.15 старт
//  04.01.15 флэш 
//  09.01.15 KN
//  10.01.16 kalibr
//  15.01.16 SPI
//  22.01.15 тара и реальный вес
//  21.01.15 тестирование 
//  23.01.15 ушел от текстовых переменных - появмлась память 
//  14.02.15        
//  16.02.15 мы в эфире
//  17.02.15  старт привет - ответ
//  26.02.15 временные задержки
//  01.03.15  последний рывок - калибровка                      
//  02.03.15  калибровка готов температура готов, ноль готов ( энергосбережение)
//  13.03.15  большая чистка    // 13.03.15
//  06.04.15  последний бой
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
  


//  Timer0  В И Д Е О А Д А П Т Е Р   
//  Timer2   Секундная метка
//  Timer3   Секундная метка для обрыва связи
//  Timer1   Мигаем светодиоодом авария и я жив

//  1. спать  1/10сек
//  2. поль - спать через 60сек
//  3. нет связи  1/5сек
//  4. температура через 10сек
//  5. 

// 17.03.15 разобраться с otl                **
// проверить отсутствие связи на аккум       **
// спать                                     **
// переход от температуры                    **
// корректировка температуры                 **
// калибрануть
// вернуть основной режим
//
//
// тест индикатора
// собака

//   28.03.15 
//   тест на надежность      *
//   собака                  **
//   вернуть иастера
//     сброс температуры      **
//     калибровка
//            запись flash в хвост  ?????????????????????????????????
//затягивание нуля ========  автгоноль*********************************************************
//TSRWF Receiver Wake-up Time — 280 500 µs From FS to RX ready
//TSRWS Receiver Wake-up Time — 600 900 µs From Stand-by to RX ready

//TSTWF Transmitter Wake-up Time — 120 500 µs From FS to TX ready.
//TSTWS Transmitter Wake-up Time — 600 900 µs From Stand-by to TX ready.


/*
	-8db	0,158mWt			
	-5db	0,316mWt			
	-2db	0,631mWt			
	0db		1mWt			Ватты = 10^(dBm/10) / 1000
	3db		2mWt			
	1db		1,3mWt			
	4db		2,5mWt			
	7db		5mWt			
	10db	10mWt			
	13db	20mWt			

	
								20db	100mWt
*/
//    PCA0CPH5 = 0x00;  
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
#define vzveshivarm     0x00
#define kalibruensia    0x01



//#define otl2
//#define ot3
//#define otl
//#define flash                             
#define rabota                              		//  25.03.15
#define _wdt										//  05.04.15
//#define otl4										//  06.04.15  реверсная работа   --- теперь slave
// Peripheral specific initialization functions,
// Called from the Init_Device() function


//#define diskret 4
int xdata regen2;
unsigned int xdata regen1;

bit faza,flag_temper,flag_temper_znak,flag_sleep,start_1_10,flag_1_10;
bit flag_kalibrovka;
bit left_old,left_new;
//bit leftchench;  /// ?????
bit minus;
bit right_old,right_new;
//bit rightchench;  /// ??????
bit tara_old,tara_new;
//bit tarachench;   /// ?????
bit null_old,null_new;   /// ?????
//bit nullchench;   ////????
bit flag_sek,flag_sekunda,flag_k,rab;
bit flag_peredacha,reseach_flag,send_flag,flag_r;
#ifdef rabota                              // 25.03.15
bit cviaz,stcv,stcv2; 
unsigned char xdata mode;
unsigned int xdata ms,ms2; 
#endif	

 // bit cs_con = 1;
  bit cs_dat = 1;
  bit one,two,flag_int0=0,flag_int1=0;
  bit flag_tara,P4;   //// ????????


unsigned int code baza[9]= {
		{ 2000},
		{ 3000},
		{ 5000},
		{10000},
		{15000},
		{20000},
		{30000},
		{50000},
		{00000}
	};
unsigned char code dis[10]={
	    { 10},
		{ 20},
		{ 30},
		{ 40},
		{ 50},
		{ 60},
		{ 70},
		{ 80},
		{ 90},
		{ 100}
	};
	
unsigned int xdata pr,tmp3;
unsigned char xdata tmp2,half,k2,selekt,k1,k22,k222,k2222,takt;				//  05.04.15
char xdata index,cod_test,nex,diskret;
unsigned int xdata msek,sek,msek2,msek_1_10;
///////////////char xdata xvost[5];

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

sbit P03=P0^3;
sbit P05=P0^5;

sbit right = P3^1;
sbit left  = P3^2;
sbit tara  = P3^3;
sbit nul   = P3^4;

sbit CSCON= P1^1;
sbit IRQ0 = P0^6;    
sbit IRQ1 = P0^7;  
sbit reset = P1^0;
 
sbit CSDAT = P1^2;

union
	{
  unsigned long Long;
  unsigned char Byte[4];
	} xdata ADC;
	
bit null_5,null_4,null_3,null_2;	
//bit flag_ocifrovka;
//bit ADC_buf_overflov;
//bit ADC_buf_empty=1; 
bit one,two;
bit startt;
bit flag_pp;   ///???????????
							 //  for ADC
#define Len_ADC_Buf 16       //16
	
	long xdata sred;
	unsigned long proc_kalib_summa;
	unsigned int proc_kalib_i;
	union zap
		{   char ch[4];
			long lo;
		};
	//int xdata tara_var;
 
 
	 union  Crr
	 {
	   unsigned int Int;
	   unsigned char Char[2];
	   
	 };
	 
	
#ifdef otl2	

union Crr xdata pack2[50];	
unsigned char my_kol; 	
	 
#endif		 
	 
#ifdef otl3		 
	 union Crr zj;
#endif	 
	 unsigned int rr;
	 
	 union  Crr2
	 {
	   int Int;
	   char Char[2];
	   
	 };
	

	 struct var 
	 {
		 char k1;
		 char k2;
		 long diskr;
	 };
	 
	 struct var perem;   //  ???????????
	 
	 union global2
	 {
		 struct var variable;
		 char yy[sizeof(perem)];
	 };
	 
	 union global2 xdata BB;
	 
	
 
		const char code InitConfigRegsPer[] = {
		/* 0 */				CHIPMODE_STBYMODE | FREQBAND_915 | VCO_TRIM_00,   //0b00110000
		/* 1 */				MODSEL_OOK | DATAMODE_PACKET | IFGAIN_0 | DATAMODE_BUFFERED | OOKTHRESHTYPE_PEAK,          //0b01101100 
		/* 2 */				0x0a,  //b`00001001,										  //0b00001001
		/* 3 */			   0x13,  //0b00010011,										  //0b00010011	
		/* 4 */				OOKFLOORTHRESH_VALUE,							  //0b00001100
		/* 5 */				0x4f,  //0b00001111,                                       //0b00001111 FIFOSIZE_64 | FIFO_THRSHOLD_1
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
		/* 26 0x1a*/ 			FC_200 | TXPOWER_8,                                           //0b01110000 
		/* 27 0x1b*/ 			CLKOUT_OFF | CLKOUT_12800,                                     //0b00000000
		/* 28 0x1c*/ 			MANCHESTER_OFF | 16,										       //0b00000010 !!!!!!!
		/* 29 0x1d*/ 			NODEADRS_VALUE,                                                //0
		/* 30 0x1e*/ 			PKT_FORMAT_FIXED | PREAMBLE_SIZE_3 | WHITENING_OFF | CRC_ON | ADRSFILT_NONE,  //0b01001000
 		/* 31 0x1f*/ 			FIFO_AUTOCLR_ON | FIFO_STBY_ACCESS_WRITE                      //0b00000000
							};	

						
							
			unsigned char  xdata TxPacket[packetlength];
			unsigned char  xdata RxPacket[packetlength];						
			unsigned char  xdata command,val;
			unsigned char  xdata A2,A3,i,a5,RxPacketLen;	
			unsigned char  xdata diagnoz[12];
							
  struct cons
		{ 
		
		long npv;
		int  nmpv;
		char diskreta;
		char half_diskret;
	 
		}; 
 struct cons var_cons;  //????????????????

 
	struct cons2
		{ 
		char cod;		
		struct cons var;
		long vedushee;   // тоимость одного дискрета
		long avtonol;
		
		}; 
//	struct cons2 var_cons2;	//?????????????????
		
    union global 
	 { struct cons2 co1;
	  char con[sizeof(struct cons2)];  ///var_cons2
	 };


 union global xdata AA;  ////////??????????????????????
	 
	 
  
struct cons code vesy[8]  = {    //   vesy[BB.variable.k1][2]   vesy[8].diskreta   vesy[8].half_diskret 
		{2000,10,1,0},
		{3000,20,1,0},
		{5000,40,2.1},
		{10000,10,5,2},
		{15000,100,5,2},
		{20000,200,10,5},
		{30000,200,10,5},
		{50000,400,20,10}
   };	 
	 
	 

 FLADDR xdata addr; 
 unsigned char  idata ii,jj;
 unsigned char idata yt;    ///?????????????????????
											//unsigned char code shift[8] =  {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
 unsigned char code codtabl[19]={~0xf6,~0x30,~0xe5,~0xf1,~0x33,~0xd3,~0xd7,~0x70,                              // 25.03.15
                                 ~0xf7,~0xf3,~0x02,~0x40,~0x20,~0x10,~0x80,~0x04,
                                 ~0x01,~0x08,~0x00 };

 unsigned char  pdata  regen[5];			// = {'1','2','3','4','5'}; //
 
 void init_flash(void);
 void copi_kalibr_ves(void);
 unsigned char read_spi_con(unsigned char A1);	
 	void write_spi_con(unsigned char A1, unsigned char value);
	void dia(void);
	void SetRFMode_my( char mode);
	void Send_Packet_my(void);
	
	void ReceiveFrame_my(void);
	 unsigned char reseach(void);
	 void write_spi_con(unsigned char A1, unsigned char value);
	void Send_Packet(void);	
    	void 	proc_kalib(void);	
	void init_read(void);							 
	float xdata u;								 
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
//   команда, вес, температура, напряжение, ведущее число, уровень нуля,k1, k2
//								
 //char xdata bu[] ="3,1234567.23,12.34,1234,1234567,1,1";
 
//***************************************************** 
// 
//  пакет калибровки
//
//  команда,ведущее число, уровень нуля, k1, k2
   //char xdata bu[] ="1, 1234, 1234567, 1, 1";	
char xdata tm[10];
 int  xdata ves_digit,ves_indik,ves_tara;  // ????????????//
 int xdata vesi;
 int xdata vesi1;
	float xdata temper;
	char xdata temper2,temper3;										// float	
	
struct pac 
	 {
		 char comm;
	unsigned	 long ves;
		 unsigned long temp;
		 float v;
		 //int vedushee;
		 //long avtonull;
		 //char k1;
		 //char k2;
	 };
	
	 
	 
union pack
	{
  struct pac var;
  unsigned char Byte[sizeof(struct pac)];
	} xdata packet,packet2;	 	

//____________________________________________________________________________________
	
	
void Timer_Init()
{
	
	TMOD      = 0x00;//0x02;
    CKCON     = 0x01;    //  System clock divided by 4
#ifdef rabota                              // 25.03.15
    TMOD  |= (1<<4);
	//CKCON   |= (1<<6);
#endif		
	CKCON     |= 0x10;
	
    TMR2CN    = 0x04;
	
//	TMR2L     = 0x06d;  //0x4a;   ///0x3e;
//	TMR2H     = 0xf6;  //0xa0;   //0X50;			 // b
//	TMR2RLH   = 0xf6;  //0X50;
//	TMR2RLL   = 0x6d;  //0X3e;   
	
	
    TMR3CN    = 0x04;
	TR2 = 0; 
#ifdef rabota	                              // 25.03.15
	TR1 = 1;
#endif	
}
//____________________________________________________________________________________

void SPI_Init()
{
	
	SPI0CFG   = 0x40;
    SPI0CN    = 0x01;
    SPI0CKR   =  0x0b; // 0x1d     0x79;
	IT0 = 1;
	IT1 = 1;
}
//____________________________________________________________________________________

void Port_IO_Init()
{
	// P0.0  -  SCK  (SPI0), Push-Pull,  Digital
    // P0.1  -  MISO (SPI0), Open-Drain, Digital
    // P0.2  -  MOSI (SPI0), Push-Pull,  Digital
    // P0.3  -  Unassigned,  Push-Pull,  Digital
    // P0.4  -  Unassigned,  Push-Pull,  Digital
    // P0.5  -  Unassigned,  Push-Pull,  Digital
    // P0.6  -  Skipped,     Push-Pull,  Digital
    // P0.7  -  Skipped,     Push-Pull,  Digital

    // P1.0  -  Unassigned,  Push-Pull,  Digital
    // P1.1  -  Unassigned,  Push-Pull,  Digital
    // P1.2  -  Unassigned,  Push-Pull,  Digital
    // P1.3  -  Unassigned,  Push-Pull,  Digital
    // P1.4  -  Unassigned,  Push-Pull,  Digital
    // P1.5  -  Unassigned,  Push-Pull,  Digital
    // P1.6  -  Unassigned,  Push-Pull,  Digital
    // P1.7  -  Unassigned,  Push-Pull,  Digital
    // P2.0  -  Unassigned,  Push-Pull,  Digital
    // P2.1  -  Unassigned,  Push-Pull,  Digital
    // P2.2  -  Unassigned,  Push-Pull,  Digital
    // P2.3  -  Unassigned,  Push-Pull,  Digital

    P0MDOUT   = 0xfD;
    P1MDOUT   = 0xFF;
    P2MDOUT   = 0xFF;
    P3MDOUT   = 0x01;
    P0SKIP    = 0xC0;
    XBR0      = 0x02;
    XBR1      = 0xC0;
	

}
//____________________________________________________________________________________


	void Oscillator_Init()
		{
			OSCICN    = 0x83;
		}
  //____________________________________________________________________________________ 
 

	void  Interrupts_Init()
	{
		 EIE1      = 0x80;	
		IT01CF    = 0xfe;
							
		//IT01CF    = 0x76;
		IE        = 0xef; //0xE7;   //E7  E2        25.030.15

	}
//____________________________________________________________________________________	
		
	
	
	void start_timer0(void)

    {
									// TMOD |=2;
									// TH0 = 0x10;
      TR0=1; //
      ET0=1;
		TR2 = 1;
									// EA=1;
    }
// Initialization function for device,
// Call Init_Device() from your main program
//____________________________________________________________________________________
void null_x(char q)
{null_5 = q;
			null_4 = q;
			null_3 = q;
			null_2 = q;
}


	void Init_Device(void)
	{
	
	jj= 1;
	ii = 0;

    SPI_Init();
    Port_IO_Init();
    Oscillator_Init();
	    Timer_Init();	
		start_timer0();
	
	 Interrupts_Init();
	addr=0x1c00;  
#ifdef flash		
		init_flash();	
		init_write();
#endif		
		init_read();	
										
		packet.var.ves = AA.co1.avtonol;
		diskret =  vesy[BB.variable.k1].diskreta;
		packet.var.temp = 20;
		half = vesy[BB.variable.k1].half_diskret;
	///	packet.var.v = 12.34;						//   28.03.15
		packet.var.comm= 3;
		ves_digit= BB.variable.diskr; 
	
		

	P03=0;
	P05=0;
		#ifdef rabota 
   
		
	cviaz = 0;                                    // 25.03.15
	stcv = 0;
	mode = 1;   
	ms2 = 0;
	// 25.03.15
	rab = 0;
						null_x(0);
						//null_5 = 0;
						//null_4 = 0;
						//null_3 = 0;
						//null_2 = 0;
						cod_test = 10+4;
						
#endif
			
	} 
//____________________________________________________________________________________

	
	//********************************
	//
	//	  запись FLASH
	//
	//********************************

	void init_write(void)
		{  
		char I;
		
		FLASH_PageErase((FLADDR) addr);
		for (I=0;I<sizeof(AA);I++)
				FLASH_ByteWrite ((FLADDR)addr+I,AA.con[I]);
																//addr += 8;
		for (I=0;I<sizeof(BB);I++)
				FLASH_ByteWrite ((FLADDR)addr+I+sizeof(AA),BB.yy[I]);	
		}	
//____________________________________________________________________________________

	
	
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
			{
			init_flash();	
			
			}
		}
//____________________________________________________________________________________

						//   28.03.15						//   28.03.15						//   28.03.15						//   28.03.15						//   28.03.15

void k100000(void)
	{			
		null_5 = ((regen1 % 100000/10000) != 0);	
					
	}
void k10000(void)
	{		
		null_4 = ((regen1 % 10000/1000) != 0);	
					
			}
void k1000(void)
	{			
		null_3 = ((regen1 % 1000/100) != 0);	
					
	}
void k100(void)
	{			
		null_2 = ((regen1 % 100/10) != 0);	
					
	}

///     Program Size: data=21.0 xdata=248 code=6853            //   28.03.15
////   Program Size: data=21.2 xdata=243 code=6597                  05.04.15




//____________________________________________________________________________________
	
	//*****************************
	//
	//  Прописываем Flash первый
	//  раз
	//
	//*****************************
		
		

	void init_flash(void)
	{
			AA.co1.cod  = 0x55;	
			//AA.co1.var.npv  = 20000;
			//AA.co1.var.nmpv = 100;
			//AA.co1.var.diskreta = 10;
			//AA.co1.var.half_diskret = 5;
			AA.co1.vedushee  = 1234;	//  калибровочный вес
			AA.co1.avtonol  = 11110;	    //  перезаписываемый автоноль
		
			BB.variable.k1 = 0;     // выбираем грузопдъемность весов    04.04.15     
			BB.variable.k2 = 0;
			BB.variable.diskr = 100; 
			    //   0x27616d-0x2719ed)/500; 
			    //=(AA.co1.vedushee-AA.co1.avtonol)/((BB.variable.k2+1)/10)*baza[BB.variable.k1] 
			
			init_write();
			//init_read();
			
	}
//____________________________________________________________________________________

	
	//**********************
	//
	//	 хождение по дискрету
	//
	//**********************
	
	
		 int zn( int izm)
		{
		 char tmp,t1;
			EA = 0;										// 06.04.15
			tmp = izm % diskret ;   							// 04.04.15   diskret  vesy[8].diskreta   vesy[8].half_diskret 
			EA = 1;										// 06.04.15
			if (izm>= 0)
				t1 = diskret;
			else
				t1 = -diskret;
				{
					if  (abs(tmp) > half)   								                     // 04.04.15    half  vesy[BB.variable.k1].half_diskret 
						return ( izm + t1 - tmp);   				// 04.04.15  diskret
					else 
						 return	( izm -  tmp);                // 04.04.15        6506       
				}
			   /*
			else
				{
					 if (abs(tmp) >= half)
					 	return (izm-tmp-diskret);
					 else	
					 	return ( izm -tmp);
				}	 */

		}
//____________________________________________________________________________________

	
	void delay(unsigned char cnt)  {
		unsigned char i,ii;
		for (i=0;i<cnt;i++)
			for (ii=0;ii<200;ii++)
				_nop_();
		}
//____________________________________________________________________________________


	//********************************
	//
	//  Вывод тестирования индикатора
	//
	//********************************
	
	void test(void)
		{
			char i,ii;
           faza =  kalibruensia;
		   null_x(1);
															//null_5 = 1;
															//null_4 = 1;
															//null_3 = 1;
															//null_2 = 1;
			rab = 0;
			for (i=0;i<8;i++)
					{
						cod_test = i+10;
															//start(i);//
						for  (ii  =0;ii<50; ii++)
						delay(200);
					
						if (!tara)
					  {
							tara_old = 0;
							tara_new = 0;
							flag_k=1;
							break;
						}
					}
			rab = 1;	
		
			if (!flag_k)
				{
				///////////	sprintf(regen,"%0.5u",vesi);   ?????????????
				null_x(1);
												//	null_5 = 1;
												//	null_4 = 1;
												//	null_3 = 1;
												//	null_2 = 1;
												//	null_5 = ((regen1 % 100000/10000) != 0);
													//null_5 = (regen[0] != 0');
					if (!null_5)
						k10000();			//		null_4 = ((regen1  % 10000 / 1000) != 0);
													//null_4 = (regen[1] != '0');
					if (!null_4)
						k1000();     //   null_3 = ((regen1 % 1000/ 100 ) != 0);
												//null_3 = (regen[2] != '0');
					if (!null_3)
							k100();    //  null_2 = ((regen1 %100 /10) != 0);
												//null_2 = (regen[3] != '0');
					
					
										
				}
			
			
		}
//____________________________________________________________________________________


	
	//*****************************
	//
	//  Вывод 5 нулей
	//
	//*****************************
	
	
	void copi_null(void)
	{
										//	char i;
	null_x(1);
										//null_5 = 1;
										//null_4 = 1;
										//null_3 = 1;
										//null_2 = 1;
																			//for(i=0;i<5;i++)
			{
				regen1 =baza[8];
					
			}
	
	}
//____________________________________________________________________________________

	
	
	//*****************************
	//
	//  Вывод веса калибровки .
	//
	//*****************************
	
	void copi_kalibr_ves(void)
	{
		float a;
	
		a = (BB.variable.k2+1)/10;
		regen1 = a*baza[BB.variable.k1];  
		null_x(1);
												//null_5 = 1;
												//null_4 = 1;
												//null_3 = 1;
												//null_2 = 1;
		k100000(); //null_5 = ((regen1 % 100000/10000) != 0);						//   28.03.15
		if (!null_5)
			k10000();      //     null_4 = ((regen1  % 10000 / 1000) != 0);
			
	}
	//____________________________________________________________________________________
	
	
	//*****************************
	//
	//  Вывод таблицы весов
	//
	//*****************************
	
	void copi(char nu)
	{
		null_x(1);
													//null_5 = 1;
													//null_4 = 1;
													//null_3 = 1;
													//null_2 = 1;
			regen1 =baza[nu];
			k100000(); //null_5 = ((regen1 % 100000/10000) != 0);						//   28.03.15
	}
//____________________________________________________________________________________

	
	//*****************************
	//
	//  Вывод таблицы % веса
	//
	//*****************************
	
	void copi_dis(char nu)
	{
			null_5 = 0;
			null_4 = 0;
			null_3 = 1;
			null_2 = 1;
			regen1 =dis[nu];
			k1000();     //   null_3 = ((regen1 % 1000/ 100 ) != 0);

	}
//____________________________________________________________________________________


	
	void sekunda(void)
	{
		if (flag_sek)
					{
						flag_sek = 0;
						regen1  = sek;
					
							k100();    //  null_2 = ((regen1 %100 /10) != 0);
					
					}		
			
	}
//____________________________________________________________________________________

	
	
	//*****************************
	//
	//  Инициализация параметров
	//
	//*****************************
	
	
	
	void init_param(void)
		{
			flag_1_10=1;
			flag_temper = 0;
			flag_sleep = 0;
		flag_kalibrovka= 0;	
		faza = vzveshivarm;
		reseach_flag = 0;
		send_flag = 0;	
		flag_r = 0;	
		flag_peredacha = 0;
		one=0;
		two=0;
		null_4 = 1;
		null_3 = 1;
		null_2  = 1;
		index =0;
		P2= 0xff;		P13 = 1;
		P14 = 0;
		P15 = 0;
		P16 = 0;
		P17 = 0;
		flag_sek = 0;
		startt = 0;
		flag_pp = 0;
		msek = 0;
		msek2 = 0;
		sek= 0;
		flag_sekunda = 0;	
		Init_Device();
		 TR2 = 1;           // Timer2 enabled
	
	// reset
	reset = 1;
	while (!one);
	// 100ucek 1
	reset = 0;
	while (!two);
	//  5 mcek 0
	_nop_();
	msek =0 ;
	 TR2 = 0;
											/*
												addr=0x1c00;  
										#ifdef flash		
												init_flash();	
												init_write();
										#endif		
												init_read();	
																				
												packet.var.ves = AA.co1.avtonol;
												packet.var.temp = 20;
											///	packet.var.v = 12.34;						//   28.03.15
												packet.var.comm= 3;
												ves_digit= BB.variable.diskr; 
														*/			
							
		left_old=1;left_new=1;
										//leftchench=0;
		right_old=1;right_new=1;
										//rightchench=0;
		tara_old=1;tara_new=1;
										//tarachench=0;
		null_old=1;null_new=1;
										//nullchench=0;
		index = 0;
		k2 = 0;
		selekt = 1;
		
		flag_k =0;
		ves_tara = 0;
		flag_tara = 0;
		#ifdef _wdt 		
				PCA0MD    = 0x00;				//  05.04.15
				PCA0CPL4  = 0x9E;
				PCA0MD    |= 0x40;
	#endif	
		
		
		}	
//____________________________________________________________________________________		
		
		
	void tes(char k1,char sel)
		{
			right_new = right;                        // БОЛЬШЕ
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
			left_new = left;							//  МЕНЬШЕ
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
//____________________________________________________________________________________		
		
		
	void kalib_1(void)
	{
														//float u;
		switch (k2)
						{
							case 0:                   	// выбираем грузопдъемность весов
																//init_read();
								index = BB.variable.k1;    // выбираем грузопдъемность весов   // 04.04.15
								k1 =  7;                   // ???????????????????/   // 04.04.15
								copi(index);
								break;
							case 1:                   // каким % будем грузить
							    BB.variable.k1= index;
																//init_write();
														//		init_read();
								index = BB.variable.k2;     // каким % будем грузить   // 04.04.15
								k1 =9;
								copi_dis(index);
								selekt = 2;
								break;
							case 2:						 // ждем нулевой вес
								 BB.variable.k2= index;
														//init_write(); //
														//init_read();
								copi_null();
								
								break;
							case 3:                     // калибровка нулевого веса  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!    пропись   !!!!!!!!!!!!!!!!!!
							    flag_kalibrovka = 1; 
								null_x(0);
															//null_5 = 0;
															//null_4 = 0;
															//null_3 = 0;
															//null_2 = 0;
								flag_sekunda = 1;
								proc_kalib_summa=0;
								proc_kalib_i =0;
								TR2 = 1;
								break;
							case 4:                   // выводим вес калибровки
							    flag_kalibrovka = 0;
							   AA.co1.avtonol= (proc_kalib_summa/proc_kalib_i);
								sek = 0;
								msek = 0;
								copi_kalibr_ves();
								break;
							case 5:                 // калебруем нагруженную платформу   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!  пропись !!!!!!!!!!!!!!!
							// запомнить ведущее число
							    flag_kalibrovka = 1;
								proc_kalib_summa=0;
								proc_kalib_i =0;
								TR2 = 1;
								null_x(0);
														//null_5 = 0;
														//null_4 = 0;
														//null_3 = 0;
														//null_2 = 0;
								flag_sekunda = 1;
								break;
							case 6:                   // конец калибровки
							    flag_kalibrovka = 0;
								AA.co1.vedushee = proc_kalib_summa/proc_kalib_i;
								u= (BB.variable.k2+1)/10;
								u *=baza[BB.variable.k1];
								BB.variable.diskr = (long)(AA.co1.vedushee-AA.co1.avtonol)/u;  
							
								init_write();
																	//		init_read();
								ves_digit = BB.variable.diskr;
								faza = vzveshivarm;
								flag_k = 0;
								// это конец калибровки
								break;
						}	
	}
//____________________________________________________________________________________

	void avto(void)				//  05.04.15
	{
		 AA.co1.avtonol = packet.var.ves;
		 init_write();
	}				//  05.04.15

	void tara_proc(void)
	{
			tara_new = tara;
			if (tara_old != tara_new) 
				{
					if (!tara_new)
						{
							k2++;         //7
							if (k2%2 == 0)
								ves_tara = vesi;
							else 
								ves_tara = 0;
						}
						tara_old = tara_new;
						
					}	
			right_new = right;				// измерение температуры
			if (right_old != right_new) 
				{
					if (!right_new)
						{
							k22++;         //7
							if (k22%2 == 0)
								flag_temper = 1;
							else 
								flag_temper  = 0;
						}
						right_old = right_new;
						
					}	
			left_new = left;				// измерение температуры
			if (left_old != left_new) 
				{
					if (!left_new)
						{
							k2222++;         //7
							if (k2222%2 == 0)
								flag_sleep = 1;
							else 
								flag_sleep  = 0;
						}
						left_old = left_new;
						
					}	
			if ((flag_temper)	& (flag_1_10))		
				{
					flag_1_10=0;
					start_1_10=1;
				}
			null_new = nul;
			if (null_old != null_new) 
				{
					if (!null_new)
						{
							k222++;         //7
							if (k222%2 == 0)
								//
								{
									avto();				//  05.04.15
							      //AA.co1.avtonol = packet.var.ves;
								  //init_write();
								}
						}
						null_old = null_new;
						
					}				
					
	}
//____________________________________________________________________________________

			
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
	///  
//____________________________________________________________________________________

		void avto2(void)   				//  05.04.15
		{
			
									//rab = 1;   //    
																		//sred = packet.var.ves-0x2719ed; //
									sred = (((packet.var.ves-AA.co1.avtonol)));
									sred = ((sred)/ves_digit );
									vesi1 = sred;
																		//vesi1 = (int)(packet.var.ves/ves_digit);
									vesi = zn(vesi1);
																		//regen1 = zn(vesi1);
									tara_proc();
									
																		//ves_indik = vesi-ves_tara;
									regen2 = vesi-ves_tara;
																		//	regen2 = 123;
									regen1 = abs(regen2);
		}
		void  kuku(void)
		{
				//EA = 0;
				
				if (cviaz)
					{
						if(packet.var.ves==0)							//  28.03.15
									regen1 =0;
								else
								{
								avto2();				//  05.04.15
								if ((regen1 > 0)&(regen1 < diskret*3))				//  05.04.15
										{
										 AA.co1.avtonol = packet.var.ves;					//  05.04.15
											avto2();
										}
														//  05.04.15
								}
									 if (regen1>100)
										 _nop_();
																		 // 
																			//sprintf(regen,"%0.5u",ves_indik);     5 4 3 2 1   1 0 0 0 0  1 0 0 0 1  0 0 1 0 1
								   //minus = (regen2 < 0); 	

					minus = (regen2 < 0); 
					EA = 0;
								   nex = 5;	
								  k100000(); // null_5 = ((regen1 % 100000/10000) != 0);                   //						//   28.03.15
											if ((minus) & (!null_5))
												nex--;
												
									
									if (!null_5)
										k10000();      //     null_4 = ((regen1  % 10000 / 1000) != 0);              //
									else
										null_4 = 1;
									  if (((minus) & (!null_4) & nex == 4) )
											nex--;
									
									if ((!null_4)&(!null_5))
										 k1000();     //   null_3 = ((regen1 % 1000/ 100 ) != 0);               //
									else null_3 = 1;
										  if (((minus) & (!null_3) & nex == 3))
											nex--; 
									if ((!null_3)&(!null_4)&(!null_5))                        //
										 k100();    //  null_2 = ((regen1 %100 /10) != 0);
									else	
										 null_2 =1;
										 if (((minus) & (!null_2) & nex == 2))	
											nex--; 
									EA = 1;	
										
										
									if (packet.var.temp==0)
										temper = 0;
									else
									{	EA = 0;							//  28.03.15
										temper =  ((float)(packet.var.temp)*2450/0x7fffff-54.300)/.205 -20; 
													EA = 1;							//  28.03.15
									//temper3 = 13;
									}
									if (temper< 0)							//  28.03.15
										flag_temper_znak = 1;
									else
										flag_temper_znak = 0;
									temper2 = abs(temper);  //(char)temper;
									rab = 1; 
					}
			  else
				  {
						rab = 0;
						null_x(0);
														//null_5 = 0;
														//null_4 = 0;
														//null_3 = 0;
														//null_2 = 0;
						cod_test = 10+4;
				  }
				//EA = 1;	 
}
//____________________________________________________________________________________

		
	void 	proc_kalib(void)
		{
										//unsigned long proc_kalib_summa;
										//unsigned int proc_kalib_i;
			if (flag_kalibrovka)
				{	
					proc_kalib_summa += packet.var.ves;
					proc_kalib_i++;
					
				}
			
		
		
		}
//____________________________________________________________________________________	
	
	
	void vzv(void)
	{
			if (!flag_r)
				{
					if (!reseach_flag)
					{
						
						write_spi_con(0x0d,(IRQ0_RX_STDBY_FIFOEMPTY | IRQ1_RX_STDBY_CRCOK));
						write_spi_con(0x0e,IRQ1_PLL_LOCK_PIN_ON);
						ReceiveFrame_my();
						startt = 1;
						takt = 0;					//  05.04.15
						stcv2 = 0;
					}
					else
					if (flag_int1)
						{
							reseach();
#ifdef rabota                                                          // 25.03.15
	msek2 = 0;
	cviaz = 1;
	ms2 =0;					// 27.03.15
	stcv=0;					// 27.03.15
#endif							
							if (faza == vzveshivarm )
								kuku();
							else 
								{proc_kalib();}
							P03=~P03;
						flag_r =~flag_r;					
						}
					else
					{
						
#ifdef otl4																// 06.04.15
						takt++;											//  05.04.15
						if (takt >256)
						{reseach_flag=0;								//  05.04.15
						i = read_spi_con(REG_IRQPARAM0);
						write_spi_con(REG_IRQPARAM0, (i | 0x01));
						reseach_flag = 0;
			send_flag = 0;
			flag_r =~flag_r;
					
						}
#endif																	// 06.04.15

						
#ifdef rabota 
                                                         // 25.03.15
						if (stcv2==0)
						{stcv = 1;              // 27.03.15
						ms2 = 0;               // 27.03.15
						}
#endif		
					}
				}
				else
				{
					if (!send_flag)
						{
							delay(50); // для slave
							
							write_spi_con(0x0d,IRQ1_TX_TXDONE);
							write_spi_con(0x0e,(IRQ0_TX_START_FIFONOTEMPTY | IRQ1_PLL_LOCK_PIN_ON));
							Send_Packet_my();
						
						}	
						else
					if (flag_int1)
						{Send_Packet();
						
							P05=~P05;
				
						flag_r =~flag_r;  }                 
				}
//dia();			
	#ifdef otl 			
				dia();
	#endif			
	}
		
		
	
//____________________________________________________________________________________	
	
	
	
void main(void)
	{
		
		
		
		
		PCA0MD &= ~0x40;
		
	//	while (1)
	//		delay(10);
		/*
			PCA0MD    &= ~0x40;   2ms
			PCA0MD    = 0x02;
			PCA0CPL4  = 0x5E;
			PCA0MD    |= 0x40;
			
				PCA0MD    &= ~0x40;   20ms
				PCA0MD    = 0x00;
				PCA0CPL4  = 0x9E;
				PCA0MD    |= 0x40;

			
			
			
		*/
	
		
		
											//temper = -12.1345;
											//temper2 = (char)temper;
											
										//	AA.co1.vedushee = 0x2784f5;
										//	AA.co1.avtonol = 0x271804;
										//	BB.variable.k1 = 0;
										//	BB.variable.k2 = 0;
										//	u= (BB.variable.k2+1)/10;
										//	u *=baza[BB.variable.k1];
										//	BB.variable.diskr = (AA.co1.vedushee-AA.co1.avtonol)/u;  
										  
	  
														
											//packet.var.ves = AA.co1.avtonol;
											//packet.var.temp = 20;
											//packet.var.v = 12.34;
											//packet.var.comm= 3;
											//ves_digit= BB.variable.diskr; 
															
													/*														
														for (ii =0; ii < sizeof(packet);ii++)
															packet2.Byte[ii] = packet.Byte[ii];
													*/	
		
											//ves_digit= BB.variable.diskr;                                 //(0x27616d-0x2719ed)/500;
	
													//regen1 = zn(vesi1);
										   // razborka();
								
		_nop_();
		init_param();
					
								
							
		flag_int0 = 0;
		flag_int1 = 0;

		val = read_spi_con(0x01);
		//	while (1)
		
			{
				i =0;
												//	val = read_spi_con(0);
												//	__delay_ms(2);
												//	val = read_spi_con(2);
				while (i < 0x20)
					{
					//регистры выставлены
								write_spi_con(i,InitConfigRegsPer[i]);
#ifdef otl2									
								val = read_spi_con(i); 
								a5 = InitConfigRegsPer[i];
#endif								
								i++;
					}
					

					//	i = 0;
#ifdef otl		
		dia();
#endif			
						
						
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
#ifdef otl		
		dia();
#endif	
					
						
												
			}
												
		//test();
		
		if (flag_k)
			{
			copi(index);
			kalib_1();
			while (flag_k)
				{
					vzv();
					kalib();
					tes(k1,selekt);
					_nop_();
				}
				null_x(1);
											//	null_5 = 1;
											//	null_4 = 1;
											//	null_3 = 1;
											//	null_2 = 1;
			
												//	regen[0] ='1';
												//	regen[1] ='2';
												//	regen[2] ='3';
												//	regen[3] ='4';
												//	regen[4] ='5';
			}
				faza = vzveshivarm ;	
			k2 = 1;
			k22 = 1;
			k222= 1;
			k2222=1;
			null_x(0);
										//null_5 = 0;
										//null_4 = 0;
										//null_3 = 0;
										//null_2 = 0;
												//regen1 = 12345;
											/*
											while (1)
											{
												delay(180); // для slave
												delay(180);
												delay(180);
												delay(180);
											delay(180); // для slave
												delay(180);
												delay(180);
											//	delay(180); // для slave
												
												
															
															write_spi_con(0x0d,IRQ1_TX_TXDONE);
															write_spi_con(0x0e,(IRQ0_TX_START_FIFONOTEMPTY | IRQ1_PLL_LOCK_PIN_ON));
															Send_Packet_my();
														while (!flag_int1);
													
														Send_Packet();
															P05=~P05;
											}	*/
			/*
			null_5 = 1;
												null_4 = 1;
											null_3 = 1;
									null_2 = 1;
			rab =1;
			flag_sleep= 0;
												regen1 =12345;
								*/				
			while (1)
				//while (1);	
				//	 PCA0CPH4 = 0x00; 					
									/*		{
													delay(50);	
													write_spi_con(0x0d,IRQ1_TX_TXDONE);
														write_spi_con(0x0e,(IRQ0_TX_START_FIFONOTEMPTY | IRQ1_PLL_LOCK_PIN_ON));
														Send_Packet_my();
													
													while (!flag_int1);
											Send_Packet();
													//	P03=~P03;
												
										}	
										*/
			{
				vzv();
			
									//*********************************
									
									// ждем 0б1 сек
									// перегружаем регистры для передачи
									
										//const char code InitConfigRegsPer[]
									
									
										/* 13 0x0d*/			//IRQ1_TX_TXDONE,                                   //0b00001000        **          
										/* 14 0x0e*/ 			//IRQ0_TX_START_FIFONOTEMPTY | IRQ1_PLL_LOCK_PIN_ON,      
									
									// отвечаем чем-то
									// переключаем регистры для приема
									
										//InitConfigRegsPri[]
										/* 13 0x0d*/			//IRQ0_RX_STDBY_FIFOEMPTY | IRQ1_RX_STDBY_CRCOK,                                   //0b00001000        **          
										/* 14 0x0e*/ 			//IRQ1_PLL_LOCK_PIN_ON,            //0b00010001
										
										
									//*********************************
									//kuku();
			}	
			
	}
//____________________________________________________________________________________

	
		
	//***********************************************
	//
	//  Регенерация видеопамяти  code_table [15]
	//  В И Д Е О А Д А П Т Е Р
	//
	//***********************************************
	
		void Timer0 (void) interrupt 1

		{   
				
			 TR0=0;
			 PCA0CPH4 = 0x00;  
			 P2 = 0xff;
			 if (P13)                //  второй слева разряд 
				 {
					P13 = 0;
					P14 = 1;
					
					if (flag_temper)
						if ((rab) & (!flag_sleep))
							{P2= codtabl[temper2 %100 /10]; }    // два разряда температуры выводим по любому
						else		
							P2 = 0xff;
					else
							{
								if (null_4)
									{
										if (rab)
											if (!flag_sleep)
											{
												P2= codtabl[regen1  % 10000 / 1000];  //  4
											}
											else
												P2 = 0xff;
										else	
											P2 = codtabl[cod_test];
									}
								else
									if ((minus) & ((nex+1) == 4) & ((cviaz) & (!flag_sleep)))
												P2 = codtabl[16];
									else		
												P2 = 0xff;
							}
				 }
			else if (P14)              //  третий слева разряд  
				{
					P14 = 0;
					P15 = 1;
					if (flag_temper)
						if ((rab) & (!flag_sleep))
							{P2 = codtabl[temper2 % 10]; }         // два разряда температуры выводим по любому     
						else		
							P2 = 0xff;
					else
					{
					if (null_3)
						if (rab)
								if (!flag_sleep)
							{
								P2= codtabl[regen1 % 1000/ 100 ];  // 3
							}
							else
												P2 = 0xff;
						else	
							P2 = codtabl[cod_test];
					else
						if ((minus)& ((nex+1) == 3) & ((cviaz) & (!flag_sleep)))
									P2 = codtabl[16];
						else		
						P2 = 0xff;
					}
				}
			else if (P15)                //  четвертый слева разряд
				{
					P15 = 0;
					P16 = 1;
					if (flag_temper) 
						if ((rab)& (!flag_sleep))
							{P2 = 0x9c;}       // градус
						else		
							P2 = 0xff;
					else
					{	
					if (null_2)
					if (rab)	
								if (!flag_sleep)
							{
								P2= codtabl[regen1 %100 /10];  // 2
						 	}
							else
												P2 = 0xff;
					else	
							P2 = codtabl[cod_test];
					else
						 if ((minus)& ((nex+1) == 2) & ((cviaz) & (!flag_sleep)))
									P2 = codtabl[16];
						else		
									P2 = 0xff;
					}
				}
			else if (P16)                         // пятый слева или первый справа разряд ~~~~~~~~~~~~~~~~~~~~
				{
					P16 = 0;
					P17 = 1;
					if (flag_temper)
						{
							if (rab)
							{
							if (P4)
								P2 = 0x39 &  codtabl[10+7];      // Цельсий
							else if (flag_sleep)
								P2 = codtabl[cod_test];
							else
								P2 = 0x39 ; 
							}
							else	
								P2 = codtabl[cod_test];//0x39;  //0x9c; //codtabl[cod_test];
						}
					else
						{
						if ((rab)& (!flag_sleep))
							if (P4)
								P2= codtabl[regen1 % 10] &  codtabl[10+7];      // 1
							else
								P2= codtabl[regen1 % 10] ;      // 1
						else	
							P2 = codtabl[cod_test];//0x39;  //0x9c; //codtabl[cod_test];
						}
				}
			else if (P17)         // крайний слева разряд ~~~~~~~~~~~~~~~~~~~~~  //
				{
					P17 = 0;
					P13 = 1;
					if (flag_temper)
						{
							if (rab)
									{
									if (flag_temper_znak)
										P2  = codtabl[16];    // знак минус      
									else
										P2 = 0xff;            // плюс не выводим
									}
								else
										P2 = 0xff;
						}
					else
						{
						if (null_5)
							if (rab)
									if (!flag_sleep)
								{
									P2= codtabl[regen1 % 100000/10000];   // 5
								}
								else
												P2 = 0xff;
							else	
								P2 = codtabl[cod_test];
						else	
							if ((minus)& ((nex+1) == 5) & ((cviaz) & (!flag_sleep)))
										P2 = codtabl[16];
							else
										P2 = 0xff;
						}
				}
			
			TR0=1;

		}
//____________________________________________________________________________________		


//
	//  1.  нет связи    - -             - -
	//  2. есть связь    -               -
	//                   1 2 3   4       5 6 7     такты
	//         1 3 5 7   0.25мсек
	//            2 6    0.5мсек
	//            4      7сек
	
#ifdef rabota 	
	void Timer1_int (void) interrupt INTERRUPT_TIMER1				// 25.03.15
		{
			if (stcv)					// 27.03.15
				{
					stcv2 = 1;
					ms2++;
					if (ms2 > 30)
						{
							cviaz = 0;
							mode=1;
							ms2=0;
							stcv= 0;
							rab = 0;
							null_x(0);
													//null_5 = 0;
												//null_4 = 0;
												//null_3 = 0;
												//null_2 = 0;
							
						}
				}
			/*
			if(!cviaz)
					
					{
						switch (mode) {
							case 1:
								mode++;
									cod_test = 18;
							//	P04 =0;
								break;
							case 2:
								ms++;
								if (ms >= 32)    //432
								{
									mode=1;
									ms = 0;
										cod_test = 10+4;
								//	P04 = 1;
								}
								
								break;
							
							
						}
					}
					*/
			if(!cviaz)
					{
						switch (mode) {
							case 1:
									ms= 0;
									mode++;
									cod_test = 18;
									//P04 =0;
								break;
							case 2:
								ms++;
								if (ms == 30)
									{
										mode++;
										ms = 0;
										cod_test = 10+4;
										//P04 = 1;
									}
								
								break;
							case 3:
								mode++;
								cod_test = 18;
								//P04 = 0;
								break;
							case 4:
								ms++;
								if (ms == 332)
								{
									mode=1;
									ms = 0;
									cod_test = 10+4;
									//P04 = 1;
								}
							
								break;
							
						}		
					}
			else
					{
						switch (mode) {
							case 1:
								mode++;
								if (!flag_sleep)
								P4 =0;
								else
									cod_test = 18;	
								break;
							case 2:
								ms++;
								if (ms == 322)
								{
									mode=1;
									ms = 0;
										if (!flag_sleep)
									P4 = 1;
								else
									cod_test = 10+4;
								}
								
								break;
							default:
							  mode = 1;
							
						}
					}		
					
		}
#endif	
	// 25.03.15


	
	//*****************************
	//
	//  Секундная метка
	//
	//*****************************
		
		
	void Timer2 (void) interrupt 5
	{
		TF2H = 0;
		msek++;
			one = 1;
		if (msek >= 3)
			two = 1;
		if (msek > 308)
		{
			msek = 0;
			sek++;
			flag_sek = 1;
		}
		if ((sek > 7)& (flag_temper))
			{
			flag_temper =0;
			TR2 = 0;
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
//____________________________________________________________________________________

										//************************************************************************************
										//    
										//  показания 0 ---- ждем 60сек                                <----   
										//                              ---->  спать (мигаем 1/10 сек       | запрос голове
										//                                                                  |
										//                                                                  |
										//  температура ----> ждем 10сек                                    |
										//									--------------------------------								
										//
										//  нет связи   -----> мигаем 1/5 сек                                 запрос голове
										//
										//
										//
										//			
	//*****************************
	//
	//  Секундная метка
	//
	//*****************************
		
		
	void Timer3_int (void) interrupt INTERRUPT_TIMER3
	{
		TMR3CN &= ~0x80; 
		flag_peredacha = 1;
			if (startt)
		{
			msek2++;
			if (msek2 > 5)
			{
				startt = 0;
				msek2 = 0;
				flag_r = 1;
				//cviaz = 0;
				//mode=0;
			reseach_flag = 0;
			}
		}	
		
		if (start_1_10)								// таймкр на 10 сек
				{
					msek_1_10++;
					if (msek_1_10 > 500)
						{
							start_1_10 	= 0;
							msek_1_10 	= 0;
							flag_1_10 	= 1;
							//right_new=~right_new;
							flag_temper  = 0;
							k22++; 
							//reseach_flag = 0;
						}
				}
		
		/*	
			if (start_1_5)								// таймкр на 5 сек
				{
					msek_1_5++;
					if (msek_1_5 > 5)
						{
							start_1_5 	= 0;
							msek_1_5 	= 0;
							flag_1_5 	= 1;
							//reseach_flag = 0;
						}
				}
			If (start_1_10)								// таймкр на 10 сек
				{
					msek_1_10++;
					if (msek_1_10 > 5)
						{
							start_1_10 	= 0;
							msek_1_10 	= 0;
							flag_1_10 	= 1;
							//reseach_flag = 0;
						}
				}
			if (start_60)								// таймкр на 60 сек
				{
					msek_60++;
					if (msek_60 > 5)
						{
							start_60 	= 0;
							msek_60 	= 0;
							flag_60 	= 1;
							//reseach_flag = 0;
						}
				}	*/
	}
//____________________________________________________________________________________
	
	//*************************************************
	//
	//  Пишем один байт в регистр команд
	//
	//*************************************************

	
	void write_spi_con(unsigned char A1, unsigned char value)
	{
		static unsigned char t;	
																				//	t = 0;    // ???????

		t = _crol_(A1,1);
		while (command != NU);
																				//	cs_con = 0;

		CSCON = 0;
		command = WR1;
		SPI0DAT = t;
		A2 = value;
		while (command != NU);
		CSCON = 1;

	}
	
//____________________________________________________________________________________	


	//*************************************************
	//
	//  Читаем один байт из регистра команд
	//
	//*************************************************


	
	unsigned char read_spi_con(unsigned char A1)
	{
			static unsigned char t;

																			//	t = 0;	
			t = _crol_(A1,1);
			t |= 0x40;
			command = RD;
																			//    cs_con = 0;

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
//____________________________________________________________________________________	


	//*************************************************
	//
	//  Пишем один байт в  FIFO
	//
	//*************************************************
	
	
		void WriteFIFO(unsigned char Dat)

	{
			CSDAT =0;
			while (command != NU);
			command = WR2;
			SPI0DAT = Dat;
			while (command != NU);
			CSDAT = 1;
	}
	
//____________________________________________________________________________________	
	
	
	
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
//____________________________________________________________________________________



	//*************************************************
	//
	//  Читаем один байт из FIFO
	//
	//*************************************************
	
	
unsigned char ReadFIFO(void)
	
		{
			CSDAT =0;                 // строб  данных
			while (command != NU);    // готов
			command = RD;             // команда  чтения данных
			SPI0DAT = 0xff;           // задвигаем команду
			while (command != NU);    // готов
			A3 =  SPI0DAT;            // выдвигаем байт
			CSDAT =1;                 // сняли строб данных
			
			return (A3);
		}
//____________________________________________________________________________________
	
#ifdef otl	
	//*************************************************
	//
	//  Расспечатка регистров MRF89XA во время отладки
	//
	//*************************************************


	void dia(void)
				{
																																	//unsigned char a;
					diagnoz[0] = read_spi_con(0x00); 	// GCONREG: GENERAL CONFIGURATION REGISTER
					diagnoz[1] = read_spi_con(0x0d);    // FTXRXIREG: FIFO TRANSMIT AND RECEIVE INTERRUPT REQUEST CONFIGURATION REGISTER
					diagnoz[2] = read_spi_con(0x0e);    // FTPRIREG: FIFO TRANSMIT PLL AND RSSI INTERRUPT REQUEST CONFIGURATION REGISTER
					diagnoz[3] = read_spi_con(0x1d);    // NADDSREG: NODE ADDRESS SETREGISTER
					diagnoz[4] = read_spi_con(0x1c);    // КОЛ-БАЙТ
					diagnoz[5] = read_spi_con(0x14);    //   RSSI Value bits мощность
					diagnoz[6] = read_spi_con(0x1a);    //  bit 3-1 TXOPVAL<2:0>:Transmit Output Power Value bits (1 step 3dB)   000= 13 dBm  
					diagnoz[7] = read_spi_con(0x12);    // SYNCREG: SYNC CONTROL REGISTER
					diagnoz[8] = read_spi_con(0x1e);    // PKTCREG: PACKET CONFIGURATION REGISTER
				    diagnoz[9] = read_spi_con(0x05);    // FIFOCREG: FIFO CONFIGURATION REGISTER 
				    diagnoz[10] = read_spi_con(0x1f);   // FCRCREG: FIFO CRC CONFIGURATION REGISTER
																																	//a=0;
																																    //	di_();
				}	
#endif		
//____________________________________________________________________________________

//*************************************************
	//
	//  Чистим массив передачи  ???????????????????
	//
	//************************************************


				
	void init_TX(void)
			{	unsigned char i;
			for(i = 0;i <32;i++)
				TxPacket[i] = i;
			
			}
			
//____________________________________________________________________________________		

	
	//*************************************************
	//
	//  Чистим массив приема  ???????????????????
	//
	//************************************************		
			
			
	void init_RX(void)
			{	unsigned char i;
			for(i = 0;i <32;i++)
				RxPacket[i] = 0;
			
			}	


	//*************************************************
	//
	//  Отправляем запрос голове ( дай и спать)
	//
	//************************************************					
			
			
			
	void Send_Packet_my(void)
	{
		unsigned int i;
#ifdef otl3			
		P03=1;
#endif		
		
		
		SetRFMode_my(RF_STANDBY);
		
		write_spi_con(0x1F, ((InitConfigRegsPer[0x1F] & 0xBF)| FIFO_STBY_ACCESS_WRITE));
		write_spi_con(0x0D, (InitConfigRegsPer[0x0D] | IRQ1_FIFO_OVERRUN_CLEAR ));
		write_spi_con(0x0E, ((InitConfigRegsPer[0x0E]) | 0x02));
		write_spi_con(0x16, 0x97);	//на передачу
		
#ifdef otl		
		dia();
#endif		
								//WriteFIFO(TxPacketLen+1);
#ifdef otl3									
		zj.Int++;							
#endif	
		WriteFIFO(0x23);    //Node_adrs
#ifdef otl3			
		WriteFIFO(zj.Char[1]);	
		WriteFIFO(zj.Char[0]);	
		
#endif		
		init_TX();
		for(i=0; i< 24; i++)
		{
		WriteFIFO(TxPacket[i]);
		}
									

									//до этого момента на ноге прерывания 0
		SetRFMode_my(RF_TRANSMITTER);
									//	EX0 = 1;
									//	EX1 = 1;
#ifdef otl					
	dia();
#endif			

	delay(10);	//????????????????????????????????????????  150
	
#ifdef otl3	
		P03=0;
#endif		
		
		send_flag = 1;
	}
//____________________________________________________________________________________	
	
	void Send_Packet(void)
	{
		unsigned char i;
	//while(!(flag_int1));
		flag_int1 = 0;
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

		SetRFMode_my(RF_STANDBY);							//	INTCONbits.GIE = 1;   //?
#ifdef otl			
		dia();
#endif		
		
		
		//Reset FIFO  очистить буфер после передачи
		i = read_spi_con(REG_IRQPARAM0);
		write_spi_con(REG_IRQPARAM0, (i | 0x01));
		send_flag = 0;

	}	
//____________________________________________________________________________________	
	
	
	
	
void  ReceiveFrame_my(void)
		{
#ifdef otl3			
			P03=~1;
#endif			
			SetRFMode_my(RF_STANDBY);
			
			///////////////////////
			write_spi_con(0x1F, (((FIFO_AUTOCLR_ON | FIFO_STBY_ACCESS_READ) & 0xBF)| FIFO_STBY_ACCESS_READ)|FIFO_AUTOCLR_OFF);// 
			write_spi_con(0x0D, (IRQ0_RX_STDBY_FIFOEMPTY | IRQ1_RX_STDBY_CRCOK | IRQ1_FIFO_OVERRUN_CLEAR ));
			write_spi_con(0x0E, (IRQ1_PLL_LOCK_PIN_ON | 0x02));
											//	write_spi_con(0x16, 0x97);		   // 0x97
											//EX0 = 1;
											//EX1 = 1;
			SetRFMode_my(RF_RECEIVER);
												//EX0 = 1;
											//EX1 = 1;

											//	dia();
			init_RX();
			reseach_flag = 1;
			delay(5);
#ifdef otl3				
													P03=~0;
#endif				
		}
		
											//	#ifdef otl		
													
											//	#endif			
			//////////////////////////
//____________________________________________________________________________________	


 unsigned char reseach(void)
	{		
	unsigned char  dat, node_adrs;
			unsigned char i = 0,i2;
		
	//	while(!(flag_int1))
			_nop_();
#ifdef otl			
													dia();
#endif		
//		delay(10);
		//dia();
			SetRFMode_my(RF_STANDBY);	
			RxPacketLen = 16;  // ReadFIFO();	
			flag_int0 = 0;
			node_adrs = ReadFIFO();
			RxPacketLen = (RxPacketLen-1);
	
			i = 0;
			while(RxPacketLen--)
			{
			//flag_int0 = 0;
			dat = ReadFIFO();
			RxPacket[i] = dat;
			i++;
			};
		RxPacketLen = i;
		i2 = (sizeof(struct pac));
		for (i =0;i<i2 ; i++)
	{
		   packet.Byte[i]=RxPacket[i];
	}

#ifdef otl2	
										pack2[my_kol].Char[1]= RxPacket[0];
										pack2[my_kol].Char[0]  = RxPacket[1];
										my_kol++;
										if  (my_kol>= 50)
										{
											 for (my_kol=0;my_kol< 50;my_kol++)
											pack2[my_kol].Int = 1;
											my_kol=0;
										}
#endif			
	flag_int1 = 0;

		///////////////// write_spi_con(0x0D, (0x0a));	//перезагрузить фифо
		//Reset FIFO
			i = read_spi_con(REG_IRQPARAM0);
		    write_spi_con(REG_IRQPARAM0, (i | 0x01));
			
			reseach_flag = 0;
			send_flag = 0;
			return node_adrs;
			
		}
//____________________________________________________________________________________	


		
	void irq0_int(void) interrupt 0
	 	{
			//unsigned char i;

			//  IE0 = 0;                        // Clear the SPIF flag
			  flag_int0 = 1;
		}
		
		
		
	void irq1_int(void) interrupt 2
	 	{


		//	  IE1 = 0;                        // Clear the SPIF flag
			  flag_int1 = 1;
		}
//____________________________________________________________________________________			
		

	 void spi_int(void) interrupt 6
	 	{


			  SPIF = 0;  
			  if (command == WR1)
			  	{
					SPI0DAT = A2;
					command = WR2;
				}                      // Clear the SPIF flag
			  else if (command == WR2)
			   		{command = NU;}
																		//	if   (!cs_con )
																		//		{cs_con = 1;
																			  // CSCON = 1;
																		//}
					
			  else if (command == RD)
			  		{command = NU;}
		}	

	