/*
 * About the cipher stealing technique, please see this detailed document
 * ref1: http://csrc.nist.gov/groups/ST/toolkit/BCM/documents/ciphertext%20stealing%20proposal.pdf
 * and the wiki entry
 * ref2: http://en.wikipedia.org/wiki/Ciphertext_stealing#CBC_ciphertext_stealing
 */


#include <common.h>
#include <aes.h>
#include <inv_aes.h>
#include <tables.h>

static PyObject* InvalidInputLength;
static PyObject* InvalidKeyLength;

#define MAES_aes_m(state) \
    MAES_init_round_m(state)\
        MAES_round_m(state, 1)\
        MAES_round_m(state, 2)\
        MAES_round_m(state, 3)\
        MAES_round_m(state, 4)\
        MAES_round_m(state, 5)\
        MAES_round_m(state, 6)\
        MAES_round_m(state, 7)\
        MAES_round_m(state, 8)\
        MAES_round_m(state, 9)\
        if (n_round > 10) {\
            MAES_round_m(state, 10)\
            MAES_round_m(state, 11)\
        }\
        if (n_round > 12) {\
            MAES_round_m(state, 12)\
            MAES_round_m(state, 13)\
        }\
    MAES_final_round_m(state)

#define MAES_inv_aes_m(state) \
    MAES_inv_init_round_m(state)\
        if (n_round > 12) {\
            MAES_inv_round_m(state, 13)\
            MAES_inv_round_m(state, 12)\
        }\
        if (n_round > 10) {\
            MAES_inv_round_m(state, 11)\
            MAES_inv_round_m(state, 10)\
        }\
        MAES_inv_round_m(state, 9)\
        MAES_inv_round_m(state, 8)\
        MAES_inv_round_m(state, 7)\
        MAES_inv_round_m(state, 6)\
        MAES_inv_round_m(state, 5)\
        MAES_inv_round_m(state, 4)\
        MAES_inv_round_m(state, 3)\
        MAES_inv_round_m(state, 2)\
        MAES_inv_round_m(state, 1)\
    MAES_inv_final_round_m(state)

#define VALIDATE_ARGS \
	if (!ok) {\
		return NULL;\
	}

