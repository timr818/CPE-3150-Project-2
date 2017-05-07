#include <reg932.h>
#include <uart.h>

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

char trans;

void main(void) {
	mode = 1;
	buttonDelay = 1000;
	
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