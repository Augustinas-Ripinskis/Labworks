//#define F_CPU 16000000UL   // Not necessary in simulator
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include<stdio.h> 
#include<stdlib.h>
#include<string.h>


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
double temp;       //temperature
double hist=0;
double sensorInput;

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

// -----------  ADC functions -----------------------

void InitADC()
{
 // Select Vref=AVcc
 //and set left adjust result
 ADMUX |= (1<<REFS0)|(1<<ADLAR); //|(1<<REFS0);
 //set prescaller to 128 and enable ADC 
 ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);    
}

uint16_t ReadADC(uint8_t ADCchannel){
 //select ADC channel with safety mask
 ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
 //single conversion mode
 ADCSRA |= (1<<ADSC);
 // wait until ADC conversion is complete
 while( ADCSRA & (1<<ADSC) );
 return ADCH;
}

// ---------  Not LCD functions -----------------------



int main()
{
 	DDRC = 0x00;
    Serial.begin(9600);
  	InitADC();
  	LCD_Init();
  	_delay_ms(500);
	//ReadADC(uint8_t PORTC);
  	char arr[3][17] =       //preset the necessary text
      { "VilniusTech Menu",
        "Temp2: ",
      };
    LCD_Printpos(0,0,arr[0]);//write VilniusTech Menu
    LCD_Printpos(1,0,arr[1]);//write Temp2:
  	sprintf(arr[2], "%.2f", temp);//transform current temerature reading to a string
  	LCD_Printpos(1,8,arr[2]);//write current temperature
  	LCD_Printpos(1,14,"oC");
	while(1) {
	sensorInput = analogRead(A1);          //read the analog sensor and store it
    temp = (double)sensorInput / 1024;   //find percentage of input reading
    temp = temp * 5;                     //multiply by 5V to get voltage
  	temp = temp - 0.5;                   //Subtract the offset 
  	temp = temp * 100;                   //Convert to degrees 
    Serial.println(temp);                //write temperature in serial monitor
      if (temp != hist){                 //if temperature changed
        LCD_Clear();                     //clear text on screen
        LCD_Printpos(0,0,arr[0]);        //re add all text 
    	  LCD_Printpos(1,0,arr[1]);
        LCD_Printpos(1,14,"oC");
     	  sprintf(arr[2], "%.2f", temp);   //transform current temerature reading to a string
  		  LCD_Printpos(1,8,arr[2]);
        hist=temp;                       //change temperature in history
      }
    _delay_ms(250);
    }
}
