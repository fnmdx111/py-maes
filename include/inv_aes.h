#ifndef INV_AES_H
#define INV_AES_H

#include "common.h"

void MAES_inv_sub_words(uint state[]);
void MAES_inv_shift_rows(uint state[]);
void MAES_inv_mix_column(uint state[], int i);
void MAES_inv_init_round(uint state[]);
void MAES_inv_round(uint state[], int round);
void MAES_inv_final_round(uint state[]);

#endif
