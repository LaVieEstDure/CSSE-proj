#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer0.h"
#include "display.h"

static volatile int32_t time;
/*
 * Warning:
 * Will be a heavily edited file, not suited to generic stuff
 * 
 */
void init_timer2(void){
	time = 0L;
	DDRD |= (1 << DDRD7) | (0<<DDRD6); 
	
	/* Clear the timer */
	TCNT2 = 0;

	/* Set the output compare value to be 124 */
	OCR2A = 124;
	
	/* Set the timer to clear on compare match (CTC mode)
	 * and to divide the clock by 64. This starts the timer
	 * running.
	 */
	TCCR2A = (1<<WGM21) | (1 << COM2A0);
	TCCR2B = (1<<CS21)|(1<<CS20);

	/* Enable an interrupt on output compare match. 
	 * Note that interrupts have to be enabled globally
	 * before the interrupts will fire.
	 */
	TIMSK2 |= (1<<OCIE2A);
	
	/* Make sure the interrupt flag is cleared by writing a 
	 * 1 to it.
	 */
	TIFR2 &= (1<<OCF2A);
}
     
ISR(TIMER2_COMPA_vect) {
    time++;
}
