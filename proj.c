#include <reg932.h>
#include <uart.h>

#define OSC_FREQ		7372800

#define C4				((OSC_FREQ/4)/261.63) 
#define Db4				((OSC_FREQ/4)/277.18)
#define D4				((OSC_FREQ/4)/293.67)
#define Eb4				((OSC_FREQ/4)/311.13)
#define E4				((OSC_FREQ/4)/329.63)
#define F4				((OSC_FREQ/4)/349.23)
#define Gb4				((OSC_FREQ/4)/369.99)
#define G4				((OSC_FREQ/4)/392.00)
#define Ab4				((OSC_FREQ/4)/415.30)
#define A4				((OSC_FREQ/4)/440.00) 
#define Bb4				((OSC_FREQ/4)/466.16) 
#define B4				((OSC_FREQ/4)/493.88) 
#define C5				((OSC_FREQ/4)/523.25) 
#define Db5				((OSC_FREQ/4)/554.37) 
#define D5				((OSC_FREQ/4)/587.33) 
#define Eb5				((OSC_FREQ/4)/622.25) 
#define E5				((OSC_FREQ/4)/659.26) 
#define F5				((OSC_FREQ/4)/698.46) 
#define Gb5				((OSC_FREQ/4)/739.99) 
#define G5				((OSC_FREQ/4)/783.99) 
#define Ab5				((OSC_FREQ/4)/830.61) 
#define A5				((OSC_FREQ/4)/880.00) 
#define Bb5				((OSC_FREQ/4)/932.33) 
#define B5				((OSC_FREQ/4)/987.77) 
#define C6				((OSC_FREQ/4)/1046.5) 
#define Db6				((OSC_FREQ/4)/1108.7) 
#define D6				((OSC_FREQ/4)/1174.7) 
#define Eb6				((OSC_FREQ/4)/1244.5) 
#define E6				((OSC_FREQ/4)/1318.5) 
#define F6				((OSC_FREQ/4)/1396.9) 
#define Gb6				((OSC_FREQ/4)/1480.0) 
#define G6				((OSC_FREQ/4)/1568.0) 
#define Ab6				((OSC_FREQ/4)/1661.2) 
#define A6				((OSC_FREQ/4)/1760.0) 
#define Bb6				((OSC_FREQ/4)/1864.7) 
#define B6				((OSC_FREQ/4)/1975.5) 

sbit light1 = P2^4;
sbit light2 = P0^5;
sbit light3 = P2^7;
sbit light4 = P0^6;
sbit light5 = P1^6;
sbit light6 = P0^4;
sbit light7 = P2^5;
sbit light8 = P0^7;
sbit light9 = P2^6;

sbit button1 = P2^0;
sbit button2 = P0^1;
sbit button3 = P2^3;
sbit button4 = P0^2;
sbit button5 = P1^4;
sbit button6 = P0^0;
sbit button7 = P2^1;
sbit button8 = P0^3;
sbit button9 = P2^2;

sbit speaker = P1^7;

static bit mtxbusy;

unsigned char mode;
unsigned int buttonDelay;
							
unsigned char smStart;
unsigned char smEnd;
unsigned char rrStart;
unsigned char rrEnd;
unsigned int freq;
unsigned int dur;
unsigned char index;

void delay(unsigned int time);
void playsound(unsigned int dur, unsigned int del);
void piano(void);
void uart_init (void);
void uart_isr(void);
void uart_transmit(char c);
void sqr_wave();
void allstar();
void rickroll();

code int SmashMouth[] = {G5, D6, B5, B5, A5, G5, G5, C6, B5, B5, A5, A5, G5, 0, G5, D6, B5, B5, A5, A5, G5, G5, E5, D5, 0};

code int durSmashMouth[] = {64, 32, 32, 64, 32, 32, 32, 64, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 32};

code int GiveYouUp[] = {G5, A5, C6, A5, E6, E6, D6, G5, A5, B5, G5, 
											D6, D6, C6, B5, A5, G5, A5, B5, G5, C6, D6, B5,
											A5, G5, 0, G5, D6, C6, 0};

