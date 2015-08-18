# PyBlume

PyBlume is a fast, scalable bloom filter implementation for Python. 

## Installation

The easiest way to use the extension is to install it from [PyPI](https://pypi.python.org/pypi/Flask-Cassandra) using pip:
```sh
$ pip install pyblume
```

You can also install the extension directly from source.

```sh
$ python setup.py install
```

## Use

This example shows how to create a bloom filter using PyBlume, and check for a match.

```python
import pyblume

BLOOM_FILTER_MAX_FILESIZE = 1024 * 1024 * 500
BLOOM_ERROR_RATE = 0.000001
fileloc = "/tmp/test.blume"

bf = pyblume.Filter(BLOOM_FILTER_MAX_FILESIZE, BLOOM_ERROR_RATE, fileloc)
bf.add("Terbium")
bf.close()

check = ['Baltimore', 'Terbium', 'Labs']

bf = pyblume.open(fileloc, for_write=False)
for x in check:
	if x in bf:
		print "Found"
bf.close()

```

## Contributions

Special thanks to Austin Appleby for placing MurmurHash3 into the public domain.

If you would like to extend the functionality of the extension, pull requests are most welcome.