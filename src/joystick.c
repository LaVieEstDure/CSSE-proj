#include <avr/io.h>
#include "joystick.h"
#include "timer0.h"

static uint16_t x, y;
static uint32_t prev_t;
static uint8_t p_dir;

void init_joy(void) {
  ADMUX = (1<<REFS0);
  ADMUX |= 0b110;
  ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);
}

void measure_joystick(void) {
    ADMUX &= ~1;
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC)) {
        ; /* Wait until conversion finished */
    }
    x = ADC;

    // get joystick y data
    ADMUX |= 1;
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC)) {
        ; /* Wait until conversion finished */
    }
    y = ADC;
}

uint16_t measure_x(void){
    return x;
}
uint16_t measure_y(void){
    return y;
}
int8_t joy_direction(void) {
  /*
   *  3 - Left
   *  2 - Forward
   *  1 - Backward
   *  0 - Right
   *  -1 - Rest
   */
  uint8_t dir;
  uint32_t t;
  measure_joystick();

  if(x < 150) {
    dir = 3;
  } else if(y > 800) {
    dir = 2;
  } else if(y < 150) {
    dir = 1;
  } else if(x > 800) {
    dir = 0;
  } else {
    dir = -1;
  }

  if(dir >= 0) { //If a direction has been selected, not rest
    if (p_dir == dir) { //If the current direction is the same as the last direction
      t = get_current_time();
      if(t < prev_t + 250) { // Has 250ms elapsed since last direction
        return -1;
      }
    }

    p_dir = dir;
    prev_t = get_current_time();
    return dir;
  } else {
    return -1; // dir is -1
  }
}
