#ifndef AES_H
#define AES_H

#include "common.h"

void MAES_init_round(uint state[]);
void MAES_round(uint state[], int round);
void MAES_final_round(uint state[]);
void MAES_mix_column(uint state[], int i);

#endif

