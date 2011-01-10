#!/bin/bash

echo "Running tests"
srcdir=$EC_TEST_DATA/corel
for f in $srcdir/*.prn; do
  testcase=`basename $f .prn`
  echo "*$testcase...."
  $EC_PYTHON/rtltest.py $f
done
