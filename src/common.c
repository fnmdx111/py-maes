#include <common.h>
#include <tables.h>

int n_round,
    n_block = 4,
    n_key;

uint round_keys[60];


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



