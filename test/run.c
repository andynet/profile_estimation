#include "../src/functions.c"

int main() {
    char *bamfile = "../data/subset.bam";
    samFile *bam_stream = sam_open(bamfile, "r");
    sam_hdr_t *bam_header = sam_hdr_read(bam_stream);
    bam1_t *bam_record = bam_init1();
    int res = sam_read1(bam_stream, bam_header, bam_record);
    if (res > 0) {
        record_t *record = read_record(bam_stream, bam_header, bam_record);
        // record_print(record);
        record = read_record(bam_stream, bam_header, bam_record);
        record = read_record(bam_stream, bam_header, bam_record);
//        record = read_record(bam_stream, bam_header, bam_record);
//        record = read_record(bam_stream, bam_header, bam_record);
//        record = read_record(bam_stream, bam_header, bam_record);
//        record = read_record(bam_stream, bam_header, bam_record);
//        record = read_record(bam_stream, bam_header, bam_record);
    }
    bam_destroy1(bam_record);
}
