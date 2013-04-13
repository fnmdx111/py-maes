import cProfile
import pstats
from enc_prof_bootstrap import key
from cbc_main import decrypt_fp


def test_():
    return decrypt_fp(key,
                      open('d:/tddownload/World-of-Warcraft-Setup-zhCN.exe.aes', 'rb'),
                      open('d:/tddownload/World-of-Warcraft-Setup-zhCN.1.exe', 'wb'))



if __name__ == '__main__':
    cProfile.run('test_()', 'decrypt_prof')
    pstats.Stats('decrypt_prof').strip_dirs().sort_stats('calls').print_stats()