code int durGiveYouUp[] = {16, 16, 16, 16, 32, 32, 64, 16, 16, 16, 16, 32, 32, 32, 16, 32, 16, 16, 16, 16, 64, 32, 48, 16, 32, 32, 32, 64, 64, 32};

char trans;

void main(void) {
	mode = 1;
	buttonDelay = 15000;
	
	TMOD = 0x21;
	EA = 1;
	ET0 = 1;
	
	//initialize the ports so buttons and lights work
	P2M1=0;
	P1M1=0x2F&P1M1;
	P0M1=0;
	
	trans = 65;
	uart_init(); //initialize serial stuff
	
	while(1) { //do FOREVER!!! MUWAHAHAHA
		light8=0;
		
		//mode 1 = Smashmouth
		//mode 2 = rickroll
		//mode 3 = Piano
		
		
		//display which mode you are in
		if (mode == 1) {
			light3 = 0;
			light2 = 1;
		} else if (mode == 2) {
			light2 = 0;
			light3 = 1;
		} else if (mode == 3) {
			light2 = 0;
			light3 = 0;
		}
		
		//increment button.
		if (!button9) {
			light9 = 0;
			if (mode >= 3) {
				mode = 0;
				delay(buttonDelay);
			} else {
				mode++;
				delay(buttonDelay);
			}
			light9 = 1;
		}
		
		//decrement button
		if (!button7) {
			light7 = 0;
			if (mode <= 1) {
				mode = 3;
				delay(buttonDelay);
			} else {
				mode--;
				delay(buttonDelay);
			}
			light7 = 1;
		}
		
		//selection mode
		if (!button8) { //mode is selected
			if (mode == 1) {
				uart_transmit('S');
				uart_transmit('M');
				uart_transmit('A');
				uart_transmit('S');
				uart_transmit('H');
				uart_transmit(' ');
				uart_transmit('M');
				uart_transmit('O');
				uart_transmit('U');
				uart_transmit('T');
				uart_transmit('H');
				uart_transmit(' ');
				
				allstar();
				delay(buttonDelay);
			} else if (mode == 2) {
				uart_transmit('R');
				uart_transmit('I');
				uart_transmit('C');
				uart_transmit('K');
				uart_transmit(' ');
				uart_transmit('R');
				uart_transmit('O');
				uart_transmit('L');
				uart_transmit('L');
				uart_transmit(' ');
				
				rickroll();
				delay(buttonDelay);
			} else if (mode == 3) {
				light2=1;
				light3=1;
				light8=1;
				delay(buttonDelay);
				piano();
			}
		}
	}
}


void piano(void) {
	unsigned char play = 1; //boolean for quiting the piano mode
	light4 = 0; light5 = 0; light6 = 0; //turn on piano key lights
	while(play) {
		
		//first key
		if (!button4) {
			light4 = 1;
			playsound(5, 7);
			light4 = 0;
		}
		
		//second key
		if (!button5) {
			light5 = 1;
			playsound(5, 5);
			light5 = 0;
		}
		
		//third key
		if (!button6) {
			light6 = 1;
			playsound(5, 3);
			light6 = 0;
		}
		
		//exit button
		if (!button8) {
			light4 = 1; light5 = 1; light6 = 1;
			delay(buttonDelay);
			play = 0;
		}
	}
}


//plays a sound for a duration and del determines pitch
void playsound(unsigned int dur, unsigned int del) {
	unsigned int i;
	for (i=0; i<dur; i++) {
		speaker = ~speaker;
		delay(del);
	}
}

//delay for a time
void delay (unsigned int time) {
	unsigned int i, j;
	for (i=0; i < time; i++) {
		for (j=0; j<5;j++) {
		}
	}
}

