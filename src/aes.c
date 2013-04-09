#include <Python.h>
#include "../include/tables.h"

#define MAES_rotate_left_m(x) (x << 8 | x >> 24)
#define MAES_sub_word_m(x) (sbox[x >> 24] << 24        | sbox[(x >> 16) & 0xff] << 16 |\
                            sbox[(x >> 8) & 0xff] << 8 | sbox[x & 0xff])
#define MAES_inv_sub_word_m(x) (inv_sbox[x >> 24] << 24        | inv_sbox[(x >> 16) & 0xff] << 16 |\
                                inv_sbox[(x >> 8) & 0xff] << 8 | inv_sbox[x & 0xff])
#define MAES_shift_row_m(w1, w2, w3, w4) ((w1 & 0xff000000) | (w2 & 0x00ff0000) |\
                                          (w3 & 0x0000ff00) | (w4 & 0x000000ff))

static PyObject* InvalidInputLength;
static PyObject* InvalidKeyLength;

int n_round,
    n_block = 4,
    n_key;

uint round_keys[60];

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
MAES_sub_words(uint state[])
{
    state[0] = MAES_sub_word_m(state[0]);
    state[1] = MAES_sub_word_m(state[1]);
    state[2] = MAES_sub_word_m(state[2]);
    state[3] = MAES_sub_word_m(state[3]);
    printf("sub_words: %08x %08x %08x %08x\n", state[0], state[1], state[2], state[3]);
}

inline void
MAES_inv_sub_words(uint state[])
{
    state[0] = MAES_inv_sub_word_m(state[0]);
    state[1] = MAES_inv_sub_word_m(state[1]);
    state[2] = MAES_inv_sub_word_m(state[2]);
    state[3] = MAES_inv_sub_word_m(state[3]);
    printf("inv_sub_words: %08x %08x %08x %08x\n", state[0], state[1], state[2], state[3]);
}

inline void
MAES_shift_rows(uint state[])
{
    uint temp[4] = {state[0], state[1], state[2], state[3]};

    state[0] = MAES_shift_row_m(temp[0], temp[1], temp[2], temp[3]);
    state[1] = MAES_shift_row_m(temp[1], temp[2], temp[3], temp[0]);
    state[2] = MAES_shift_row_m(temp[2], temp[3], temp[0], temp[1]);
    state[3] = MAES_shift_row_m(temp[3], temp[0], temp[1], temp[2]);

    printf("shift_rows: %08x %08x %08x %08x\n", state[0], state[1], state[2], state[3]);
}

inline void
MAES_inv_shift_rows(uint state[])
{
    uint temp[4] = {state[0], state[1], state[2], state[3]};

    state[0] = MAES_shift_row_m(temp[0], temp[3], temp[2], temp[1]);
    state[1] = MAES_shift_row_m(temp[1], temp[0], temp[3], temp[2]);
    state[2] = MAES_shift_row_m(temp[2], temp[1], temp[0], temp[3]);
    state[3] = MAES_shift_row_m(temp[3], temp[2], temp[1], temp[0]);

    printf("inv_shift_rows: %08x %08x %08x %08x\n", state[0], state[1], state[2], state[3]);
}

void
MAES_mix_column(uint state[],
                int  i)
{
    uint byte1 = state[i] >> 24,
         byte2 = (state[i] >> 16) & 0xff,
         byte3 = (state[i] >> 8) & 0xff,
         byte4 = state[i] & 0xff;

    state[i] = 0;
    state[i] |= (mul0x2[byte1] ^ mul0x3[byte2] ^ byte3 ^ byte4) << 24;
    state[i] |= (byte1 ^ mul0x2[byte2] ^ mul0x3[byte3] ^ byte4) << 16;
    state[i] |= (byte1 ^ byte2 ^ mul0x2[byte3] ^ mul0x3[byte4]) << 8;
    state[i] |= (mul0x3[byte1] ^ byte2 ^ byte3 ^ mul0x2[byte4]);
}

