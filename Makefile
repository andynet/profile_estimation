SHELL := bash
.ONESHELL:
.SHELLFLAGS := -euo pipefail -c
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules

CC     := gcc
# CFLAGS := -Wall -Wextra -pedantic -march=native -std=c99 -O3
CFLAGS := -Wall -Wextra -march=native -O3

# git clone --recurse-submodules https://github.com/andynet/profile_estimation.git
# cd profile_estimation/
# mkdir build
# wget https://github.com/samtools/htslib/releases/download/1.14/htslib-1.14.tar.bz2
# tar -xvjf htslib-1.14.tar.bz2
# cd htslib-1.14
# make

# wget https://github.com/yaml/libyaml/releases/download/0.2.5/yaml-0.2.5.tar.gz
# tar -xvzf yaml-0.2.5.tar.gz

.PHONY: all
all: build_libs hts yaml build/profile_estimation build/profile_estimation_timed

.PHONY: build_libs
build_libs: build/libndarray.a build/libhashing.a build/libsafe_alloc.a

.PHONY: hts
hts:
	mkdir -p hts
	wget https://github.com/samtools/htslib/releases/download/1.14/htslib-1.14.tar.bz2
	tar -xvjf htslib-1.14.tar.bz2 -C hts --strip-components=1
	rm htslib-1.14.tar.bz2
	cd hts
	./configure
	make

.PHONY: yaml
yaml:
	mkdir -p yaml
	wget https://github.com/yaml/libyaml/releases/download/0.2.5/yaml-0.2.5.tar.gz
	tar -xvzf yaml-0.2.5.tar.gz -C yaml --strip-components=1
	rm yaml-0.2.5.tar.gz
	cd yaml
	./configure
	make

build/libndarray.a: src/ndarray.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $^ -o build/ndarray.o
	ar rcs $@ build/ndarray.o
	rm build/ndarray.o

build/libhashing.a: hashing/src/hashing.c
	$(CC) $(CFLAGS) -c $^ -o build/hashing.o
	ar rcs $@ build/hashing.o
	rm build/hashing.o

build/libsafe_alloc.a: hashing/src/safe_alloc.c
	$(CC) $(CFLAGS) -c $^ -o build/safe_alloc.o
	ar rcs $@ build/safe_alloc.o
	rm build/safe_alloc.o

build/profile_estimation: src/profile_estimation.c
	$(CC) $(CFLAGS) -o $@ $^ \
		-I hts -L hts \
		-I hashing/src -I src -L build \
		-I yaml/include -L yaml/src/.libs \
		-Wl,-rpath=$(shell realpath hts/libhts.so) \
		-lndarray -lhashing -lsafe_alloc -lyaml -lhts -lm

build/profile_estimation_timed: src/timed_profile_estimation.c
	$(CC) $(CFLAGS) -o $@ $^ \
		-I hts -L hts \
		-I hashing/src -I src -L build \
		-I yaml/include -L yaml/src/.libs \
		-Wl,-rpath=$(shell realpath hts/libhts.so) \
		-lndarray -lhashing -lsafe_alloc -lyaml -lhts -lm

.PHONY: clean
clean:
	rm -r build/*


# ndarray_test: test/test_ndarray.c
# 	gcc -o $@ $^ -lcriterion -lndarray
 
# test_functions: test/test_functions.c
# 	gcc -o $@ $^ -lcriterion -lndarray -lhashing -lsafe_alloc -lyaml -lhts -lm
 
# run: test/run.c
# 	ndarray hashing safe_alloc yaml hts m
