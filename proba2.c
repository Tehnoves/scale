#include "compiler_defs.h"
#include "C8051F350_defs.h"
#define temperatura     0x01
#define cell            0x02
#define pervij		    0x00
#define vtoroi		    0x01
#define Len_ADC_Buf 8   //16  
#define ON              0x01
#define OFF             0x00 
#define AD0ISEL		    0x04
#define AD0VREF		    0x02
#define AD0PL		    0x04
#define AD0BPHE		    0x07
#define AD0BPLE		    0x06



void ADC_Init()
{
   //вариант измерения температуры	

  //   REF0CN    = 0x03;
 
	
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
	//ADC0CF  |= (1<<AD0VREF);
	ADC0CF &=  ~(1<<AD0VREF);
	
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
	//  ADC0MUX   = 0x78;     // датчик темпера    0xF8
		
		
		
		
		
	//ADC0BUF   = 0x40;
							// AD0BPHE		    0x07
							// AD0BPLE		    0x06
											// Биты5-4:AD0BPS: Настройка буферов для положительного канала. 
							// 00 = Буферы не используются, т.е. зашунтированы накоротко(по умолчанию). 
							// 01 = Используется«младший» буфер. 
							// 10 = Используется«старший» буфер. 
							// 11 = Зарезервировано.
				
				
	
	//    370 ~ 50Hz 
	ADC0CLK   = 0x09;
	ADC0DECH  = 0x07;
    ADC0DECL  = 0x7f;
	
	
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
float xdata temper,temper2;		
 bit ADC_buf_overflov;
  bit ADC_buf_empty=1,flag_adc; 
 unsigned char xdata sel; 
   bit flag_ocifrovka,flag_ocifrovka_temper; 
 unsigned long xdata ADC_buf[Len_ADC_Buf];
	unsigned long xdata *start_ADC_buf;
	unsigned long xdata *end_ADC_buf;
    long cell_long,cell_long2;                                    // xdata
	unsigned long xdata ADC_srednee; 
  unsigned char ind;
   unsigned char xdata jta;
  struct pac 
	 {
		 char comm;
		 long ves;
		 long temp;
		 float v;
		
	 };
	 
union pack
	{
  struct pac var;
  unsigned char Byte[sizeof(struct pac)];
	} xdata packet,packet2;	 
	unsigned char as;
 void otv(void)
		{
		//	P04=1;
		//	char ii;
			if (flag_adc)      // оцифровка произошла нужно обработать
			{
				as=REF0CN;
					//if (!(ind%0xf))
						if (sel == temperatura)
						{ //sel = temperatura;
						 if (flag_ocifrovka_temper == pervij)
																											//if (sel== temperatura)
														{ temper = 	((float)(ADC.Long)*2450/0x7fffff-54.300)/.205 ;}								//  ((float)(ADC.Long)*2450/0x7fffff-54.300)/.205 ;}
																											//else
																											//	{ cell_long = ADC_srednee/16;}
												else
																									//	if (sel== temperatura)
														{ temper2 = ((float)(ADC.Long)*2450/0x7fffff-54.300)/.205 ;;                      //((float)(ADC.Long)*2450/0x7fffff-54.300)/.205 ;//
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
																{ cell_long = ADC_srednee/8;
																									//ves =(cell_long-2481000);		
																									//ves =500*ves/29600;
																									//ves =(cell_long-3528185);		
																									//ves =ves/84;
																}
														else
																									//if (sel== temperatura)
																									//	{ temper2 = ((float)(ADC_srednee/16)*2450/0x7fffff-54.300)/.205 ;
																									//	}
																									//else
																{ cell_long2 = ADC_srednee/8;
																									//ves2 =(cell_long2-2481000);		
																									//ves2 =500*ves2/29600;
																									//			ves2 =(cell_long2-3528185);		
																									//ves2 =ves2/84;
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
					ind++;
					if (!(ind%0xf))
						{ sel = temperatura;
						  ADC0CN  |=  (1<<AD0PL);//ADC0CN  |=  (1<<AD0PL);	// биполярный
						  ADC0MUX   = 0xff;    	// датчик температуры  0xf8
						}
					else
						{ sel =  cell;
						  ADC0MUX   = 0x78; // тензодатчик 78 58  0x58
							ADC0CN  &= ~(1<<AD0PL);	 	// биполярный		 ADC0CN  |=  (1<<AD0PL);			//ADC0CN  &= ~(1<<AD0PL);
						}   
				
				//	ADC0MD=0x82;		// пока не пришел запрос продолжаем оцифровку	
										//	PCON=1;
					flag_adc = 0;  //  оцифровка обработана   
			}
			jta++;
				EA = 0;
				if (flag_ocifrovka == pervij)
					{packet.var.ves=cell_long2;}
				else
					{packet.var.ves=cell_long;}
				if (flag_ocifrovka_temper == pervij)
					{packet.var.temp=temper2;}
				else
					{packet.var.temp = temper;}	
				EA = 1;
				ADC0MD=0x82;													//packet.var.comm = 3;	
																	//packet.var.v = 0.0;
																	//for (ii =0; ii < sizeof(packet);ii++)
																	//    TxPacket[ii] = packet.Byte[ii];   // ??????
																			
																	//TxPacket[i]
	//	P04 =0;	
		}





void main(void)
{
	PCA0MD &= ~0x40; 
	Init_Device();
			ADC0MD=0x81;
	flag_adc=0;
	while (1)
		if (flag_adc)
		{
		otv();
	flag_adc=0;
		}
}


void  ADC_convert(void) interrupt 10
		{
					ADC.Byte[0]=0;
				ADC.Byte[1]=ADC0H;
				ADC.Byte[2]=ADC0M;
				ADC.Byte[3]=ADC0L;
					flag_adc = 1; 
													//ADC_calculate();
		
				AD0INT =0;	 		// если флаг установлен то преобразование закончено
									// его нужно сбросить программно
											//otv();							
			//	ADC0MD=0x82;		// пока не пришел запрос продолжаем оцифровку			 
		//	 temper = 	  ((float)(ADC.Long)*2450/0x7fffff-54.300)/.205 ;
				//	ADC0MD=0x82;	
				//P04 =~P04;					
		}