void uart_init (void) {
  // configure UART
  // clear SMOD0
  PCON &= ~0x40;
  SCON = 0x50;
  // set or clear SMOD1
  PCON &= 0x7F;
  PCON |= (0 << 8);
  SSTAT = 0x00;

  // enable break detect
  AUXR1 |= 0x40;

  // configure baud rate generator
  
	BRGCON = 0x00;
  BRGR0 = 0xF0;
  BRGR1 = 0x02;
  BRGCON = 0x03;
	/**/
	/*
	EA = 1;
	ET1 = 1;
	TMOD = 0x21;
	TH1 = -6;
	TR1 = 1;
	/**/

  // TxD = push-pull, RxD = input
  P1M1 = 0x02;
  P1M2 = 0x01;

  // initially not busy
  mtxbusy = 0;

  // set isr priority to 0
  IP0 = 1;
  IP0H = 1;
  
  // enable uart interrupt
  ES = 1;
  EA = 1;


} // uart_init


void uart_isr(void) interrupt 4 using 1 {
  if (RI)
  {
    //do nothing upon receiving stuff
		
    // clear interrupt flag
    RI = 0;
  }

  if (TI)
  {
    // clear interrupt flag
    TI = 0;
    // no longer busy
    mtxbusy = 0;
  }
}

void uart_transmit(char c) {
  while(mtxbusy);
  mtxbusy = 1;
  SBUF = c;
}

void allstar() {
	
	//display which mode you are in
		if (mode == 1) {
			light3 = 0;
			light2 = 1;
		} else if (mode == 2) {
			light2 = 0;
			light3 = 1;
		} else if (mode == 3) {
			light2 = 0;
			light3 = 0;
		}
		
		//increment button.
		if (!button9) {
			light9 = 0;
			if (mode >= 3) {
				mode = 0;
				delay(buttonDelay);
			} else {
				mode++;
				delay(buttonDelay);
			}
			light9 = 1;
		}
		
		//decrement button
		if (!button7) {
			light7 = 0;
			if (mode <= 1) {
				mode = 3;
				delay(buttonDelay);
			} else {
				mode--;
				delay(buttonDelay);
			}
			light7 = 1;
		}
		
		if (!button8) {
			index = 0;
		}
	
	for(index = 0; index < 25; index++) {
		//display which mode you are in
		if (mode == 1) {
			light3 = 0;
			light2 = 1;
		} else if (mode == 2) {
			light2 = 0;
			light3 = 1;
		} else if (mode == 3) {
			light2 = 0;
			light3 = 0;
		}
		
		//increment button.
		if (!button9) {
			light9 = 0;
			if (mode >= 3) {
				mode = 0;
			} else {
				mode++;
			}
			light9 = 1;
		}
		
		//decrement button
		if (!button7) {
			light7 = 0;
			if (mode <= 1) {
				mode = 3;
			} else {
				mode--;
			}
			light7 = 1;
		}	
		
		delay(200);
		if (SmashMouth[index] != 0) {
			freq = SmashMouth[index];
			dur = durSmashMouth[index] * 500;
			TR0 = 1;
			delay(dur);
		} else if (SmashMouth[index] == 0) {
			TR0 = 0;
			dur = durSmashMouth[index];
			delay(dur * 500);
		} else {
			TR0 = 0;
		}
	}
}

void rickroll() {
	for(index = 0; index < 30; index++) {
		
		//display which mode you are in
		if (mode == 1) {
			light3 = 0;
			light2 = 1;
		} else if (mode == 2) {
			light2 = 0;
			light3 = 1;
		} else if (mode == 3) {
			light2 = 0;
			light3 = 0;
		}
		
		//increment button.
		if (!button9) {
			light9 = 0;
			if (mode >= 3) {
				mode = 0;
			} else {
				mode++;
			}
			light9 = 1;
		}
		
		//decrement button
		if (!button7) {
			light7 = 0;
			if (mode <= 1) {
				mode = 3;
			} else {
				mode--;
			}
			light7 = 1;
		}
		
		delay(200);
		if (GiveYouUp[index] != 0) {
			freq = GiveYouUp[index];
			dur = durGiveYouUp[index] * 500;
			TR0 = 1;
			delay(dur);
		} else if (GiveYouUp[index] == 0) {
			TR0 = 0;
			dur = durGiveYouUp[index];
			delay(dur * 500);
		} else {
			TR0 = 0;
		}
	}
}

void sqr_wave() interrupt 1 {
	speaker = ~speaker;
	TH0 = -(freq)>>8;
	TL0 = -(freq);
}