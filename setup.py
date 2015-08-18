from distutils.core import setup, Extension
setup(name="pyblume", version="1.0",
      ext_modules=[Extension("pyblume", ["pyblume.c", "murmur.c", "blume.c"])])
