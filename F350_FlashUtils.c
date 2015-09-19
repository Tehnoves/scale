//-----------------------------------------------------------------------------
// F350_FlashUtils.c
//-----------------------------------------------------------------------------
// Copyright 2004 Silicon Laboratories, Inc.
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// AUTH: BW & GP
// DATE: 21 JUL 04
//
// Target: C8051F35x
// Tool chain: KEIL C51 7.06
//
// Release 1.0
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
//#include <c8051f410.h>
#include "F350_FlashPrimitives.h"
#include "F350_FlashUtils.h" 
#include <intrins.h>
sbit dclk = P2^6;//P3^5;
sbit din  = P2^5;//P3^6;
sbit load = P2^4;//P3^7;
extern unsigned char idata byte[10];
// int i;
// unsigned char j;
//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
#define SYSCLK       24500000 / 64     // SYSCLK frequency in Hz
// FLASH read/write/erase routines
void FLASH_Write (FLADDR dest, char *src, unsigned numbytes);
char * FLASH_Read (char *dest, FLADDR src, unsigned numbytes);
void FLASH_Clear (FLADDR addr, unsigned numbytes);

// FLASH update/copy routines
void FLASH_Update (FLADDR dest, char *src, unsigned numbytes);
void FLASH_Copy (FLADDR dest, FLADDR src, unsigned numbytes);

// FLASH test routines
void FLASH_Fill (FLADDR addr, ULONG length, UCHAR fill);
void OSCILLATOR_Init (void);
void PORT_Init (void);
void ADC0_ISR (void);
void SPI_ISR (void);
void SPI_Init();
void Timer3_Init (int counts);
void ADC0_Init (void);
#define INT_DEC      256               // integrate and decimate ratio
sfr16 ADC0     = 0xbd;                 // ADC0 data
#define SAMPLE_RATE  50000    
sfr16 TMR3RL   = 0x92;                 // Timer3 reload value
sfr16 TMR3     = 0x94;                 // Timer3 counter
//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FLASH Routines -- no SFLE
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// FLASH_Clear
//-----------------------------------------------------------------------------
//
// This routine erases <numbytes> starting from the FLASH addressed by
// <dest> by performing a read-modify-write operation using <FLASH_TEMP> as
// a temporary holding area.  This function accepts <numbytes> up to
// <FLASH_PAGESIZE>.
//
sbit LED = P2^1;  
sbit LEDZ = P0^7;  
sbit LEDP = P0^6; 
sbit CS = P0^4; 
sbit RST = P0^3; 


