MAES
====

环境
----

MAES是为32bit环境下编写的，所以请用32位的python。
如果需要自己编译，也请用32位的编译器（或目标环境设为32位）。

原则上来讲，64bit或16bit下，程序会不能得出正常结果，因为只有32位下一个int为32位。

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


TODO
----

* replace `unsigned int` with `uint32_t` for the sake of portability
* remove all `printf` for RELEASE


另外
----

性能待测。文档待添。

email: chsc4698@gmail.com

ps 实在是想不出来一个好玩点的递归名字


