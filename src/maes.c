#include <common.h>
#include <aes.h>
#include <inv_aes.h>
#include <tables.h>

static PyObject* InvalidInputLength;
static PyObject* InvalidKeyLength;


static PyObject*
MAES_test_mix_columns(PyObject* self, PyObject* args)
{
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

        MAES_char_arr_to_uint_arr_m(key_raw,
                                    key,
                                    key_size);
        MAES_key_schedule(key_raw);
    }

    MAES_char_arr_to_uint_arr_m(state,
                                plaintext,
                                plaintext_size);

    MAES_init_round_m(state);
    for (i = 1; i < n_round; ++i) {
        MAES_round_m(state, i);
    }
    MAES_final_round_m(state);

    MAES_uint_arr_to_uchar_arr_m(cipher,
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

        MAES_char_arr_to_uint_arr_m(key_raw,
                                    key,
                                    key_size);
        MAES_key_schedule(key_raw);
    }

    MAES_char_arr_to_uint_arr_m(state,
                                cipher,
                                cipher_size);

    MAES_inv_init_round_m(state);
    for (i = n_round - 1; i > 0; --i) {
        MAES_inv_round_m(state, i);
    }
    MAES_inv_final_round_m(state);

    MAES_uint_arr_to_uchar_arr_m(plaintext,
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


