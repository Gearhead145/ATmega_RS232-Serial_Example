// By Dan Daniel Corda
// Credit for Serial Read and Write Basics goes to Robert Paz
// https://www.youtube.com/watch?v=W5xt6w5EMP0

/*This program is setup for Arduino UNO using Atmel328p-pu and an DB-9 
 levelshifter such as https://www.sparkfun.com/products/133
 **The above level shifter does not have RTS/CTS pins for flow control. 
 **If your application need flow control, make sure to purchase one that 
 has the appropriate RTS/CTS pins.
 
 One with Flow Control (They also sell a male variant)
 http://www.amazon.com/Ultra-Compact-RS232-Converter-Female/dp/B00OPTOKI0/ref=pd_sim_328_4?ie=UTF8&dpID=41JLqBZikFL&dpSrc=sims&preST=_AC_UL160_SR160%2C160_&refRID=0Q4E692QJPHN0YVSWT1C
  
 This code It will work on other ATMEL and TI variants with small modifications
 ***If using a chip with only 1 Rx/Tx pair Arduino UNO you will need to unplug your 
 levelshifter and plug in USB power when you upload code. Arduino UNO uses the Serial
 RX/TX pair during code upload. You can plug your levelshifter back-in once the code
 uploaded.  

 DB9 to USB Serial Adapter
 Connects to the level shifter and allows your computer to talk to the microcontroller 
 http://www.amazon.com/gp/product/B00BUZ0K68?psc=1&redirect=true&ref_=oh_aui_detailpage_o00_s00


 DTE Serial Terminal
 While you can use the arduino Serial Monitor to recieve data. Realterm makes it initially easier
 to check if your frame size and stop bits are set properly.
 https://sourceforge.net/projects/realterm/?source=typ_redirect
*/


//This defines the 9600 Baud Rate
//You will need to change the hex code if communicating at another rate
#define BAUD_PRESCALER 0x0067
/*
BAUD     U2X0=0    U2X0=1
2400     0x01A0    0x0340
4800     0x00CF    0x01A0
9600     0x0067    0x00CF
14400    0x0044    0x008A
19200    0x0033    0x0067
28800    0x0022    0x0044
38400    0x0019    0x0033
57600    0x0010    0x0022
76800    0x000C    0x0019
115200   0x0008    0x0010
230400   0x0003    0x0008
250000   0x0003    0x0007
500000   0x0001    0x0003
100000   0x0000    0x0001
*/

// Go Below to find ATMega 328 Register Sheet
//  http://www.atmel.com/images/atmel-8271-8-bit-avr-microcontroller-atmega48a-48pa-88a-88pa-168a-168pa-328-328p_datasheet_complete.pdf
//  and for ATMega 2560 Register Sheet 
//  http://www.atmel.com/images/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf

void USART_init(void){
  //Set the baud rate prescale register
  UBRR0 = BAUD_PRESCALER;

  //Now set Frame Size
  //Bits = Bts shown in the order 
  UCSR0C = ( (1 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00) );
  //          1->2stop bts   1 frame bit 1   1 frame bit 0             
  //          0->1stop bt 
  
  UCSR0B = (0 << UCSZ02);
  //        0 frame bit 2
 
  /*For us we are communicating with a Morningstar Sunsaver
  According to Morningstars documentation they use 8 data bits 
  and 2 Stop Bits with no parity. Frame Size is set to 001 to 
  a 6 bit frame size. This is because 8 data bits and 2 stop bits
  is more than a single 8 or 9 bit frame. We need to read multiple 
  frames to recieve all the information from the morningstar device
  this means we will have xxxxSS and xxxxSS to read the 8 databits
  and hence 2 6 bit data frames 

UCSZ
  00 01 02
  0  0  0 5-bit
  0  0  1 6-bit
  0  1  0 7-bit
  0  1  1 8-bit
  1  0  0  ---
  1  0  1  ---
  1  1  0  ---
  1  1  0 9-bit
  */
 
  //Enable receiver and transmitter
  UCSR0B = ( (1 << RXEN0) | (1 << TXEN0) );
}

void USART_send(unsigned char data){
  //While the transmit buffer is not empty loop
  while( !(UCSR0A & (1 << UDRE0) ) );
  //when the buffer is empty write data to the transferred
  UDR0 = data;
}

unsigned char USART_Recieve(void){
  //wait for data to be recieved
  while( !(UCSR0A & (1 << RXC0) ) );
  //Get and return received data from buffer
  return UDR0;
}

void USART_putstring(char* StringPtr){
  while(*StringPtr != 0x00){
    USART_send(*StringPtr);
    StringPtr++;
  }
}

void setup() {
  // put your setup code here, to run once:
  
  //Initialize Registers for Send and Recieve by calling our function
  USART_init();
  USART_putstring("RS232 RX TX 0 GO!");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  unsigned char data = 0;
  data = USART_Recieve();

  if(data != 0){
  USART_send(data);
  }
}
