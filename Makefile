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

.PHONY: build_libs
build_libs: build/libndarray.a build/libhashing.a build/libsafe_alloc.a

build/libndarray.a: src/ndarray.c
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
	$(CC) $(CFLAGS) -o $@ $^ -static \
		-I htslib-1.14 -L htslib-1.14 \
		-I hashing/src -I src -L build \
		-L yaml-0.2.5/src/.libs -I yaml-0.2.5/include
		-lndarray -lhashing -lsafe_alloc -lyaml -lhts -lm

.PHONY: clean
clean:
	rm -r build/*


# ndarray_test: test/test_ndarray.c
# 	gcc -o $@ $^ -lcriterion -lndarray
# 
# test_functions: test/test_functions.c
# 	gcc -o $@ $^ -lcriterion -lndarray -lhashing -lsafe_alloc -lyaml -lhts -lm
# 
# run: test/run.c
# 	ndarray hashing safe_alloc yaml hts m
# 
# profile_estimation: src/profile_estimation.c
# 	ndarray hashing safe_alloc yaml hts m
# 
# timed_profile_estimation: src/timed_profile_estimation.c
# 	ndarray hashing safe_alloc yaml hts m
