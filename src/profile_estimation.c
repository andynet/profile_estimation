#include <htslib/sam.h>
#include "../hashing/src/hashing.h"
#include "functions.c"

int main(int argc, char** argv) {
    // parse_arguments
    const char *bamfile         = "../data/subset.bam";
    const char *metadata        = "../data/subset.meta.tsv";
    const char *variants_file   = "../data/variants.txt";
    const char *lineages        = "../data/lineages.yaml";
    const char *output          = "../data/output.tsv";

    char **variants = NULL;
    uint num_variants = 0;
    load_variants(variants_file, &variants, &num_variants);

    map_t id2pangolin = get_id2pangolin(metadata);
    map_t pangolin2parent = get_pangolin2parent(lineages);

    // open(bam)
    samFile *bam_stream = sam_open(bamfile, "r");
    sam_hdr_t *bam_header = sam_hdr_read(bam_stream);
    uint ref_size = get_ref_size(bam_header);

    char alphabet[] = {'A', 'C', 'G', 'T', 'N', '-'};
    uint alphabet_size = 6;

    uint dims[] = { ref_size, num_variants, alphabet_size };
    uint ***table = init_3d_array(ref_size, num_variants, alphabet_size);

    int ret;
    bam1_t *aln = bam_init1();
    ret = sam_read1(bam_stream, bam_header, aln);
    while (ret > 0) {
        record_t *record = record_read(bam_stream, bam_header, aln, &ret);
        record->variant = get_variant(record, id2pangolin, pangolin2parent);
        add_counts(table, record, variants, alphabet, num_variants, alphabet_size, ref_size);
        // if (record->variant == NULL) printf("%-64s\tNULL\n", record->id);
        // else printf("%-64s\t%s\n", record->id, record->variant);
        record_destroy(record);
    }

    bam_destroy1(aln);
    sam_hdr_destroy(bam_header);
    sam_close(bam_stream);

    // store(table, output);
    free(table);
}
