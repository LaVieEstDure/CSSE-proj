#ifndef HIGHSCORE_H_
#define HIGHSCORE_H_

typedef struct {
    char name[15];
    int8_t score;
} Entry;
void read_highscore(int8_t rank);
void init_highscore(void);
void print_highscores(void);
void new_highscore_board(void);
void set_highscore(char* name, uint8_t score);
#endif