#ifndef JOYSTICK_H_
#define JOYSTICK_H_
#include <stdint.h>

void init_joy(void);
void joy_meas(void);
int8_t joy_direction(void);
#endif