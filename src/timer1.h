#ifndef TIMER1_H_
#define TIMER1_H_
void init_timer1(void);
void start_buzz(uint16_t length);
void turnon_buzz(void);
void turnoff_buzz(void);

void set_frequency(uint16_t freq);
void check_muted(void);
void mute(void);
void unmute(void);
#endif