import os
from maes import encrypt, decrypt

block_size = 8192 * 128

rng = range(0, block_size, 16)


def len_fp(fp):
    fp.seek(0, os.SEEK_END)
    length = fp.tell()
    fp.seek(0)

    return length


def padding(size):
    """
    >>> padding(0)
    '\\x08\\x08\\x08\\x08\\x08\\x08\\x08\\x08'
    >>> padding(3)
    '\\x05\\x05\\x05\\x05\\x05'
    """
    pad_size = 16 - size % 16
    return chr(pad_size) * (pad_size if pad_size else 16)


def encrypt_fp(secret_key, plaintext_fp, cipher_fp, init_vec=''):
    encrypt('\x00' * 16, secret_key)

    while True:
        block = plaintext_fp.read(block_size)
        block_len = len(block)
        if 0 < block_len < block_size:
            block += padding(block_len)
            cipher_fp.write(''.join([encrypt(block[start:start + 16]) for start in rng[:block_len / 16 + 1]]))
            break

        cipher_fp.write(''.join([encrypt(block[start:start + 16]) for start in rng]))

    return cipher_fp


def decrypt_fp(secret_key, cipher_fp, plaintext_fp, init_vec=''):
    encrypt('\x00' * 16, secret_key)

    length = len_fp(cipher_fp)
    while True:
        block = cipher_fp.read(block_size)
        block_len = len(block)
        if block_len < block_size:
            plaintext_block = ''.join([decrypt(block[start:start + 16]) for start in rng[:block_len / 16]])
        else:
            plaintext_block = ''.join([decrypt(block[start:start + 16]) for start in rng])

        if cipher_fp.tell() == length:
            plaintext_block = plaintext_block[:-ord(plaintext_block[-1])]
            plaintext_fp.write(plaintext_block)
            break

        plaintext_fp.write(plaintext_block)

    return plaintext_fp


