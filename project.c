#include <avr/io.h>
/* Seven segment display values */
uint8_t seven_seg[10] = { 63,6,91,79,102,109,125,7,127,111};
int main(void) {
    uint8_t digit;
    /* Set port A pins to be outputs, port C pins to be inputs */
    DDRA = 0xFF;
    DDRC = 0; /* This is the default, could omit. */
    while(1) {
        /* Read in a digit from lower half of port C pins */
        /* We read the whole byte and mask out upper bits */
        digit = PINC & 0xFF;
        /* Write out seven segment display value to port A */
        if(digit < 16) {
            PORTA = digit;
        } else {
            PORTA = 0;
        }
    }
}
