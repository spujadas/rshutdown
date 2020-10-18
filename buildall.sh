#!/bin/bash

mkdir -p build/{linux-x64,rpi-arm32v6}

pushd build/linux-x64
rm -Rf *
cmake ../..
make -j4 package
popd

pushd build/rpi-arm32v6
rm -Rf *
cmake -DCMAKE_TOOLCHAIN_FILE=../../rpi-toolchain.cmake ../..
make -j4 package
popd
