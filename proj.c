#include <reg932.h>
#include <uart.h>

#define OSC_FREQ		7372800

#define C4				((OSC_FREQ/4)/261.63)
#define D4				((OSC_FREQ/4)/293.67)
#define E4				((OSC_FREQ/4)/329.63)
#define F4				((OSC_FREQ/4)/349.23)
#define G4				((OSC_FREQ/4)/392.00)
#define A4				((OSC_FREQ/4)/440.00) 
#define B4				((OSC_FREQ/4)/493.88) 
#define C5				((OSC_FREQ/4)/523.25) 
#define D5				((OSC_FREQ/4)/587.33)
#define E5				((OSC_FREQ/4)/659.26) 
#define F5				((OSC_FREQ/4)/698.46) 
#define G5				((OSC_FREQ/4)/783.99) 
#define A5				((OSC_FREQ/4)/880.00) 
#define B5				((OSC_FREQ/4)/987.77) 
#define C6				((OSC_FREQ/4)/1046.5) 
#define D6				((OSC_FREQ/4)/1174.7) 
#define E6				((OSC_FREQ/4)/1318.5) 
#define F6				((OSC_FREQ/4)/1396.9) 
#define G6				((OSC_FREQ/4)/1568.0) 
#define A6				((OSC_FREQ/4)/1760.0) 
#define B6				((OSC_FREQ/4)/1975.5) 
#define C7				((OSC_FREQ/4)/2093.0) 

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

sbit bread1 = P0^6;
sbit bread2 = P1^6;
sbit bread3 = P0^4;

sbit speaker = P1^7;

static bit mtxbusy;

unsigned char mode;
unsigned int buttonDelay;

unsigned int tempo;
unsigned int freq;
unsigned int dur;
unsigned char lyric; //holds the lyrics to output, one char at a time
unsigned char index;
unsigned char index2;

void delay(unsigned int time);
void playsound(unsigned int dur, unsigned int del);
void piano(void);
void uart_init (void);
void uart_isr(void);
void uart_transmit(char c);
void sqr_wave();
void allstar();
void rickroll();
void sandstorm();
void notebread(char letter);
void breadoff();

code int SmashMouth[] = {G5, D6, B5, B5, A5, G5, G5, C6, B5, B5, A5, A5, G5, 0, G5, D6, B5, B5, A5, A5, G5, G5, E5, D5, 0};
	
code char SmashNote[] = {'G', 'D', 'B', 'B', 'B', 'A', 'G', 'G', 'C', 'B', 'B', 'A', 'A', 'G', '#', 'G', 'D', 'B', 'B', 'A', 'A', 'G', 'G', 'E', 'D', '#'};

code int durSmashMouth[] = {64, 32, 32, 64, 32, 32, 32, 64, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 32};
	
code char lyrSmashMouth[] = "Somebody once told me the world is gonna owe me, I ain't the sharpest tool in the shed.#";

code int GiveYouUp[] = {G5, A5, C6, A5, E6, E6, D6, G5, A5, B5, G5, 
											D6, D6, C6, B5, A5, G5, A5, B5, G5, C6, D6, B5,
											A5, G5, 0, G5, D6, C6, 0};

code char GiveYouNote[] = {'G', 'A', 'C', 'A', 'E', 'E', 'D', 'G', 'A', 'B', 'G', 
											'D', 'D', 'C', 'B', 'A', 'G', 'A', 'B','G', 'C', 'D', 'B',
											'A', 'G', '#', 'G', 'D', 'C', 0};

code char lyrGiveYouUp[] = { "Never gonna give you up, never gonna let you down. Never gonna run around and, desert you.#" };

code int durGiveYouUp[] = {16, 16, 16, 16, 32, 32, 64, 16, 16, 16, 16, 32, 32, 32, 16, 32, 16, 16, 16, 16, 64, 32, 48, 16, 32, 32, 32, 64, 64, 32};
	
code int Darude[] = { A5, A5, A5, A5, A5, A5, A5, A5, A5, A5, A5, A5, D6, D6, D6, D6, D6, D6, D6, C7, C7, C7, C7, C7, C7, C7, G5, G5, A5, A5, A5, A5, A5, A5, A5, A5, A5, A5, A5, A5, D6, D6, A5, A5, A5, A5, A5, A5, A5, A5, A5, A5, A5, A5}; //54
  
