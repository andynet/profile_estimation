# profile_estimation

## Dependencies
- hts
- yaml
- criterion
- hashing


## Installation
```
git clone --recurse-submodules https://github.com/andynet/profile_estimation.git
cd profile_estimation
mkdir build
make
```

## Run with
```
./build/profile_estimation          \
    -b data/test1/subset.bam        \
    -m data/test1/subset.meta.tsv   \
    -v data/test1/variants.txt      \
    -l data/test1/lineages.yml      \
    -o output.csv

./build/profile_estimation_timed    \
    -b data/test1/subset.bam        \
    -m data/test1/subset.meta.tsv   \
    -v data/test1/variants.txt      \
    -l data/test1/lineages.yml      \
    -o output.csv                   \
    -d 2020-12-15                   \
    -s 30
```
