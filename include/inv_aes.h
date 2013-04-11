#ifndef INV_AES_H
#define INV_AES_H

#include "common.h"
#define MAES_inv_mix_column_m(state, i) {\
    uint byte1 = state[i] >> 24,\
         byte2 = (state[i] >> 16) & 0xff,\
         byte3 = (state[i] >> 8) & 0xff,\
         byte4 = state[i] & 0xff;\
    state[i] = 0;\
    state[i] |= (mul0xe[byte1] ^ mul0xb[byte2] ^ mul0xd[byte3] ^ mul0x9[byte4]) << 24;\
    state[i] |= (mul0x9[byte1] ^ mul0xe[byte2] ^ mul0xb[byte3] ^ mul0xd[byte4]) << 16;\
    state[i] |= (mul0xd[byte1] ^ mul0x9[byte2] ^ mul0xe[byte3] ^ mul0xb[byte4]) << 8;\
    state[i] |= (mul0xb[byte1] ^ mul0xd[byte2] ^ mul0x9[byte3] ^ mul0xe[byte4]);\
}

#define MAES_inv_sub_words_m(state) {\
    state[0] = MAES_inv_sub_word_m(state[0]);\
    state[1] = MAES_inv_sub_word_m(state[1]);\
    state[2] = MAES_inv_sub_word_m(state[2]);\
    state[3] = MAES_inv_sub_word_m(state[3]);\
}
#define MAES_inv_shift_rows_m(state) {\
    uint temp[4] = {state[0], state[1], state[2], state[3]};\
    state[0] = MAES_shift_row_m(temp[0], temp[3], temp[2], temp[1]);\
    state[1] = MAES_shift_row_m(temp[1], temp[0], temp[3], temp[2]);\
    state[2] = MAES_shift_row_m(temp[2], temp[1], temp[0], temp[3]);\
    state[3] = MAES_shift_row_m(temp[3], temp[2], temp[1], temp[0]);\
}
#define MAES_inv_init_round_m(state) MAES_add_round_keys_m(state, round_keys, n_round * 4)
#define MAES_inv_round_m(state, round) {\
    MAES_inv_shift_rows_m(state);\
    MAES_inv_sub_words_m(state);\
    MAES_add_round_keys_m(state, round_keys, round * 4);\
    MAES_mix_columns_m(state, MAES_inv_mix_column_m);\
}
#define MAES_inv_final_round_m(state) {\
    MAES_inv_shift_rows_m(state);\
    MAES_inv_sub_words_m(state);\
    MAES_add_round_keys_m(state, round_keys, 0);\
}

#endif
