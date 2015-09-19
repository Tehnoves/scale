#include "compiler_defs.h"
#include "C8051F350_defs.h"
void ADC_Init()
{
      /*    ADC0MD    = 0x80;
    ADC0CLK   = 0x09;
    ADC0DECH  = 0x01;
    ADC0DECL  = 0x7D;
    ADC0MUX   = 0xFF;
	*/
	
 ADC0CN    = 0x00;
    ADC0MD    = 0x80;
    ADC0CLK   = 0x09;
    ADC0MUX   = 0x78;
	
}

void Oscillator_Init()
{
    OSCICN    = 0x83;
}

void Interrupts_Init()
{
    EIE1      = 0x08;
    IE        = 0x80;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    ADC_Init();
    Oscillator_Init();
    Interrupts_Init();
}
	union
	{
	  unsigned long Long;
	  unsigned char Byte[4];
	} xdata ADC;
float temper;


void delay(unsigned char y)
		{	
			unsigned char i1,ii1;
			for (ii1=0;ii1<y;ii1++)
			{
				for(i1=0;i1<200;i1++)
				{
					_nop_();
				}
			}	
		}

void main(void)
{
	PCA0MD &= ~0x40; 
	Init_Device();
			ADC0MD=0x81;
	while (1);
	
}


void  ADC_convert(void) interrupt 10
		{
			ADC.Byte[0]=0;
				ADC.Byte[1]=ADC0H;
				ADC.Byte[2]=ADC0M;
				ADC.Byte[3]=ADC0L;
			
													//ADC_calculate();
		
				AD0INT =0;	 		// если флаг установлен то преобразование закончено
									// его нужно сбросить программно
											//otv();							
			//	ADC0MD=0x82;		// пока не пришел запрос продолжаем оцифровку			 
			 temper = 	  ((float)(ADC.Long)*2450/0x7fffff-54.300)/.205 ;
		//	 delay(100);
					ADC0MD=0x82;	
				//	PCON = 1;
			_nop_();
				//P04 =~P04;					
		}