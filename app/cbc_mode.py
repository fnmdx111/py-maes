import os
from maes import cbc_aes, inv_cbc_aes, encrypt

block_size = 8192 * 128


def encrypt_fp(secret_key, plaintext_fp, cipher_fp, init_vector='\x00' * 16):
    encrypt('\x00' * 16, secret_key)

    plaintext_fp.seek(0, 2)
    fp_size = plaintext_fp.tell()
    while True:
        block = plaintext_fp.read(block_size + 16)
        if len(block) == block_size:
            cipher, init_vector, _ = cbc_aes(block, init_vector)

        cipher, init_vector, _ = cbc_aes(block, init_vector)
        cipher_fp.write(cipher)
        if len(block) < block_size:
            break

    return cipher_fp


def decrypt_fp(secret_key, cipher_fp, plaintext_fp, init_vector='\x00' * 16):
    encrypt('\x00' * 16, secret_key)

    while True:
        block = cipher_fp.read(block_size)
        plaintext, init_vector, _ = inv_cbc_aes(block, init_vector)
        plaintext_fp.write(plaintext)
        if len(block) < block_size:
            break

    return plaintext_fp


