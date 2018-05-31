#include <avr/io.h>
#include "joystick.h"
#include "timer0.h"

static uint16_t x, y;

void init_joy(void) {
  ADMUX = (1<<REFS0);
  ADMUX |= 0b110;
  ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);
}

void measure_joystick(void) {
    ADMUX &= ~1;
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC)) {
        ; 
    }
    x = ADC;

    
    ADMUX |= 1;
    ADCSRA |= (1<<ADSC);
    while(ADCSRA & (1<<ADSC)) {
        ; 
    }
    y = ADC;
}

int8_t joy_direction(void) {
  uint8_t dir;
  measure_joystick();
  int8_t xup = 0;
  int8_t yup = 0;
  if(x > 800) { xup = -1; }
  if(x < 150) { xup = 1;}
  if(y > 800) { yup = 1; }
  if(y < 150) { yup = -1;}

  if(xup == 1 && yup == 1){ 
    dir = 4; // top left
  } else if(xup == -1 && yup == 1){
    dir = 5; // top right
  } else if(xup == 1 && yup == -1){
    dir = 6; // bottom left
  } else if(xup == -1 && yup == -1){
    dir = 7; // bottom right
  } else if (xup == 1&& yup == 0) {
    dir = 3;
  } else if(yup == 1 && xup == 0) {
    dir = 2;
  } else if(yup == -1 && xup == 0) {
    dir = 1;
  } else if(xup == -1 && yup == 0) {
    dir = 0;
  } else {
    dir = -1;
  }
  return dir;  
}
