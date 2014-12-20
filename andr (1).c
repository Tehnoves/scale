 //***************************************
//
//	œÓÂÍÚ ‚ÂÒ˚ ‰Îˇ ¿Ì‰ÂÂ‚Ë˜‡
//	07.05.14
//	29.05.14		—¡Œ– ¿ Ã¿ ≈“¿
//
//***************************************



#include "compiler_defs.h"
#include "C8051F350_defs.h"



//#include <c8051f350.h>                       // SFR declarations
#include <stdio.h>
#include <string.h>
#include <intrins.h> 
#include <math.h>
#include <stdlib.h>
#define  RD          0x00 
#define  RD2        0x04 
#define  WR1       0x01 
#define  WR2        0x02 
#define  NU        0x08 





		//register description
    #define GCONREG     0x00
    #define DMODREG     0x01
    #define FDEVREG     0x02
    #define BRREG       0x03
    #define FLTHREG     0x04
    #define FIFOCREG    0x05	
    #define R1CNTREG    0x06
    #define P1CNTREG    0x07
    #define S1CNTREG    0x08
    #define R2CNTREG    0x09
    #define P2CNTREG    0x0a
    #define S2CNTREG    0x0b
    #define PACONREG    0x0c
    #define FTXRXIREG   0x0d
    #define FTPRIREG    0x0e
    #define RSTHIREG    0x0f
    #define FILCONREG   0x10
    #define PFILCREG    0x11
    #define SYNCREG     0x12
    #define RESVREG     0x13
    #define RSTSREG     0x14
    #define OOKCREG     0x15
    #define SYNCV31REG  0x16
    #define SYNCV23REG  0x17
    #define SYNCV15REG  0x18
    #define SYNCV07REG  0x19
    #define TXPARAMREG  0x1a
    #define CLKOUTREG   0x1b
    #define PLOADREG    0x1c
    #define NADDREG     0x1d
    #define PCONREG     0x1e
    #define FCRCREG     0x1f
		sbit P03 = P0^3;   
		sbit P01 = P0^1;   
		sbit P02 = P0^2;  
		sbit IRQ0 = P0^6;    
		sbit IRQ1 = P0^7;  
		sbit reset = P1^0;
		sbit CSCON= P1^3;
		sbit CSDAT = P1^2;
 
  bit cs_con = 1;
  bit cs_dat = 1;
  bit one,two;

unsigned char xdata A2,A3;
unsigned char xdata command,val;
unsigned char xdata msek,i;

 void Timer_Init()
{
    TMOD      = 0x20;
    CKCON     = 0x01;
    TH1       = 0x60;
}

void UART_Init()
{
    SCON0     = 0x10;
}

void SPI_Init()
{
   // SPI0CFG   = 0x40;
   // SPI0CN    = 0x01;
   // SPI0CKR   = 0x1D;

	  SPI0CFG   = 0x40;
    SPI0CN    = 0x01;
    SPI0CKR   = 0x79;


}

void ADC_Init()
{
    ADC0MD    = 0x80;
    ADC0BUF   = 0x40;
    ADC0MUX   = 0x08;
}

void Port_IO_Init()
{
   

// This example is intended to be used with the SPI0_Slave example.
//
// Pinout:
//
// P0.0 - SPI SCK    (digital output, push-pull)
// P0.1 - SPI MISO   (digital input, open-drain)
// P0.2 - SPI MOSI   (digital output, push-pull)
// P0.3 - SPI NSS    (digital output, push-pull)
//
// P0.6 - LED        (digital output, push-pull)


  // P0.0  -  SCK  (SPI0), Push-Pull,  Digital
    // P0.1  -  MISO (SPI0), Open-Drain, Digital
    // P0.2  -  MOSI (SPI0), Push-Pull,  Digital
    // P0.3  -  Unassigned,  Open-Drain, Digital
    // P0.4  -  Unassigned,  Open-Drain, Digital
    // P0.5  -  Unassigned,  Open-Drain, Digital
    // P0.6  -  Unassigned,  Push-Pull,  Digital
    // P0.7  -  Unassigned,  Push-Pull,  Digital

    // P1.0  -  Unassigned,  Push-Pull,  Digital
    // P1.1  -  Unassigned,  Open-Drain, Digital
    // P1.2  -  Unassigned,  Push-Pull,  Digital
    // P1.3  -  Unassigned,  Push-Pull,  Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  Unassigned,  Open-Drain, Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital
        P0MDOUT   = 0xD7;
    P1MDOUT   = 0x0F;
    P0SKIP    = 0x01;
    XBR0      = 0x02;
    XBR1      = 0x40;

/*	P03 =0;
	P03 =1;
	 P01 =0;
	 P01 = 1;
	 P02 = 0;
	 P02 =1;

		 */
    


}

void Oscillator_Init()
{
    OSCICN    = 0x83;
}

void Interrupts_Init()
{
   

	   EIE1      = 0x08;
    IT01CF    = 0x76;
    IE        = 0xE5;


}

 void Timer2_Init (void)
	{
	  
	   CKCON     = 0x10;
      TMR2CN    = 0x04;

	   	

  						//  TMR2CN    = 0x04;
	TMR2L     = 0x06d;  //0x4a;   ///0x3e;
	TMR2H     = 0xf6;  //0xa0;   //0X50;			 // b
	TMR2RLH   = 0xf6;  //0X50;
	TMR2RLL   = 0x6d;  //0X3e;   
	     
	
	}
// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    Port_IO_Init();

    Timer_Init();
    UART_Init();
    SPI_Init();
    ADC_Init();
   
    Oscillator_Init();
   Timer2_Init();
    Interrupts_Init();
	 one = 0;
	 two = 0;
	 msek = 0;
	 TR2 = 1;           // Timer0 enabled
	// reset
	reset = 1;
	while (!one);
	// 100ucek 1
	reset = 0;
	while (!two);
	//  5 mcek 0
	_nop_();
	 TR2 = 0;

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
			SPI0DAT = 0xff;
			while (command != NU);
			A3 =  SPI0DAT;
		   	CSCON = 1;
		return (A3);
	}





void main(void)
	{
		PCA0MD &= ~0x40; 
		Init_Device();
			CSCON = 0;
		   	CSCON = 1;
			CSDAT  =0;
			CSDAT  =1;


	//	while (1);
		command = NU;
		i =0;
		  while (i <0x20)
		  {

		// write_spi_con(0x01,0x48);
		val = read_spi_con(i++);
			val = read_spi_con(0x02);
		}
		write_spi_con(0x01,0x21);
			write_spi_con(0x10,0x21);



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
						   CSCON = 1;
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


			  EX0 = 0;                        // Clear the SPIF flag
		}
		void irq1_int(void) interrupt 2
	 	{


			  EX1 = 0;                        // Clear the SPIF flag
		}
