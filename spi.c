/* 
 * File:   spi.c
 * Author: РљРѕР·С‹СЂРµРІ РЎ.Рђ.
 *  18.12.13 d:\1\spi
 *
 * 
 * РЅР°РїРёСЃР°РЅРёРµ РїРѕРґ MPLAB X8C РЅР° 16F1847 
 *  , PICkit3
 * СЂР°Р±РѕС‚Р° СЃ РїСЂРёРµРјРѕРїРµСЂРµРґР°С‚С‡РёРєРѕРј MRF89XA
 * 
 * 
 * V1.0 01.06.14
 *
 * RB0 (7) IRQ     - in IOC
 * RB2 (8) SDI2
 * RB3 (9) IRQ     - in IOC
 * RB5 (11) SCK2
 * RA0 (17) SDO2
 * RA7 (16) RESET  - OUT(IN)
 * RA2 (1) SCON    - OUT
 * RA3 (2) SDAT    - OUT
 * 16.09.14
 * 23.09.14
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h> // include standard header file
#include "MRF89XA.h"


// set Config bits
#pragma config FOSC=INTOSC, PLLEN=OFF, WDTE=OFF, MCLRE=ON,
#pragma config CLKOUTEN=OFF, IESO=OFF, FCMEN=OFF,CP=OFF, CPD=OFF,BOREN=OFF
#pragma config WRT=OFF,STVREN=ON,BORV=LO,LVP=OFF

		// set Config bits РґР»СЏ 16F886
		//.#pragma config FOSC=INTOSC, PLLEN=OFF, WDTE=OFF, MCLRE=ON,
		//#pragma config CLKOUTEN=OFF, IESO=OFF, FCMEN=OFF,CP=OFF, CPD=OFF,BOREN=OFF
		
		
		// РґР»СЏ 16F1823
		//
//#pragma config CP=OFF,CPD=OFF,BOR4V=BOR40V,WDTE=OFF
//#pragma config WRT=OFF,DEBUG=ON,LVP=OFF,CPD=OFF,FOSC=  INTRC_NOCLKOUT, MCLRE =	ON //BORV=LO,STVREN=ON

		//  РѕСЃС‚Р°С‚РєРё РѕС‚ 16F1822 Microchip 14f_1822_ADC
		//__CONFIG    _CONFIG1, _LVP_OFF & _FCMEN_ON & _IESO_OFF & _BOR_OFF & _CPD_OFF & _CP_OFF & _MCLRE_ON & 
		//_PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT
		//	__CONFIG    _CONFIG2, _WRT_OFF & _BOR21V
		// Definitions
#define _XTAL_FREQ  16000000        // this is used by the __delay_ms(xx) and __delay_us(xx) functions
#define temp     -(4000)   
#define   RESET_  LATA7
#define   CSCON  TRISAbits.TRISA2

ROM char InitConfigRegs[] = {
		/* 0 */				CHIPMODE_STBYMODE | FREQBAND_915 | VCO_TRIM_00,   //0b00110000
		/* 1 */				MODSEL_OOK | DATAMODE_PACKET | IFGAIN_0 | DATAMODE_BUFFERED | OOKTHRESHTYPE_PEAK,          //0b01101100 
		/* 2 */				0b00001001,										  //0b00001001
		/* 3 */				0b00010011,										  //0b00010011	
		/* 4 */				OOKFLOORTHRESH_VALUE,							  //0b00001100
		/* 5 */				0b00001111,                                       //0b00001111 FIFOSIZE_64 | FIFO_THRSHOLD_1
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
		/* 23 */ 			0x37, // 2nd byte of Sync word,
		/* 24 */ 			0xF1, // 3rd byte of Sync word,
		/* 25 */ 			0xC3, // 4th byte of Sync word,
		/* 26 */ 			FC_200 | TXPOWER_13,                                           //0b01110000 
		/* 27 */ 			CLKOUT_OFF | CLKOUT_12800,                                     //0b00000000
		/* 28 */ 			MANCHESTER_OFF | 2,										       //0b00000010
		/* 29 */ 			NODEADRS_VALUE,                                                //0
		/* 30 */ 			PKT_FORMAT_FIXED | PREAMBLE_SIZE_3 | WHITENING_OFF | CRC_ON | ADRSFILT_NONE,  //0b01001000
 		/* 31 */ 			FIFO_AUTOCLR_ON | FIFO_STBY_ACCESS_WRITE                      //0b00000000
							};	

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
	#define packetlength  32
#define  RD          0x00 
//#define  RD2        0x04 
#define  WR1       0x01 
#define  WR2        0x02 
#define  NU        0x08 

  bit cs_con,bff;
