#include "../src/functions.c"
#include "../src/ndarray.c"

int main() {
    uint daydiff;
    map_t id2date = get_tsv_pair_map("../data/test1/subset.meta.tsv", 0, 3);
    daydiff = get_daydiff("hCoV-19/England/CAMB-755E9/2020", id2date, "2020-02-15");

    uint weight = get_gaussian_weight(daydiff, 0, 15) * 1024;
    printf("%u\n", weight);

}

