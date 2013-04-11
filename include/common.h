#ifndef COMMON_H
#define COMMON_H

#include <Python.h>

#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#define DEVNULL "/dev/null"
#elif defined(_WIN32)
#define DEVNULL "nul"
#endif

#if defined(_MSC_VER)
#define uint32_t unsigned __int32
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
#define MAES_add_round_keys_m(state, keys, start) {\
	state[0] ^= keys[start];\
	state[1] ^= keys[start + 1];\
	state[2] ^= keys[start + 2];\
	state[3] ^= keys[start + 3];\
}
#define MAES_mix_columns_m(state, f) {\
	f(state, 0);\
	f(state, 1);\
	f(state, 2);\
	f(state, 3);\
}

#define MAES_char_arr_to_uint_arr_m(dest, src_, size) {\
    int i, j;\
    uchar* src = (uchar*) src_;\
    for (i = j = 0; j < size; ++i, j += 4) {\
        dest[i] = src[j] << 24 | src[j + 1] << 16 | src[j + 2] << 8 | src[j + 3];\
    }\
}
#define MAES_uint_arr_to_uchar_arr_m(dest, src, size) {\
    int i, j;\
    for (i = j = 0; j < size; i += 4, ++j) {\
        dest[i] = src[j] >> 24;\
        dest[i + 1] = (src[j] >> 16) & 0xff;\
        dest[i + 2] = (src[j] >> 8) & 0xff;\
        dest[i + 3] = src[j] & 0xff;\
    }\
}

extern int n_round;
extern int n_block;
extern int n_key;
extern uint round_keys[];

void MAES_key_schedule(uint key[]);

#endif
