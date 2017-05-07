#include <reg932.h>
#include <uart.h>

#define OSC_FREQ		7372800
#define MAX_DELAY      700 //(OSC_FREQ/10532)
#define TEMPO			(OSC_FREQ/204800)		/* Duration of a 64th note */
#define N_PAUSE        250 //(OSC_FREQ/29491)

#define E3				((OSC_FREQ/4)/164.81)  // P1,P2-allon   	(61)
#define F3				((OSC_FREQ/4)/174.61)  // P1,P0-allon		(60)
#define G3				((OSC_FREQ/4)/196.00)  // P2-allon			(29)
#define Ab3				((OSC_FREQ/4)/207.65)  // P0-allon			(28)
#define A3				((OSC_FREQ/4)/220.00)  // P1,P0-Yel,Amb		(62)
#define Bb3				((OSC_FREQ/4)/233.08)  // P1,P2-Yel,Grn		(49)
#define B3				((OSC_FREQ/4)/246.94)  // P0-Yel,Amb		(30)
#define C4				((OSC_FREQ/4)/261.63)  // P2-Yel,Grn		(17)
#define Db4				((OSC_FREQ/4)/277.18)  // P1,P0-Red,Grn		(46)
#define D4				((OSC_FREQ/4)/293.67)  // P0-Red,Grn		(14)
#define Eb4				((OSC_FREQ/4)/311.13)  // P1,P2-Red,Amb		(43)
#define E4				((OSC_FREQ/4)/329.63)  // P2-Red,Amb		(11)
#define F4				((OSC_FREQ/4)/349.23)  // P1,P0-Yel,Grn		(48)
#define Gb4				((OSC_FREQ/4)/369.99)  // P1,P0-Amb,Grn		(50)
#define G4				((OSC_FREQ/4)/392.00)  // P0-Amb,Grn		(18)
#define Ab4				((OSC_FREQ/4)/415.30)  // P1,P0-Red,Yel		(44)
#define A4				((OSC_FREQ/4)/440.00)  // P0-Red,Yel		(12)
#define Bb4				((OSC_FREQ/4)/466.16)  // P1,P0-Red,Amb		(42)
#define B4				((OSC_FREQ/4)/493.88)  // P0-Red,Amb		(10)
#define C5				((OSC_FREQ/4)/523.25)  // P0-Yel,Grn		(16)
#define Db5				((OSC_FREQ/4)/554.37)  // P1, P2-Red,Grn	(47)
#define D5				((OSC_FREQ/4)/587.33)  // P2-Red,Grn		(15)
#define Eb5				((OSC_FREQ/4)/622.25)  // P1,P2-Amb,Grn		(51)
#define E5				((OSC_FREQ/4)/659.26)  // P2-Amb,Grn		(19)
#define F5				((OSC_FREQ/4)/698.46)  // P2-Yel,Amb		(31)
#define Gb5				((OSC_FREQ/4)/739.99)  // P1,P2-Yel,Amb		(63)
#define G5				((OSC_FREQ/4)/783.99)  // P2-Red,Yel		(13)
#define Ab5				((OSC_FREQ/4)/830.61)  // P1,P2-Red,Yel 	(45)	
#define A5				((OSC_FREQ/4)/880.00)  // P1,P0-Grn			(38)
#define Bb5				((OSC_FREQ/4)/932.33)  // P1,P0-Yel			(36)
#define B5				((OSC_FREQ/4)/987.77)  // P1,P2-Grn			(39)
#define C6				((OSC_FREQ/4)/1046.5)  // P2-Grn (per_08)	(7)
#define Db6				((OSC_FREQ/4)/1108.7)  // P1,P0-Red			(40)
#define D6				((OSC_FREQ/4)/1174.7)  // P0-Red			(8)
#define Eb6				((OSC_FREQ/4)/1244.5)  // P1,P2-Amb			(41)
#define E6				((OSC_FREQ/4)/1318.5)  // P2-Amb (per_04)	(9)
#define F6				((OSC_FREQ/4)/1396.9)  // P0-Grn			(6)
#define Gb6				((OSC_FREQ/4)/1480.0)  // P1,P2-Yel			(35)
#define G6				((OSC_FREQ/4)/1568.0)  // P2-Yel (per_02)	(3)
#define Ab6				((OSC_FREQ/4)/1661.2)  // P1,P0-Amb			(34)
#define A6				((OSC_FREQ/4)/1760.0)  // P0-Amb			(2)
#define Bb6				((OSC_FREQ/4)/1864.7)  // P1,P2-Red			(33)
#define B6				((OSC_FREQ/4)/1975.5)  // P2-Red (per_01)	(1)
#define C7				((OSC_FREQ/4)/2093.0)  // P0-Yel			(4)
#define D7				((OSC_FREQ/4)/2349.3)  // P1				(32)

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

void delay(unsigned int time);
void playsound(unsigned int dur, unsigned int del);
void piano(void);
void uart_init (void);
void uart_isr(void);
void uart_transmit(char c);
void playnote(char n,char d);
void sdelay(int time);

unsigned int 	delay_t = MAX_DELAY;

code int period[] = { B6,A6,G6,C7,F6,C6,D6,E6,B4,E4,
												A4,G5,D4,D5,C5,C4,G4,E5,Ab3,G3,
												B3,F5,D7,Bb6,Ab6,Gb6,Bb5,A5,B5,Db6,
												Eb6,Bb4,Eb4,Ab4,Ab5,Db4,Db5,F4,Bb3,Gb4,
												Eb5,F3,E3,A3,Gb5};
int       *p_per;

void main(void) {
	mode = 1;
	buttonDelay = 1000;
	
	//initialize the ports so buttons and lights work
	P2M1=0;
	P1M1=0x2F&P1M1;
	P0M1=0;
	
	uart_init(); //initialize serial stuff
	
	while(1) { //do FOREVER!!! MUWAHAHAHA
		light8=0;
		
		//playnote(2, 100);
		
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
				light1=0;
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
				light1=1;
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
  //BRGCON = 0x00;
  //BRGR0 = 0xF0;
  //BRGR1 = 0x02;
  //BRGCON = 0x03;
	EA = 1;
	ET1 = 1;
	TMOD = 0x21;
	TH1 = -3;
	TR1 = 1;
	

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

// Play routine-sets p_per value used by timer1 interrupt
// n used to read p_per value
// d used to set duration of note
void playnote(char n,char d) {
	int dur_time;
	int t;
	if (d==0)
	{
		dur_time=delay_t; // delay_t is a global variable used by simon
		t=0;
	}
	else
	{
		dur_time=TEMPO*d; // TEMPO defines speed of all songs
		t=20;	     	  // t=20 puts a small delay between notes
	}

  	p_per = &period[n];

    if( *p_per != 0 ) {
        /* Enable timer */
        TH0=-(*p_per)>>8;
        TL0=-(*p_per)&0x0ff;
        TR0=1;
        ET0=1;
    }

    sdelay(dur_time);

    /* Disable timer */
    ET0=0;
    TR0=0;

	if(t!=0) sdelay(t);
}

void sdelay(int time ) {
    unsigned int    i;

    while( time-- > 0 ) {
        for( i=0 ; i<N_PAUSE ; i++ ) ;
    }
}