char buf[5];
unsigned char ii;
FLADDR re;
/*

void main(void)
{
 long temperature;                   // temperature in hundredths of a
                                       // degree C
   int temp_int, temp_frac;            // integer and fractional portions of
   PCA0MD &= ~0x40;  

/*
FLASH_PageErase(0x1000);
FLASH_ByteWrite (0x1000,0x5a);
FLASH_ByteWrite (0x1001,0x5a);
FLASH_ByteWrite (0x1002,0x5a);
FLASH_ByteWrite (0x1003,0x5a);
FLASH_ByteWrite (0x1004,0x5a);
FLASH_ByteWrite (0x1005,0x5a);
 FLASH_ByteWrite (0x1006,0x5a);
 FLASH_ByteWrite (0x1010,0x5a);
  */
  /*
FLASH_PageErase(0x1000);
FLASH_PageErase(0x1100);
FLASH_PageErase(0x1200);
re = 0x1100;
for (ii=0;ii<255;ii++)
{
  
   FLASH_ByteWrite (re,0x5a);
    re++;
 };
 // PCA0MD    &= ~0x40;
    PCA0MD    &= ~0x40;
   // PCA0MD    = 0x00;
   // PCA0CPL2  = 0x00;
   // PCA0MD    |= 0x20;
//	PFE0CN    = 0x00;
/*
  PCA0MD    &= ~0x40;
    PCA0MD    = 0x00;
    PCA0CPL5  = 0x00;
    PCA0MD    |= 0x20;
  */
  /*
  PORT_Init();
OSCILLATOR_Init (); 
 VDM0CN    = 0x80;

   // for (i = 0; i < 350; i++);  // Wait 100us for initialization
 //  PORT_Init (); 
  
    LEDZ=0;
	LEDP =1;
	 LEDZ=1;
	LEDP =0;
LEDZ=0;
	LEDP =1;
	 LEDZ=1;
	LEDP =0;

	 Timer3_Init (SYSCLK/SAMPLE_RATE);   // initialize Timer3 to overflow at
                                       // sample rate

   ADC0_Init ();                       // init ADC
                         // enable ADC
	SPI_Init();
    
   //...	EIE1=0x08; 
   //...IE        = 0x80;
    IE        = 0xC0;
//...EA = 1;   
//...   AD0EN = 1; 

 //   PCA0MD    = 0x00;
  //  PCA0CPL5  = 0x00;
 //  PCA0MD    |= 0x20;
//	WDTCN = 0xde;                       // disable watchdog timer
 // WDTCN = 0xad;
  CS =0;
 RST =0;
 RST = 1;
  CS =1;
   CS =0;
   
while (1) 
SPI0DAT =0x9f;
{
      EA = 0;                          // disable interrupts
      temperature = result;
      EA = 1;                          // re-enable interrupts

      // calculate temperature in hundredths of a degree
      temperature = temperature - 1444;
      temperature = (temperature * 100L) / 5;
      temp_int = temperature / 100;
      temp_frac = temperature - (temp_int * 100);
//      printf ("Temperature is %+02d.%02d\n", temp_int, temp_frac);

//      LED = SW1;                       // LED reflects state of switch
	}
   FLASH_PageErase (0xf00);
	FLASH_ByteWrite (0xf06,0x01);
	j= FLASH_ByteRead (0xf06);
		FLASH_ByteWrite (0xf06,01);
	j= FLASH_ByteRead (0xf06);
//	FLASH_ByteWrite (0x1001,0x02);
//	FLASH_ByteWrite (0x1003,0x03);
	FLASH_Fill (0xf00,0x30,0xa0);

}
   */


//-----------------------------------------------------------------------------
// FLASH_Update
//-----------------------------------------------------------------------------
//
// This routine replaces <numbytes> from <src> to the FLASH addressed by
// <dest>.  This function calls FLASH_Clear() to handle the dirty work of
// initializing all <dest> bytes to 0xff's prior to copying the bytes from
// <src> to <dest>. This function accepts <numbytes> up to <FLASH_PAGESIZE>.
//	 
/*
void FLASH_Update (FLADDR dest, char *src, unsigned numbytes)
{
  
   FLASH_Clear (dest, numbytes);

   
   FLASH_Write (dest, src, numbytes);  // 2. Write <numbytes> from <src> to <dest>
}	   */



//-----------------------------------------------------------------------------
// FLASH_Write
//-----------------------------------------------------------------------------
//
// This routine copies <numbytes> from <src> to the linear FLASH address
// <dest>.
//
/*
void FLASH_Write (FLADDR dest, char *src, unsigned numbytes)
{
   FLADDR i;

   for (i = dest; i < dest+numbytes; i++) {
      FLASH_ByteWrite (i, *src++);
   }
}
  */
//-----------------------------------------------------------------------------
// FLASH_Read
//-----------------------------------------------------------------------------
//
// This routine copies <numbytes> from the linear FLASH address <src> to
// <dest>.
//	 
/*
char * FLASH_Read (char *dest, FLADDR src, unsigned numbytes)
{
   FLADDR i;

   for (i = 0; i < numbytes; i++) {
      *dest++ = FLASH_ByteRead (src+i);
   }
   return dest;
}
  */
//-----------------------------------------------------------------------------
// FLASH_Copy
//-----------------------------------------------------------------------------
//
// This routine copies <numbytes> from <src> to the linear FLASH address
// <dest>.
//
/*
void FLASH_Copy (FLADDR dest, FLADDR src, unsigned numbytes)
{
   FLADDR i;

   for (i = 0; i < numbytes; i++) {

      FLASH_ByteWrite ((FLADDR) dest+i, FLASH_ByteRead((FLADDR) src+i));
   }
}
  */


