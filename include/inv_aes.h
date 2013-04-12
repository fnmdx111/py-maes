#ifndef INV_AES_H
#define INV_AES_H

#include "common.h"
#define MAES_inv_mix_column_m(state, i) \
    temp[0] = state[i] >> 24,\
    temp[1] = (state[i] >> 16) & 0xff,\
    temp[2] = (state[i] >> 8) & 0xff,\
    temp[3] = state[i] & 0xff,\
    state[i] = ((mul0xe[temp[0]] ^ mul0xb[temp[1]] ^ mul0xd[temp[2]] ^ mul0x9[temp[3]]) << 24) |\
               ((mul0x9[temp[0]] ^ mul0xe[temp[1]] ^ mul0xb[temp[2]] ^ mul0xd[temp[3]]) << 16) |\
               ((mul0xd[temp[0]] ^ mul0x9[temp[1]] ^ mul0xe[temp[2]] ^ mul0xb[temp[3]]) << 8)  |\
               (mul0xb[temp[0]] ^ mul0xd[temp[1]] ^ mul0x9[temp[2]] ^ mul0xe[temp[3]]);


#define MAES_inv_sub_words_m(state) \
    state[0] = MAES_inv_sub_word_m(state[0]);\
    state[1] = MAES_inv_sub_word_m(state[1]);\
    state[2] = MAES_inv_sub_word_m(state[2]);\
    state[3] = MAES_inv_sub_word_m(state[3]);\

#define MAES_inv_shift_rows_m(state) \
    temp[0] = state[0], temp[1] = state[1], temp[2] = state[2],  temp[3] = state[3];\
    state[0] = MAES_shift_row_m(temp[0], temp[3], temp[2], temp[1]);\
    state[1] = MAES_shift_row_m(temp[1], temp[0], temp[3], temp[2]);\
    state[2] = MAES_shift_row_m(temp[2], temp[1], temp[0], temp[3]);\
    state[3] = MAES_shift_row_m(temp[3], temp[2], temp[1], temp[0]);\

#define MAES_inv_init_round_m(state) MAES_add_round_keys_m(state, round_keys, n_round * 4)
#define MAES_inv_round_m(state, round) \
    MAES_inv_shift_rows_m(state)\
    MAES_inv_sub_words_m(state)\
    MAES_add_round_keys_m(state, round_keys, round * 4)\
    MAES_mix_columns_m(state, MAES_inv_mix_column_m)\

#define MAES_inv_final_round_m(state) \
    MAES_inv_shift_rows_m(state)\
    MAES_inv_sub_words_m(state)\
    MAES_add_round_keys_m(state, round_keys, 0)\


#endif
