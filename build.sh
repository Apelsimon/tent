#!/bin/bash

ROOT=`pwd`
LIBTORRENT_BUILD_DIR=libtorrent/build
BUILD_DIR=build

# TODO: parse input
rm -rf libtorrent/build 
rm -rf build

if [ ! -d "$LIBTORRET_BUILD_DIR" ]; then 
    mkdir "$LIBTORRET_BUILD_DIR"
fi

cd "$LIBTORRET_BUILD_DIR"
cmake ..
make 
cd $ROOT 

if [ ! -d "$BUILD_DIR" ]; then 
    mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"
cmake ..
make