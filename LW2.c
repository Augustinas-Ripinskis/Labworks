#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h> 
#include <avr/interrupt.h>

int count=0;
bool mimic = 0;
bool why2=0;
  
int main(void) {
  DDRB = 0b00111100; 
  DDRC = 0xff;
  // Main part 1
  DDRD &= ~(1<<PD7); // PD7 dicertion as input
  PORTD |= (1<<PD7); // set pull-up resistors of PD7
  PCICR |= (1 << PCIE2); // set PCIE2 to enable PCMSK2 scan
  PCMSK2 |= (1 << PCINT23); // set PCINT23 to trigger an interrupt on state change 
  
  // Main part 2
  DDRD &= ~(1<<PD3); // PD3 dicertion as input
  PORTD |= (1<<PD3);  // set pull-up resistors of PD3
  EICRA |= (1 << ISC11); // set ISC11 to enable EIMSK scan
  EIMSK |= (1 << INT1); // set INT1 to interrupt on rising trigger
  
  // Additional part
  PCICR |= (1 << PCIE0); // set PCIE0 to enable PCMSK0 scan
  PCMSK0 |= (1 << PCINT1); // set PCINT1 to trigger an interrupt on state change
  
    
  while(1) {
    sei(); // Allowing interrupts to occur
  PORTC  = count;
  
    //additional part
  if(mimic == 1){ PORTB = (PORTC<<3);}
  else { PORTB = 0x00;}
  _delay_ms(100); // 100 ms delay
    
  }
}
//ISR(PCINT2_vect){//interrupt action
    //PORTB ^= 0b100;
    //_delay_ms(100);
 // }
ISR(INT1_vect){//interrupt action
    count+=1;
  	count=count%8;
    _delay_ms(100);
  }
ISR(PCINT0_vect){//interrupt action
	if (PINB & (1<<1) == 2) { //the button turned on
      why2^=1;
      if(why2==1) {
      mimic ^= 1;
      }
    }
  _delay_ms(100);
  }
/*
Serial.begin(9600);
Serial.println(mimic);
*/
