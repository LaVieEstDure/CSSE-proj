/*
 * timer0.c
 *
 * Author: Peter Sutton
 *
 * We setup timer0 to generate an interrupt every 1ms
 * We update a global clock tick variable - whose value
 * can be retrieved using the get_clock_ticks() function.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "display.h"
#include "timer0.h"
#include "timer1.h"

/* Our internal clock tick count - incremented every 
 * millisecond. Will overflow every ~49 days. */
static volatile uint32_t clockTicks;
static volatile uint16_t delay;
volatile uint16_t buzzer_len;
/* Set up timer 0 to generate an interrupt every 1ms. 
 * We will divide the clock by 64 and count up to 124.
 * We will therefore get an interrupt every 64 x 125
 * clock cycles, i.e. every 1 milliseconds with an 8MHz
 * clock. 
 * The counter will be reset to 0 when it reaches it's
 * output compare value.
 */

void buzz(uint16_t len){
	buzzer_len = len;
}



void init_timer0(void) {
	/* Reset clock tick count. L indicates a long (32 bit) 
	 * constant. 
	 */
	buzzer_len = 0;
	clockTicks = 0L;
	paused = 0;
	delay = 0;
	/* Clear the timer */
	TCNT0 = 0;

	/* Set the output compare value to be 124 */
	OCR0A = 124;
	
	/* Set the timer to clear on compare match (CTC mode)
	 * and to divide the clock by 64. This starts the timer
	 * running.
	 */
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS01)|(1<<CS00);

	/* Enable an interrupt on output compare match. 
	 * Note that interrupts have to be enabled globally
	 * before the interrupts will fire.
	 */
	TIMSK0 |= (1<<OCIE0A);
	
	/* Make sure the interrupt flag is cleared by writing a 
	 * 1 to it.
	 */
	TIFR0 &= (1<<OCF0A);
}

uint32_t get_current_time(void) {
	uint32_t returnValue;

	/* Disable interrupts so we can be sure that the interrupt
	 * doesn't fire when we've copied just a couple of bytes
	 * of the value. Interrupts are re-enabled if they were
	 * enabled at the start.
	 */
	uint8_t interruptsOn = bit_is_set(SREG, SREG_I);
	cli();
	returnValue = clockTicks;
	if(interruptsOn) {
		sei();
	}
	return returnValue;
}

void toggle_pause(){
	paused = !paused;
} 

void delay_time(uint16_t secondths){
	delay = secondths * 100;
}

uint8_t is_delayed(void){
	return ((delay > 0) ? 1 : 0); 
}
static int8_t sound_on = -1;

void turn_on_sound(void){
	sound_on = 0;
}
static int16_t sound[] = {500, 15, 15, 500, 70, 150, 20, 100, -1};
static uint16_t freq[] = {2408, 2273, 2025, 1911, 1703, 1517, 1432, 1276, 200};

static uint8_t alternate = 0;
static int16_t sound2[] = {150, 400,  15, 500, 70, 15, 500, 70, -1};
static uint16_t freq2[] = {500, 2273, 200, 2408, 2273, 2025, 2408, 2273, 200};

void alternate_sound(void){
	alternate = 1;
	sound_on = 0;
}

uint16_t sdelay = 0;
ISR(TIMER0_COMPA_vect) {
	/* Increment our clock tick count */
	switch_disp();
	if(!(delay > 0)){
		if(!paused){
			clockTicks++;
		}	
	} else {
		delay--;
	}
	if(!paused){
	if(sound_on == -1){
		if(buzzer_len > 0){
			turnon_buzz();
			buzzer_len--;	
		} else {
			turnoff_buzz();
		}
	} else {
		if(buzzer_len > 0){
			turnon_buzz();
			buzzer_len--;
		} else {
			if(sdelay > 0){
				sdelay--;
				turnoff_buzz();
			} else {
				if(sound[sound_on] == -1){
					if(alternate){
						alternate = 0;
					}
					sound_on = -1;
					set_frequency(200);
				} else {
					if(!alternate){
						buzzer_len = sound[sound_on];
						set_frequency(freq[sound_on]);
					} else {
						buzzer_len = sound2[sound_on];
						set_frequency(freq2[sound_on]);
					}
					sound_on++;
					sdelay = 250;						
				}
			}
		}
	}} 
}
