import os
from maes import cbc_aes, inv_cbc_aes, encrypt

block_size = 8192 * 128
block_size_and_a_byte = block_size + 16


def encrypt_fp(secret_key, plaintext_fp, cipher_fp, init_vector='\x00' * 16):
    encrypt('\x00' * 16, secret_key)

    plaintext_fp.seek(0, os.SEEK_END)
    rest_size = plaintext_fp.tell()

    plaintext_fp.seek(0, os.SEEK_SET)
    while True:
        if not rest_size:
            break

        if rest_size >= block_size_and_a_byte:
            size = block_size
        else:
            size = rest_size
        cipher, init_vector, _ = cbc_aes(plaintext_fp.read(size),
                                         init_vector)
        cipher_fp.write(cipher)
        rest_size -= size

    return cipher_fp


def decrypt_fp(secret_key, cipher_fp, plaintext_fp, init_vector='\x00' * 16):
    encrypt('\x00' * 16, secret_key)

    cipher_fp.seek(0, os.SEEK_END)
    rest_size = cipher_fp.tell()

    cipher_fp.seek(0, os.SEEK_SET)
    while True:
        if not rest_size:
            break

        if rest_size >= block_size_and_a_byte:
            size = block_size
        else:
            size = rest_size
        plaintext, init_vector, _ = inv_cbc_aes(cipher_fp.read(size),
                                                init_vector)
        plaintext_fp.write(plaintext)
        rest_size -= size

    return plaintext_fp


