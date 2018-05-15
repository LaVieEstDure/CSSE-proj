#include <avr/io.h>

#include "timer.h"

#include <avr/interrupt.h>

static volatile uint32_t clockticks;

static volatile uint16_t lap_clock_ticks;


static volatile uint8_t lap_timer = 0;


void init_timer1(void) {
    lap_clock_ticks = 0;
    clockticks = 0L;


    TCNT1 = 0;

    OCR1A =  12499;

    TCCR1A = 0;
    TCCR1B = (1<<WGM12)|(1<<CS11)|(1<<CS10);
	TIMSK1 |= (1<<OCIE1A);
	TIFR1 &= (1<<OCF1A);
}



