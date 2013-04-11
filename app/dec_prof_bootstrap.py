import cProfile
import pstats
from example import key
from main import decrypt_fp


def test_():
    return decrypt_fp(key,
                      open('d:/tddownload/QQ2013Beta1.exe.aes', 'rb'),
                      open('d:/tddownload/QQ2013Beta1.1.exe', 'wb'))



if __name__ == '__main__':
    cProfile.run('test_()', 'decrypt_prof')
    pstats.Stats('decrypt_prof').strip_dirs().sort_stats('calls').print_stats()

