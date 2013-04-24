from distutils.core import setup, Extension


setup(name='maes',
      version='0.1',
      description='c-implemented aes for python',
      author='mad4alcohol@github',
      author_email='chsc4698@gmail.com',
      maintainer='mad4alcohol@github',
      url='https://github.com/mad4alcohol/py-maes',
      ext_modules=[Extension('maes',
                             sources=['src/common.c',
                                      'src/tables.c', 
                                      'src/maes.c'],
                             include_dirs=['include'],
                             define_macros=[('CHUNK_SIZE', '8192 * 128 * 8')])])

