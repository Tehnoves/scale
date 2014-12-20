//#include <c_math.c>
const unsigned short dclk = 0x02,din=0x01, load = 0x00;
char txt[7];
float ftemper;
int t;
int count;
unsigned char temp_h,temp_l,fCNT_PER_C,fCNT_REM,bvminus,decimal,os;

   //void interrupt() {             // Interrupt is triggered by the watchdog
   //    if ( INTCON & 0x08)
   //    {
   //    if (INTCON & 0x01)
   //      {
   //
   //     }
   //  }
   //  }
       const unsigned char dscrc_table[] = {
0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
157,195, 33,127,252,162, 64, 30, 95, 1,227,189, 62, 96,130,220,
35,125,159,193, 66, 28,254,160,225,191, 93, 3,128,222, 60, 98,
190,224, 2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89, 7,
219,133,103, 57,186,228, 6, 88, 25, 71,165,251,120, 38,196,154,
101, 59,217,135, 4, 90,184,230,167,249, 27, 69,198,152,122, 36,
248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91, 5,231,185,
140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
202,148,118, 40,171,245, 23, 73, 8, 86,180,234,105, 55,213,139,
87, 9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

void null() {
  GPIO |= (1<<dclk);
  asm nop
  GPIO &= ~(1<< dclk);
  asm nop
  GPIO &= ~(1<< din);
  asm nop
  }
void edin () {
  GPIO |= (1<<dclk);
  asm nop
  GPIO &= ~(1<< dclk);
  asm nop
  GPIO |= (1<< din);
  asm nop
}
void minus() {
     null();
      null();
      null();
      null();
      null();
      edin();
      null();
      }

 void zero () {
      edin();
      edin();
      edin();
      edin();
      edin();
      null();
      edin();
 }

 void one() {
      null();
      edin();//null();
      edin();//null();
      null();
      null();// edin();
      null();
      null();// edin();
 }

 void two() {
      edin();
      null();
      edin();
      edin();
      null();
      edin();
      edin();
 }

 void tree() {
      edin();
      edin();//null();
      edin();//null();
      edin();
      null();//edin();
      edin();
      null();//edin();
 }
 void four() {
      null();
      edin();
      edin();//null();
      null();
      edin();
      edin();
      null();//edin();
 }
 void five() {
      edin();
      edin();
      null();
      edin();
      edin();
      edin();
      null();
 }
 void six() {
      edin();
      edin();
      null();//edin();
      edin();
      edin();
      edin();
      edin();//null();
 }
 void seven() {
      null();//edin();
      edin();//null();
      edin();//null();
      edin();//null();
     null();//edin();
      null();
      null();//edin();
 }
 void eight() {
      edin();
      edin();
      edin();
      edin();
      edin();
      edin();
      edin();
 }
 void nine() {
      edin();
      edin();
      edin();//edin();
      edin();
      edin();
      edin();//edin();
      null();//edin();
 }
 void pusto() {
      null();
      null();
      null();
      null();
      null();
      null();
      null();
 }

 void cas(unsigned char qwe) {

      switch (qwe)
           {
          case  0x30: zero();break;
          case  0x31: one();break;
          case  0x32: two();break;
          case  0x33: tree();break;
          case  0x34: four();break;
          case  0x35: five();break;
          case  0x36: six(); break;
          case  0x37: seven(); break;
          case  0x38: eight();break;
          case  0x39: nine();break;
          case  0x20: pusto();break;
          case  '-': minus();break;
           }
 }

 void indik (void)
 {


    cas(txt[5]);
    if (bvminus == 1)
        {
                 if (decimal == 1)
                       null();
                 else
                      edin();
        }
        else
            edin();


    if (txt[4] == ' ')
         txt[4] = '0';
    cas(txt[4]);
    null();              // точка
    null();              // точка
    cas(txt[3]);
    null();



     GPIO |= (1<<load);
    asm nop
    asm nop
    asm nop
    asm nop
    GPIO &= ~(1<< load);

 }


void main () {
// os = dscrc_table[1];
 GPIO = 0;
 CMCON0  = 0x00;
 ANSEL = 0;
                                           //IOC |= 0x30;
 TRISIO = 0x30;
 INTCON = 0;                                          //INTCON |= (1 << GPIE);
 PIE1 = 0;                                          //INTCON |= (1 << GPIF);
 PIR1 = 0;                                         //INTCON |= (1 << GIE);
 T1CON  = 0x30;
    /*
 rr:
      OSCCON = 0x61;
       txt[5] ='1';
        txt[4] ='1';
        txt[3] ='1';
        indik();
        TMR1H = 0x0;
       TMR1L = 0x0;
        T1CON  = 0x30;
        T1CON |= (1<< TMR1ON);
       while ((PIR1 & 0x01) == 0);
       T1CON &= ~(1<< TMR1ON);
        PIR1 &= ~(1<< TMR1IF);
         TMR1H = 0x0;
       TMR1L = 0x0;
        T1CON  = 0x30;
        T1CON |= (1<< TMR1ON);
       while ((PIR1 & 0x01) == 0);
       T1CON &= ~(1<< TMR1ON);
        PIR1 &= ~(1<< TMR1IF);
         TMR1H = 0x0;
       TMR1L = 0x0;
        T1CON |= (1<< TMR1ON);
       while ((PIR1 & 0x01) == 0);
       T1CON &= ~(1<< TMR1ON);
        PIR1 &= ~(1<< TMR1IF);
        OSCCON = 0x01;
        txt[5] ='2';
        txt[4] ='2';
        txt[3] ='2';
        indik();
        T1CON  = 0x30;
        TMR1H = 0xaf;
       TMR1L = 0xff;
       T1CON |= (1<< TMR1ON);
       while ((PIR1 & 0x01) == 0);
       T1CON &= ~(1<< TMR1ON);
        PIR1 &= ~(1<< TMR1IF);

       goto rr
      */
/// os = OSCCON;
 do {

       Ow_Reset(&GPIO,4);        // Onewire reset signal
       Ow_Write(&GPIO,4,0xCC);   // Issue command SKIP_ROM
       Ow_Write(&GPIO,4,0x44);   // Issue command CONVERT_T
       Delay_ms(720);

       Ow_Reset(&GPIO,4);
       Ow_Write(&GPIO,4,0xCC);   // Issue command SKIP_ROM
       Ow_Write(&GPIO,4,0xBE);   // Issue command READ_SCRATCHPAD
                                      // Delay_ms(400);

       temp_l = Ow_Read(&GPIO,4);     // Get temperature LSB
       temp_h = Ow_Read(&GPIO,4);     // Get temperature MSB
       
       OSCCON = 0x01;

       if (temp_h>>4 == 0x0f)
       	    {
  	         ftemper = 0xffff -(temp_h*256 +temp_l);
             ftemper = ftemper/16;
             t=ftemper*10;
             decimal = 0;
             if (t >= 100)
                      { t = t/10;
                      decimal = 1;}
                      bvminus = 1;

                     	}
             else
   	             {
                	ftemper = temp_h*256+temp_l;
                  ftemper = ftemper /16;
                  bvminus = 0;
                  ftemper *= 10;
                  t = ftemper;
                }

        IntToStr(t, txt);  //

        if (bvminus == 1)
              txt[3] = '-';
        indik();

                             //  T1CON  = 0x30;

       TMR1H = 0xef;
       TMR1L = 0xff;
       T1CON |= (1<< TMR1ON);
       while ((PIR1 & 0x01) == 0);
       T1CON &= ~(1<< TMR1ON);
       PIR1 &= ~(1<< TMR1IF);
       OSCCON = 0x61;
       
                       ///     OSCCON &= 0x0f;
                       ///     os = OSCCON;
                       ///     TMR1H = 0xdf;
                          ///    TMR1L = 0xff;
                             ///    T1CON |= (1<< TMR1ON);
                               ///     ee:
                                     // goto ee
                                          //  while ((PIR1 & 0x01) == 0);
                                              //   T1CON &= ~(1<< TMR1ON);

                                                 ///    OSCCON |= 0x60;
                                                     ///   os = OSCCON;
                                                         //   delay_ms(1000);
       delay_us(500);
   } while (1);
    



}