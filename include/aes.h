#ifndef AES_H
#define AES_H

#include "common.h"

inline void MAES_init_round(uint state[]);
inline void MAES_round(uint state[], int round);
inline void MAES_final_round(uint state[]);
inline void MAES_mix_column(uint state[], int i);

#endif