bit enable,flag;
unsigned char  TxPacket[packetlength];
unsigned char  RxPacket[packetlength];
unsigned char takt,a4,a5;
unsigned char RF_Mode = RF_STANDBY;
	bit IRQ0,IRQ0_sekond,IRQ0_sekond2;
bit IRQ1,IRQ1_sekond,IRQ1_sekond2;
unsigned char i,nn,a,b;

unsigned char delay;
unsigned char  A2,A3;
unsigned char  command,val,RxPacketLen;




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
 union Crr  Crc_send,Crc1_send,Crc2_send;
							 
	unsigned char   *tr_buf,flag_read,*tr_bu,flag_ok,flag_write,*tr_bu3,flag_razborka;

	unsigned char  buf[40];
//	unsigned char  buf3[40];
  const unsigned char  tes[] = "#,34,001,+250.1,+12.3,-23.4, +8.927689\r"; 



			unsigned int crc;

void interrupt Interrupt(void);


	int main(int argc, char** argv);





	void interrupt Interrupt()
{

  	if (PIR4bits.SSP2IF == 1 )  //////////////////// ВТОРОЙ ТАЙМЕР
		{  PIR4bits.SSP2IF = 0;
				
			 if (command == WR1)
			  	{
					SSP2BUF = A2;
					//SPI0DAT = A2;
					command = WR2;
				}                      // Clear the SPIF flag
			  else if (command == WR2)
			   		{command = NU;
						if   (!cs_con )
							{cs_con = 1;
						  //   LATA1 = 1;  //	LATAbits.LATA1 = 1;
						 //  CSCON = 1;
							}
					}
			  else if (command == RD)
			  		{command = NU;
					
					}
			if (	SSP2STATbits.BF == 1)
					bff = 1;
		} 

	if (PIR3bits.TMR4IF == 1 )  //////////////////// ВТОРОЙ ТАЙМЕР
		{  PIR3bits.TMR4IF = 0;
			

			
		}
	if (INTCONbits.IOCIF)
		{
			 if (IOCBFbits.IOCBF0 == 1)
					{
						if (IOCBNbits.IOCBN0 == 1)
							{
								IOCBNbits.IOCBN0 = 0;
								IOCBPbits.IOCBP0 = 1;	
								IRQ0 = 0;	
								IRQ0_sekond = 1;
							}
						else if (IOCBPbits.IOCBP0 == 1)
							{ 
								IOCBNbits.IOCBN0 = 1;
								IOCBPbits.IOCBP0 = 0;		
								IRQ0 = 1;
								IRQ0_sekond2 = 1;
							}
						IOCBFbits.IOCBF0 = 0;
					}
					
				if (IOCBFbits.IOCBF3 == 1)
					{
						if (IOCBNbits.IOCBN3 == 1)
							{
								IOCBNbits.IOCBN3 = 0;
								IOCBPbits.IOCBP3 = 1;	
								IRQ1 = 0;	
								IRQ1_sekond = 1;
							}
						else if (IOCBPbits.IOCBP3 == 1)
							{ 
								IOCBNbits.IOCBN3 = 1;
								IOCBPbits.IOCBP3 = 0;		
								IRQ1 = 1;
								IRQ1_sekond2 = 1;
							}
						IOCBFbits.IOCBF3 = 0;
					}	
					
					
		}

	
		/*
     PIR1bits.TMR1IF)  // there is only one interrupt vector so you should check to verify what caused the interrupt
		{
		
			TMR1L=(0xff & temp);
			TMR1H =(0xff & (temp >> 8));  
			flag = 1;
		//	PORTBbits.RB2=~PORTBbits.RB2;
            PIR1bits.TMR1IF=0;        // ...and then clear the interrupt flag before exiting
		}*/
  
 
}


void write_spi_con(unsigned char A1, unsigned char value)

	{
		static unsigned char t;	
		t = 0;  
		t = (A1 << 1);
		while (command != NU);
		cs_con = 0;
		LATA1 = 0;    
		command = WR1;
		SSP2BUF = t;
		A2 = value;
		while (command != NU);
		LATA1 = 1;
	}
	
	
void WriteFIFO(BYTE Data)

	{
									//volatile BYTE tmp0RFIE = PHY_IRQ1_En;
			
									//PHY_IRQ1_En = 0;
			LATB1 =0;
			while (command != NU);
			command = WR2;
			SSP2BUF = Data;
			while (command != NU);
			LATA1 = 1;
									//PHY_IRQ1_En = tmp0RFIE;
			
	}	
	
