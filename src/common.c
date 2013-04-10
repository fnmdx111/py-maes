#include <common.h>
#include <tables.h>

int n_round,
    n_block = 4,
    n_key;

uint round_keys[60];


inline void
MAES_add_round_keys(uint state[],
                    uint keys[])
{
    state[0] ^= keys[0];
    state[1] ^= keys[1];
    state[2] ^= keys[2];
    state[3] ^= keys[3];
}

void
MAES_uchar_arr_to_uint_arr(uint dest[],
                           uchar src[],
                           int size)
{
    int i, j;
    for (i = j = 0; j < size; ++i, j += 4) {
        dest[i] = src[j] << 24 | src[j + 1] << 16 | src[j + 2] << 8 | src[j + 3];
    }
}

void
MAES_uint_arr_to_uchar_arr(uchar dest[],
                           uint src[],
                           int size)
{
    int i, j;
    for (i = j = 0; j < size; i += 4, ++j) {
        dest[i] = src[j] >> 24;
        dest[i + 1] = (src[j] >> 16) & 0xff;
        dest[i + 2] = (src[j] >> 8) & 0xff;
        dest[i + 3] = src[j] & 0xff;
    }
}

void
MAES_key_schedule(uint key[])
{
    int i;
    uint temp;

    for (i = 0; i < n_key; round_keys[i] = key[i], ++i)
        ;
    for (; i < n_block * (n_round + 1); ++i) {
        temp = round_keys[i - 1];
        if (!(i % n_key)) {
            temp = MAES_sub_word_m(MAES_rotate_left_m(temp)) ^ rcon[i / n_key];
        } else if (n_key > 6 && i % n_key == 4) {
            temp = MAES_sub_word_m(temp);
        }
        round_keys[i] = round_keys[i - n_key] ^ temp;
    }
}


inline void
MAES_mix_columns(uint state[],
                 void (*mix_column)(uint[], int))
{
    mix_column(state, 0);
    mix_column(state, 1);
    mix_column(state, 2);
    mix_column(state, 3);
}


