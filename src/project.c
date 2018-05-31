/*
 * FroggerProject.c
 *
 * Main file
 *
 * Author: Peter Sutton. Modified by <YOUR NAME HERE>
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "helpers.h"
#include "ledmatrix.h"
#include "scrolling_char_display.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "score.h"
#include "timer0.h"
#include "timer1.h"

#include "game.h"
#include "display.h"
#define F_CPU 8000000L
#include <util/delay.h>
#include "highscore.h"
#include "joystick.h"
// Timer length in 10ths of seconds
#define TIMER_LENGTH 500
// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void splash_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

// ASCII code for Escape character
#define ESCAPE_CHAR 27

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete
	splash_screen();
	
	while(1) {
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void) {
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);
	init_ssg();
	init_timer0();
	init_timer1();
	init_joy();
	// Turn on global interrupts
	sei();
}

void splash_screen(void) {
	// Clear terminal screen and output a message
	clear_terminal();
	move_cursor(10,1);
	printf_P(PSTR("Frogger"));
	move_cursor(10,2);
	printf_P(PSTR("CSSE2010 project by Raghav Mishra\n"));
	move_cursor(10,3);
	printf_P(PSTR("44839370"));
	init_highscore();
	print_highscores();

	// Output the scrolling message to the LED matrix
	// and wait for a push button to be pushed.
	ledmatrix_clear();
	while(1) {
		set_scrolling_display_text("RAGHAV MISHRA 44939370", COLOUR_RED);
		// Scroll the message until it has scrolled off the 
		// display or a button is pushed
		while(scroll_display()) {
			_delay_ms(150);
			if(button_pushed() != NO_BUTTON_PUSHED) {
				return;
			}
		}
	}
	
}

void new_game(void) {
	// Initialise the game and display
	initialise_game();
	// Clear the serial terminal
	clear_terminal();
	// Initialise the score
	init_score();
	
	setup_lives();

	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}

void play_game(void) {
	int8_t button;
	char serial_input, escape_sequence_char;
	uint8_t characters_into_escape_sequence = 0;
	alternate_sound();
	uint8_t joy_direc;
	
	// Get the current time and remember this as the last time the vehicles
	// and logs were moved.
	uint32_t current_time = get_current_time();
	uint32_t last_time = current_time;
	uint32_t enable_time = current_time;
	uint32_t last_move_times[5] = {current_time, current_time, current_time, current_time, current_time};
	uint32_t move_delays[5] = {750, 850, 1300, 1000, 1100};
	uint16_t secondths_left = TIMER_LENGTH;
	set_num(secondths_left);
	uint32_t last_held_time = current_time;
	
	
	int lives = 3;
	int8_t level = 1;
	move_cursor(15, 0);
	printf_P(PSTR("LEVEL: %d"), level);
	display_lives(lives);
	move_cursor(0, 24);
	printf_P(PSTR("LIVES: %d"), lives);
	update_score(0);
	uint8_t last_joy_dir = -1;
	uint32_t last_joy_time = get_current_time();
	uint32_t last_joy_repeat = get_current_time();
	// We play the game while the frog is alive and we haven't filled up the 
	// far riverbank
	while(!is_frog_dead() && lives) {
		
		if(is_riverbank_full()){
			level++;
			remix_data();
			if(lives < 5){
				lives++;
				display_lives(lives);
			}
			move_cursor(15, 0);
			printf_P(PSTR("LEVEL: %d"), level);
			scroll_out();
			resurrect_frog();			
			for(int i=0;i<5; i++){
				move_delays[i] = move_delays[i] -  (move_delays[i] / 5);
			}
			remix_colours();
			redraw_roads();
			put_frog_in_start_position();
			reset_riverbank();
		}


		if(!is_frog_dead() && frog_has_reached_riverbank()) {
			// Frog reached the other side successfully but the
			// riverbank isn't full, put a new frog at the start
			put_frog_in_start_position();
			secondths_left = TIMER_LENGTH;
		}
		
		// Check for input - which could be a button push or serial input.
		// Serial input may be part of an escape sequence, e.g. ESC [ D
		// is a left cursor key press. At most one of the following three
		// variables will be set to a value other than -1 if input is available.
		// (We don't initalise button to -1 since button_pushed() will return -1
		// if no button pushes are waiting to be returned.)
		// Button pushes take priority over serial input. If there are both then
		// we'll retrieve the serial input the next time through this loop
		serial_input = -1;
		escape_sequence_char = -1;
		button = button_pushed();

		if(button == NO_BUTTON_PUSHED) {
			// No push button was pushed, see if there is any serial input
			if(serial_input_available()) {
				// Serial data was available - read the data from standard input
				serial_input = fgetc(stdin);
				// Check if the character is part of an escape sequence
				if(characters_into_escape_sequence == 0 && serial_input == ESCAPE_CHAR) {
					// We've hit the first character in an escape sequence (escape)
					characters_into_escape_sequence++;
					serial_input = -1; // Don't further process this character
				} else if(characters_into_escape_sequence == 1 && serial_input == '[') {
					// We've hit the second character in an escape sequence
					characters_into_escape_sequence++;
					serial_input = -1; // Don't further process this character
				} else if(characters_into_escape_sequence == 2) {
					// Third (and last) character in the escape sequence
					escape_sequence_char = serial_input;
					serial_input = -1;  // Don't further process this character - we
										// deal with it as part of the escape sequence
					characters_into_escape_sequence = 0;
				} else {
					// Character was not part of an escape sequence (or we received
					// an invalid second character in the sequence). We'll process 
					// the data in the serial_input variable.
					characters_into_escape_sequence = 0;
				}
			}
		}


		if(!paused && !is_delayed()){
			if(button == 3 || button == 2 
			|| button == 1 || button == 0){
				enable_time = current_time;
			}
			if((current_time >= last_held_time + 150)
			&& (current_time >= enable_time + 1000)) {
				last_held_time = current_time;
				int8_t a = pressed_buttons();
				if(a && !(a & (a-1))){
					switch(a){
						case 0b0001:
							button = 0;
							break;
						case 0b0010:
							button = 1;
							break;
						case 0b0100:
							button = 2;
							break;
						case 0b1000:
							button = 3;
							break;
					}
				}
			}

			if(last_joy_dir >= 0){
				joy_direc = joy_direction();
				if(joy_direc >= 0){
					if(get_current_time() >= last_joy_time + 500){
						if(get_current_time() > last_joy_repeat + 150){
							last_joy_repeat = get_current_time();
						} else {
							joy_direc = -1;
						}
					} else {
						joy_direc = -1;
					}
				}
			} else if ((joy_direc = joy_direction()) >= 0){
				last_joy_dir = joy_direc;
				last_joy_time = get_current_time();
			}
			
			
			switch(joy_direc){
				case 0:
					move_frog_to_right();
					buzz(15);
					break;
				case 1:
					move_frog_backward();
					buzz(15);
					break;
				case 2:
					move_frog_forward();
					buzz(15);
					break;
				case 3:
					move_frog_to_left();
					buzz(15);
					break;
				case 4:
					move_frog_topleft();
					buzz(15);
					break;
				case 5:
					move_frog_topright();
					buzz(15);
					break;
				case 6:
					move_frog_bottomleft();
					buzz(15);
					break;
				case 7:
					move_frog_bottomright();
					buzz(15);
					break;
			}
			// Process the input. 
			if(button==3 || escape_sequence_char=='D' || serial_input=='L' || serial_input=='l') {
				// Attempt to move left
				move_frog_to_left();
				buzz(15);
			} else if(button==2 || escape_sequence_char=='A' || serial_input=='U' || serial_input=='u' ) {
				// Attempt to move forward
				move_frog_forward();
				buzz(15);
			} else if(button==1 || escape_sequence_char=='B' || serial_input=='D' || serial_input=='d' ) {
				// Attempt to move down
				move_frog_backward();
				buzz(15);
			} else if(button==0 || escape_sequence_char=='C' || serial_input=='R' || serial_input=='r' ) {
				// Attempt to move right
				move_frog_to_right();
				buzz(15);
			}
		}

		
		if(serial_input == 'p' || serial_input == 'P') {
			pause();
		} 
		// else - invalid input or we're part way through an escape sequence -
		// do nothing
		
		current_time = get_current_time();

		if(!is_frog_dead()){
			for(int i=0; i<5; i++){
				if(current_time >=last_move_times[i] + move_delays[i]){
					if(i<3){
						scroll_vehicle_lane(i, pow(-1,i));
						last_move_times[i] = current_time;
					} else {
						scroll_river_channel(i%2, pow(-1,i));
						last_move_times[i] = current_time;
					}
				}
			}
		}
		// Handle the timer

		if(current_time >= last_time + 100){
			last_time = current_time;
			secondths_left--;
			set_num(secondths_left);
			if(secondths_left <= 0){
				kill_frog();
				secondths_left = TIMER_LENGTH;
			}
		}

		if(is_frog_dead() && lives){
			lives--;
			buzz(1000);
			delay_time(20);
			while(is_delayed()){
				;
			}
			resurrect_frog();
			put_frog_in_start_position();
			display_lives(lives);
			move_cursor(0, 24);
			printf_P(PSTR("LIVES: %d"), lives);
		}

	}
	// We get here if the riverbank is full
	// The game is over.
	char name[15];
	move_cursor(10, 10);
	turn_on_sound();
	printf_P(PSTR("What is your name? Press enter when done"));
	int i = 0;
	show_cursor();
	move_cursor(10, 11);
	int esc = 0;
	while(i < 15){
		if(serial_input_available()){
			char serial_input = fgetc(stdin);
			if(serial_input == '\n'){
				break;
			}
			if(serial_input == ESCAPE_CHAR){
				esc = 3;		
			}
			if(esc > 0){
				esc--;
				continue;
			}
			if(isalpha(serial_input)||isdigit(serial_input)||serial_input==' ') {
				name[i] = serial_input;
				putc(serial_input, stdin);
				i++;
			}

		} else {}
	}
	hide_cursor();
	set_highscore((char*) &name, get_score());
}

void handle_game_over() {
	move_cursor(10,2);
	printf_P(PSTR("GAME OVER"));
	move_cursor(10,3);
	printf_P(PSTR("Press a button to start again"));
	print_highscores();
	while(button_pushed() == NO_BUTTON_PUSHED) {
		; // wait
	}
	print_highscores();

}