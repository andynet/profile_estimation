#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>
#include <htslib/sam.h>
#include "../hashing/src/hashing.h"

typedef struct {
    char *key;
    char *value;
} pair_t;

void pair_print(pair_t *pair) {
    if (pair == NULL) printf("NULL\n");
    else printf("%s\t%s\n", pair->key, pair->value);
}

pair_t *pair_create(char *key, char *value) {
    pair_t *pair = malloc(sizeof (*pair));
    pair->key = malloc(sizeof (*key) * strlen(key));
    strcpy(pair->key, key);
    pair->value = malloc(sizeof (*value) * strlen(value));
    strcpy(pair->value, value);
    // pair_print(pair);
    return pair;
}

typedef struct {
    char *id;
    char *seq;
    char *variant;
} record_t;

record_t *record_create(char *id, uint length) {
    record_t *record = malloc(sizeof (*record));
    record->id = malloc(sizeof (*id) * (strlen(id)+1));
    strcpy(record->id, id);
    record->seq = malloc(sizeof (char) * length);
    for (uint i=0; i<length; i++) {record->seq[i] = '.';}
    record->variant = NULL;
}

void record_print(record_t *record) {
    printf("%s\t%s\t", record->id, record->seq);
    if (record->variant == NULL) printf("NULL\n");
    else printf("%s\n", record->variant);
}

uint get_n_lines(const char *filename) {
    FILE *fp = fopen(filename, "r");
    uint res = 0;
    char *lineptr = NULL;
    size_t n = 0;
    while (!feof(fp)) {
        getline(&lineptr, &n, fp);
        res++;
    }
    fclose(fp);
    fp = NULL;
    return res;
}

void strip(char *str) {
    for (uint i=0; i < strlen(str); i++) {
        if (str[i] == '\n') {
            str[i] = '\0';
            return;
        }
    }
}

void load_variants(const char *filename, char ***variants_ptr, uint *number_ptr) {
    FILE *stream = fopen(filename, "r");
    (*variants_ptr) = (char **) malloc(sizeof (**variants_ptr) * 8);

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stream)) != -1) {
        (*variants_ptr)[*number_ptr] = (char *) malloc(len);
        strip(line);
        strcpy((*variants_ptr)[*number_ptr], line);
        (*number_ptr)++;
    }
    free(line);
    line = NULL;

    fclose(stream);
}

void dealloc_variants(char ***variants_ptr, const uint *number_ptr) {
    for (uint i=0; i<(*number_ptr); i++) {
        free((*variants_ptr)[i]);
    }
    free((*variants_ptr));
}

char *read_tsv_rec(const char *line, uint pos) {
    char *original, *token, *rest;

    original = strdup(line);
    rest = original;

    for (uint i = 0; (token = strtok_r(rest, "\t", &rest)); i++) {
        if (i == pos) {
            strip(token);
            break;
        }
    }
    return token;
}

uint hash(const void *item1) {
    pair_t *i1 = (pair_t *) item1;
    char *key = i1->key;
    ulong h = 0;
    int c;
    while ((c = *(key++)))
        h = c + (h << 6) + (h << 16) - h;   // http://www.cse.yorku.ca/~oz/hash.html
    return h;
}

int cmp(const void *item1, const void *item2) {
    pair_t *i1 = (pair_t *) item1;
    pair_t *i2 = (pair_t *) item2;

    return strcmp(i1->key, i2->key);
}

map_t get_id2pangolin(const char *filename) {
    map_t id2pangolin = map_create(1024, hash, cmp);

    FILE *stream = fopen(filename, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stream)) != -1) {
        pair_t *item1 = malloc(sizeof (pair_t));
        item1->key = read_tsv_rec(line, 0);
        item1->value = read_tsv_rec(line, 10);
        map_insert(&id2pangolin, item1);
    }
    return id2pangolin;
}

void yaml_parser_parse_checked(yaml_parser_t *parser, yaml_event_t *event) {
    if (!yaml_parser_parse(parser, event)) {
        printf("Parser error %d\n", parser->error);
        exit(EXIT_FAILURE);
    }
}

char *get_name(yaml_parser_t *parser, yaml_event_t *event) {
    yaml_parser_parse_checked(parser, event);
    char *tmp = (char *)event->data.scalar.value;
    char *result = malloc(sizeof (*result) * strlen(tmp));
    strcpy(result, tmp);
    yaml_event_delete(event);
    return result;
}

