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

//Test(core, create_id2pangolin_works) {
//    char *metadata_file = "../data/subset.meta.tsv";
//    FILE *stream = fopen(metadata_file, "r");
//
//    char *line = NULL;
//    size_t len = 0;
//    ssize_t nread;
//    for (uint i=0; i<4; i++) {
//        nread = getline(&line, &len, stream);
//        printf("%s\n", line);
//    }
//}

Test(core, read_tsv_rec_returns_correct_value) {
    char *token;
    char *line = "AA\tBB\tCC\tDD\n";
    token = read_tsv_rec(line, 0);
    cr_assert(strcmp(token, "AA") == 0);
    token = read_tsv_rec(line, 1);
    cr_assert(strcmp(token, "BB") == 0);
    token = read_tsv_rec(line, 2);
    cr_assert(strcmp(token, "CC") == 0);
    token = read_tsv_rec(line, 3);
    cr_assert(strcmp(token, "DD") == 0);
    token = read_tsv_rec(line, 4);
    cr_assert(token == NULL);
    token = read_tsv_rec(line, 20);
    cr_assert(token == NULL);
}

Test(core, items_are_hashed_correctly) {
    item item1 = { .key = "test1_key", .value = "test1_value"};
    item item2 = { .key = "test2_key", .value = "test2_value"};
    uint h1, h2;

    h1 = hash(&item1);
    h2 = hash(&item2);
    cr_assert(h1 != h2);

    h2 = hash(&item1);
    cr_assert(h1 == h2);
}

Test(core, comparison_of_items_works) {
    item item1 = { .key = "test1_key", .value = "test1_value"};
    item item2 = { .key = "test2_key", .value = "test2_value"};
    cr_assert(cmp(&item1, &item2) != 0);
    cr_assert(cmp(&item1, &item1) == 0);
}

Test(core, get_id2pangolin_random_check) {
    map_t id2pangolin = get_id2pangolin("../data/subset.meta.tsv");
    item item1 = { .key = "hCoV-19/Germany/BY-MVP-0069/2020", .value = "B.1.1" };
    item item2 = * (item*)map_search(id2pangolin, &item1);
    cr_assert(strcmp(item1.value, item2.value) == 0);
}