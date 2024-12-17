#!/bin/sh
set -ex

llvm_version=19

echo "Setting /usr/bin/clang* to clang*-${llvm_version}"

ln -s $(which clang-19) /usr/bin/clang
ln -s $(which clang++-19) /usr/bin/clang++
