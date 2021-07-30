#include <htslib/sam.h>
#include "functions.c"

typedef struct {} record;

int main(int argc, char** argv) {
    // parse_arguments
    const char *bamfile         = "../data/test/subset.bam";
    const char *metadata        = "../data/test/metadata.tsv";
    const char *variants_file   = "../data/test/variants.txt";
    const char *lineages        = "../data/test/lineages.yaml";
    const char *output          = "../data/test/output.tsv";

    char **variants = NULL;
    uint number = 0;
    load_variants(variants_file, &variants, &number);

    // create_map_id2pangolin(meta)

    // create_lineage_map

    int ***table = init_table();

    // open(bam)
    samFile *fp = sam_open(bamfile, "r");
    sam_hdr_t *h = sam_hdr_read(fp);
    bam1_t *b = bam_init1();
    int res = sam_read1(fp, h, b);

    // while not the end:
    while (res >= 0) {
        record record = read_record(fp, h, b);
        record.variant = get_variant(record, id2pangolin, lineage_map);
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
