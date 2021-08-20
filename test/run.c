#include "../src/functions.c"
#include "../src/ndarray.c"

int main() {
//    uint daydiff;
//    map_t id2date = get_tsv_pair_map("../data/test1/subset.meta.tsv", 0, 3);
//    daydiff = get_daydiff("hCoV-19/England/CAMB-755E9/2020", id2date, "2020-03-15");
//
//    double weight = get_gaussian_weight(daydiff, 0, 15);
//    printf("%lf\n", weight);
    double sum = 0;
    for (int i = (-10); i <= 10; i++ ) {
        printf("%d\t%lf\n", i, get_gaussian_weight(i, 0, 1));
        sum += get_gaussian_weight(i, 0, 1);
    }
    printf("%lf", sum);
}

