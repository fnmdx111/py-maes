MAES
====

环境
----

MAES是为32bit环境下编写的，所以请用32位的python。
如果需要自己编译，也请用32位的编译器（或目标环境设为32位）。

原则上来讲，64bit或16bit下，程序会不能得出正常结果，因为只有32位下一个int为32位。

update: `unsigned int`改为了`uint32_t`，现在应该是平台无关的了。

本库在win7 64位，mingw32 4.7.1，python2.7 32位下测试通过。


编译
----

不知道为什么在vs下编译会报语法错误（目前只试了vs2013express）。所以请使用如下命令

    python setup.py build --compiler=mingw32

如果出现`未识别的参数-mno-cygwin`，那么请打`cygwinccompiler.patch`（这个问题出现于高版本的mingw32上）。其中`cygwinccompiler.py`位于`PYTHON_HOME/Lib/distutils/`中。

编译后会在`build/lib.win32-2.7`下出现`maes.pyd`文件，复制到你的项目下就可以了。


用法
----

    import maes

    maes.encrypt(plaintext, key='') => cipher
    maes.decrypt(cipher, key='') => plaintext
    maes.cached_round_keys() => round_keys

如果key关键字被留空，则使用缓存起来的`round_keys`，可以避免每次加密都要重新计算轮密钥。

e.g.

    >>> import maes
    >>> maes.encrypt('\x00\x01\x00\x01\x01\xa1\x98\xaf\xda\x78\x17\x34\x86\x15\x35\x66',
                     '\x00\x01\x20\x01\x71\x01\x98\xae\xda\x79\x17\x14\x60\x15\x35\x94')
    'l\xddYk\x8fVB\xcb\xd2;G\x98\x1aeB*'
    >>> maes.decrypt('l\xddYk\x8fVB\xcb\xd2;G\x98\x1aeB*') # 使用了encrypt里提供的密钥
    '\x00\x01\x00\x01\x01\xa1\x98\xaf\xdax\x174\x86\x155f'


TODO
----

* iterartor for large file encryption, e.g. CFB mode
* reduce bit operations, e.g. change 1 uint to an uint array with 4 uint(thus expand 1 uchar to 1 uint)


性能
----

以下为在ECB模式下，使用128bit密钥加密50MB的QQ2013Beta1.exe的profiler结果。padding模式为PKCS#7。

    Thu Apr 11 10:05:00 2013    encrypt_prof_ix

             3280143 function calls in 4.424 seconds

       Ordered by: call count

       ncalls  tottime  percall  cumtime  percall filename:lineno(function)
      3279931    2.429    0.000    2.429    0.000 {maes.encrypt}
           51    0.055    0.001    0.055    0.001 {method 'read' of 'file' objects}
           51    0.000    0.000    0.000    0.000 {len}
           51    0.036    0.001    0.036    0.001 {method 'write' of 'file' objects}

           51    0.101    0.002    0.101    0.002 {method 'join' of 'str' objects}
            2    0.007    0.004    0.007    0.004 {open}
            1    1.794    1.794    4.415    4.415 main.py:14(encrypt_fp)
            1    0.000    0.000    0.000    0.000 misc.py:116(padding)
            1    0.000    0.000    4.422    4.422 example.py:9(test)
            1    0.000    0.000    0.000    0.000 {chr}
            1    0.001    0.001    4.424    4.424 <string>:1(<module>)
            1    0.000    0.000    0.000    0.000 {method 'disable' of '_lsprof.Profiler' objects}

以下为解密上述加密结果的profiler结果。

    Thu Apr 11 10:09:31 2013    decrypt_prof

             3280197 function calls in 4.550 seconds

       Ordered by: call count

       ncalls  tottime  percall  cumtime  percall filename:lineno(function)
      3279930    2.390    0.000    2.390    0.000 {maes.decrypt}
           52    0.001    0.000    0.001    0.000 {method 'tell' of 'file' objects}
           51    0.057    0.001    0.057    0.001 {method 'read' of 'file' objects}
           51    0.039    0.001    0.039    0.001 {method 'write' of 'file' objects}

           51    0.103    0.002    0.103    0.002 {method 'join' of 'str' objects}
           51    0.000    0.000    0.000    0.000 {len}
            2    0.000    0.000    0.000    0.000 {method 'seek' of 'file' objects}
            2    0.007    0.004    0.007    0.004 {open}
            1    0.000    0.000    0.000    0.000 misc.py:95(len_fp)
            1    1.799    1.799    4.389    4.389 main.py:30(decrypt_fp)
            1    0.000    0.000    0.000    0.000 {method 'disable' of '_lsprof.Profiler' objects}
            1    0.154    0.154    4.550    4.550 <string>:1(<module>)
            1    0.000    0.000    0.000    0.000 {ord}
            1    0.000    0.000    4.396    4.396 dec.py:7(test_)
            1    0.000    0.000    0.000    0.000 {maes.encrypt}

以上性能测试使用的代码可以在`app`下找到。


Changelog
---------

* 2013/4/12 消除了所有可以去掉的循环，速度略有提升

* 2013/4/11 所有inline函数转换成了宏，在50MB文件加密中，速度提高了大约1秒。



另外
----

文档待添。

email: chsc4698@gmail.com

ps 实在是想不出来一个好玩点的递归名字


