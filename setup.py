from distutils.core import setup, Extension


setup(name='maes',
      version='0.1',
      description='c-implemented aes for python',
      ext_modules=[Extension('maes',
                             sources=['src/common.c',
                                      'src/tables.c', 
                                      'src/maes.c'],
                             include_dirs=['include'])])