void
MAES_inv_mix_column(uint state[],
                    int i)
{
    uint byte1 = state[i] >> 24,
         byte2 = (state[i] >> 16) & 0xff,
         byte3 = (state[i] >> 8) & 0xff,
         byte4 = state[i] & 0xff;

    state[i] = 0;
    state[i] |= (mul0xe[byte1] ^ mul0xb[byte2] ^ mul0xd[byte3] ^ mul0x9[byte4]) << 24;
    state[i] |= (mul0x9[byte1] ^ mul0xe[byte2] ^ mul0xb[byte3] ^ mul0xd[byte4]) << 16;
    state[i] |= (mul0xd[byte1] ^ mul0x9[byte2] ^ mul0xe[byte3] ^ mul0xb[byte4]) << 8;
    state[i] |= (mul0xb[byte1] ^ mul0xd[byte2] ^ mul0x9[byte3] ^ mul0xe[byte4]);
}

inline void
MAES_mix_columns(uint state[],
                 void (*mix_column)(uint[], int))
{
    mix_column(state, 0);
    mix_column(state, 1);
    mix_column(state, 2);
    mix_column(state, 3);
    printf("mix_columns: %08x %08x %08x %08x\n", state[0], state[1], state[2], state[3]);
}

inline void
MAES_add_round_keys(uint state[],
                    uint keys[])
{
    state[0] ^= keys[0];
    state[1] ^= keys[1];
    state[2] ^= keys[2];
    state[3] ^= keys[3];
    printf("round_key: %08x %08x %08x %08x\n", keys[0], keys[1], keys[2], keys[3]);
    printf("add_round_keys: %08x %08x %08x %08x\n", state[0], state[1], state[2], state[3]);
}

void
MAES_init_round(uint state[])
{
    printf("--- initial round ---\n");

    MAES_add_round_keys(state, &round_keys[0]);
}

void
MAES_round(uint state[],
           int  round)
{
    printf("--- %d round ---\n", round);

    MAES_sub_words(state);
    MAES_shift_rows(state);
    MAES_mix_columns(state, MAES_mix_column);
    MAES_add_round_keys(state, &round_keys[round * 4]);
}

void
MAES_final_round(uint state[])
{
    printf("--- final round ---\n");

    MAES_sub_words(state);
    MAES_shift_rows(state);
    MAES_add_round_keys(state, &round_keys[n_round * 4]);
}

void
MAES_inv_init_round(uint state[])
{
    printf("--- first inv round ---\n");
    MAES_add_round_keys(state, &round_keys[n_round * 4]);
}

void
MAES_inv_round(uint state[],
               int  round)
{
    printf("--- %d inv round ---\n", round);
    MAES_inv_shift_rows(state);
    MAES_inv_sub_words(state);
    MAES_add_round_keys(state, &round_keys[round * 4]);
    MAES_mix_columns(state, MAES_inv_mix_column);
}

void
MAES_inv_final_round(uint state[])
{
    printf("--- final inv round ---\n");
    MAES_inv_shift_rows(state);
    MAES_inv_sub_words(state);
    MAES_add_round_keys(state, &round_keys[0]);
}

static PyObject*
MAES_test_mix_columns(PyObject* self, PyObject* args)
{
    uint state[] = {0xdbf2d42d, 0x130ad426, 0x5322d431, 0x455cd54c};
    printf("before:  %x %x %x %x\n", state[0], state[1], state[2], state[3]);
    MAES_mix_column(state, 0);
    printf("1st col: %x %x %x %x\n", state[0], state[1], state[2], state[3]);
    MAES_mix_column(state, 1);
    printf("2nd col: %x %x %x %x\n", state[0], state[1], state[2], state[3]);
    MAES_mix_column(state, 2);
    printf("3rd col: %x %x %x %x\n", state[0], state[1], state[2], state[3]);
    MAES_mix_column(state, 3);
    printf("4th col: %x %x %x %x\n", state[0], state[1], state[2], state[3]);

    return Py_BuildValue("IIII", state[0], state[1], state[2], state[3]);
}

