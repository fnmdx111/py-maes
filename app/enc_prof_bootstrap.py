import cProfile
import pstats
from main import encrypt_fp


key = '\x01\x23\x34\x56\x78\x9a\xbc\xde' * 2


def test():
    return encrypt_fp(key,
                      open('d:/tddownload/QQ2013Beta1.exe', 'rb'),
                      open('d:/tddownload/QQ2013Beta1.exe.aes', 'wb'))


if __name__ == '__main__':
    cProfile.run('test()', 'encrypt_prof_ix')
    pstats.Stats('encrypt_prof_ix').strip_dirs().sort_stats('calls').print_stats()


