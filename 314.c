
/////////////////////////////////////
//  ������ ���� ��� ����������     //
//  ��������� F314
//  03.01.15 �����
//  04.01.15 ���� 
//  09.01.15 KN
//  10.01.16 kalibr
//           ������� �����������
//
//
//
//    ������ ���������� � �����������
//							����
//			2000�� 1��        20   = 2000
//			3000�� 1��        20   = 3000
//			5000�� 2��        40   = 2500
//			10000�� 5��       100  = 2000
//			15000�� 5��       100  = 3000
//			20000�� 10��      200  = 2000
//			30000�� 10��      200  = 3000
//			50000�� 20��      400  = 2500
//
//
//
//	������		���, ��	����, ��	���� �������, ��	������ �����, ��	����� �����, ��	����, ���.
//	��-2000�	2000	20			1					550						12			48000
//	��-3000�	3000	20			1					620						15			49900
//	��-5000�	5000	40			2					750						15			59900
//	��-10000�	10000	100			5					970						27			75000
//	��-15000�	15000	100			5					1120					47			85000
//	��-20000�	20000	200			10					1220					50			95000
//	��-30000�	30000	200			10					1400					101			39900
//	��-50000�	50000	400			20					1700					160			190000
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

sbit CSCON= P1^2;
sbit IRQ0 = P0^6;    
sbit IRQ1 = P0^7;  
sbit reset = P1^0;
 
sbit CSDAT = P1^1;

union
	{
  unsigned long Long;
  unsigned char Byte[4];
	} xdata ADC;
	
bit null_5,null_4,null_3,null_2;	
bit flag_ocifrovka;
bit ADC_buf_overflov;
bit ADC_buf_empty=1; 
bit one,two;
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
			unsigned char xdata command,val;
			unsigned char xdata A2,A3,i,a5,RxPacketLen;	
			unsigned char xdata diagnoz[12];
							
  
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
unsigned char read_spi_con(unsigned char A1);	
	

//******************************
//
//	 �������� ���������� �����
//
//******************************


/*
void ADC_calculate(void)

	{ // static unsigned long sred;
   	ADC.Byte[1]=1;//ADC0H;
   	ADC.Byte[2]=2;//ADC0M;
   	ADC.Byte[3]=3;//ADC0L;

   	if(ADC_buf_empty)						// ���� ������� ������
       {									// Time_request = 0;
						 start_ADC_buf 		= ADC_buf;		// ������ ������
						 end_ADC_buf 		= ADC_buf;		// ����� ������
						 end_ADC_buf[0]		= ADC.Long;
						 ADC_buf_empty		= OFF;
						 ADC_buf_overflov	= OFF;			// ��������� �������������
						 ADC_srednee		= 0;
       }
    else
       {
					 if(ADC_buf_overflov)
							{
								   ADC_srednee -= start_ADC_buf[0];	         // ����� ?
								   ADC_srednee += ADC.Long;	    // ������� �������� ���
																// sred = ADC_srednee;
																//  sred/=Len_ADC_Buf;		  // 16 �������� � ������
								
									
									if (flag_ocifrovka == pervij)
										{ rrez1 = ADC_srednee;				}
									else
										{ rrez2 = ADC_srednee;
										}
									 flag_ocifrovka = ~flag_ocifrovka;
								  
								   //  ��������� ���������



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
	
	/* SPI0CFG   = 0x40;
    SPI0CN    = 0x01;
    SPI0CKR   = 0x97;		   //  	 0x1D
	IT0 = 1;
	IT1 = 1;*/
	
	
	
    SPI0CFG   = 0x40;
    SPI0CN    = 0x01;
    SPI0CKR   = 0x1D;
}

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

    P0MDOUT   = 0xFD;
    P1MDOUT   = 0xFF;
    P2MDOUT   = 0xFF;
    P3MDOUT   = 0x1F;
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
		IT01CF    = 0xFE;
							
		//IT01CF    = 0x76;
		IE        = 0xE7;

	}
	void start_timer0(void)

    {
									// TMOD |=2;
									// TH0 = 0x10;
      TR0=1;
      ET0=1;
		TR2 = 1;
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
	//	  ������ FLASH
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
	//	 ������ FLASH
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
	//  ����������� Flash ������
	//  ���
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
	//	 �������� �� ��������
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
	//  ����� ������������ ����������
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
	//  ����� 5 �����
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
	//  ����� ���� ���������� .
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
	//  ����� ������� �����
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
	//  ����� ������� % ����
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
	//  ������������� ����������
	//
	//*****************************
	
	
	
	void init_param(void)
		{
			one=0;
			two=0;
			null_4 = 1;
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
		flag_sek = 0;
		msek = 0;
		sek= 0;
		flag_sekunda = 0;	
		Init_Device();
		 TR2 = 1;           // Timer0 enabled
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
							// ��������� ������� �����
								TR2 = 1;
								null_5 = 0;
								null_4 = 0;
								null_3 = 0;
								null_2 = 0;
								flag_sekunda = 1;
								break;
							case 6:
								flag_k = 0;
								// ��� ����� ����������
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
		val = read_spi_con(0x01);
		
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
	//  ����������� �����������
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
	//  ��������� �����
	//
	//*****************************
		
		
	void Timer2 (void) interrupt 5
	{
		TF2H = 0;
		msek++;
			one = 1;
		if (msek==2)
			two = 1;
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
					diagnoz[4] = read_spi_con(0x1c);  // ���-����
					diagnoz[5] = read_spi_con(0x14);  //   RSSI Value bits ��������
					diagnoz[6] = read_spi_con(0x1a);  //  bit 3-1 TXOPVAL<2:0>:Transmit Output Power Value bits (1 step �3dB)   000= 13 dBm  
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
		write_spi_con(0x16, 0x97);	//�� ��������
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
		//�� ����� ������� �� ���� ���������� 0
		SetRFMode_my(RF_TRANSMITTER);
		dia();
		for(i=0;i<255;i++)
		{
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
		}

	while(!(flag_int0));
		flag_int0 = 0;
	//����� ������ �� ���� irq1 ��������� ��� 1
	//     IT01CF    = 0xFE;
    //     IE        = 0x85; ( 0x05) EX1 EX0 )
	//    TCON     (IE1   IE0 )  0 - � ������           1 - � ������

	//
	//
		{
		_nop_();	//�������� ���������� ����� �������� ��������� ���������.
		_nop_();
		_nop_();
		_nop_();
		}

	//	INTCONbits.GIE = 1;   //?
		dia();
		
		
		write_spi_con(0x16, 0x68);
		SetRFMode_my(RF_RECEIVER);	//rfmode = �������
		//Reset FIFO  �������� ����� ����� ��������
		i = read_spi_con(REG_IRQPARAM0);
		write_spi_con(REG_IRQPARAM0, (i | 0x01));

	}	
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

			//write_spi_con(0x0D, (0x0a));	//������������� ����
				i = read_spi_con(REG_IRQPARAM0);
		        write_spi_con(REG_IRQPARAM0, (i | 0x01));
			
			
			return node_adrs;
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

	