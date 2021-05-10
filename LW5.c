//#define F_CPU 16000000UL   // Not necessary in simulator
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Your global variables here

// Defination of pot and pin variables
#define LCD_Port         PORTD	// Define LCD Port (PORTA, PORTB, PORTC, PORTD)
#define LCD_DPin         DDRD	// Define 4-Bit Pins (PD4-PD7 at PORT D)
#define LCD_DATA0_PIN    PD4    // < pin for 4bit data bit 0  
#define LCD_DATA1_PIN    PD5    // < pin for 4bit data bit 1  
#define LCD_DATA2_PIN    PD6    // < pin for 4bit data bit 2  
#define LCD_DATA3_PIN    PD7    // < pin for 4bit data bit 3  
#define RSPIN 			 PD3	// RS Pin
#define ENPIN            PD2 	// E Pin
int runtime;			 //Timer for LCD
int count=0;			 //counter for word movement
int n=0;				 //lights on or off
int t=16;				 //number for selecting led lights
int t2=1;				 //change t for array
#define pressb1 !(PINB & (1<<PB2))
#define pressb2 !(PINB & (1<<PB1))
#define pressb3 !(PINB & (1<<PB0))
char arr[6][17] =
      { "VilniusTech Menu",
        "LED Red: ",
        "LED Green: ",
        "LED Yellow: ",
        "OFF",
        "ON"
      };
// LCD initialization function with the minimum commands list
void LCD_Init (void)
{
    // Data and control pins as outputs
    LCD_DPin |= (1<<LCD_DATA0_PIN)|(1<<LCD_DATA1_PIN)|(1<<LCD_DATA2_PIN)|(1<<LCD_DATA3_PIN)|(1<<RSPIN)|(1<<ENPIN);		//Control LCD Pins (D4-D7)
	_delay_ms(16);		//Wait more when 15 ms after start
  
  
	LCD_Action(0x02);	    // Returns the cursor to the home position (Address 0). Returns display to its original state if it was shifted. 
	LCD_Action(0x28);       // Data sent or received in 4 bit lengths (DB7-DB4)
	LCD_Action(0x0C);       // Display on, cursor off
	LCD_Action(0x06);       // Increment cursor (shift cursor to right)
	LCD_Action(0x01);       // Clean LCD
	_delay_ms(5);
}

// Function to send commands and data to the LCD
void LCD_Action( unsigned char cmnd )
{
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0);
	LCD_Port &= ~ (1<<RSPIN);
	LCD_Port |= (1<<ENPIN);
	_delay_us(1);
	LCD_Port &= ~ (1<<ENPIN);
	_delay_us(200);
	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);
	LCD_Port |= (1<<ENPIN);
	_delay_us(1);
	LCD_Port &= ~ (1<<ENPIN);
	_delay_ms(2);
}

// Clear whole LCD
void LCD_Clear()
{
	LCD_Action (0x01);		//Clear LCD
	_delay_ms(2);			//Wait to clean LCD
	LCD_Action (0x80);		//Move to Position Line 1, Position 1
}

// Prints string of chars 
void LCD_Print (char *str)
{
	int i;
	for(i=0; str[i]!=0; i++)
	{
		LCD_Port = (LCD_Port & 0x0F) | (str[i] & 0xF0);
		LCD_Port |= (1<<RSPIN);
		LCD_Port|= (1<<ENPIN);
		_delay_us(1);
		LCD_Port &= ~ (1<<ENPIN);
		_delay_us(200);
		LCD_Port = (LCD_Port & 0x0F) | (str[i] << 4);
		LCD_Port |= (1<<ENPIN);
		_delay_us(1);
		LCD_Port &= ~ (1<<ENPIN);
		_delay_ms(2);
	}
}

// Prints string of chars to the specific location
// row - the row 0 or 1;
// pos - the column from 0 till 16
void LCD_Printpos (char row, char pos, char *str)
{
	if (row == 0 && pos<16)
	LCD_Action((pos & 0x0F)|0x80);
	else if (row == 1 && pos<16)
	LCD_Action((pos & 0x0F)|0xC0);
	LCD_Print(str);
}

// Your functions here

int main()
{
  	DDRB = 0b00111000; // Directions of register B (leds & buttons)
 	PCICR |= (1 << PCIE0); // for interupts in PB
  	PCMSK0 |= (1 << PCINT0)|(1 << PCINT1)|(1 << PCINT2); // interupts for pb0-2
    
  	LCD_Init();
  
  	//main part 1
  	char  *word="VilniusTech";
  	LCD_Printpos(1,0, word);
  
	while(1) {
      sei(); // Allowing interrupts to occur
      
      /*
      //main part 2
      LCD_Clear();
	  LCD_Printpos(0,count, word);
      count=(count+1)%16;
      int i=strlen(word);
      _delay_ms(150);
      */
      //side   
    _delay_ms(100);
	}
}
ISR(PCINT0_vect){//interrupt action
  if (pressb1) { 
    t*=2;		//move light to the left
  	if (t==64) t/=8;}//if t is out of bounds loop it back
  if (pressb2) { 
    t/=2;		//move light to the left
  	if (t==4) t*=8;}//if t is out of bounds loop it back
  if (pressb3) { 
    n=(n+1)%2;	//turn light on/off
  	if (n==1) PORTB=t;
  	else PORTB=0;}
  if (PORTB!=0) PORTB=t;
  if (t==8) t2=1;
  else if (t==16) t2=2;
  else t2=3;

  LCD_Clear();
  LCD_Printpos(0,0,arr[0]);
  LCD_Printpos(1,0,arr[t2]);
  LCD_Printpos(1,13,arr[n+4]);
}
