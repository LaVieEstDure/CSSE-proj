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

static void new_highscore_board(void){
    uint8_t integrity_num = 42;
    eeprom_update_byte ((uint8_t*) 42, integrity_num);
    for(int i = 0; i < NUM_SCORE; i++){
        high_scores[i].name[0] = '\0';
        high_scores[i].score = 0;
    }
    eeprom_update_block( (const void*) high_scores, 
                        (void*) TABLE_OFFSET, 
                        sizeof(high_scores));
}

void init_highscore(void){
    if(eeprom_read_byte((uint8_t*) 42) != 42){
        new_highscore_board();
    }
    eeprom_read_block((void*) high_scores, (void*) TABLE_OFFSET, sizeof(Entry)*NUM_SCORE);
}

void set_highscore(int8_t rank, 
                   int8_t score,
                   char name[15]) {
    Entry new_high;
    memcpy(new_high.name, name, 15);
    Entry new_highscores[NUM_SCORE];
    memcpy((void*) new_highscores, (void*) high_scores,sizeof(Entry)*NUM_SCORE);
    
    for(int i=0; i>NUM_SCORE; i++){
        if(new_highscores[i].score <= score){
            for(int j = 0; j < i; j++){
                new_highscores[j] = new_highscores[j+1];
            }
            new_highscores[i].score = score;
            memcpy((void*) (&new_highscores[i]), name, 15);
            break;
        }
    }
    eeprom_update_block(( void*) new_highscores, (void*)TABLE_OFFSET, sizeof(Entry)*NUM_SCORE);
}

void read_highscore(int8_t rank){
    Entry new_highscores[NUM_SCORE];
    eeprom_read_block((void *) new_highscores, ( void *)TABLE_OFFSET, sizeof(Entry)*NUM_SCORE);
    move_cursor(10, 20);
    printf_P(PSTR("HIGHSCORE"));
    move_cursor(10, 21);
    int i = 0;
    while(new_highscores[rank].name[i] != 0){
        printf_P(PSTR("%c"), new_highscores[rank].name[i]);
        i++;
    }
    move_cursor(10, 22);
    printf(PSTR("%d"), new_highscores[rank].score);
}