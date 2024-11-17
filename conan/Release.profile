[settings]
arch=armv8
build_type=Release
compiler=clang
compiler.cppstd=20
compiler.libcxx=libc++
compiler.version=17
os=Macos

[runenv]
CC=/opt/homebrew/opt/llvm@17/bin/clang
CXX=/opt/homebrew/opt/llvm@17/bin/clang++

[buildenv]
CC=/opt/homebrew/opt/llvm@17/bin/clang
CXX=/opt/homebrew/opt/llvm@17/bin/clang++
