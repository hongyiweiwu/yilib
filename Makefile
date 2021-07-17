CPPCOMPILER := /usr/local/opt/llvm/bin/clang++
CPPFLAG := -nostdlib -nostdinc++ -std=c++20 -Iinclude -W -Wall -Wextra -Wpedantic -Wno-literal-suffix
LDFLAGS := -L/usr/local/opt/llvm/lib -lc++abi

SRC_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(SRC_FILES))

output: $(OBJ_FILES)
	$(CPPCOMPILER) $(LDFLAGS) -g -o $@ $^

obj/%.o: src/%.cpp
	$(CPPCOMPILER) $(CPPFLAG) -g -c -o $@ $<

clean:
	rm obj/*