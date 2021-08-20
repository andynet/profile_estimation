#include <htslib/sam.h>
#include <unistd.h>
#include "functions.c"

enum Arguments{BAM=0, META, VARIANTS, LINEAGES, OUTPUT};
char **parse_args(int argc, char** argv);

int main(int argc, char** argv) {
    argc = 11;
    char *test_argv[] = {
        "profile_estimation",
        "-b", "../data/test1/subset.bam",
        "-m", "../data/test1/subset.meta.tsv",
        "-v", "../data/test1/variants.txt",
        "-l", "../data/test1/lineages.yml",
        "-o", "../data/test1/output.tsv"
    };
    argv = test_argv;

    char **args = parse_args(argc, argv);

    uint num_variants = 0;
    char **variants = NULL;
    load_variants(args[VARIANTS], &variants, &num_variants);
    add_variant(&variants, &num_variants, "other");

    #define ID 0
    #define PANGOLIN 11
    #define DATE 3
    map_t id2pangolin = get_tsv_pair_map(args[META], ID, PANGOLIN);
    map_t id2date = get_tsv_pair_map(args[META], ID, DATE);

    map_t pangolin2parent = get_pangolin2parent(args[LINEAGES]);
    add_root(pangolin2parent, "other");
    isolate_subclades(pangolin2parent, variants, num_variants);

    samFile *bam_stream = sam_open(args[BAM], "r");
    sam_hdr_t *bam_header = sam_hdr_read(bam_stream);
    uint ref_size = get_ref_size(bam_header);

    uint alphabet_size = 6;
    char alphabet[] = {'A', 'C', 'G', 'T', 'N', '-'};

    ndarray_t *table = ndarray_create(3, ref_size, num_variants, alphabet_size);

    int ret;
    uint i = 0;
    bam1_t *aln = bam_init1();
    ret = sam_read1(bam_stream, bam_header, aln);
    while (ret > 0) {
        record_t *record = record_read(bam_stream, bam_header, aln, &ret);
        record->variant = get_variant(record, id2pangolin, pangolin2parent);
        if (record->variant != NULL) {
            uint daydiff = get_daydiff(record->id, id2date, "2020-01-01");
            add_counts(table, record, variants, alphabet);
        }
        record_destroy(record);
        i++;
        if (i % 1000 == 0) printf("Processed %d records.\n", i);
    }

    bam_destroy1(aln);
    sam_hdr_destroy(bam_header);
    sam_close(bam_stream);

    FILE *out = fopen(args[OUTPUT], "w");
    array_3d_print(table, variants, alphabet, out);
    fclose(out);

    ndarray_destroy(&table);
    free_map_content(&id2pangolin);
    map_destroy(&id2pangolin);
    free_map_content(&pangolin2parent);
    map_destroy(&pangolin2parent);
    dealloc_variants(&variants, &num_variants);
    free(args);
}

void print_usage(char **argv) {
    fprintf(
            stderr,
            "Usage: %s "
            "-b <bam> "
            "-m <metadata.tsv> "
            "-v <variants_file> "
            "-l <lineages.yml> "
            "-o <output.tsv>\n",
            argv[0]
    );
    exit(EXIT_FAILURE);
}

char **parse_args(int argc, char **argv){
    char **args = malloc(sizeof (*args) * 5);
    int opt;
    while ((opt = getopt(argc, argv, "b:m:v:l:o:")) != -1) {
        switch (opt) {
            case 'b': args[BAM] = optarg; break;
            case 'm': args[META] = optarg; break;
            case 'v': args[VARIANTS] = optarg; break;
            case 'l': args[LINEAGES] = optarg; break;
            case 'o': args[OUTPUT] = optarg; break;
            default : print_usage(argv);
        }
    }
    return args;
}
