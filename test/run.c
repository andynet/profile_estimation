#include "../src/functions.c"

int main() {
    samFile *bam_stream = sam_open("../data/subset.bam", "r");

    bam_hdr_t *bam_header = sam_hdr_read(bam_stream);
    bam1_t *aln = bam_init1();

    // kstring_t ks = KS_INITIALIZE;
    // bam_aux_get(); // to get tags
    if (sam_read1(bam_stream, bam_header, aln) > 0) {
        uint ref_size = get_ref_size(bam_header);
        char *qname = bam_get_qname(aln);
        printf("%ld\n", aln->core.pos);
        record_t *record = record_create(qname, ref_size);
        record_add_aln(record, aln);
    }
    bam_destroy1(aln);
    sam_hdr_destroy(bam_header);
    sam_close(bam_stream);
}

//sam_format1(bam_header, aln, &ks);
//printf("%s\n", ks_str(&ks));
//int op = bam_cigar_table[(unsigned char) ch];
// bam1_core_t *core = aln->core.n_cigar;
//core->pos;
// core->n_cigar;
//uint32_t n_cigars = aln->core.n_cigar;
//uint32_t *cigar = bam_get_cigar(aln);
//
// sam_parse_cigar(const char *in, char **end, &cigar, size_t *a_mem);
//uint32_t *buf = NULL;
//char *cig = "2M3X1I10M5D";
//char *end;
//size_t m = 0;
//ssize_t n = sam_parse_cigar(cig, &end, &buf, &m);
