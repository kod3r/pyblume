from distutils.core import setup, Extension
setup(name="pyblume", 
	version="1.0", 
	url='http://terbiumlabs.com/pyblume', 
	license='BSD', 
	author="Terbium Labs", 
	author_email="floss_dev@terbiumlabs.com", 
	description='A fast, scalable Bloom Filter for Python', 
	zip_safe=False,
    include_package_data=True,
    platforms='Linux',
    ext_modules=[Extension("pyblume", ["pyblume.c", "murmur.c", "blume.c"])],
	classifiers=[
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Programming Language :: Python',
        'Programming Language :: C']
    )