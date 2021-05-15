CPPCOMPILER := /usr/local/opt/llvm/bin/clang++
CPPFLAG := -nostdlib -nostdinc++ -std=c++20 -Iinclude
LDFLAGS := -L/usr/local/opt/llvm/lib -lc++abi

SRC_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(SRC_FILES))

output: $(OBJ_FILES)
	$(CPPCOMPILER) $(LDFLAGS) -o $@ $^

obj/%.o: src/%.cpp
	$(CPPCOMPILER) $(CPPFLAG) -c -o $@ $<

clean:
	rm obj/*