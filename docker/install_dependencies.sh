#!/bin/sh
set -ex

echo "Installing dependencies"

apt-get update

DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata

apt-get install -y software-properties-common

add-apt-repository -y ppa:ubuntu-toolchain-r/test

apt-get install -y wget

# LLVM
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
add-apt-repository -y "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-19 main"

# CMake
wget -O - https://apt.kitware.com/keys/kitware-platform::Archive-latest.asc | apt-key add -
apt-add-repository 'deb https://apt.kitware.com/ubuntu/ jammy main'

apt-get update

apt-get install -y \
    pkg-config \
    unzip \
    git \
    make \
    cmake \
    lsb-release \
    gnupg \
    build-essential \
    ninja-build \
    linux-tools-common \
    linux-tools-generic \
    clang-19 \
    libc++-19-dev \
    libc++abi-19-dev \
    libclang-rt-19-dev \
    binutils-dev \
    libdwarf-dev \
    libdw-dev \
    pipx

pipx install conan
