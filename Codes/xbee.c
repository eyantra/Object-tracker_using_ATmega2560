/*****************************************************************************
 This experiment enables the user to control the robot motion through Serial
 Comunication from the PC Wirelessly. UART0 is dedicated to Wireless serial
 communication using on-board Zigbee Module. 

 Byte Commands for respective direction are as Follows:

 0x51 -----> FORWARD
 0x52 -----> BACKWARD
 0x53 -----> LEFT
 0x54 -----> Right
 0x50 -----> Stop

******************************************************************************/

#include<avr/io.h>
#include<avr/interrupt.h>
#include<avr/signal.h>
#include<util/delay.h>
#include<avr/delay.h>

unsigned char data; //to store received data from UDR0

/***** Function To Initialize Ports *****/
void init_ports()
{
	PORTA = 0x00;
	DDRA = 0x0F;
	PORTL = 0x18;
	DDRL = 0x18;
}

/***** UART0 initialization *****/
// desired baud rate:9600
// actual baud rate:9600 (0.0%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x47; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x98;
}

SIGNAL(SIG_USART0_RECV) // ISR for receive complete interrupt
{
	data = UDR0; 		//making copy of data from UDR in data variable 

	UDR0 = data; 		//echo data back to PC

		if(data == 0x51)
		{
			PORTA=0x06;  //forward
		}

		if(data == 0x52)
		{
			PORTA=0x09; //back
		}

		if(data == 0x53)
		{
			PORTA=0x05;  //left
		}

		if(data == 0x54)
		{
			PORTA=0x0A; //right
		}

		if(data == 0x50)
		{
			PORTA=0x00; //stop
		}

}

/***** Function To Initialize All The Devices *****/	
void init_devices()
{
 asm("cli");

 init_ports();  //Initializes all the ports
 //XDIV=0x00;
 XMCRA=0x00;
 XMCRB=0x00;
 uart0_init(); //Initailize UART1 for serial communiaction
 EIMSK  = 0x00;
 TIMSK0 = 0x00; //timer0 interrupt sources
 TIMSK1 = 0x00; //timer1 interrupt sources
 TIMSK2 = 0x00; //timer2 interrupt sources
 TIMSK3 = 0x00; //timer3 interrupt sources
 TIMSK4 = 0x00; //timer4 interrupt sources
 TIMSK5 = 0x00; //timer5 interrupt sources
 

 asm("sei");   // Enables the global interrupt 
}

/***** Main Function *****/
int main(void)
{
	init_devices();
	while(1);
}
