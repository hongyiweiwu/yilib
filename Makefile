CPPCOMPILER=/usr/local/opt/llvm/bin/clang++
INCLUDE=-Iinclude
CPPFLAG=-std=c++20
LIBDIR=/usr/local/opt/llvm/lib

CPPAPI=c++abi

all:
	$(CPPCOMPILER) $(CPPFLAG) -o src/* $(INCLUDE) -L$(LIBDIR) -l$(CPPAPI)