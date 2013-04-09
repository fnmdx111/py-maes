#include "../include/common.h"

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

