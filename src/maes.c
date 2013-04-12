#include <common.h>
#include <aes.h>
#include <inv_aes.h>
#include <tables.h>

static PyObject* InvalidInputLength;
static PyObject* InvalidKeyLength;

#define MAES_aes_m(state) \
    MAES_init_round_m(state)\
    for (i = 1; i < n_round; ++i) {\
        MAES_round_m(state, i)\
    }\
    MAES_final_round_m(state)

#define MAES_inv_aes_m(state) \
    MAES_inv_init_round_m(state)\
    for (i = n_round - 1; i > 0; --i) {\
        MAES_inv_round_m(state, i)\
    }\
    MAES_inv_final_round_m(state)

#define VALIDATE_LEN_2(param) \
	if (!ok) {\
		return NULL;\
	}\
    if (param ## _size % 16 != 0) {\
        PyErr_SetString(InvalidInputLength, "Invalid " #param " length");\
        return NULL;\
    }\
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

#define INIT_VARS int ok, i; uint temp[4], state[4], key_raw[8];
#define INIT_DATA_VARS(keyword)\
    int keyword ## _size, key_size = 0;\
    char *keyword, *key = NULL;

#define PREPARE_KEYS \
	if (key_size) {\
        n_key = key_size / 4;\
        n_round = 10 + n_key - 4;\
        MAES_char_arr_to_uint_arr_m(key_raw, key, key_size, 0)\
        MAES_key_schedule(key_raw);\
    }

uchar buf_uchar[8192 * 128];

static PyObject*
MAES_test_mix_columns(PyObject* self, PyObject* args)
{
    uint temp[4];
    uint state[] = {0xdbf2d42d, 0x130ad426, 0x5322d431, 0x455cd54c};
    printf("before:  %x %x %x %x\n", state[0], state[1], state[2], state[3]);
    MAES_mix_column_m(state, 0);
    printf("1st col: %x %x %x %x\n", state[0], state[1], state[2], state[3]);
    MAES_mix_column_m(state, 1);
    printf("2nd col: %x %x %x %x\n", state[0], state[1], state[2], state[3]);
    MAES_mix_column_m(state, 2);
    printf("3rd col: %x %x %x %x\n", state[0], state[1], state[2], state[3]);
    MAES_mix_column_m(state, 3);
    printf("4th col: %x %x %x %x\n", state[0], state[1], state[2], state[3]);

    return Py_BuildValue("IIII", state[0], state[1], state[2], state[3]);
}

static PyObject*
MAES_encrypt(PyObject* self,
             PyObject* args)
{
    INIT_VARS
    INIT_DATA_VARS(plaintext)

    uchar cipher[16];

    ok = PyArg_ParseTuple(args,
                          "s#|s#",
                          &plaintext, &plaintext_size,
                          &key,       &key_size);
    VALIDATE_LEN_2(plaintext)
    PREPARE_KEYS

    MAES_char_arr_to_uint_arr_m(state, plaintext, plaintext_size, 0)

    MAES_aes_m(state)

    MAES_uint_arr_to_uchar_arr_m(cipher, state, 4, 0) 

	return Py_BuildValue("s#",
                         cipher,
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

    char* init_vec;
    int   init_vec_size, enc_times, offset_from, offset_to;
    uchar* plt;

    ok = PyArg_ParseTuple(args,
                          "s#s#|s#",
                          &plaintext, &plaintext_size,
                          &init_vec,  &init_vec_size,
                          &key,       &key_size);
    if (init_vec_size != 16) {
        PyErr_SetString(InvalidInputLength, "Invalid initial vector length");
        return NULL;
    }
    VALIDATE_LEN_2(plaintext)
    PREPARE_KEYS


    enc_times = plaintext_size / 16;
    // XXX debug here
    plt = (uchar*) plaintext;
    for (i = offset_from = offset_to = 0; i < enc_times; ++i) {
        MAES_uchar_16_to_uint_4_m(state, plt, offset_from)

        // Mi (+) I -> State
        MAES_add_round_keys_m(state, init_vec, 0);

        // State -> Ci
        MAES_aes_m(state)
        // Ci -> Output
        MAES_uint_4_to_uchar_16_m(buf_uchar, state, offset_to)

        // Ci -> I
        init_vec[0] = state[0]; init_vec[1] = state[1];
        init_vec[2] = state[2]; init_vec[3] = state[3];
    }
    // TODO implement cipher diversion

    return Py_BuildValue("s#",
                         buf_uchar,
                         plaintext_size);
}

static PyObject*
MAES_decrypt(PyObject* self,
             PyObject* args)
{
    INIT_VARS
    INIT_DATA_VARS(cipher)

    uchar plaintext[16];
    ok = PyArg_ParseTuple(args,
                          "s#|s#",
                          &cipher, &cipher_size,
                          &key,    &key_size);
    VALIDATE_LEN_2(cipher)
    PREPARE_KEYS

    MAES_char_arr_to_uint_arr_m(state, cipher, cipher_size, 0)

    MAES_inv_aes_m(state)

    MAES_uint_arr_to_uchar_arr_m(plaintext, state, 4, 0)

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
        "cbc_aes", (PyCFunction) MAES_cbc_aes, METH_VARARGS,
        "run aes on given block of plaintext and key in cbc mode"
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


