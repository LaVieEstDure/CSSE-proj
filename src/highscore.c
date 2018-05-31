#include <avr/eeprom.h>
#include <stdint.h>
#include <string.h>
#include "highscore.h"
#include <avr/pgmspace.h>
#include "terminalio.h"
#include <stdio.h>

#define TABLE_OFFSET 45
#define NUM_SCORE 5

static Entry high_scores[NUM_SCORE];

void new_highscore_board(void){
    uint8_t integrity_num = 42;
    eeprom_update_byte ((uint8_t*) 42, integrity_num);
    for(int i = 0; i < NUM_SCORE; i++){
        high_scores[i].name[0] = '\0';
        high_scores[i].score = 0;
    }
    eeprom_update_block((const void*) &high_scores, 
                        (void*) TABLE_OFFSET, 
                        sizeof(Entry)*NUM_SCORE);
}

void init_highscore(void){
    if(eeprom_read_byte((uint8_t*) 42) != 42){
        new_highscore_board();
    }
    eeprom_read_block((void*) &high_scores, 
                      (const void*) TABLE_OFFSET, 
                      sizeof(Entry)*NUM_SCORE);
}

void set_highscore(char *name, uint8_t score){
    for(int i = 0; i < NUM_SCORE; i++){
        if(high_scores[i].score > score){
            continue;
        } else {
            for(int j = NUM_SCORE - 1; j > i; j--){
                high_scores[j].score = high_scores[j-1].score;
                memcpy(&high_scores[i].name, &high_scores[i-1].name, sizeof(char)*15);
            }
            high_scores[i].score = score;
            memcpy(&high_scores[i], name, sizeof(char)*15);
            break;
        }
    }
    eeprom_update_block((const void*) &high_scores, 
                        (void*) TABLE_OFFSET, 
                        sizeof(Entry)*NUM_SCORE);
}

void print_highscores(void){
    move_cursor(16, 9);
    printf_P(PSTR("HIGSCORES"));
    for(int i=0; i < NUM_SCORE; i++){
        move_cursor(10,10 + i);
        clear_to_end_of_line();
        if(high_scores[i].score != 0){
            printf_P(PSTR("%d. Name: %s"),
                        i, 
                        &high_scores[i].name); 
            move_cursor(25,10 + i);
            printf_P(PSTR("Score: %d"), high_scores[i].score);
        }
    }
}

