#include "../src/functions.c"

int main() {
    samFile *bam_stream = sam_open("../data/subset.bam", "r");

    bam_hdr_t *bam_header = sam_hdr_read(bam_stream);
    bam1_t *aln = bam_init1();

    int ret, n=0;
    ret = sam_read1(bam_stream, bam_header, aln);
    while (ret > 0 && n < 10) {
        record_t *record = record_read(bam_stream, bam_header, aln, &ret);
        record_print(record);
        record_destroy(record);
        n++;
    }
    bam_destroy1(aln);
    sam_hdr_destroy(bam_header);
    sam_close(bam_stream);
}
