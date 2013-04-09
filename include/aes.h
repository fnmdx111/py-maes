#ifndef AES_H
#define AES_H

#include "common.h"

extern int n_round;
extern int n_block;
extern int n_key;
extern uint round_keys[];

void MAES_add_round_keys(uint state[], uint keys[]);
void MAES_mix_columns(uint state[], void (*mix_column)(uint[], int));

#endif
