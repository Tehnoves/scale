/////////////////////////////////////
//  ������ ���� ��� ����������     //
//  ��������� F314
//  03.01.15 �����
//  04.01.15 ���� 
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
#define ON              0x01
#define OFF             0x00 
#define pervij		   0x00
#define vtoroi		   0x01
// Peripheral specific initialization functions,
// Called from the Init_Device() function
#define diskret 4

unsigned int pr,tmp3;
unsigned char tmp2,half;


union
	{
  unsigned long Long;
  unsigned char Byte[4];
	} xdata ADC;
	
	
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
   {  char ch[4];
   long lo;
   };
 int xdata tara;
 
 
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
 unsigned char xdata tek_ves1[8],tek_ves2[8],tek_ves3[8],zad_ves1[8],zad_ves2[8],zad_ves3[8],granica;

 struct constanta 
   {
	union Crr2 zad_ves1_int;		  // �����
	union Crr2 zad_ves2_int;		  // �����
	union Crr2 zad_ves3_int;		  // �����
	union zap dad1;					  // ������� �����
	union zap dad2;					  // 
	union zap dad3;					  // ������� �����
    union zap dad4;					  // ������� �����
	union zap dad5;					  // ������� �����
	union zap dad6;					  // ������� �����
	union zap zero1;				  // ������� �����
	union zap zero2;				  // ������� �����
	union zap zero3;				  // ������� �����
    union zap zero4;				  // ������� �����
	union zap zero5;				  // ������� �����
	union zap zero6;				  // ������� �����
    union Crr2 cell_weight1;		  // �����
	union Crr2 cell_weight2;		  // �����
	union Crr2 cell_weight3;		  // �����
	union Crr2 cell_weight4;		  // �����
    union Crr2 cell_weight5;		  // �����
	union Crr2 cell_weight6;		  // �����

	union Crr2 max_weight1;			 // �����
    union Crr2 max_weight2;			 // �����
	union Crr2 max_weight3;			 // �����
	union Crr2 max_weight4;			 // �����
	union Crr2 max_weight5;			 // �����
    union Crr2 max_weight6;			 // �����

     union zap avto_null1;				 // ������� �����
	union zap avto_null2;				 // ������� �����
	union zap avto_null3;				 // ������� �����
	union zap avto_null4;				 // ������� �����
     union zap avto_null5;				 // ������� �����
	union zap avto_null6;				 // ������� �����
	
  };
 struct constanta coc;


 union global 
	{ struct constanta co;
	  char con[sizeof(coc)];
	};


 union global xdata AA;

 FLADDR xdata addr; 
 unsigned char  idata ii,jj;
 unsigned char idata yt;
 unsigned char code shift[8] =  {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
 unsigned char code codtabl[18]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
                                0x7f,0x6f,0x40,0x00,0x71,0x71,0x71,0x71,
                                0x71,0x71 };

 unsigned char  pdata  regen[8] = {'1','2','3','4','5','6','7','8'};
 
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
//	 �������� ���������� �����
//
//******************************



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
void Timer_Init()
{
											//TMOD      = 0x01;
											//TMR2CN    = 0x28;
    TMOD      = 0x01;
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
	//	  ������ FLASH
	//
	//********************************

	void init_write(void)
		{  
		char I;
   
		//AA.co.zad_ves1_int.Int = 500;
 		point(AA.co.zad_ves1_int.Int,&zad_ves1);
 		//AA.co.zad_ves2_int.Int = 120;
 		point(AA.co.zad_ves2_int.Int,&zad_ves2);		   
 		//AA.co.zad_ves3_int.Int = 50;
 		point(AA.co.zad_ves3_int.Int,&zad_ves3);
 		/*
 		AA.co.dad1.lo = 2793314;
		AA.co.zero1.lo =243606;
		AA.co.dad2.lo = 2788880;
		AA.co.zero2.lo =278228;
		AA.co.dad3.lo = 2836009;
		AA.co.zero3.lo =254833;
		AA.co.dad4.lo = 12931138;
		AA.co.zero4.lo =1222894;
		AA.co.dad5.lo = 12958486;
		AA.co.zero5.lo =1365327;
		AA.co.dad6.lo = 12925383;
		AA.co.zero6.lo =1358715;
		  */

		FLASH_PageErase((FLADDR) addr);
		for (I=0;I<sizeof(AA);I++)
				FLASH_ByteWrite ((FLADDR)addr+I,AA.con[I]);

	}	
	
	
	//**********************
	//
	//
	//	 ������ FLASH
	//
	//
	//**********************
	
	
void init_read(void)
		{
		char i;
		for (i=0;i<sizeof(AA);i++)
   				AA.con[i]=FLASH_ByteRead ((FLADDR)addr+i);

		//AA.co.zad_ves1_int.Char[0]=FLASH_ByteRead ((FLADDR)addr);
		//AA.co.zad_ves1_int.Char[1]=FLASH_ByteRead ((FLADDR)addr+1);
 		point(AA.co.zad_ves1_int.Int,&zad_ves1);
		//AA.co.zad_ves2_int.Char[0]=FLASH_ByteRead ((FLADDR)addr+2);
		//AA.co.zad_ves2_int.Char[1]=FLASH_ByteRead ((FLADDR)addr+3);

        point(AA.co.zad_ves2_int.Int,&zad_ves2);
		//AA.co.zad_ves3_int.Char[0]=FLASH_ByteRead ((FLADDR)addr+4);
		//AA.co.zad_ves3_int.Char[1]=FLASH_ByteRead ((FLADDR)addr+5);

 		point(AA.co.zad_ves3_int.Int,&zad_ves3);


	//	point(AA.co.zad_ves3_int.Int,&zad_ves3);

	//	cell_weight1;

	//	  	cell_weight = 500;
	//	sprintf(cell_weight_char,"%#4.4u",AA.co.cell_weight1); 
		}
	
unsigned int zn(unsigned int izm)
			{
				unsigned char tmp;
				tmp = izm % diskret;
			if  (tmp >= half)
				return ( izm + diskret - tmp);
			else 
				 return	( izm - tmp); 
			}
		
	
	
void main(void)
	{
			PCA0MD &= ~0x40; 
		
		  pr = 12340;
		half = diskret / 2;
		while (1)
		{
			tmp3 = zn(pr);
			pr++;
		}
			Init_Device();
			addr=0x7c00-512;
			
			while (1); 
				{
					_nop_();
				}
			
			
	}
	void Timer0 (void) interrupt 1

   {   
   		
     TR0=0;
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
	 	 P2=codtabl[ii];
     TR0=1;

   }