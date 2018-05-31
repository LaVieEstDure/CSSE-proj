#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer1.h"
#include "timer2.h"

#include <avr/io.h>
#define F_CPU 8000000UL	// 8MHz
#include <util/delay.h>
#include <stdint.h>
uint16_t pulsewidth;
uint16_t clockperiod;
uint16_t time;
volatile uint16_t buzzlength;
// For a given frequency (Hz), return the clock period (in terms of the
// number of clock cycles of a 1MHz clock)
uint16_t freq_to_clock_period(uint16_t freq) {
	return (1000000UL / freq);	// UL makes the constant an unsigned long (32 bits)
								// and ensures we do 32 bit arithmetic, not 16
}

// Return the width of a pulse (in clock cycles) given a duty cycle (%) and
// the period of the clock (measured in clock cycles)
uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod) {
	return (dutycycle * clockperiod) / 100;
}

void init_timer1(void){
    uint16_t freq = 200;	// Hz
	float dutycycle = 2;	// %
	uint16_t clockperiod = freq_to_clock_period(freq);
	uint16_t pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);

    DDRD&= ~(1<<PORTB4);
    OCR1A = clockperiod - 1;

    if(pulsewidth == 0) {
		OCR1B = 0;
	} else {
		OCR1B = pulsewidth - 1;
	}

   	TCCR1A = (1 << WGM11) | (1 <<WGM10) | (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1);
	
	TCCR1B = (1 << WGM12) | (1 << WGM13) | (0 << CS12) | (1 << CS11) | (0 << CS10);

}
void turnoff_buzz(void){
    DDRD &= ~(1<<PORTB4);
}
void turnon_buzz(void){
    DDRD |= (1<<PORTB4);
}

void start_buzz(uint16_t length){
    buzzlength = length;
}
void set_frequency(uint16_t freq){
    float dutycycle = 2;	// %
	clockperiod = freq_to_clock_period(freq);
	pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
    OCR1B = pulsewidth - 1;
    OCR1A = clockperiod - 1;
}

ISR(TIMER1_COMPA_vect) {
    if(pulsewidth > 0) {
			// The compare value is one less than the number of clock cycles in the pulse width
			OCR1B = pulsewidth - 1;
    } else {
        OCR1B = 0;
    }
    // Note that a compare value of 0 results in special behaviour - see page 169 of the
    // datasheet (10/2016 version)
    
    // Set the maximum count value for timer/counter 1 to be one less than the clockperiod
    OCR1A = clockperiod - 1;
}