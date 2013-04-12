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
#define MAES_add_round_keys_m(state, keys, start) \
	state[0] ^= keys[start];\
	state[1] ^= keys[start + 1];\
	state[2] ^= keys[start + 2];\
	state[3] ^= keys[start + 3];

#define MAES_mix_columns_m(state, f) \
	f(state, 0)\
	f(state, 1)\
	f(state, 2)\
	f(state, 3)


#define MAES_char_arr_to_uint_arr_m(dest, src_, size, offset) {\
    int i, j;\
    uchar* src = (uchar*) src_;\
    for (i = 0, j = offset; j < size; ++i, j += 4) {\
        dest[i] = src[j] << 24 | src[j + 1] << 16 | src[j + 2] << 8 | src[j + 3];\
    }\
}

#define MAES_uchar_16_to_uint_4_auto_m(dest, src, offset) \
    dest[0] = src[offset] << 24 | src[offset + 1] << 16 | src[offset + 2] << 8 | src[offset + 3];\
    offset += 4;\
    dest[1] = src[offset] << 24 | src[offset + 1] << 16 | src[offset + 2] << 8 | src[offset + 3];\
    offset += 4;\
    dest[2] = src[offset] << 24 | src[offset + 1] << 16 | src[offset + 2] << 8 | src[offset + 3];\
    offset += 4;\
    dest[3] = src[offset] << 24 | src[offset + 1] << 16 | src[offset + 2] << 8 | src[offset + 3];\
    offset += 4;

#define MAES_uint_4_to_uchar_16_auto_m(dest, src, offset) \
    dest[offset] = src[0] >> 24; dest[offset + 1] = (src[0] >> 16) & 0xff; dest[offset + 2] = (src[0] >> 8) & 0xff; dest[offset + 3] = src[0] & 0xff;\
    offset += 4;\
    dest[offset] = src[1] >> 24; dest[offset + 1] = (src[1] >> 16) & 0xff; dest[offset + 2] = (src[1] >> 8) & 0xff; dest[offset + 3] = src[1] & 0xff;\
    offset += 4;\
    dest[offset] = src[2] >> 24; dest[offset + 1] = (src[2] >> 16) & 0xff; dest[offset + 2] = (src[2] >> 8) & 0xff; dest[offset + 3] = src[2] & 0xff;\
    offset += 4;\
    dest[offset] = src[3] >> 24; dest[offset + 1] = (src[3] >> 16) & 0xff; dest[offset + 2] = (src[3] >> 8) & 0xff; dest[offset + 3] = src[3] & 0xff;\
    offset += 4;

#define MAES_uchar_16_to_uint_4_m(dest, src, offset) \
    dest[0] = src[offset] << 24 | src[offset + 1] << 16 | src[offset + 2] << 8 | src[offset + 3];\
    dest[1] = src[offset + 4] << 24 | src[offset + 5] << 16 | src[offset + 6] << 8 | src[offset + 7];\
    dest[2] = src[offset + 8] << 24 | src[offset + 9] << 16 | src[offset + 10] << 8 | src[offset + 11];\
    dest[3] = src[offset + 12] << 24 | src[offset + 13] << 16 | src[offset + 14] << 8 | src[offset + 15];\

#define MAES_uint_4_to_uchar_16_m(dest, src, offset) \
    dest[offset] = src[0] >> 24; dest[offset + 1] = (src[0] >> 16) & 0xff; dest[offset + 2] = (src[0] >> 8) & 0xff; dest[offset + 3] = src[0] & 0xff;\
    dest[offset + 4] = src[1] >> 24; dest[offset + 5] = (src[1] >> 16) & 0xff; dest[offset + 6] = (src[1] >> 8) & 0xff; dest[offset + 7] = src[1] & 0xff;\
    dest[offset + 8] = src[2] >> 24; dest[offset + 9] = (src[2] >> 16) & 0xff; dest[offset + 10] = (src[2] >> 8) & 0xff; dest[offset + 11] = src[2] & 0xff;\
    dest[offset + 12] = src[3] >> 24; dest[offset + 13] = (src[3] >> 16) & 0xff; dest[offset + 14] = (src[3] >> 8) & 0xff; dest[offset + 15] = src[3] & 0xff;\


extern int n_round;
extern int n_block;
extern int n_key;
extern uint round_keys[];

void MAES_key_schedule(uint key[]);

#endif
