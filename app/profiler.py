import cProfile
import pstats
import argparse
import sys
import struct




parser = argparse.ArgumentParser()
parser.add_argument('action', type=str, action='store', default='enc',
                    help='enc(rypt) or dec(rypt)')
parser.add_argument('in_path', type=str, action='store', default='',
                    help='path of file that will be encrypted')
parser.add_argument('-o', '--out', type=str, action='store', dest='out_path',
                    help='path of file which cipher is output')
parser.add_argument('-k', '--key', type=str, action='store', dest='raw_key', default='0001020304050607' * 2,
                    help='key that will be used in encryption')
parser.add_argument('-m', '--mode', type=str, action='store', dest='mode', default='cbc',
                    help='mode of operation, only ECB(ecb) and CBC(cbc) are supported')
parser.add_argument('-iv', '--init-vector', type=str, action='store', dest='raw_init_vec', default='00' * 16,
                    help='initialization vector used in CBC mode')


def process_raw_input(raw):
    res = []
    for i in range(0, len(raw), 2):
        res.append(struct.pack('B', int(raw[i:i + 2], base=16)))
    return ''.join(res)



if __name__ == '__main__':
    args = parser.parse_args(sys.argv[1:])

    in_path = args.in_path

    key = process_raw_input(args.raw_key)
    init_vec = process_raw_input(args.raw_init_vec)

    if args.mode == 'cbc':
        from cbc_mode import encrypt_fp, decrypt_fp
    elif args.mode == 'ecb':
        from ecb_mode import encrypt_fp, decrypt_fp
    else:
        def encrypt_fp(key, i_fp, o_fp, init_vec=''):
            print 'invalid mode'

        def decrypt_fp(key, i_fp, o_fp, init_vec=''):
            print 'invalid mode'

    if args.action == 'enc':
        prof_name = 'enc_prof'
        f = encrypt_fp
        out_path = (in_path + '.aes') if not args.out_path else args.out_path
    elif args.action == 'dec':
        prof_name = 'dec_prof'
        f = decrypt_fp
        out_path = (in_path + '.inv_aes') if not args.out_path else args.out_path
    else:
        def f(a, b, c, d=''):
            print 'invalid action'

    def test():
        return f(key,
                 open(in_path, 'rb'),
                 open(out_path, 'wb'),
                 init_vec)

    cProfile.run('test()', prof_name)
    pstats.Stats(prof_name).strip_dirs().sort_stats('calls').print_stats()


