MAES
====

MAES是为Python编写的外挂AES库，在AES中，操作单位是字节，本实现中为了最大程度利用机器性能，将一个字节的数据存储在一个本地具有字长长度的数据类型（通常是`int`）中。 另外，为了减少调用开销，程序基本上是使用宏完成的，但是尽可能保证了可读性。


环境
----

本库在win7 64位，mingw32 4.7.1，python2.7 32位下测试通过。


编译
----

不知道为什么在vs下编译会报语法错误（目前只试了vs2012express）。所以请使用如下命令

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

* implement CBC mode for n MB block


性能
----

以下为在ECB模式下，使用128bit密钥加密50MB的QQ2013Beta1.exe的profiler结果。padding模式为PKCS#7。其中，按块加密函数是调用的maes中的函数，其他调用均为Python调用。

    Fri Apr 12 17:44:19 2013    encrypt_prof_ix

             3280143 function calls in 4.460 seconds

       Ordered by: call count

       ncalls  tottime  percall  cumtime  percall filename:lineno(function)
      3279931    2.284    0.000    2.284    0.000 {maes.encrypt}
           51    0.054    0.001    0.054    0.001 {method 'read' of 'file' objects}
           51    0.000    0.000    0.000    0.000 {len}
           51    0.036    0.001    0.036    0.001 {method 'write' of 'file' objects}
           51    0.098    0.002    0.098    0.002 {method 'join' of 'str' objects}
            2    0.007    0.003    0.007    0.003 {open}
            1    0.000    0.000    4.459    4.459 enc_prof_bootstrap.py:9(test)
            1    1.980    1.980    4.452    4.452 main.py:28(encrypt_fp)
            1    0.000    0.000    0.000    0.000 {chr}
            1    0.001    0.001    4.460    4.460 <string>:1(<module>)
            1    0.000    0.000    0.000    0.000 {method 'disable' of '_lsprof.Profiler' objects}
            1    0.000    0.000    0.000    0.000 main.py:17(padding)


以下为解密上述加密结果的profiler结果。

    Fri Apr 12 17:44:28 2013    decrypt_prof

             3280197 function calls in 5.131 seconds

       Ordered by: call count

       ncalls  tottime  percall  cumtime  percall filename:lineno(function)
      3279930    2.708    0.000    2.708    0.000 {maes.decrypt}
           52    0.001    0.000    0.001    0.000 {method 'tell' of 'file' objects}
           51    0.037    0.001    0.037    0.001 {method 'write' of 'file' objects}
           51    0.100    0.002    0.100    0.002 {method 'join' of 'str' objects}
           51    0.000    0.000    0.000    0.000 {len}
           51    0.056    0.001    0.056    0.001 {method 'read' of 'file' objects}
            2    0.000    0.000    0.000    0.000 {method 'seek' of 'file' objects}
            2    0.007    0.004    0.007    0.004 {open}
            1    0.000    0.000    0.000    0.000 main.py:9(len_fp)
            1    2.065    2.065    4.968    4.968 main.py:44(decrypt_fp)
            1    0.156    0.156    5.131    5.131 <string>:1(<module>)
            1    0.000    0.000    0.000    0.000 {method 'disable' of '_lsprof.Profiler' objects}
            1    0.000    0.000    0.000    0.000 {ord}
            1    0.000    0.000    4.975    4.975 dec_prof_bootstrap.py:7(test_)
            1    0.000    0.000    0.000    0.000 {maes.encrypt}
    

以上性能测试使用的代码可以在`app`下找到。

测试机配置：AMD 640 3.00 GHz，4G 1333，5200RPM，win7 x64，python2.7 x86，mingw32 4.7.1 -O


Changelog
---------

* 2013/4/12 将byte扩展至uint，而不是4个byte缩进一个uint，速度有很小的提升，但是解密速度变慢了一些

* 2013/4/12 消除了所有可以去掉的循环，速度略有提升

* 2013/4/11 所有inline函数转换成了宏，在50MB文件加密中，速度提高了大约1秒。



另外
----

文档待添。

email: chsc4698@gmail.com

ps 实在是想不出来一个好玩点的递归名字


