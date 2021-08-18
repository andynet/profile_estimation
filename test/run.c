#include "../src/functions.c"
#include "../src/ndarray.c"

int main() {
    uint num_variants = 2;
    char *variants[] = {"A", "B"};

    uint alphabet_size = 6;
    char alphabet[] = {'A', 'C', 'G', 'T', 'N', '-'};

    uint ref_size = 4;

    ndarray_t *table = ndarray_create(3, ref_size, num_variants, alphabet_size);
    record_t *record;

    record = record_full_create("seq1", "AAAA", "A");
    add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
    record_destroy(record);
    array_3d_print(table, ref_size, num_variants, alphabet_size, variants, alphabet, stdout);

    record = record_full_create("seq3", "CAAC", "A");
    add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
    record_destroy(record);
    array_3d_print(table, ref_size, num_variants, alphabet_size, variants, alphabet, stdout);

    record = record_full_create("seq2", "CGTA", "B");
    add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
    record_destroy(record);
    array_3d_print(table, ref_size, num_variants, alphabet_size, variants, alphabet, stdout);

    record = record_full_create("seq4", ".-NU", "B");
    add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
    record_destroy(record);
    array_3d_print(table, ref_size, num_variants, alphabet_size, variants, alphabet, stdout);

    record = record_full_create("seq5", "ACGT", "C");
    add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
    record_destroy(record);
    array_3d_print(table, ref_size, num_variants, alphabet_size, variants, alphabet, stdout);

//    cr_assert(*ndarray_at(table, 0, 0, 0) == 1);
//    cr_assert(*ndarray_at(table, 1, 0, 0) == 2);
//    cr_assert(*ndarray_at(table, 0, 1, 1) == 1);
//    cr_assert(*ndarray_at(table, 1, 1, 5) == 1);

    array_3d_print(table, ref_size, num_variants, alphabet_size, variants, alphabet, stdout);
    ndarray_destroy(&table);
}

