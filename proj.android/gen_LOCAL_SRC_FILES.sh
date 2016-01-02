#!/bin/bash

# This script generates content of LOCAL_SRC_FILES variable based on current 
# status of project sources

# Those variables are required to be defined in jni/Android.mk file

cat <(
echo -n "./ratnroll/main.cpp "
for i in `find ../Classes -iname '*.cpp' ! -iname 'gen.cpp'`; do
    echo -n "../$i "
done
)


