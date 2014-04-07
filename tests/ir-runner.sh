#!/bin/sh
TEST_INPUT=$1
BASE_NAME=$(basename $TEST_INPUT .hir)
../src/irvm $TEST_INPUT > $BASE_NAME.std 2> $BASE_NAME.err 
../src/irvm -t $TEST_INPUT > $BASE_NAME.out.trace 2> /dev/null

diff $BASE_NAME.std $BASE_NAME.out.trace > /dev/null 2>&1
if [ "$?" != 0 ]; then
    echo "trace mode alters output" > /dev/stderr
    diff -u $BASE_NAME.std $BASE_NAME.out.trace
    exit 1
fi

cat $BASE_NAME.std $BASE_NAME.err > $BASE_NAME.out
diff $BASE_NAME.exp $BASE_NAME.out  >/dev/null 2>&1
if [ "$?" != 0 ]; then
    diff -u $BASE_NAME.exp $BASE_NAME.out
    exit 1
else
    exit 0
fi
