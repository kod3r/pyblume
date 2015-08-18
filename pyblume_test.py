#!/usr/bin/env python

import hashlib
import time
import struct
import random
import tempfile
import os

import pyblume

def pyblume_test_fpp(N):
    print '   a. Creating filter'
    bloom = pyblume.Filter(1024 * 1024 * 10, 0.000001, "/tmp/pyblume_fpp_test.bf")


    print '   b. Loading filter'
    start = time.time()
    for k in xrange(0, N, 10):
        bloom.add(str(k))
    end = time.time()
    print '         bloom.add: %d/s' % ((N / 10) / (end - start))

    print '   c. Computing false positive rate'
    fn_count = 0
    fp_count = 0
    start = time.time()
    for k in xrange(0, N):
        hit = str(k) in bloom
        if hit and k % 10 != 0:
            fp_count += 1
        elif not hit and k % 10 == 0:
            fn_count += 1
    end = time.time()
    print '         bloom.check: %d/s' % (N / (end - start))
    print '   d. %d false positives out of %d trials (p = %0.4f)' % (fp_count, N, float(fp_count) / N)

def pyblume_test_fromfile(N):
    filepath = '/tmp/pyblume_fromfile_test.bf'

    try:
        os.unlink(filepath)
    except:
        pass

    def create_filter():
        print '   a. Creating filter at "%s"' % (filepath,)
        bloom = pyblume.Filter(1024 * 1024 * 10, 0.000001, filepath)

        print '   b. Loading filter'
        start = time.time()
        for k in xrange(0, N, 10):
            bloom.add(str(k))
        end = time.time()
        print '         bloom.add: %d/s' % ((N / 10) / (end - start))
        bloom.close()

    def test_filter():
        bloom = pyblume.open(filepath)

        print '   c. Computing false positive rate'
        fn_count = 0
        fp_count = 0
        start = time.time()
        for k in xrange(0, N):
            hit = str(k) in bloom
            if hit and k % 10 != 0:
                fp_count += 1
            elif not hit and k % 10 == 0:
                fn_count += 1
        end = time.time()
        print '         bloom.check: %d/s' % (N / (end - start))
        print '   d. %d false positives out of %d trials (p = %0.4f)' % (fp_count, N, float(fp_count) / N)

    create_filter()
    test_filter()

def pyblume_test_full(N):
    print '   a. Creating filter'
    bloom = pyblume.Filter(1024 * 512, 0.000001, "/tmp/pyblume_full_test.bf")


    print '   b. Loading filter'
    start = time.time()
    for k in xrange(0, N, 10):
        bloom.add(str(k))
    end = time.time()
    print '         bloom.add: %d/s' % ((N / 10) / (end - start))

def pyblume_test_locking(N):
    print '   a. Creating filter'
    bloom = pyblume.Filter(1024 * 512, 0.000001, "/tmp/pyblume_locking_test.bf")

    bloom2 = pyblume.open("/tmp/pyblume_locking_test.bf", for_write=1)


    print '   b. Loading filter'
    start = time.time()
    for k in xrange(0, N, 10):
        bloom.add(str(k))
    end = time.time()
    print '         bloom.add: %d/s' % ((N / 10) / (end - start))

try:
    pyblume_test_locking(1000000 * 10)
except pyblume.LockedException as e:
    print 'OK!'

try:
    pyblume_test_full(1000000 * 10)
except pyblume.FullException as e:
    print 'OK!'

pyblume_test_fpp(1000000 * 10)
pyblume_test_fromfile(1000000 * 10)
