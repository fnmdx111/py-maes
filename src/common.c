#include <common.h>
#include <tables.h>

int n_round,
    n_block = 4,
    n_key;

uint t_keys[60];

uint round_keys[60 * 4];

#define MAES_rotate_left_m(x) (x << 8 | x >> 24)
#define MAES_sub_word_m(x) (sbox[x >> 24] << 24        | sbox[(x >> 16) & 0xff] << 16 |\
                            sbox[(x >> 8) & 0xff] << 8 | sbox[x & 0xff])
#define MAES_inv_sub_word_m(x) (inv_sbox[x >> 24] << 24        | inv_sbox[(x >> 16) & 0xff] << 16 |\
                                inv_sbox[(x >> 8) & 0xff] << 8 | inv_sbox[x & 0xff])
#define MAES_shift_row_m(w1, w2, w3, w4) ((w1 & 0xff000000) | (w2 & 0x00ff0000) |\
                                          (w3 & 0x0000ff00) | (w4 & 0x000000ff))
void
MAES_key_schedule(uint key[])
{
    int i;
    uint temp;

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
        printf("%08x\n", t_keys[i]);
    }

    for (i = 0; i < n_block * (n_round + 1); ++i) {
        round_keys[i * 4] = t_keys[i] >> 24;
        round_keys[i * 4 + 1] = (t_keys[i] >> 16) & 0xff;
        round_keys[i * 4 + 2] = (t_keys[i] >> 8) & 0xff;
        round_keys[i * 4 + 3] = t_keys[i] & 0xff;
    }
}