static PyObject*
MAES_encrypt(PyObject* self,
             PyObject* args)
{
	int	ok, i;
	int plaintext_size,
		key_size = 0;

	char* plaintext;
	char* key = NULL;

    uint state[] = {0, 0, 0, 0},
         key_raw[] = {0, 0, 0, 0, 0, 0, 0, 0};
    uchar cipher[16];

	ok = PyArg_ParseTuple(args,
                          "s#|s#",
                          &plaintext, &plaintext_size,
                          &key,       &key_size);
	if (!ok) {
		return NULL;
	}

    if (plaintext_size != 16) {
        PyErr_SetString(InvalidInputLength, "Invalid plaintext length");
        return NULL;
    }
    if (key_size == 0  || key_size == 16 ||
        key_size == 24 || key_size == 32) {
    } else {
        PyErr_SetString(InvalidKeyLength, "Invalid key length");
        return NULL;
    }

	if (key_size) {
        n_key = key_size / 4;
        n_round = 10 + n_key - 4;

        MAES_uchar_arr_to_uint_arr(key_raw,
                                   (uchar*) key,
                                   key_size);
        MAES_key_schedule(key_raw);
    }

    MAES_uchar_arr_to_uint_arr(state,
                               (uchar*) plaintext,
                               plaintext_size);

    printf("state: %08x %08x %08x %08x\n", state[0], state[1], state[2], state[3]);
    MAES_init_round(state);
    for (i = 1; i < n_round; ++i) {
        MAES_round(state, i);
    }
    MAES_final_round(state);

    MAES_uint_arr_to_uchar_arr(cipher,
                               state,
                               4);

	return Py_BuildValue("s#",
                         cipher,
                         n_block * 4);
}

static PyObject*
MAES_decrypt(PyObject* self,
             PyObject* args)
{
	int	ok, i;
	int cipher_size,
		key_size = 0;

	char* cipher;
	char* key = NULL;

    uint state[] = {0, 0, 0, 0},
         key_raw[] = {0, 0, 0, 0, 0, 0, 0, 0};
    uchar plaintext[16];

	ok = PyArg_ParseTuple(args,
                          "s#|s#",
                          &cipher, &cipher_size,
                          &key,    &key_size);
	if (!ok) {
		return NULL;
	}

    if (cipher_size != 16) {
        PyErr_SetString(InvalidInputLength, "Invalid cipher length");
        return NULL;
    }
    if (key_size == 0  || key_size == 16 ||
        key_size == 24 || key_size == 32) {
    } else {
        PyErr_SetString(InvalidKeyLength, "Invalid key length");
        return NULL;
    }

	if (key_size) {
        n_key = key_size / 4;
        n_round = 10 + n_key - 4;

        MAES_uchar_arr_to_uint_arr(key_raw,
                                   (uchar*) key,
                                   key_size);
        MAES_key_schedule(key_raw);
    }

    MAES_uchar_arr_to_uint_arr(state,
                               (uchar*) cipher,
                               cipher_size);

    printf("state: %08x %08x %08x %08x\n", state[0], state[1], state[2], state[3]);
    MAES_inv_init_round(state);
    for (i = n_round - 1; i > 0; --i) {
        MAES_inv_round(state, i);
    }
    MAES_inv_final_round(state);

    MAES_uint_arr_to_uchar_arr(plaintext,
                               state,
                               4);

	return Py_BuildValue("s#",
                         plaintext,
                         n_block * 4);
}

static PyObject*
MAES_cached_round_keys(PyObject* self,
                       PyObject* args)
{
    PyObject* ret; 
    int i, n = n_block * (n_round + 1);

    ret = PyTuple_New(n);
    if (!ret) {
        return NULL;
    }

    for (i = 0; i < n; ++i) {
        PyTuple_SET_ITEM(ret, i, PyLong_FromUnsignedLong(round_keys[i]));
    }

    return ret;
}

static PyMethodDef maesMethods[] = {
	{
		"encrypt", (PyCFunction) MAES_encrypt, METH_VARARGS,
		"run aes on given plaintext and key"
	},
	{
		"decrypt", (PyCFunction) MAES_decrypt, METH_VARARGS,
		"run inverse aes on given plaintext and key"
	},
    {
        "cached_round_keys", (PyCFunction) MAES_cached_round_keys, METH_VARARGS,
        "get cached round keys"
    },
    {
        "test_mix_columns", (PyCFunction) MAES_test_mix_columns, METH_VARARGS,
        "test mix columns"
    },
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initmaes()
{
	PyObject* mod;

    mod = Py_InitModule("maes", maesMethods);
    if (mod == NULL) {
        return;
    }

    InvalidInputLength = PyErr_NewException("maes.error", NULL, NULL);
    Py_INCREF(InvalidInputLength);
    PyModule_AddObject(mod, "error", InvalidInputLength);

    InvalidKeyLength = PyErr_NewException("maes.error", NULL, NULL);
    Py_INCREF(InvalidKeyLength);
    PyModule_AddObject(mod, "error", InvalidKeyLength);
}


