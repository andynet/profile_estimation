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
    pair_t item1 = { .key = "test1_key", .value = "test1_value"};
    pair_t item2 = { .key = "test2_key", .value = "test2_value"};
    uint h1, h2;

    h1 = hash(&item1);
    h2 = hash(&item2);
    cr_assert(h1 != h2);

    h2 = hash(&item1);
    cr_assert(h1 == h2);
}

Test(core, comparison_of_items_works) {
    pair_t item1 = { .key = "test1_key", .value = "test1_value"};
    pair_t item2 = { .key = "test2_key", .value = "test2_value"};
    cr_assert(cmp(&item1, &item2) != 0);
    cr_assert(cmp(&item1, &item1) == 0);
}

Test(core, get_id2pangolin_random_check) {
    map_t id2pangolin = get_id2pangolin("../data/subset.meta.tsv");
    pair_t result;

    pair_t item1 = { .key = "hCoV-19/Germany/BY-MVP-0069/2020", .value = "B.1.1" };
    result = * (pair_t*)map_search(id2pangolin, &item1);
    cr_assert(strcmp(item1.value, result.value) == 0);

    pair_t item2 = { .key = "hCoV-19/New_Zealand/20VR3012/2020", .value = "B" };
    result = * (pair_t*)map_search(id2pangolin, &item2);
    cr_assert(strcmp(item2.value, result.value) == 0);
}

Test(core, get_pangolin2parent_random_check) {
    map_t pangolin2parent = get_pangolin2parent("../data/lineages.yml");
    pair_t result;

    pair_t pair1 = { .key = "AA.1", .value = "B.1.177.15" };
    result = *(pair_t *) map_search(pangolin2parent, &pair1);
    cr_assert(strcmp(pair1.value, result.value) == 0);

    pair_t pair2 = { .key = "A.2.5.2", .value = "A.2.5" };
    result = *(pair_t *) map_search(pangolin2parent, &pair2);
    cr_assert(strcmp(pair2.value, result.value) == 0);
}

Test(core, get_pangolin2parent_small_check) {
    map_t pangolin2parent = get_pangolin2parent("../data/lineages_small.yml");
    pair_t result;

    pair_t pair1 = { .key = "A.2.5.1", .value = "A.2"};
    result = *(pair_t *) map_search(pangolin2parent, &pair1);
    cr_assert(strcmp(pair1.value, result.value) == 0);

    pair_t pair2 = { .key = "A.2", .value = "A"};
    result = *(pair_t *) map_search(pangolin2parent, &pair2);
    cr_assert(strcmp(pair2.value, result.value) == 0);

    pair_t pair3 = { .key = "A.2.2", .value = "A.2"};
    result = *(pair_t *) map_search(pangolin2parent, &pair3);
    cr_assert(strcmp(pair3.value, result.value) == 0);
}

Test(core, pair_are_created_correctly) {
    pair_t *pair;
    char *key = "key1";
    pair = pair_create(key, "value1");
    cr_assert(strcmp(pair->key, key) == 0);
    cr_assert(strcmp(pair->value, "value1") == 0);
    key = "key2";
    pair = pair_create(key, "value2");
    cr_assert(strcmp(pair->key, key) == 0);
    cr_assert(strcmp(pair->value, "value2") == 0);
}

Test(core, initialization_of_3d_array_has_expected_dimensions) {
    int x, y, z;
    x = 2; y = 3; z = 4;
    int ***array = init_3d_array(x, y, z);
    array[0]  [0]  [0]   = 1;
    array[0]  [0]  [z-1] = 2;
    array[0]  [y-1][0]   = 3;
    array[0]  [y-1][z-1] = 4;
    array[x-1][0]  [0]   = 5;
    array[x-1][0]  [z-1] = 6;
    array[x-1][y-1][0]   = 7;
    array[x-1][y-1][z-1] = 8;
}

Test(core, is_reading_whole_bam) {
    samFile *bam_stream = sam_open("../data/subset.bam", "r");

    bam_hdr_t *bam_header = sam_hdr_read(bam_stream);
    bam1_t *aln = bam_init1();

    int ret, n = 0;
    ret = sam_read1(bam_stream, bam_header, aln);
    while (ret > 0) {
        record_t *record = record_read(bam_stream, bam_header, aln, &ret);
        record_destroy(record);
        n++;
    }
    cr_assert(ret == -1);
    cr_assert(n == 1230);

    bam_destroy1(aln);
    sam_hdr_destroy(bam_header);
    sam_close(bam_stream);
}

Test(core, record_read_reads_correct_sequences_random_check) {
    samFile *bam_stream = sam_open("../data/subset.bam", "r");

    bam_hdr_t *bam_header = sam_hdr_read(bam_stream);
    bam1_t *aln = bam_init1();

    int ret;
    record_t *record;
    ret = sam_read1(bam_stream, bam_header, aln);

    record = record_read(bam_stream, bam_header, aln, &ret);
    cr_assert(strcmp(record->id, "hCoV-19/England/CAMB-755E9/2020") == 0);
    cr_assert(record->seq[120] == 'C');
    record_destroy(record);

    record = record_read(bam_stream, bam_header, aln, &ret);
    cr_assert(strcmp(record->id, "hCoV-19/Iran/Qom255194/2020") == 0);
    cr_assert(record->seq[160] == '.');
    record_destroy(record);

    record = record_read(bam_stream, bam_header, aln, &ret);
    cr_assert(strcmp(record->id, "hCoV-19/Australia/NSW153/2020") == 0);
    cr_assert(record->seq[29040] == 'G');
    record_destroy(record);

    bam_destroy1(aln);
    sam_hdr_destroy(bam_header);
    sam_close(bam_stream);
}