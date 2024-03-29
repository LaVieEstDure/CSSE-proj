/*
 * timer0.h
 *
 * Author: Peter Sutton
 *
 * We set up timer 0 to give us an interrupt
 * every millisecond. Tasks that have to occur
 * regularly (every millisecond or few) can be added 
 * to the interrupt handler (in timer0.c) or can
 * be added to the main event loop that checks the
 * clock tick value. This value (32 bits) can be 
 * obtained using the get_clock_ticks() function.
 * (Any tasks undertaken in the interrupt handler
 * should be kept short so that we don't run the 
 * risk of missing an interrupt in future.)
 */

#ifndef TIMER0_H_
#define TIMER0_H_
#include <stdint.h>
typedef int8_t bool;

/* Set up our timer to give us an interrupt every millisecond
 * and update our time reference.
 */
void init_timer0(void);
void toggle_pause(void);
bool paused;
/* Return the current clock tick value - milliseconds since the timer was
 * initialised.
 */
uint32_t get_current_time(void);
void delay_time(uint16_t scondths);
uint8_t is_delayed(void);
void buzz(uint16_t len);
void turn_on_sound(void);
void alternate_sound(void);
#endif