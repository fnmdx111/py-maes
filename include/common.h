#ifndef COMMON_H
#define COMMON_H

#include <Python.h>

#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#define DEVNULL "/dev/null"
#elif defined(_WIN32)
#define DEVNULL "nul"
#endif

typedef unsigned char uchar;
typedef uint32_t uint;

#define MAES_rotate_left_m(x) (x << 8 | x >> 24)
#define MAES_sub_word_m(x) (sbox[x >> 24] << 24        | sbox[(x >> 16) & 0xff] << 16 |\
                            sbox[(x >> 8) & 0xff] << 8 | sbox[x & 0xff])
#define MAES_inv_sub_word_m(x) (inv_sbox[x >> 24] << 24        | inv_sbox[(x >> 16) & 0xff] << 16 |\
                                inv_sbox[(x >> 8) & 0xff] << 8 | inv_sbox[x & 0xff])
#define MAES_shift_row_m(w1, w2, w3, w4) ((w1 & 0xff000000) | (w2 & 0x00ff0000) |\
                                          (w3 & 0x0000ff00) | (w4 & 0x000000ff))

extern int n_round;
extern int n_block;
extern int n_key;
extern uint round_keys[];

void MAES_add_round_keys(uint state[], uint keys[]);
void MAES_uchar_arr_to_uint_arr(uint dest[], uchar src[], int size);
void MAES_uint_arr_to_uchar_arr(uchar dest[], uint src[], int size);
void MAES_key_schedule(uint key[]);
void MAES_mix_columns(uint state[], void (*mix_column)(uint[], int));

#endif
