#!/bin/bash

cd `dirname $0`

mkdir -p tmp
cd tmp
cmake ..
make -j `nproc` $*

