#ifndef AES_H
#define AES_H

#include "common.h"

#define MAES_mix_column_m(state, i) \
    temp[0] = state[i], temp[1] = state[i + 1], temp[2] = state[i + 2], temp[3] = state[i + 3];\
    state[i] = mul0x2[temp[0]] ^ mul0x3[temp[1]] ^ temp[2] ^ temp[3],\
    state[i + 1] = temp[0] ^ mul0x2[temp[1]] ^ mul0x3[temp[2]] ^ temp[3],\
    state[i + 2] = temp[0] ^ temp[1] ^ mul0x2[temp[2]] ^ mul0x3[temp[3]],\
    state[i + 3] = mul0x3[temp[0]] ^ temp[1] ^ temp[2] ^ mul0x2[temp[3]];

#define MAES_sub_words_m(state) \
    state[0x0] = sbox[state[0x0]], state[0x1] = sbox[state[0x1]], state[0x2] = sbox[state[0x2]], state[0x3] = sbox[state[0x3]];\
    state[0x4] = sbox[state[0x4]], state[0x5] = sbox[state[0x5]], state[0x6] = sbox[state[0x6]], state[0x7] = sbox[state[0x7]];\
    state[0x8] = sbox[state[0x8]], state[0x9] = sbox[state[0x9]], state[0xa] = sbox[state[0xa]], state[0xb] = sbox[state[0xb]];\
    state[0xc] = sbox[state[0xc]], state[0xd] = sbox[state[0xd]], state[0xe] = sbox[state[0xe]], state[0xf] = sbox[state[0xf]];

#define MAES_shift_rows_m(state) \
    temp[0x0] = state[0x0], temp[0x1] = state[0x1], temp[0x2] = state[0x2], temp[0x3] = state[0x3];\
    temp[0x4] = state[0x4], temp[0x5] = state[0x5], temp[0x6] = state[0x6], temp[0x7] = state[0x7];\
    temp[0x8] = state[0x8], temp[0x9] = state[0x9], temp[0xa] = state[0xa], temp[0xb] = state[0xb];\
    temp[0xc] = state[0xc], temp[0xd] = state[0xd], temp[0xe] = state[0xe], temp[0xf] = state[0xf];\
    state[0x0] = temp[0x0], state[0x1] = temp[0x5], state[0x2] = temp[0xa], state[0x3] = temp[0xf];\
    state[0x4] = temp[0x4], state[0x5] = temp[0x9], state[0x6] = temp[0xe], state[0x7] = temp[0x3];\
    state[0x8] = temp[0x8], state[0x9] = temp[0xd], state[0xa] = temp[0x2], state[0xb] = temp[0x7];\
    state[0xc] = temp[0xc], state[0xd] = temp[0x1], state[0xe] = temp[0x6], state[0xf] = temp[0xb];


#define MAES_init_round_m(state) MAES_add_round_keys_m(state, round_keys, 0)
#define MAES_round_m(state, round) \
    MAES_sub_words_m(state)\
    MAES_shift_rows_m(state)\
    MAES_mix_columns_m(state, MAES_mix_column_m)\
    MAES_add_round_keys_m(state, round_keys, round * 16)

#define MAES_final_round_m(state) \
    MAES_sub_words_m(state)\
    MAES_shift_rows_m(state)\
    MAES_add_round_keys_m(state, round_keys, n_round * 16)


#endif

