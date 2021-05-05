#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h> 
#include <avr/interrupt.h>
/*
int buttonPushCounter = 0;
int main() {
  	DDRB = 0b00111110; // Directions of register B (leds & button)
	PCICR |= (1 << PCIE0); // enable the first bus 
	PCMSK0 = (1 << PCINT0); //Use PCINT0 as interrupt vector
  	// Event loop
	while (1) {
		sei(); // Allowing interrupts to occur
        
        PORTB ^= 0b100000; // Set high voltage level in PB5
        _delay_ms(1000); // 1 s ms delay
        PORTB ^= 0b000100; // Set high voltage level in PB(5,2)
        _delay_ms(1000); // 1 s delay
        PORTB ^= 0b001100; // Set high voltage level in PB(5,3)
        _delay_ms(1000); // 1 s delay
        PORTB ^= 0b000100; // Set high voltage level in PB(5,3,2)
        _delay_ms(1000); // 1 s delay
        PORTB ^= 0b101100; // Set low voltage level
        _delay_ms(1000); // 1 s delay
	}
}
ISR(PCINT0_vect){//interrupt action
    if (PINB & (1<<0) == 1) { //the button turned on
      buttonPushCounter += 1;
    }
    _delay_ms(10);
  if (buttonPushCounter == 2) {//Change power in PB1 and reset counter
    PORTB ^= 0b10;
    buttonPushCounter=0;
  }
}
*/
//additional part 1
int button = 0;
int number[10] = {252,96,218,242,102,182,190,224,254,246};
int main(void) {
  DDRD = 0b11111111; // Set pins of port D as output
  DDRB = 0b00000000; // Direcctions of register B (leds & button)
  PCICR |= (1 << PCIE0); // We will use first bus, therefore, we need to enable it
  PCMSK0 = (1 << PCINT0); // We will use PCINT0 as interrupt vector
  
  while(1) {
    sei(); // Allowing interrupts to occur
  	PORTD = number[button];
  _delay_ms(200); // 200 ms delay
  }
}
  ISR(PCINT0_vect){//interrupt action
    if (PINB & (1<<0) == 1) { //the button turned on
      button += 1;
      button = button % 10;
    }
    _delay_ms(10);
  }
/*
//additional part 2
int button = 0;
int number[10] = {252,96,218,242,102,182,190,224,254,246};
int count = 0;
int main(void) {
  DDRD = 0b11111111; // Set pins of port D as output
  DDRB = 0b00111110; // Direcctions of register B (leds & button)
  PCICR |= (1 << PCIE0); // We will use first bus, therefore, we need to enable it
  PCMSK0 = (1 << PCINT0); // We will use PCINT0 as interrupt vector
  
  while(1) {
    sei(); // Allowing interrupts to occur
  
  if (button == 0) 
  	PORTD = number[count];
  else
    PORTB = count*4;
   count+=1;
   count%=10;
  _delay_ms(500); // 500 ms delay
  }

}
  ISR(PCINT0_vect){//interrupt action
    if (PINB & (1<<0) == 1) { //the button turned on
      button ^= 1;
      PORTD = 0b00000000;
      PORTB = 0b000000;
    }
    _delay_ms(10);
  }
*/
