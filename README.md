# profile_estimation

## Dependencies
- hts
- yaml
- criterion
- hashing

https://www.wpsoftware.net/andrew/pages/libyaml.html

To install yaml parsing:
```
sudo apt-get update -y
sudo apt-get install -y libyaml-dev
```

## Installation
```
git clone --recurse-submodules https://github.com/andynet/profile_estimation.git
cd profile_estimation
mkdir build
cmake -S . -B build/
make
```
