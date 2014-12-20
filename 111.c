/////////////////////////////////////
//  Generated Initialization File  //
/////////////////////////////////////

#include "compiler_defs.h"
#include "C8051F350_defs.h"

// Peripheral specific initialization functions,
// Called from the Init_Device() function
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
    SPI0CFG   = 0x40;
    SPI0CN    = 0x01;
    SPI0CKR   = 0x1D;
}

void ADC_Init()
{
    ADC0MD    = 0x80;
    ADC0BUF   = 0x40;
    ADC0MUX   = 0x08;
}

void Port_IO_Init()
{
    // P0.0  -  SCK  (SPI0), Push-Pull,  Digital
    // P0.1  -  MISO (SPI0), Push-Pull,  Digital
    // P0.2  -  MOSI (SPI0), Push-Pull,  Digital
    // P0.3  -  Unassigned,  Open-Drain, Digital
    // P0.4  -  TX0 (UART0), Push-Pull,  Digital
    // P0.5  -  RX0 (UART0), Push-Pull,  Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Open-Drain, Digital

    // P1.0  -  Unassigned,  Push-Pull,  Digital
    // P1.1  -  Unassigned,  Push-Pull,  Digital
    // P1.2  -  Unassigned,  Open-Drain, Digital
    // P1.3  -  Unassigned,  Open-Drain, Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  Unassigned,  Open-Drain, Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital

    P0MDOUT   = 0x37;
    P1MDOUT   = 0x03;
    XBR0      = 0x03;
    XBR1      = 0x40;
}

void Oscillator_Init()
{
    OSCICN    = 0x83;
}

void Interrupts_Init()
{
    EIE1      = 0x08;
    IT01CF    = 0x76;
    IE        = 0xD5;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    Timer_Init();
    UART_Init();
    SPI_Init();
    ADC_Init();
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
}
