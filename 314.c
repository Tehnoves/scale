
/////////////////////////////////////
//  Проект весы для Андреевича     //
//  процессор F314
//  03.01.15 старт
//  04.01.15 флэш 
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

#define ON              0x01
#define OFF             0x00 
#define pervij		   0x00
#define vtoroi		   0x01

// Peripheral specific initialization functions,
// Called from the Init_Device() function


#define diskret 4

bit left_old,left_new,leftchench;
bit right_old,right_new,rightchench;
bit tara_old,tara_new,tarachench;
bit null_old,null_new,nullchench;


char code baza[8][5]= {
		{'0','2','0','0','0'},
		{'0','3','0','0','0'},
		{'0','5','0','0','0'},
		{'1','0','0','0','0'},
		{'1','5','0','0','0'},
		{'2','0','0','0','0'},
		{'3','0','0','0','0'},
		{'5','0','0','0','0'}		
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
	
unsigned int pr,tmp3;
unsigned char tmp2,half;
char index;

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
sbit left = P3^2;
sbit tara = P3^3;
sbit nul = P3^4;



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
	unsigned long xdata ADC_buf[Len_ADC_Buf];
	unsigned long xdata *start_ADC_buf;
	unsigned long xdata *end_ADC_buf;

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

	struct cons
		{ 
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

  
 struct cons var_cons;


 union global 
	{ struct cons co1;
	  char con[sizeof(var_cons)];
	};


 union global xdata AA;

 FLADDR xdata addr; 
 unsigned char  idata ii,jj;
 unsigned char idata yt;
 unsigned char code shift[8] =  {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
 unsigned char code codtabl[18]={~0xf6,~0x30,~0xe5,~0xf1,~0x33,~0xd3,~0xd7,~0x70,
                                ~0xf7,~0xf3,~0x02,~0x40,~0x20,~0x10,~0x80,~0x04,
                                ~0x01,~0x08 };

 unsigned char  pdata  regen[5] = {'1','2','3','4','5'};
 
 void point(int f, char xdata *hu)
	 	{
			
		int t;
			hu[0] = '<';
			if (f >= 0)
				hu[0] = ' ';
			else 
				hu[0] = '-';
				f = abs(f);
			t = f / 100;
				hu[1] = t + '0';
		    t = f % 100;
        	t = t / 10;
			hu[2] = t + '0';
            t = f % 100;
        	t = t % 10;
			hu[3] = t+'0';
            hu[4] = 0;
      	} 

//******************************
//
//	 наверное скользящее целое
//
//******************************



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
void Timer_Init()
{
											//TMOD      = 0x01;
											//TMR2CN    = 0x28;
   // TMOD      = 0x01;
   // TMR2CN    = 0x28;
	//    TMOD      = 0x01;
    //CKCON     = 0x01;
   // TMR2CN    = 0x28;
	    TMOD      = 0x02;
    CKCON     = 0x01;
    TMR2CN    = 0x28;
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
									//IT01CF    = 0x76;
									//IE        = 0xC7;
	IT01CF    = 0x76;
    IE        = 0x82;
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
		
   		AA.co1.npv =20000;
		AA.co1.nmpv =100;
		AA.co1.diskreta =10;
		
		for (I=0;I<sizeof(AA);I++)
				FLASH_ByteWrite ((FLADDR)addr+I,AA.con[I]);
		}	
	
	
	//**********************
	//
	//	 чтение FLASH
	//
	//**********************
	
	
	void init_read(void)
		{
		char i;
			
		for (i=0;i<sizeof(AA);i++)
   				AA.con[i]=FLASH_ByteRead ((FLADDR)addr+i);
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

void test(void)
	{
		char i,j=0x3a;
		for (i=0;i<10;i++)
				{
					start(j+i);
					delay(40);
				}
				/*
					start(0x3b);
					delay(40);
					start(0x3c);
					delay(40);
					start(0x3d);
					delay(40);
					start(0x3e);
					delay(40);
					start(0x3f);
					delay(40);
					start(0x40);
					delay(40);
					start(0x41);
					delay(40);
				*/
					regen[0] ='1';
		regen[1] ='2';
		regen[2] ='3';
		regen[3] ='4';
		regen[4] ='5';
		delay(40);
		delay(40);
		delay(40);
		delay(40);
	}
	
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
	void init_param(void)
		{null_4 = 1;
		null_3 = 1;
		null_2  = 1;
		//copi(3);
		index =0;
		regen[0] ='1';
		regen[1] ='2';
		regen[2] ='3';
		regen[3] ='4';
		regen[4] ='5';
		P2= 0xff;		P13 = 1;
		P14 = 0;
		P15 = 0;
		P16 = 0;
		P17 = 0;
		Init_Device();
		addr=0x1E00+400;  
		//init_write();
		AA.co1.npv =0;
		AA.co1.nmpv =0;
		AA.co1.diskreta =0;
		left_old=1;left_new=1;leftchench=0;
		right_old=1;right_new=1;rightchench=0;
		tara_old=1;tara_new=1;tarachench=0;
		null_old=1;null_new=1;nullchench=0;
		index = 0;
		}	
	void tes(void)
		{
			right_new = right;
			if (right_old != right_new) 
				{
					if (!right_new)
						{
							if (++index > 9 )          //7
								index = 0;
							copi_dis(index);
						}
						right_old = right_new;
				}
			left_new = left;
			if (left_old != left_new) 
				{
					if (!left_new)
						{
							if (--index < 0 )
								index = 9;           //
							copi_dis(index);
						}
						left_old = left_new;
				}	
		}
	
	
	
void main(void)
	{
		PCA0MD &= ~0x40;
		_nop_();
		init_param();
		// pr = 12340;
		//half = diskret / 2;
		
		//init_read();
		/*
				P13 = 0;
		P14 = 0;
		P15 = 0;
		P16 = 0;
		P17 = 1;
			while (1)
			{	
				P2= 0xff;
				P2 = codtabl[0];
				P2 = codtabl[1];
				P2 = codtabl[2];
				P2 = codtabl[3];
				P2 = codtabl[4];
				P2 = codtabl[5];
				P2 = codtabl[6];
				P2 = codtabl[7];
				P2 = codtabl[8];
				P2 = codtabl[9];
				
				
			}
			*/
			{
			//tmp3 = zn(pr);
			//pr++;
			}
		//addr=0x1E00+400;              	// конец памяти или последний 512-байтный блок
									// в него будем писать константы
									//
									// мах вес НВП             дискрет                             1/2_дискрета         ADC_одного дискрета  NULL
									//
									// long    unsign char     unsigned char                        unsigned char       long                 long
			//copi(index);						//
			copi_dis(index);
		while (1)
				{
					tes();
					_nop_();
					
				}
			
			
	}
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
				//if (null_4)
					P2= codtabl[regen[4] - 0x30];
				//else
				//	P2 = 0xff;
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
			
			/*
			 if (jj==8) jj=1;
			   else jj++;
			yt = regen[jj-1];
													//ii=0;
			if (yt == '-')
				{ii = 10;}
			else if 	(yt == '+')
				{ii = 11;}
			else
				{ii = yt-0x30;}
													//while ((regen[jj-1] != codex[ii]) & (ii < 18)) ii++;

			 P2=0;
			 P0=shift[jj-1];
													//P0 = shift[ii];
			

			 if ((jj ==7) | (jj == 3))
				   P2=codtabl[ii] +0x80;
			 else
				 P2=codtabl[ii]; */
			 TR0=1;

		}