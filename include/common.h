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
typedef unsigned int uint;

#define DEBUG_16(param, offset) \
    printf(#param " %02x %02x %02x %02x\n", param[offset + 0x0], param[offset + 0x1], param[offset + 0x2], param[offset + 0x3]);\
    printf(#param " %02x %02x %02x %02x\n", param[offset + 0x4], param[offset + 0x5], param[offset + 0x6], param[offset + 0x7]);\
    printf(#param " %02x %02x %02x %02x\n", param[offset + 0x8], param[offset + 0x9], param[offset + 0xa], param[offset + 0xb]);\
    printf(#param " %02x %02x %02x %02x\n", param[offset + 0xc], param[offset + 0xd], param[offset + 0xe], param[offset + 0xf]);

#define MAES_add_round_keys_m(state, keys, start) \
    state[0x0] ^= keys[start], state[0x1] ^= keys[start + 1], state[0x2] ^= keys[start + 2], state[0x3] ^= keys[start + 3];\
    state[0x4] ^= keys[start + 4], state[0x5] ^= keys[start + 5], state[0x6] ^= keys[start + 6], state[0x7] ^= keys[start + 7];\
    state[0x8] ^= keys[start + 8], state[0x9] ^= keys[start + 9], state[0xa] ^= keys[start + 10], state[0xb] ^= keys[start + 11];\
    state[0xc] ^= keys[start + 12], state[0xd] ^= keys[start + 13], state[0xe] ^= keys[start + 14], state[0xf] ^= keys[start + 15];

#define MAES_mix_columns_m(state, f) \
	f(state, 0)\
	f(state, 4)\
	f(state, 8)\
	f(state, 12)


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
    dest[3] = src[offset + 12] << 24 | src[offset + 13] << 16 | src[offset + 14] << 8 | src[offset + 15];

#define MAES_uint_4_to_uchar_16_m(dest, src, offset) \
    dest[offset] = src[0] >> 24; dest[offset + 1] = (src[0] >> 16) & 0xff; dest[offset + 2] = (src[0] >> 8) & 0xff; dest[offset + 3] = src[0] & 0xff;\
    dest[offset + 4] = src[1] >> 24; dest[offset + 5] = (src[1] >> 16) & 0xff; dest[offset + 6] = (src[1] >> 8) & 0xff; dest[offset + 7] = src[1] & 0xff;\
    dest[offset + 8] = src[2] >> 24; dest[offset + 9] = (src[2] >> 16) & 0xff; dest[offset + 10] = (src[2] >> 8) & 0xff; dest[offset + 11] = src[2] & 0xff;\
    dest[offset + 12] = src[3] >> 24; dest[offset + 13] = (src[3] >> 16) & 0xff; dest[offset + 14] = (src[3] >> 8) & 0xff; dest[offset + 15] = src[3] & 0xff;

#define MAES_copy_16_m(dest, src, offset) \
    dest[0] = src[offset], dest[1] = src[offset + 1], dest[2] = src[offset + 2], dest[3] = src[offset + 3];\
    dest[4] = src[offset + 4], dest[5] = src[offset + 5], dest[6] = src[offset + 6], dest[7] = src[offset + 7];\
    dest[8] = src[offset + 8], dest[9] = src[offset + 9], dest[10] = src[offset + 10], dest[11] = src[offset + 11];\
    dest[12] = src[offset + 12], dest[13] = src[offset + 13], dest[14] = src[offset + 14], dest[15] = src[offset + 15];

#define MAES_copy_16_src_inc_m(dest, src, offset) \
    dest[0] = src[offset], dest[1] = src[offset + 1], dest[2] = src[offset + 2], dest[3] = src[offset + 3];\
    offset += 4;\
    dest[4] = src[offset], dest[5] = src[offset + 1], dest[6] = src[offset + 2], dest[7] = src[offset + 3];\
    offset += 4;\
    dest[8] = src[offset], dest[9] = src[offset + 1], dest[10] = src[offset + 2], dest[11] = src[offset + 3];\
    offset += 4;\
    dest[12] = src[offset], dest[13] = src[offset + 1], dest[14] = src[offset + 2], dest[15] = src[offset + 3];\
    offset += 4;

#define MAES_copy_16_dest_inc_m(dest, src, offset) \
    dest[offset] = src[0], dest[offset + 1] = src[1], dest[offset + 2] = src[2], dest[offset + 3] = src[3];\
    offset += 4;\
    dest[offset] = src[4], dest[offset + 1] = src[5], dest[offset + 2] = src[6], dest[offset + 3] = src[7];\
    offset += 4;\
    dest[offset] = src[8], dest[offset + 1] = src[9], dest[offset + 2] = src[10], dest[offset + 3] = src[11];\
    offset += 4;\
    dest[offset] = src[12], dest[offset + 1] = src[13], dest[offset + 2] = src[14], dest[offset + 3] = src[15];\
    offset += 4;

extern int n_round;
extern int n_block;
extern int n_key;
extern uint round_keys[];

void MAES_key_schedule(uint key[]);

#endif