void insert_children(yaml_parser_t *parser, yaml_event_t *event, map_t *pangolin2parent, char *name) {
    char *child;
    yaml_parser_parse_checked(parser, event);
    yaml_event_delete(event);
    yaml_parser_parse_checked(parser, event);
    while (event->type != YAML_SEQUENCE_END_EVENT) {
        child = (char *)event->data.scalar.value;
        pair_t *pair = pair_create(child, name);

        pair_t *found = map_search(*pangolin2parent, pair);
        if (found != NULL) map_delete(*pangolin2parent, found);

        map_insert(pangolin2parent, pair);
        yaml_event_delete(event);
        yaml_parser_parse_checked(parser, event);
    }
}

void insert_parent(yaml_parser_t *parser, yaml_event_t *event, map_t *pangolin2parent, char *name) {
    char *parent;
    yaml_parser_parse_checked(parser, event);
    parent = (char *)event->data.scalar.value;
    pair_t *pair = pair_create(name, parent);

    pair_t *found = map_search(*pangolin2parent, pair);
    if (found != NULL) map_delete(*pangolin2parent, found);

    map_insert(pangolin2parent, pair);
    yaml_event_delete(event);
}

void process_mapping(yaml_parser_t *parser, yaml_event_t *event, map_t *pangolin2parent) {
    char *name = NULL;
    yaml_parser_parse_checked(parser, event);

    while (event->type != YAML_MAPPING_END_EVENT) {
        if (event->type == YAML_SCALAR_EVENT) {
            char *value = (char *)event->data.scalar.value;
            if (strcmp(value, "name") == 0) {
                name = get_name(parser, event);
            } else if (strcmp(value, "children") == 0) {
                insert_children(parser, event, pangolin2parent, name);
            } else if (strcmp(value, "parent") == 0) {
                insert_parent(parser, event, pangolin2parent, name);
            }
        }
        yaml_event_delete(event);
        yaml_parser_parse_checked(parser, event);
    }
}

void print_map(map_t map) {
    uint idx = 0;
    void *item = NULL;
    map_iterate(map, &idx, &item);
    pair_print(item);
    while (item != NULL) {
        idx++;
        map_iterate(map, &idx, &item);
        if (item != NULL) pair_print(item);
    }
}

map_t get_pangolin2parent(const char *filename) {
    map_t pangolin2parent = map_create(32, hash, cmp);

    FILE *stream = fopen(filename, "r");
    if(stream == NULL)
        fputs("Failed to open file!\n", stderr);

    /* Initialize parser */
    yaml_parser_t parser;
    yaml_event_t event;
    if(!yaml_parser_initialize(&parser)) {
        fputs("Failed to initialize parser!\n", stderr);
    }

    /* Set input file */
    yaml_parser_set_input_file(&parser, stream);

    /* start processing */
    do {
        yaml_parser_parse_checked(&parser, &event);
        if (event.type == YAML_MAPPING_START_EVENT) {
            process_mapping(&parser, &event, &pangolin2parent);
        }
    } while (event.type != YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);

    /* Cleanup */
    yaml_parser_delete(&parser);
    fclose(stream);

    return pangolin2parent;
}

int ***init_3d_array(uint x, uint y, uint z) {
    int ***res;
    res = malloc(sizeof (**res) * x);
    for (uint i=0; i<x; i++) {
        res[i] = malloc(sizeof (*res) * y);
        for (uint j=0; j<y; j++) {
            res[i][j] = malloc(sizeof (res) * z);
            for (uint k=0; k<z; k++) {
                res[i][j][k] = 0;
            }
        }
    }
    return res;
}

uint get_ref_size(sam_hdr_t *bam_header) {
    if (sam_hdr_nref(bam_header) != 1) {
        printf("BAM file is mapped to more than 1 reference. (%d)\n", bam_header->n_targets);
        exit(EXIT_FAILURE);
    }
    return bam_header->target_len[0];
}

record_t *record_read(samFile *bam_stream, sam_hdr_t *bam_header, bam1_t *bam_record, int *ret) {
    char *qname, *cigar, *seq;
    qname = bam_get_qname(bam_record);
    uint ref_size = get_ref_size(bam_header);
    record_t *result = record_create(qname, ref_size);
    do {
        // add_seq(result, bam_record);
        // cigar = bam_get_cigar(bam_record);
        char base = bam_seqi(bam_get_seq(bam_record), 0);
        // printf("%s\n", qname);
        (*ret) = sam_read1(bam_stream, bam_header, bam_record);
        qname = bam_get_qname(bam_record);
    } while ((*ret) > 0 && strcmp(qname, result->id) == 0);
    return result;
}

void record_destroy(record_t *record) {
    free(record->variant);
    free(record->seq);
    free(record->id);
    free(record);
    record = NULL;
}