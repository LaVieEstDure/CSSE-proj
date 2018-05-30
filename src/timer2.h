
#ifndef TIMER2_H_
#define TIMER2_H_
#include <stdint.h>
/* Set up our timer to give us an interrupt every millisecond
 * and update our time reference.
 */
void init_timer2(void);
/* Return the current clock tick value - milliseconds since the timer was
 * initialised.
 */
uint32_t get_current_time_2(void);

#endif