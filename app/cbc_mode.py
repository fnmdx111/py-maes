import os
from maes import cbc_aes, inv_cbc_aes, encrypt
from functools import partial

block_size = 8192 * 128
block_size_and_a_byte = block_size + 16


def protocrypt_fp(func, secret_key, in_fp, out_fp, init_vector='\x00' * 16):
    encrypt('\x00' * 16, secret_key)

    in_fp.seek(0, os.SEEK_END)
    rest_size = in_fp.tell()
    in_fp.seek(0, os.SEEK_SET)

    while True:
        if not rest_size:
            break
        elif rest_size > block_size_and_a_byte:
            size = block_size
        else:
            size = rest_size
        out_text, init_vector = func(in_fp.read(size),
                                     init_vector)
        out_fp.write(out_text)
        rest_size -= size

    return out_fp


encrypt_fp = partial(protocrypt_fp, cbc_aes)
decrypt_fp = partial(protocrypt_fp, inv_cbc_aes)

