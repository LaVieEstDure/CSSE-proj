#ifndef HIGHSCORE_H_
#define HIGHSCORE_H_

typedef struct {
    char name[15];
    int8_t score;
} Entry;
void read_highscore(int8_t rank);
void init_highscore(void);
void set_highscore(int8_t rank, 
                   int8_t score,
                   char name[15]);
#endif