/////////////////////////////////////
//  Проект весы для Андреевича     //
//  процессор F314
//  03.01.15 старт
/////////////////////////////////////

#include "compiler_defs.h"
#include "C8051F310_defs.h"
#include <stdio.h>
#include <string.h>
#include <intrins.h> 
#include <math.h>
#include <stdlib.h>



// Peripheral specific initialization functions,
// Called from the Init_Device() function

 unsigned char  idata ii,jj;
 unsigned char idata yt;
 unsigned char code shift[8] =  {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
 unsigned char code codtabl[18]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
                                0x7f,0x6f,0x40,0x00,0x71,0x71,0x71,0x71,
                                0x71,0x71 };

 unsigned char  pdata  regen[8] = {'1','2','3','4','5','6','7','8'};
 
 
 
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

void main(void)
	{
			PCA0MD &= ~0x40; 
			Init_Device();
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