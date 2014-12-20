/* 0x00 GCONREG*/		0b00101000    0b00110000,	//VCOT вызывает вопросы  001= Stand-by mode (default   ((ox28))
								// 10= 950-960 MHz or 863- 870 MHz (application circuit dependant)
								
/* 0x01 DMODREG*/		0b01101100,  //  	01= OOK	                                       ((0x88))
										bit 5 DMODE0:Data Mode 0 bit
										Setting this bit selects the data operational mode as LSB. Use this bit with DMODE1 to select the 
										operational mode.
										0= Default						
										01= Peak Mode (default)
										bit 2 DMODE1: Data Mode 1 bit
														Setting this bit selects the data operational mode asMSB. Use this bit with DMODE0 to select the 
														operational mode.
														0= Default
														bit 1-0 IFGAIN<1:0>:IF Gain bits.
														Selects gain on the IF chain. 
														11= -13.5 dB 
														10= -9 dB
														01= -4.5 dB
														00= 0 dB (maximal gain) (default)
														
/* 0x02 FDEVREG*/		0b00001001,	//отклонения частоты 40кhz	  	                 ((0x03))

/* 0x03 BRSREG*/		0b00010011,	//битрейт 10киб/с		                         ((0x07))

/* 0x04 FLTHREG*/		0b00001100,	                                                 ((0x0c))

/* 0x05 FIFOCREG*/		0b00001111,	//спорно крайне		00= 16 bytes (defaul         ((0x0f))
													FTINT<5:0> = 001111(default)
													
/* 0x06 R1CREG*/		153,	//1 три параметра                                    ((0x77))	119												
/* 0x07 P1CREG*/		122,	//2 отвечающих                                       ((0x64))   100
/* 0x08 S1CREG*/		68,	//3 за несущую частоту									  ((0x32))
/* 0x09 R2CREG*/		0,															((0x74))
/* 0x0A P2CREG*/		0,															((ox62))
/* 0x0B S2CREG*/		0,
/* 0x0C PACREG*/		0b00111000,    // 001= Reserved (default
								   // 11= 23 µs (default)
								   
/* 0x0D	FTXRXIREG*/		0b00001000,	// последние 3 вызывают вопросы		
								// 00= 00= Sync (default)
								// 00= 00= CRCOK (defaultPLREADY(2)(default
								// 1= TXDONE
								// 0= FIFO not ful
								// 0= FIFO not ful
								//  0= No FIFO Overrun occurred
/* 0x0E FTPRIREG*/ 		0b00010001,	// вызывает очень большие вопросы								
/* 0x0F RSTHIREG*/ 		0b00000000,
/* 0x10 FILCREG*/ 		0b10100011,	//вызывает некоторые вопросы   1010= 378 kHz (default
/* 0x11 PFCREG*/		0b00111000,	//вызывает некоторые вопросы  default
/* 0x12 SYNCREG*/		0b10111000,	//полифазный фильтр вызывает вопросы
								// 1= Polyphase filter enabled
								// 0= Bit Synchronizer enabled (default)
								// 1= ON
								// 11= 32 bits (default)
								// 00= 0 Errors (default
								// 0= Reserved (default)
/* 0x13	RSVREG*/	    0b00000111, //зарезервирован
/* 0x14 RSTSREG*/	    0,
/* 0x15 OOKREG*/	    0b00000000,	//вызывает вопросы   000= 0.5 dB (default
													// 000= once in each chip period (default
													// 00= fc~ BR/8.π(default)(1)
													
/* 0x16	SYNCV31REG*/	0x97,	// синхрослово													
/* 0x17 SYNCV23REG*/	0x37,								
/* 0x18 SYNCV15REG*/    0xF1,
/* 0x19 SYNCV07REG*/    0xC3,
/* 0x1A TXCONREG*/      0b01110000,   // 
/* 0x1B CLKOREG*/       0b00000000,
/* 0x1C PLOADREG*/      0b00000010,	//| packetlength,	//packetlength - длина пакета
/* 0x1D NADDSREG*/ 		0,
/* 0x1E PKTCREG*/ 		0b01001000,   // 0= Fixed Length Format (default)
                                // 10= 3 bytes (default)
								// 
/* 0x1F FCRCREG*/ 		0b00000000,								
							bit 7 ACFCRC: Auto Clear FIFO CRC bit 
								  This bit when enabled auto clears FIFO if CRC failed for the current packet.
									1= Disabled 
								0= Enabled (default)
							bit 6 FRWAXS:FIFO Read/Write Access bit 
									This bit indicate the read/write access for FIFO in Stand-by mode.
								1 = Read 
								0= Write (default)
							bit 5-0 Reserved<5:0>:Reserved bits; do not use
											00000 = Reserved (default)	
											
											


- Устанавливаем standby
- инициализируем регистры
- заливаем данные в FIFO
- включаем синтезатор и ждем lock PLL
- включаем передачу
- ждем окончания передачи по биту 

 Overrun occurred     	произошло переполнение							
								


						   
								   
