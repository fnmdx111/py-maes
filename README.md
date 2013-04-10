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

使用如下命令

    python setup.py build

如果使用mingw32，那么命令为

    python setup.py build --compiler=mingw32

如果出现`未识别的参数-mno-cygwin`，那么请打`cygwinccompiler.patch`（这个问题出现于高版本的mingw32上）。

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
    >>> maes.encrypt('\x00\x01\x00\x01\x01\xa1\x98\xaf\xda\x78\x17\x34\x86\x15\x35\x66', '\x00\x01\x20\x01\x71\x01\x98\xae\xda\x79\x17\x14\x60\x15\x35\x94')
    'l\xddYk\x8fVB\xcb\xd2;G\x98\x1aeB*'
    >>> maes.decrypt('l\xddYk\x8fVB\xcb\xd2;G\x98\x1aeB*') # 使用了encrypt里提供的密钥
    '\x00\x01\x00\x01\x01\xa1\x98\xaf\xdax\x174\x86\x155f'


TODO
----

* iterartor for large file encryption


另外
----

性能待测。文档待添。

email: chsc4698@gmail.com

ps 实在是想不出来一个好玩点的递归名字


