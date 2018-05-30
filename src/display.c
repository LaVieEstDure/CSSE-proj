#include "display.h"
#include <avr/io.h>

static uint8_t seven_seg[10] = { 63,6,91,79,102,109,125,7,127,111};
static uint16_t num_displayed;
void init_ssg(){
    DDRC = 0xFF;
    DDRA |= 0b10000;
}

void set_num(uint16_t num){
    num_displayed = num;
}

void switch_disp(){
    if(!((PORTA >> 4)&1)){
        if(num_displayed >= 100){
            PORTC = seven_seg[(num_displayed / 100) % 100];
        } else if ((num_displayed < 100) & (num_displayed >= 10)){
            PORTC = 0;
        } else {
            PORTC = 0b10000000 | seven_seg[0];
        }
    } else { 
        if(num_displayed <= 10){
            PORTC = seven_seg[num_displayed % 10];
        } else {
            PORTC = seven_seg[num_displayed / 10 % 10];
        }
    }
    PORTA ^= 1 << 4;
}   