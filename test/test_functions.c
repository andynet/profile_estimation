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
    char **variants = NULL;
    uint number = 0;

    load_variants("../data/variants.txt", &variants, &number);
    cr_assert(strcmp(variants[0], "B.1.1.7") == 0);
    cr_assert(strcmp(variants[1], "B.1.160") == 0);
    cr_assert(strcmp(variants[2], "B.1.177") == 0);
    cr_assert(strcmp(variants[3], "B.1.258") == 0);
}

Test(core, add_variant_adds_variants_and_increases_counter) {
    char **variants = NULL;
    uint number = 0;

    load_variants("../data/variants.txt", &variants, &number);
    add_variant(&variants, &number, "other");

    cr_assert(strcmp(variants[4], "other") == 0);
    cr_assert(number == 5);
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

Test(core, read_tsv_rec_works_with_multiple_tabs) {
    char *token;
    char *line = "A\t\tC\n";
    token = read_tsv_rec(line, 0);
    cr_assert(strcmp(token, "A") == 0);
    token = read_tsv_rec(line, 1);
    cr_assert(strcmp(token, "") == 0);
    token = read_tsv_rec(line, 2);
    cr_assert(strcmp(token, "C") == 0);
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

Test(core, get_pangolin2parent_does_not_leak) {
    map_t pangolin2parent = get_pangolin2parent("../data/lineages_small.yml");
    free_map_content(&pangolin2parent);
    map_destroy(&pangolin2parent);
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

Test(core, get_root_returns_correct_root) {
    map_t pangolin2parent = get_pangolin2parent("../data/lineages_small.yml");
    pair_t *pair, *root;

    uint idx = 0;
    void *item = NULL;

    map_iterate(pangolin2parent, &idx, &item);
    idx++;
    while (item != NULL) {
        pair = (pair_t *)item;
        root = get_root(pair, pangolin2parent);
        cr_assert(strcmp(root->key, root->value) == 0);
        cr_assert(strcmp(root->key, "A") == 0);

        map_iterate(pangolin2parent, &idx, &item);
        idx++;
    }

    pair = pair_create("B", "B");
    root = get_root(pair, pangolin2parent);
    cr_assert(root == NULL);
    pair_free(pair);

    free_map_content(&pangolin2parent);
    map_destroy(&pangolin2parent);
}

Test(core, add_root_works) {
    map_t pangolin2parent = get_pangolin2parent("../data/lineages_small.yml");
    add_root(pangolin2parent, "other");
    pair_t *pair, *root;

    uint idx = 0;
    void *item = NULL;

    map_iterate(pangolin2parent, &idx, &item);
    idx++;
    while (item != NULL) {
        pair = (pair_t *)item;
        root = get_root(pair, pangolin2parent);
        cr_assert(strcmp(root->key, root->value) == 0);
        cr_assert(strcmp(root->key, "other") == 0);

        map_iterate(pangolin2parent, &idx, &item);
        idx++;
    }
    free_map_content(&pangolin2parent);
    map_destroy(&pangolin2parent);
}

Test(core, get_variants_returns_correct_variants) {
    record_t *record;
    map_t id2pangolin = get_id2pangolin("../data/subset.meta.tsv");
    map_t pangolin2parent = get_pangolin2parent("../data/lineages.yml");

    record = record_create("hCoV-19/England/CAMB-755E9/2020", 3);
    record->variant = get_variant(record, id2pangolin, pangolin2parent);
    cr_assert(strcmp(record->variant, "A") == 0);
    record_destroy(record);

    record = record_create("hCoV-19/Iran/Qom255194/2020", 3);
    record->variant = get_variant(record, id2pangolin, pangolin2parent);
    cr_assert(record->variant == NULL);
    record_destroy(record);

    record = record_create("hCoV-19/Australia/NSW153/2020", 3);
    record->variant = get_variant(record, id2pangolin, pangolin2parent);
    cr_assert(strcmp(record->variant, "A") == 0);
    record_destroy(record);

    record = record_create("hCoV-19/Guangzhou/GZMU0034/2020", 3);
    record->variant = get_variant(record, id2pangolin, pangolin2parent);
    cr_assert(strcmp(record->variant, "A") == 0);
    record_destroy(record);

    free_map_content(&pangolin2parent);
    map_destroy(&pangolin2parent);

    free_map_content(&id2pangolin);
    map_destroy(&id2pangolin);
}

Test(core, add_counts_works_random_check) {
    uint num_variants = 2;
    char *variants[] = {"A", "B"};

    uint alphabet_size = 6;
    char alphabet[] = {'A', 'C', 'G', 'T', 'N', '-'};

    uint ref_size = 4;

    uint ***table = init_3d_array(ref_size, num_variants, alphabet_size);
    record_t *record;

    record = record_full_create("seq1", "AAAA", "A");
    add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
    record_destroy(record);

    record = record_full_create("seq3", "CAAC", "A");
    add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
    record_destroy(record);

    record = record_full_create("seq2", "CGTA", "B");
    add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
    record_destroy(record);

    record = record_full_create("seq4", ".-NU", "B");
    add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
    record_destroy(record);

    record = record_full_create("seq5", "ACGT", "C");
    add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
    record_destroy(record);

    cr_assert(table[0][0][0] == 1);
    cr_assert(table[1][0][0] == 2);
    cr_assert(table[0][1][1] == 1);
    cr_assert(table[1][1][5] == 1);
    // array_3d_print(table, ref_size, num_variants, alphabet_size, variants, alphabet);
    array_3d_free(table, ref_size, num_variants, alphabet_size);
}

Test(core, test_isolate_subclades_random_check) {
    uint num_variants = 0;
    char **variants = NULL;
    load_variants("../data/variants.txt", &variants, &num_variants);
    add_variant(&variants, &num_variants, "other");

    map_t pangolin2parent = get_pangolin2parent("../data/lineages.yml");
    add_root(pangolin2parent, "other");
    isolate_subclades(pangolin2parent, variants, num_variants);

    pair_t *pair, *root;
    pair = pair_create("B.1.258.20", "B.1.258.20");
    root = get_root(pair, pangolin2parent);
    cr_assert(strcmp(root->key, "B.1.258") == 0);

    strcpy(pair->value, "AY.2");
    root = get_root(pair, pangolin2parent);
    cr_assert(strcmp(root->key, "other") == 0);

    strcpy(pair->value, "Y.1");
    root = get_root(pair, pangolin2parent);
    cr_assert(strcmp(root->key, "B.1.177") == 0);

    strcpy(pair->value, "L.1");
    root = get_root(pair, pangolin2parent);
    cr_assert(strcmp(root->key, "other") == 0);

    strcpy(pair->value, "B.1.160");
    root = get_root(pair, pangolin2parent);
    cr_assert(strcmp(root->key, "B.1.160") == 0);

    pair_free(pair);
    free_map_content(&pangolin2parent);
    map_destroy(&pangolin2parent);
    dealloc_variants(&variants, &num_variants);
}