//-----------------------------------------------------------------------------
// FLASH_Fill
//-----------------------------------------------------------------------------
//
// This routine fills the FLASH beginning at <addr> with <lenght> bytes.
//
/*
void FLASH_Fill (FLADDR addr, ULONG length, UCHAR fill)
{
   FLADDR i;

   for (i = 0; i < length; i++) {
      FLASH_ByteWrite (addr+i, fill);
   }
} */
//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal 24.5MHz / 64
// oscillator as its clock source.  
//
//-----------------------------------------------------------------------------
/*
void OSCILLATOR_Init (void)
{
   OSCICN    = 0x83;
   /*
    P1        |= 0x02;
    OSCXCN    = 0x47;
    OSCICN    = 0x00;
    CLKSEL    = 0x03;
	 */
		/*



				//int i = 0;
 				//   OSCXCN    = 0x60;
 				//  for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
  				//  while ((OSCXCN & 0x80) == 0);
  				//  OSCICN    = 0x85;
   				// Configure internal oscillator
}	  */
/*
void PORT_Init (void)
{
P0MDOUT   = 0xFF;
    XBR0      = 0x02;
    XBR1      = 0x40;
	
	//...P1MDIN    = 0xFD;
    //...P1SKIP    = 0x02;



   //.. P0MDOUT   = 0xC0;
   //.. XBR1      = 0x40;

  // XBR0     = 0x00;                    // No digital peripherals selected
                   // Enable crossbar and weak pull-ups
  // P2MDOUT |= 0x02;                    // Enable LED as a push-pull output
 // P1MDOUT = 0X08;
 // P0MDOUT = 0X40;
  
}
  */
  /*
void SPI_ISR (void) interrupt 6 using 1
{
	SPIF =0;
	if (TXBMT==0)
	{
	buf[0] = SPI0DAT;
	while (TXBMT==1);
	buf[1] = SPI0DAT;
	while (TXBMT==1);
		buf[2] = SPI0DAT;
		while (TXBMT==1);
			buf[3] = SPI0DAT;
				buf[4] = 	buf[3];
		}
}	*/
 //-----------------------------------------------------------------------------
// ADC0_ISR
//-----------------------------------------------------------------------------
//
// ADC0 end-of-conversion ISR 
// Here we take the ADC0 sample, add it to a running total <accumulator>, and
// decrement our local decimation counter <int_dec>.  When <int_dec> reaches
// zero, we post the decimated result in the global variable <result>.
//

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Configure ADC0 to use Timer3 overflows as conversion source, to
// generate an interrupt on conversion complete, and to use left-justified
// output mode.  Enables ADC end of conversion interrupt. Leaves ADC disabled.
//
void ADC0_Init (void)
{
	//   REG0CN    = 0x00; 					// 2,1V
	ADC0MX    = 0x18;                       // выбрали температуру
        ADC0CF    = 0x76; //76;//ADC0CF    = 0xc0;						// частоты дискретизации
											//	00 одно преобразование
											//	01	4-преобразований
											//	10	8-преобразований
											//	11	16-преобразований
    ADC0CN    = 0x40;						//	АЦП включен + преобразование от timer3
 //   ADC0TK    = 0xfb;  //4c;						// регистр выбора режима слежения
//	REF0CN    = 0x0e;
  	  REF0CN    = 0x37;						// последний вариант !!


    
//... ADC0MX    = 0x18;
  //  ADC0CN    = 0x81;
//    ADC0MX    = 0x18;
 //   ADC0CF    = 0x4E;
  //  ADC0CN    = 0xC1;

 //	  ADC0MX    = 0x18;
 //   ADC0CF    = 0x96;
 //   ADC0CN    = 0xc1;
 //   ADC0TK    = 0xFB;
  										// ADC0CN = 0x05;                      // ADC0 disabled; normal tracking
                                       // mode; ADC0 conversions are initiated 
                                       // on overflow of Timer3; ADC0 data is
                                       // left-justified
  // REF0CN = 0x0e;//e;                      // enable temp sensor, on-chip VREF,
    									//   ADC0TK    = 0xFB;                                    // and VREF output buffer
  										// AMX0SL = 0x0f;                      // Select TEMP sens as ADC mux output
 										//  ADC0CF = 0x80;                      // ADC conversion clock = SYSCLK/16
 										//  ADC0CF |= 0x01;                     // PGA gain = 2

 //  EIE1 |= 0x08;                       // enable ADC interrupts
}

