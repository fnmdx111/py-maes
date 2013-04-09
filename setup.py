from distutils.core import setup, Extension


setup(name='PackageName',
      version='0.1',
      description='This is a demo package',
      ext_modules=[Extension('maes',
                             sources=['src/tables.c', 
                                      'src/aes.c'],
                             include_dirs=['include'])])