#define VALIDATE_INPUT_SIZE(param) \
    if (param ## _size == 0 || param ## _size % 16 != 0) {\
        PyErr_SetString(InvalidInputLength, "Invalid " #param " length");\
        return NULL;\
    }\

#define VALIDATE_KEY_SIZE \
    if (key_size == 0) {\
        if (!n_key) {\
            PyErr_SetString(InvalidKeyLength, "No round keys are cached");\
            return NULL;\
        }\
    } else if (key_size == 16 || key_size == 24 || key_size == 32) {\
    } else {\
        PyErr_SetString(InvalidKeyLength, "Invalid key length");\
        return NULL;\
    }

#define INIT_VARS int ok; uint key_raw[8];
#define INIT_CIPHER uint temp[16], state[16];
#define INIT_DATA_VARS(keyword)\
    int keyword ## _size, key_size = 0;\
    char *keyword, *key = NULL;\
    uchar *uchar_ ## keyword;

#define PREPARE_KEYS \
	if (key_size) {\
        n_key = key_size / 4;\
        n_round = 10 + n_key - 4;\
        MAES_char_arr_to_uint_arr_m(key_raw, key, key_size, 0)\
        MAES_key_schedule(key_raw);\
    }

uchar buf_uchar[CHUNK_SIZE]; // defined at compile time, see setup.py

static PyObject*
MAES_test_mix_columns(PyObject* self, PyObject* args)
{
    uint temp[4];
    uint state[] = {0xdb, 0xf2, 0xd4, 0x2d,
                    0x13, 0x0a, 0xd4, 0x26,
                    0x53, 0x22, 0xd4, 0x31,
                    0x45, 0x5c, 0xd5, 0x4c};
    printf("before:  %x %x %x %x\n", state[0], state[1], state[2], state[3]);
    MAES_mix_column_m(state, 0)
    printf("1st col: %x %x %x %x\n", state[0], state[1], state[2], state[3]);
    MAES_mix_column_m(state, 4)
    printf("2nd col: %x %x %x %x\n", state[4], state[5], state[6], state[7]);
    MAES_mix_column_m(state, 8)
    printf("3rd col: %x %x %x %x\n", state[8], state[9], state[10], state[11]);
    MAES_mix_column_m(state, 12)
    printf("4th col: %x %x %x %x\n", state[12], state[13], state[14], state[15]);

    return Py_BuildValue("IIIIIIIIIIIIIIII",
                         state[0], state[1], state[2], state[3],
                         state[4], state[5], state[6], state[7],
                         state[8], state[9], state[10], state[11],
                         state[12], state[13], state[14], state[15]);
}


static PyObject*
MAES_encrypt(PyObject* self,
             PyObject* args)
{
    INIT_VARS
    INIT_DATA_VARS(plaintext)

    uchar cipher[16];

    INIT_CIPHER

    ok = PyArg_ParseTuple(args,
                          "s#|s#",
                          &plaintext, &plaintext_size,
                          &key,       &key_size);
    VALIDATE_ARGS
    VALIDATE_INPUT_SIZE(plaintext)
    VALIDATE_KEY_SIZE
    PREPARE_KEYS

    uchar_plaintext = (uchar*) plaintext;


    MAES_copy_16_m(state, uchar_plaintext, 0)

    MAES_aes_m(state)

    MAES_copy_16_m(cipher, state, 0)

	return Py_BuildValue("s#",
                         cipher,
                         n_block * 4);
}

static PyObject*
MAES_decrypt(PyObject* self,
             PyObject* args)
{
    INIT_VARS
    INIT_DATA_VARS(cipher)

    uchar plaintext[16];

    INIT_CIPHER

    ok = PyArg_ParseTuple(args,
                          "s#|s#",
                          &cipher, &cipher_size,
                          &key,    &key_size);
    VALIDATE_ARGS
    VALIDATE_INPUT_SIZE(cipher)
    VALIDATE_KEY_SIZE
    PREPARE_KEYS

    uchar_cipher = (uchar*) cipher;

    MAES_copy_16_m(state, uchar_cipher, 0)

    MAES_inv_aes_m(state)

    MAES_copy_16_m(plaintext, state, 0)

	return Py_BuildValue("s#",
                         plaintext,
                         n_block * 4);
}

static PyObject*
MAES_cbc_aes(PyObject* self,
             PyObject* args)
/*
 * each time pass in 8192 * 128
 */
{
    INIT_VARS
    INIT_DATA_VARS(plaintext)

    char* init_vec_cstr;
    int   init_vec_size, enc_times, offset_from, offset_to,
          i, j, rest;
    uchar* init_vec_uchar;
    uint  init_vec[16];

    INIT_CIPHER

    ok = PyArg_ParseTuple(args,
                          "s#s#|s#",
                          &plaintext,     &plaintext_size,
                          &init_vec_cstr, &init_vec_size,
                          &key,           &key_size);
    if (init_vec_size != 16) {
        PyErr_SetString(InvalidInputLength, "Invalid initial vector length");
        return NULL;
    } else {
        init_vec_uchar = (uchar*) init_vec_cstr;
        MAES_copy_16_m(init_vec, init_vec_uchar, 0)
    }
    VALIDATE_ARGS
    VALIDATE_KEY_SIZE
    PREPARE_KEYS

    uchar_plaintext = (uchar*) plaintext;

    if (plaintext_size < 32) {
        rest = plaintext_size - 16; // d = len(P) - b

        offset_from = 0;
        MAES_copy_16_src_inc_m(state, uchar_plaintext, offset_from) // P[n - 1] -> state

        MAES_add_round_keys_m(state, init_vec, 0) // P[n - 1] (+) IV -> state
        MAES_aes_m(state) // state -> C[n - 1]

        for (i = offset_to = 0; i < rest; ++i, ++offset_to) {
            buf_uchar[offset_to] = state[i];
        } // output C[n - 1]*, i.e. MSB(C[n - 1], d)

        for (i = 0; i < rest; ++i, ++offset_from) {
            state[i] ^= uchar_plaintext[offset_from];
        } // P[n]* (+) C[n - 1]* || C[n - 1]** -> state
          // i.e. P[n] (+) MSB(C[n - 1], d) || LSB(C[n - 1], b - d) -> state
          // see figure in ref1#CBC-CS-Encrypt

        MAES_aes_m(state) // E(state, K)
        MAES_copy_16_dest_inc_m(buf_uchar, state, offset_to) // output C[n]

        for (i = 0; i < 16; ++i) {
            init_vec[i] = 0xff; // set init vector invalid
        }
    } else {
        enc_times = plaintext_size / 16;

        for (i = offset_from = offset_to = 0; i < enc_times; ++i) {
            MAES_copy_16_src_inc_m(state, uchar_plaintext, offset_from)
            MAES_add_round_keys_m(state, init_vec, 0)
            MAES_aes_m(state)
            MAES_copy_16_dest_inc_m(buf_uchar, state, offset_to)
            MAES_copy_16_m(init_vec, state, 0) // E[n - 1] -> init_vec
        }

        if (offset_from != offset_to) {
            return NULL;
        }

        if (offset_from < plaintext_size) {
            for (i = offset_to, j = 0; i < plaintext_size; ++i, ++j) {
                buf_uchar[i] = init_vec[j];
            } // Head(E[n - 1], rest) -> C[n]

            for (i = offset_from, j = 0; i < plaintext_size; ++i, ++j) {
                state[j] = uchar_plaintext[i];
            }
            for (; j < 16; ++j) {
                state[j] = 0;
            } // P[n] || 0, ..., 0 -> P

            MAES_add_round_keys_m(state, init_vec, 0) // P (+) E[n - 1] -> D[n]
            MAES_aes_m(state) // E(D[n], K) -> state
            if (offset_to >= 16) {
                offset_to -= 16;
                MAES_copy_16_dest_inc_m(buf_uchar, state, offset_to) // state -> C[n - 1]
            }
            MAES_copy_16_m(init_vec, state, 0)
        }
    }

    return Py_BuildValue("s#s#i",
                         buf_uchar,
                         plaintext_size,
                         init_vec,
                         16,
                         plaintext_size <= 16 ? 1 : 0);
}

static PyObject*
MAES_inv_cbc_aes(PyObject* self,
             PyObject* args)
/*
 * each time pass in 8192 * 128
 */
{
    INIT_VARS
    INIT_DATA_VARS(cipher)

    char* init_vec_cstr;
    int   init_vec_size, dec_times, offset_from, offset_to,
          i, j, rest;
    uchar* init_vec_uchar;
    uint  init_vec[16], cipher_cache[16];

    INIT_CIPHER

    ok = PyArg_ParseTuple(args,
                          "s#s#|s#",
                          &cipher,        &cipher_size,
                          &init_vec_cstr, &init_vec_size,
                          &key,           &key_size);
    if (init_vec_size != 16) {
        PyErr_SetString(InvalidInputLength, "Invalid initial vector length");
        return NULL;
    } else {
        init_vec_uchar = (uchar*) init_vec_cstr;
        MAES_copy_16_m(init_vec, init_vec_uchar, 0)
    }
    VALIDATE_ARGS
    VALIDATE_KEY_SIZE
    PREPARE_KEYS

    uchar_cipher = (uchar*) cipher;

    if (cipher_size < 16) {
        PyErr_SetString(InvalidInputLength, "Input is too small to decrypt");
    } else if (cipher_size < 32) {
        rest = cipher_size - 16;

        MAES_copy_16_m(state, uchar_cipher, rest) // C[n] -> state

        MAES_inv_aes_m(state) // D(state, K) -> state
                              // i.e. D(state, K) -> P[n]* (+) C[n - 1]* || C[n - 1]**

        for (i = 0, offset_to = 16; i < rest; ++i, ++offset_to) {
            buf_uchar[offset_to] = state[i] ^ uchar_cipher[i]; // P[n]* (+) C[n - 1]* -> P[n]
        }

        for (i = 0; i < rest; ++i) {
            state[i] = uchar_cipher[i]; // C[n - 1]* || C[n - 1]** -> C[n - 1]
                                        // i.e. C[n - 1]* || C[n - 1]** -> state
        }

        MAES_inv_aes_m(state) // C[i - 1] -> D[n - 1]
        MAES_add_round_keys_m(state, init_vec, 0) // D[i - 1] (+) IV -> P[i - 1]

        offset_to = 0;
        MAES_copy_16_dest_inc_m(buf_uchar, state, offset_to) // P[i - 1] -> buf_uchar
        for (i = 0; i < 16; ++i) {
            init_vec[i] = 0xff; // set init vector invalid
        }
    } else {
        dec_times = cipher_size / 16;

        for (i = offset_from = offset_to = 0; i < dec_times; ++i) {
            MAES_copy_16_src_inc_m(state, uchar_cipher, offset_from) // C[i] -> state
            MAES_copy_16_m(cipher_cache, state, 0) // D[i] -> cipher_cache
            MAES_inv_aes_m(state) // D[i] = D(C[i], K) -> state
            MAES_add_round_keys_m(state, init_vec, 0) // P[i] -> D[i] (+) init_vec -> state
            MAES_copy_16_dest_inc_m(buf_uchar, state, offset_to)
            MAES_copy_16_m(init_vec, cipher_cache, 0) // D[i] -> init_vec
        }

        if (offset_from != offset_to) {
            return NULL;
        }

        if (offset_from < cipher_size) {
            if (offset_to >= 16) {
                offset_to -= 16;
                MAES_copy_16_m(state, uchar_cipher, offset_to) // C[n - 1] -> state
                offset_to += 16;
                MAES_inv_aes_m(state) // D[n] = D(C[n - 1], K) -> state
            }

            for (i = offset_from, j = 0; i < cipher_size; ++i, ++j) {
                cipher_cache[j] = uchar_cipher[i]; // C[n] -> cipher_cache
            }
            for (; j < 16; ++j) {
                cipher_cache[j] = 0;
            } // C = C[n] || 0, ..., 0 -> temp

            MAES_add_round_keys_m(state, cipher_cache, 0) // X[n] = D[n] ^ C -> state
            for (i = offset_to, j = 0; i < cipher_size; ++i, ++j) {
                buf_uchar[i] = state[j];
            } // Head(X[n], rest) -> P[n]

            for (i = offset_from, j = 0; i < cipher_size; ++i, ++j) {
                state[j] = uchar_cipher[i];
            } // E[n - 1] = C[n] || Tail(X[n], need) -> state

            MAES_inv_aes_m(state) // X[n - 1] = D(E[n - 1], K) -> state

            if (offset_to >= 32) {
                offset_to -= 32;
                MAES_copy_16_m(cipher_cache, uchar_cipher, offset_to) // C[n - 2] -> cipher_cache
                offset_to += 32;
            }
            MAES_add_round_keys_m(state, cipher_cache, 0) // P[n - 1] = X[n - 1] ^ C[n - 2] -> state

            offset_to -= 16;
            MAES_copy_16_dest_inc_m(buf_uchar, state, offset_to)
        }
    }

    return Py_BuildValue("s#s#i",
                         buf_uchar,
                         cipher_size,
                         init_vec,
                         16,
                         cipher_size <= 16 ? 1 : 0);
}

static PyObject*
MAES_cached_round_keys(PyObject* self,
                       PyObject* args)
{
    PyObject* ret; 
    int i, n = n_block * (n_round + 1) * 4;

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
        "cbc_aes", (PyCFunction) MAES_cbc_aes, METH_VARARGS,
        "run aes on given block of plaintext and key in cbc mode"
    },
    {
        "inv_cbc_aes", (PyCFunction) MAES_inv_cbc_aes, METH_VARARGS,
        "run inverse aes on given block of plaintext and key in cbc mode"
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


