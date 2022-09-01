#!/bin/sh

set -x      # 调试

if [ ! -d "./build" ]; then
  mkdir ./build 
fi

if [ ! -d "./LogFiles" ]; then
  mkdir ./LogFiles
fi

cd ./build \
  && cmake .. \
  && make
