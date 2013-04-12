#include <common.h>
#include <tables.h>

int n_round,
    n_block = 4,
    n_key;

uint round_keys[60 * 4];

#define MAES_rotate_left_m(x) (x << 8 | x >> 24)
#define MAES_sub_word_m(x) (sbox[x >> 24] << 24        | sbox[(x >> 16) & 0xff] << 16 |\
                            sbox[(x >> 8) & 0xff] << 8 | sbox[x & 0xff])

void
MAES_key_schedule(uint key[])
{
    int i;
    uint temp, t_keys[60];

    for (i = 0; i < n_key; t_keys[i] = key[i], ++i)
        ;
    for (; i < n_block * (n_round + 1); ++i) {
        temp = t_keys[i - 1];
        if (!(i % n_key)) {
            temp = MAES_sub_word_m(MAES_rotate_left_m(temp)) ^ rcon[i / n_key];
        } else if (n_key > 6 && i % n_key == 4) {
            temp = MAES_sub_word_m(temp);
        }
        t_keys[i] = t_keys[i - n_key] ^ temp;
    }

    for (i = 0; i < n_block * (n_round + 1); ++i) {
        round_keys[i * 4] = t_keys[i] >> 24;
        round_keys[i * 4 + 1] = (t_keys[i] >> 16) & 0xff;
        round_keys[i * 4 + 2] = (t_keys[i] >> 8) & 0xff;
        round_keys[i * 4 + 3] = t_keys[i] & 0xff;
    }
}



