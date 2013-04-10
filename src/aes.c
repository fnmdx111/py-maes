#include <common.h>
#include <tables.h>


inline void
MAES_sub_words(uint state[])
{
    state[0] = MAES_sub_word_m(state[0]);
    state[1] = MAES_sub_word_m(state[1]);
    state[2] = MAES_sub_word_m(state[2]);
    state[3] = MAES_sub_word_m(state[3]);
}

inline void
MAES_shift_rows(uint state[])
{
    uint temp[4] = {state[0], state[1], state[2], state[3]};

    state[0] = MAES_shift_row_m(temp[0], temp[1], temp[2], temp[3]);
    state[1] = MAES_shift_row_m(temp[1], temp[2], temp[3], temp[0]);
    state[2] = MAES_shift_row_m(temp[2], temp[3], temp[0], temp[1]);
    state[3] = MAES_shift_row_m(temp[3], temp[0], temp[1], temp[2]);
}

inline void
MAES_mix_column(uint state[],
                int  i)
{
    uint byte1 = state[i] >> 24,
         byte2 = (state[i] >> 16) & 0xff,
         byte3 = (state[i] >> 8) & 0xff,
         byte4 = state[i] & 0xff;

    state[i] = 0;
    state[i] |= (mul0x2[byte1] ^ mul0x3[byte2] ^ byte3 ^ byte4) << 24;
    state[i] |= (byte1 ^ mul0x2[byte2] ^ mul0x3[byte3] ^ byte4) << 16;
    state[i] |= (byte1 ^ byte2 ^ mul0x2[byte3] ^ mul0x3[byte4]) << 8;
    state[i] |= (mul0x3[byte1] ^ byte2 ^ byte3 ^ mul0x2[byte4]);
}


inline void
MAES_init_round(uint state[])
{
    MAES_add_round_keys(state, &round_keys[0]);
}

inline void
MAES_round(uint state[],
           int  round)
{
    MAES_sub_words(state);
    MAES_shift_rows(state);
    MAES_mix_columns(state, MAES_mix_column);
    MAES_add_round_keys(state, &round_keys[round * 4]);
}

inline void
MAES_final_round(uint state[])
{
    MAES_sub_words(state);
    MAES_shift_rows(state);
    MAES_add_round_keys(state, &round_keys[n_round * 4]);
}

