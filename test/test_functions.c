#include <criterion/criterion.h>
#include "../src/functions.c"

Test(core, empty_test) {}

Test(core, get_num_lines_returns_correct_value) {
    uint n_lines = get_n_lines("../data/variants.txt");
    cr_assert(n_lines == 5);
}

Test(core, load_variant_loads) {
    // Why can't I use const in the test?
    // const char *variants_file = "../data/test/variants.txt";
    char **variants;
    uint number;

    load_variants("../data/variants.txt", &variants, &number);
    cr_assert(strcmp(variants[0], "B.1.1.7") == 0);
    cr_assert(strcmp(variants[1], "B.1.160") == 0);
    cr_assert(strcmp(variants[2], "B.1.177") == 0);
    cr_assert(strcmp(variants[3], "B.1.258") == 0);
}

Test(core, create_id2pangolin_works) {
    char *metadata_file = "../data/test/subset.meta.tsv";
}