//-----------------------------------------------------------------------------
// Timer3_Init
//-----------------------------------------------------------------------------
//
// Configure Timer3 to auto-reload at interval specified by <counts> (no 
// interrupt generated) using SYSCLK as its time base.
//
void Timer3_Init (int counts)
{

 //  CKCON     = 0x50;
 //   TMR3CN    = 0x24;


  TMR3CN = 0x02;                      // Stop Timer3; Clear TF3;
                                       // use SYSCLK as timebase
   TMR3RL  = -counts;                  // Init reload values
   TMR3    = 0xffff;                   // set to reload immediately
//...   EIE2   &= ~0x01;                    // disable Timer3 interrupts
   TMR3CN |= 0x04;                     // start Timer3
}
/*
void SPI_Init()
{
    SPI0CFG   = 0x40;
    SPI0CN    = 0x01;
    SPI0CKR   = 0x5;
}
  */





	/*
void init(void)
{
byte[0]=0;
byte[1]=0;
byte[2]=0;
byte[3]=0;
byte[4]=0;
byte[5]=0;
dclk=0;
din=0;
load=0;
}	  */
 /*
	   z_new = (z & 0x80);		 		//	 выделили P1.7 
		z_old = (z1 & 0x80);	 		//   выделили сохраненный P1.7
										//   если в порту клавиша нажата 
	 	if ((z_new & 0x80) == 0x0)	 
										   
			{  if ((z_new && 0x80) && (z_old && 0x80))  //  мы ее и не отпускали
				  {_nop_();}	//   ничего не изменилось
				else
				  {_nop_();}	//	 1 --> 0		 мы ее нажали
			}
		else if ((z_new & 0x80) == 0x80)
		 	{	if ((z_new && 0x80) && (z_old && 0x80))
					{_nop_();}//	  ничего не изменилось
				else
					{_nop_();}//	 0 --> 1		 мы ее отпустили
			 }
		z_new = (z & 0x40);		 		//	 выделили P1.6 
		z_old = (z1 & 0x40);	 		//   выделили сохраненный P1.6
	 	if ((z_new && 0x40) == 0x0)	    //   если в порту клавиша нажата 
			{  if ((z_new & 0x40) && (z_old & 0x40))  //  мы ее и не отпускали
					{_nop_();}//   ничего не изменилось
				else
					{_nop_();}//	 1 --> 0		 мы ее нажали
			}
		else if ((z_new & 0x40) == 0x40)
		 	{	if ((z_new && 0x40) && (z_old && 0x40))
					{_nop_();}//	  ничего не изменилось
				else
					{_nop_();}//	 0 --> 1		 мы ее отпустили
			 }
			 z_new = (z & 0x20);		 		//	 выделили P1.5 
		z_old = (z1 & 0x20);	 		//   выделили сохраненный P1.5
	 	if ((z_new && 0x20) == 0x0)	    //   если в порту клавиша нажата 
			{  if ((z_new && 0x20) && (z_old && 0x20))  //  мы ее и не отпускали
					{_nop_();}//   ничего не изменилось
				else
					{_nop_();}//	 1 --> 0		 мы ее нажали
			}
		else if ((z_new && 0x20) == 0x20)
		 	{	if ((z_new && 0x20) && (z_old && 0x20))
					{_nop_();}//	  ничего не изменилось
				else
					{_nop_();}//	 0 --> 1		 мы ее отпустили
			 }
		  z_new = (z & 0x10);		 		//	 выделили P1.4 
		z_old = (z1 & 0x10);	 		//   выделили сохраненный P1.4
	 	if ((z_new && 0x10) == 0x0)	    //   если в порту клавиша нажата 
			{  if ((z_new & 0x10) && (z_old & 0x10))  //  мы ее и не отпускали
					{_nop_();}//   ничего не изменилось
				else
					{_nop_();}//	 1 --> 0		 мы ее нажали
			}
		else if ((z_new && 0x10) == 0x10)
		 	{	if ((z_new & 0x10) && (z_old & 0x0))
					{_nop_();}//	  ничего не изменилось
				else
					{_nop_();}//	 0 --> 1		 мы ее отпустили
			 }		*/


			  // /*

  	// * /

		  
 
   /*
   void ti (void )
  {
   int i1=0;
   addr =0x2000;
   i = 0;
   
   	   		time.cek   = 00;
 			time.min   = 00;
 			time.hour  = 22;
 			time.day   = 18;
 			time.month = 5;
 			time.year  = 7;

 convert(&time);
 rez.rezult[1] = rez1.rezult[1];	  // результат распаковіваем в двойное длинное целое (формат RTClock)
	rez.rezult[0]=rez1.rezult[0];
   while (addr < 0x7c00)
   {
	time1.cek   = 0;			// это 00:10 01.01.07г.
 	time1.min   = 10;
	time1.hour  = 0;
	time1.hour  = 0;
 	time1.day   = 0;
 	time1.month = 1;
 	time1.year  = 7;

	convert(&time1);       // 10 мин перевели в тики в rez1
		   // получили текущее время в тиках в rez
	tmp = rez.rezult[0]>>16;
  	tmp2 = rez.rezult[1]<<16;
    tmp =tmp+tmp2;			   // упаковали в длинное целое
	tmp3= rez1.rezult[0]>>16;
  	tmp2 = rez1.rezult[1]<<16;
    tmp3 =tmp3+tmp2;		   // упаковали в длинное целое
	tmp = tmp + tmp3;		   // получили ожидаемое время на 10 мин вперед
   	rez1.rezult[1] = tmp >> 16;		 // распаковали в два длинных целых
	rez1.rezult[0] = tmp << 16;
   		today(&time1,&rez1);				// тики текущего времени  в rez1
	    rez.rezult[1] = rez1.rezult[1];	  // результат распаковіваем в двойное длинное целое (формат RTClock)
	rez.rezult[0]=rez1.rezult[0];
   			if (i1 < 65)
	    	{  
			zaz.za[i1].da.year  = time1.year;		// 		   текущий год
			zaz.za[i1].da.month = time1.month;		//		   месяц
			zaz.za[i1].da.day   = time1.day;			//		   день месяца
			zaz.za[i1].da.hour  = time1.hour;		//		   время суток
			zaz.za[i1].da.min   = time1.min;			//		   минуты
			zaz.za[i1].da.cek   = time1.cek;			//		   секунды
			zaz.za[i1].te.grad  = i1;			//		   целые градусы
			zaz.za[i1].te.dec   = 0;		//		   дробные градусы
			i1++;
			if (byte[0]<9)
			{byte[0]++;}
			else
			{byte[0]=0;
			byte[1]++;}
			 ekran();
		    }
		else 
		{
		if (addr == 0x7c00)
		 {addr = 6144;
		 byte[2] =0;
		 byte[3]=0;}
		 byte[1]=0;
		 byte[0]=0;
		 if (byte[2]<9)
		 	{byte[2]++;}
		 else
		 	{byte[2]=0;
			byte[3]++;}
				 ekran();
		FLASH_PageErase((FLADDR) addr);        // чистим FLASH страницу 512 байт
		 for (i1=0;i1<512;i1++)
			FLASH_ByteWrite ((FLADDR)addr+i1,zaz.byt[i1]);
 			addr += 0x200;					   // определяем адрес новой страницы
			i1 =0;	
		}
   } 
   }	 		  */
		//  Поведение в конце FLASH

  /*
	  addr =0x7c00-16;
	  while (1)
	  {
	  tdata (addr);
	  if (addr < (0x7c00-8))
	  addr += 8;
	  else
	  	addr = 6144;
	  }
		 //  Поведение в конце FLASH
	   addr =0x7c00-48*2;					 // это одна минута
	  while (1)
	  {
	  tdata (addr);
	  if (addr < (0x7c00-48))
	  addr += 48;
	  else
	  	addr = 6144;
	  }	   */
			//  Поведение в конце FLASH
	  /*
	  addr =0x7c00-1152*2+8;
	  while (1)
	  {
	  tdata (addr);
	  if (addr < (0x7c00-1152))
	  addr += 1152;
	  else
	  {
		addr += 1152;
	  	addr = (addr-0x7c00)+6144;
		}
	  }
	  */
	/*
   i = 0;
   while (addr < 0x7c00)
   {
		for (i2=0;i2<512;i2++)
			{

			zaz.byt[i2] = FLASH_ByteRead (addr+i2);
			}
		addr += 0x200;	
		 i++;
   }
   	  addr =0x7c00-0x200;
   i = 0;
   while (addr > (6144-0x200))
   {
		for (i2=0;i2<512;i2++)
			{

			zaz.byt[i2] = FLASH_ByteRead (addr+i2);
			}
		addr -= 0x200;	
		 i++;
   } 	*/	
