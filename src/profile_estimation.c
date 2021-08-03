#include <htslib/sam.h>
#include "../hashing/src/hashing.h"
#include "functions.c"

int main(int argc, char** argv) {
    // parse_arguments
    const char *bamfile         = "../data/test/subset.bam";
    const char *metadata        = "../data/test/metadata.tsv";
    const char *variants_file   = "../data/test/variants.txt";
    const char *lineages        = "../data/test/lineages.yaml";
    const char *output          = "../data/test/output.tsv";

    char **variants = NULL;
    uint num_variants = 0;
    load_variants(variants_file, &variants, &num_variants);

    map_t id2pangolin = get_id2pangolin("../data/subset.meta.tsv");
    map_t pangolin2parent = get_pangolin2parent("../data/lineages.yml");

    // open(bam)
    samFile *bam_stream = sam_open(bamfile, "r");
    sam_hdr_t *bam_header = sam_hdr_read(bam_stream);
    uint ref_size = get_ref_size(bam_header);

    char alphabet[] = {'A', 'C', 'G', 'T', 'N', '-'};
    uint alphabet_size = 6;

    int ***table = init_3d_array(ref_size, num_variants, alphabet_size);

    bam1_t *bam_record = bam_init1();
    int res = sam_read1(bam_stream, bam_header, bam_record);

    // while not the end:
    while (res >= 0) {
        record_t *record = record_read(bam_stream, bam_header, bam_record);
        record->variant = get_variant(record, id2pangolin, pangolin2parent);
        add_counts(table, record);
    }

    store(table, output);
    free(table);

//        << bam_get_qname(b) << '\n'
//        << bam_get_cigar(b) << '\n'
//        << bam_get_seq(b)   << '\n'
//        << bam_get_qual(b)  << '\n'
//        << bam_get_aux(b)   << '\n'
//        ;
        // bam_get_qname, bam_get_cigar, bam_get_seq, bam_get_qual and bam_get_aux
}