unsigned char read_spi_con(unsigned char A1)
	{
			static unsigned char t;
			t = 0;	
			t = (A1 << 1);
			t |= 0x40;
			command = RD;
		    cs_con = 0;
			LATA1 = 0;
			SSP2BUF = t;
			while (command != NU);
			command = RD;
			SSP2BUF = 0xff;
			while (command != NU);
			A3 =  SSP2BUF;
			LATA1 = 1;
		return (A3);
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
void Send_Packet_my(void)
	{
		unsigned int i;
		SetRFMode_my(RF_STANDBY);
		
		write_spi_con(0x1F, ((InitConfigRegs[0x1F] & 0xBF)| FIFO_STBY_ACCESS_WRITE));// FIFO_STBY_ACCESS_READ
		write_spi_con(0x0D, (InitConfigRegs[0x0D] | IRQ1_FIFO_OVERRUN_CLEAR ));
		write_spi_con(0x0E, ((InitConfigRegs[0x0E]) | 0x02));
		write_spi_con(0x16, 0x97);	//на передачу

		//WriteFIFO(TxPacketLen+1);
		WriteFIFO(0x31);	//Node_adrs

		for(i=0; i< packetlength; i++)
		{
		WriteFIFO(TxPacket[i]);
		}

	//	INTCONbits.GIE = 0;    //?
		//до этого момента на ноге прерывания 0
		SetRFMode_my(RF_TRANSMITTER);

		for(i=0;i<255;i++)
		{
	NOP();NOP();NOP();NOP();
	NOP();NOP();NOP();NOP();
		}

		while(!(RB3)) //здесь должно на ноге irq1 появиться лог 1
		{
		NOP();	//ожидание прерывания будет заменено временной задержкой.
		NOP();
		NOP();
		NOP();
		}

	//	INTCONbits.GIE = 1;   //?

		write_spi_con(0x16, 0x68);
		SetRFMode_my(RF_RECEIVER);	//rfmode = ресивер
		//Reset FIFO  очистить буфер после передачи
		i = read_spi_con(REG_IRQPARAM0);
		write_spi_con(REG_IRQPARAM0, (i | 0x01));

	}	

	
	
	unsigned char ReadFIFO(void)
	
		{
								//volatile BYTE tmp0RFIE = PHY_IRQ1_En;
			LATB1 =0;
			while (command != NU);
			command = RD;
			SSP2BUF = 0xff;
			while (command != NU);
			A3 =  SSP2BUF;
			LATB1 =1;
								//PHY_IRQ1_En = tmp0RFIE;
			return (A3);
		}
/*
	unsigned char ReceiveFrame_my(void)
		{
			unsigned char data, node_adrs, a;
			unsigned char i = 0;

			node_adrs = ReadFIFO();

			for(a = 0;a < (packetlength - 1);a++)
			{
			data = ReadFIFO();
			RxPacket[a] = data;
			}

			write_spi_con(0x0D, (0b00001001));	//перезагрузить фифо
			return node_adrs;
		}	*/
		
	unsigned char ReceiveFrame_my(void)
		{
			unsigned char  dat, node_adrs, a;
			unsigned char i = 0;
			write_spi_con(0x1F, ((InitConfigRegs[0x1F] & 0xBF)| FIFO_STBY_ACCESS_READ));// 
			write_spi_con(0x0D, (InitConfigRegs[0x0D] | IRQ1_FIFO_OVERRUN_CLEAR ));
			write_spi_con(0x0E, ((InitConfigRegs[0x0E]) | 0x02));
			write_spi_con(0x16, 0x97);
			SetRFMode_my(RF_RECEIVER);
			
			while(!(RB3));   // IRQ0
			RxPacketLen = ReadFIFO();	
			//flag_int0 = 0;
			node_adrs = ReadFIFO();
			RxPacketLen = (RxPacketLen-1);
			

			while(RxPacketLen--)
			{
			//flag_int0 = 0;
			dat = ReadFIFO();
			RxPacket[i] = dat;
			i++;
			};
		RxPacketLen = i;
			
	IRQ1_sekond2 = 0;

			//write_spi_con(0x0D, (0x0a));	//перезагрузить фифо
				i = read_spi_con(REG_IRQPARAM0);
		        write_spi_con(REG_IRQPARAM0, (i | 0x01));
			
			
			return node_adrs;
		}
		

void initc(void)
		{
		unsigned char i;
		
		for(i = 0;i <32;i++)
			{
				TxPacket[i] = i;
			}
		
		
	
cs_con = 1;		
RESET_ = 1;
__delay_us(100);
RESET_ = 0;
__delay_ms(5);

	//PORTBbits.RB3=0;
		}

	/*
	* 
	*/

	int main(int argc, char** argv) {

    // used to store ADC result after capture
   

	TMR1L=(0xff & temp);
	TMR1H =(0xff & (temp >> 8));
 
  
	OSCCONbits.SCS=0x10;    //set the SCS bits to select internal oscillator block
	OSCCONbits.IRCF=0x0a;   // 16mHz
	OSCCONbits.SPLLEN=0x00;  // pll dicable	
    OPTION_REGbits.TMR0CS = 0;
	OPTION_REGbits.TMR0CS = 0;

    
				// PORT C Assignments
	PORTA  = 0;
	PORTB  = 0;
	LATA   = 0;
	LATB   = 0;
	ANSELB = 0;
	ANSELA = 0;


    TRISB0 = 1;	// RB0 = IRQ0 
    TRISB1 = 1;	// RB1 = CSDATA
    TRISB2 = 1;	// RB2 = SDI2 
    TRISB3 = 1;	// RB3 = IRQ1 
    TRISB4 = 0;	// RB4 = SCK1
    TRISB5 = 0;	// RB5 = SCK2     
	TRISB6 = 0;	// RB6 = 
    TRISB7 = 0;	// RB7 = 


	TRISA0 = 0;	// RA0 = SDO2
    TRISA1 = 0;	// RA1 = CSCON
    TRISA2 = 0;	// RA2 = RESET
    TRISA3 = 0;	// RA3 =  
    TRISA4 = 0;	// RA4 
    TRISA5 = 0;	// RC5 
	TRISA6 = 0;	// RA6 = SDO
    TRISA7 = 1;	// RA7 = 
	//
	// Set up ADC
	//
	T4CONbits.T4OUTPS =0x0F;								// CKE = 0 1
	T4CONbits.T4CKPS =0x03;									// SMP = 1 1
	PR4=38;  //40											// CKP = 1 0
	T4CONbits.TMR4ON=1;
																					//	ANSELB=0;
																					//	ANSELA =0;
																					//	ANSELAbits.ANSA2=1;	
																					//	ANSELAbits.ANSA4=1;	

	
	SSP2STATbits.CKE = 1;   //Transmit occurs on transition from active to Idle clock state 
	SSP2STATbits.SMP = 1;   //SPI Master mode 0= Input data sampled at middle of data output time
	
	SSP2CON1bits.CKP =   0;  //  Idle state for clock is a low leve
	SSP2CON1bits.SSPM0 =   0;
    SSP2CON1bits.SSPM1 =   1; //SPI Master mode, clock = FOSC/64  .SSPM
  	SSP2CON1bits.SSPM2 =   0;
  	SSP2CON1bits.SSPM3 =   0;

	SSP2CON1bits.SSPEN =1;  // Synchronous Serial Port Enable bitSynchronous Serial Port Enable bit
	IOCBPbits.IOCBP0 = 1;
	IOCBPbits.IOCBP3 = 1;
    

	PIE3bits.TMR4IE=1; 
    INTCONbits.TMR0IF =1;
	PIE4bits.SSP2IE=1;
    INTCONbits.PEIE=1;          // Enable peripheral interrupt
    INTCONbits.IOCIE=1;     	
    INTCONbits.GIE=1;           // enable global interrupt
														//  PIE1bits.TMR1IE=0x01;       // enable the Timer 1 parator interrupt

 	initc();
													//PIE1bits.RCIE=0x01; 

														//RCSTAbits.CREN = 0X01; 
														 //RCSTAbits.SPEN = 0X01;
	command = NU;	

														//__delay_ms(10);

														 //LATAbits.LATA1 = 0; 
															//PORTBbits.RB3=1;
															//IE1bits.TXIE=0x01; 
															//a =SSP1BUF;
while (1)	
{	


while (1)
{
i =0;
												//	val = read_spi_con(0);
												//	__delay_ms(2);
												//	val = read_spi_con(2);
	  while (i < 0x20)
		  {
				
//for (i = 0 ; i <= 31; i++)
{

} //регистры выставлены
			write_spi_con(i,InitConfigRegs[i]);
												//__delay_ms(2);
												//	 write_spi_con(0x10,i);
													//	 write_spi_con(a,b);

												//	LATAbits.LATA1 = 0;
												//	LATAbits.LATA1 = 1;
				val = read_spi_con(i); 
												//	__delay_ms(2);
				a5 = InitConfigRegs[i];
												//	val = read_spi_con(1); 
				i++;
		}
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
while (!(val & 0b00000010));
ReceiveFrame_my();
//Send_Packet_my();
i++;
}

}

   

	

    return (EXIT_SUCCESS);
}

/*
int FastCRC16(char crcData, int crcReg)
{  unsigned char table;
   table=((char)(crcReg >> 8)) ^ crcData;
   crcReg=((crcReg << 8) ^ crc16LUT[table]);
   return(crcReg);
}
	
*/
//	int Mm;
//	unsigned int AnalogValue;       // used to store ADC result after capture
// unsigned int ADCValue;
//unsigned int Read_ADC_Value(void);

//const  unsigned int code crc16LUT[256];/*
/*
int FastCRC16(char crcData, int crcReg);
const unsigned int   crc16LUT[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};   */