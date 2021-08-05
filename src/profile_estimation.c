#include <htslib/sam.h>
#include "../hashing/src/hashing.h"
#include "functions.c"

int main(int argc, char** argv) {
    // parse_arguments
    const char *bamfile         = "../data/subset.bam";
    const char *metadata        = "../data/subset.meta.tsv";
    const char *variants_file   = "../data/variants.txt";
    const char *lineages        = "../data/lineages.yml";
    const char *output          = "../data/output.tsv";

    uint num_variants = 0;
    char **variants = NULL;
    load_variants(variants_file, &variants, &num_variants);
    add_variant(&variants, &num_variants, "other");

    map_t id2pangolin = get_id2pangolin(metadata);
    map_t pangolin2parent = get_pangolin2parent(lineages);
    // add_root(pangolin2parent, "other");
    // isolate_subclades(pangolin2parent, variants);

    samFile *bam_stream = sam_open(bamfile, "r");
    sam_hdr_t *bam_header = sam_hdr_read(bam_stream);
    uint ref_size = get_ref_size(bam_header);

    uint alphabet_size = 6;
    char alphabet[] = {'A', 'C', 'G', 'T', 'N', '-'};

    uint ***table = init_3d_array(ref_size, num_variants, alphabet_size);

    int ret;
    bam1_t *aln = bam_init1();
    ret = sam_read1(bam_stream, bam_header, aln);
    while (ret > 0) {
        record_t *record = record_read(bam_stream, bam_header, aln, &ret);
        record->variant = get_variant(record, id2pangolin, pangolin2parent);
        if (record->variant != NULL)
            add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
        record_destroy(record);
    }

    bam_destroy1(aln);
    sam_hdr_destroy(bam_header);
    sam_close(bam_stream);

    FILE *out = fopen(output, "w");
    array_3d_print(table, ref_size, num_variants, alphabet_size, variants, alphabet, out);
    fclose(out);

    free(table);
}