code int durDarude[] = {8, 8, 8, 8, 24, 8, 8, 8, 8, 8, 8, 24, 8, 8, 8,8, 8, 8, 24, 8,8, 8, 8, 8, 8, 24, 8, 8, 8, 8, 8, 8, 24, 8, 8,8, 8, 8, 8, 24,8, 8, 8, 8, 8, 8, 24, 8, 8, 8,8, 8, 8, 24};

char trans;

void main(void) {
	mode = 1;
	buttonDelay = 15000;
	
	tempo = 500;
	
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
			light1 = 1;
			light3 = 0;
			light2 = 1;
		} else if (mode == 2) {
			light1 = 1;
			light2 = 0;
			light3 = 1;
		} else if (mode == 3) {
			light1 = 1;
			light2 = 0;
			light3 = 0;
		} else if (mode == 4) {
			light1 = 0;
			light2 = 1;
			light3 = 1;
		}
		
		//increment button.
		if (!button9) {
			light9 = 0;
			if (mode >= 4) {
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
			} else if (mode == 4) {
				light1 = 1;
				light8 = 1;
				uart_transmit('D');
				uart_transmit('A');
				uart_transmit('R');
				uart_transmit('U');
				uart_transmit('D');
				uart_transmit('E');
				sandstorm();
			}
		}
	}
}


void piano(void) {
	unsigned char play = 1; //boolean for quiting the piano mode
	light4 = 0; light5 = 0; light6 = 0; //turn on piano key lights
	while(play) {
		if (button4 && button5 && button6) {
			TR0 = 0;
			light4=0;
			light5=0;
			light6=0;
		}
		//first key
		if (!button4) {
			freq = G4;
			dur = 5;
			TR0 = 1;
			light4=1;
		}
		
		//second key
		if (!button5) {
			freq = G5;
			dur = 5;
			TR0 = 1;
			light5=1;
		}
		
		//third key
		if (!button6) {
			freq = G6;
			dur = 5;
			TR0 = 1;
			light6=1;
		}
		
		//exit button
		if (!button8) {
			light4 = 1; light5 = 1; light6 = 1;
			delay(buttonDelay);
			play = 0;
		}
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

	index2 = 0;
	lyric = ' ';
	while(lyric != '#') {
		lyric = lyrSmashMouth[index2];
		uart_transmit(lyric);
	 	index2++;
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
			dur = durSmashMouth[index] * tempo;
			
			notebread(SmashNote[index]);
			
			TR0 = 1;
			delay(dur);
			breadoff();
		} else if (SmashMouth[index] == 0) {
			TR0 = 0;
			dur = durSmashMouth[index];
			delay(dur * 400);
		} else {
			TR0 = 0;
		}
	}
}

void breadoff() {
	bread1 = 1; bread2 = 1; bread3 = 1;
}

void notebread(char letter) {
	
	if (letter == 'A') {
		bread1 = 1; bread2 = 1; bread3 = 0;
	}
	if (letter == 'B') {
		bread1 = 1; bread2 = 0; bread3 = 1;
	}
	if (letter == 'C') {
		bread1 = 1; bread2 = 0; bread3 = 0;
	}
	if (letter == 'D') {
		bread1 = 0; bread2 = 1; bread3 = 1;
	}
	if (letter == 'E') {
		bread1 = 0; bread2 = 1; bread3 = 0;
	}
	if (letter == 'F') {
		bread1 = 0; bread2 = 0; bread3 = 1;
	}
	if (letter == 'G') {
		bread1 = 0; bread2 = 0; bread3 = 0;
	}
}

void rickroll() {
		
	index2 = 0;
	lyric = ' ';
	while(lyric != '#') {
		lyric = lyrGiveYouUp[index2];
		uart_transmit(lyric);
	 	index2++;
	}
	
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
			notebread(GiveYouNote[index]);
			freq = GiveYouUp[index];
			dur = durGiveYouUp[index] * tempo;
			TR0 = 1;
			delay(dur);
			breadoff();
		} else if (GiveYouUp[index] == 0) {
			TR0 = 0;
			dur = durGiveYouUp[index];
			delay(dur * 400);
		} else {
			TR0 = 0;
		}
	}
}

void sandstorm() {
	index2 = 0;
	lyric = ' ';
	
	for(index = 0; index < 54; index++) {		
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
		if (Darude[index] != 0) {
			freq = Darude[index];
			dur = durDarude[index] * tempo;
			TR0 = 1;
			delay(dur);
			// Darudey notes
			TR0 = 0;
			dur = 8; // add breaks between notes
			delay(dur * 400);
		}
	}
}

void sqr_wave() interrupt 1 {
	speaker = ~speaker;
	TH0 = -(freq)>>8;
	TL0 = -(freq);
}