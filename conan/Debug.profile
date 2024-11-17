[settings]
arch=armv8
build_type=Debug
compiler=clang
compiler.cppstd=20
compiler.libcxx=libc++
compiler.version=17
os=Macos

[buildenv]
CC=/opt/homebrew/opt/llvm@17/bin/clang
CXX=/opt/homebrew/opt/llvm@17/bin/clang++

[runenv]
CC=/opt/homebrew/opt/llvm@17/bin/clang
CXX=/opt/homebrew/opt/llvm@17/bin/clang++