/* 
   addr =6144;
   i = 0;
   while (addr < 0x7c00)
   {
   			for (i2=0;i2<512;i2++)
			{
				if (((i2+1) % 8) == 0)
				zaz.byt [i2] = i2 /8;
				else if (((i2+2)% 8) ==0)
				zaz.byt[i2] = i;
				else 
				zaz.byt[i2] = 55;
			}
			 FLASH_PageErase((FLADDR) addr);
		for (i2=0;i2<512;i2++)
			{
				FLASH_ByteWrite ((FLADDR)addr+i2,zaz.byt[i2]);
			}
		addr += 0x200;	
		 i++;
   }  */ 

   /*
		 void ti (void )
  {
   int i1=0;
   addr =6144;
   i = 0;
   
   	   		time.cek   = 00;
 			time.min   = 00;
 			time.hour  = 20;
 			time.day   = 17;
 			time.month = 4;
 			time.year  = 8;

 convert(&time);
 rez.rezult[1] = rez1.rezult[1];	  // результат распаковіваем в двойное длинное целое (формат RTClock)
	rez.rezult[0]=rez1.rezult[0];
   while (addr < 0x7c00)
   {
	time1.cek   = 0;			// это 00:10 01.01.07г.
 	time1.min   = 10;
	time1.hour  = 0;
	time1.hour  = 0;
 	time1.day   = 0;
 	time1.month = 1;
 	time1.year  = 7;

	convert(&time1);       // 10 мин перевели в тики в rez1
		   // получили текущее время в тиках в rez
	tmp = rez.rezult[0]>>16;
  	tmp2 = rez.rezult[1]<<16;
    tmp =tmp+tmp2;			   // упаковали в длинное целое
	tmp3= rez1.rezult[0]>>16;
  	tmp2 = rez1.rezult[1]<<16;
    tmp3 =tmp3+tmp2;		   // упаковали в длинное целое
	tmp = tmp + tmp3;		   // получили ожидаемое время на 10 мин вперед
   	rez1.rezult[1] = tmp >> 16;		 // распаковали в два длинных целых
	rez1.rezult[0] = tmp << 16;
   		today(&time1,&rez1);				// тики текущего времени  в rez1
	    rez.rezult[1] = rez1.rezult[1];	  // результат распаковіваем в двойное длинное целое (формат RTClock)
	rez.rezult[0]=rez1.rezult[0];
   			if (i1 < 65)
	    	{  
			zaz.za[i1].da.year  = time1.year;		// 		   текущий год
			zaz.za[i1].da.month = time1.month;		//		   месяц
			zaz.za[i1].da.day   = time1.day;			//		   день месяца
			zaz.za[i1].da.hour  = time1.hour;		//		   время суток
			zaz.za[i1].da.min   = time1.min;			//		   минуты
			zaz.za[i1].da.cek   = time1.cek;			//		   секунды
			zaz.za[i1].te.grad  = i1;			//		   целые градусы
			zaz.za[i1].te.dec   = 0;		//		   дробные градусы
			i1++;
			if (byte[0]<9)
			{byte[0]++;}
			else
			{byte[0]=0;
			byte[1]++;}
		//	 ekran();
		    }
		else 
		{
		if (addr == 0x7c00)
		 {addr = 6144;
		 byte[2] =0;
		 byte[3]=0;}
		 byte[1]=0;
		 byte[0]=0;
		 if (byte[2]<9)
		 	{byte[2]++;}
		 else
		 	{byte[2]=0;
			byte[3]++;}
			//	 ekran();
		FLASH_PageErase((FLADDR) addr);        // чистим FLASH страницу 512 байт
		 for (i1=0;i1<512;i1++)
			FLASH_ByteWrite ((FLADDR)addr+i1,zaz.byt[i1]);
 			addr += 0x200;					   // определяем адрес новой страницы
			i1 =0;	
		}
   } 
   }	*/