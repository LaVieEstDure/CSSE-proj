#include "helpers.h"
#include "score.h"
#include "terminalio.h"
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdio.h>
#include "timer0.h"
#define SET_BOTTOM(x, y) x=((x & 0xF0)|y)


void update_score(int score){
	add_to_score(score);
	move_cursor(65,24);
	printf_P(PSTR("SCORE:"));
	move_cursor(75, 24);
	printf_P(PSTR("%8d"), get_score());
}

void setup_lives(){
	DDRA &= ~0xF;
	DDRA |= 0xF;
}

void display_lives(int8_t lives){
	int8_t out = 0;
	while (lives>=1){
		out = (out << 1) + 1;
		lives--;
	}
	PORTA &= ~0xF;
	PORTA |= out;
}

void pause(){
	toggle_pause();
}