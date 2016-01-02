#!/bin/bash

# This script generates content of LOCAL_C_INCLUDES variable based on current 
# status of project sources

# Those variables are required to be defined in jni/Android.mk file

cat <(
for i in `find ../Classes -type d `; do
    echo -n "\$(LOCAL_PATH)/../$i "
done
)

