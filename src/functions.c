#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>
#include <htslib/sam.h>
#include "../hashing/src/hashing.h"
#include "../src/ndarray.h"
// #include "../src/ndarray.c"

typedef struct {
    char *key;
    char *value;
} pair_t;

void pair_print(pair_t *pair) {
    if (pair == NULL) printf("NULL\n");
    else printf("%-10s\t%-10s\n", pair->key, pair->value);
}

pair_t *pair_create(char *key, char *value) {
    pair_t *pair = malloc(sizeof (*pair));
    // malloc needs to be big enough for all possible pangolin IDs
    pair->key = malloc(sizeof (*key) * (strlen(key) + 10));
    strcpy(pair->key, key);
    pair->value = malloc(sizeof (*value) * (strlen(value) + 10));
    strcpy(pair->value, value);
    return pair;
}

void pair_free(pair_t *pair) {
    free(pair->key);
    free(pair->value);
    free(pair);
    pair = NULL;
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
    record->seq = malloc(sizeof (char) * (length+1));
    for (uint i=0; i<length; i++) {record->seq[i] = '.';}
    record->seq[length] = '\0';
    record->variant = NULL;
    return record;
}

record_t *record_full_create(char *id, char *seq, char *variant) {
    record_t *record = malloc(sizeof (*record));
    record->id = malloc(sizeof (*id) * (strlen(id) + 1));
    strcpy(record->id, id);
    record->seq = malloc(sizeof (*seq) * (strlen(seq) + 1));
    strcpy(record->seq, seq);
    record->variant = malloc( sizeof (*variant) * (strlen(variant) + 1));
    strcpy(record->variant, variant);
    return record;
}

void record_print(record_t *record) {
    printf("%-32.32s\t%s\t", record->id, record->seq);
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
    if (stream == NULL) {
        printf("Unable to open file %s.\n", filename);
        exit(EXIT_FAILURE);
    }
    (*variants_ptr) = (char **) malloc(sizeof (**variants_ptr) * get_n_lines(filename));

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

void add_variant(char ***variants_ptr, uint *number_ptr, char *variant) {
    (*variants_ptr)[*number_ptr] = malloc(strlen(variant) + 1);
    strcpy((*variants_ptr)[*number_ptr], variant);
    (*number_ptr)++;
}

void dealloc_variants(char ***variants_ptr, const uint *number_ptr) {
    for (uint i=0; i<(*number_ptr); i++) {
        free((*variants_ptr)[i]);
    }
    free((*variants_ptr));
}

char *tokenize(char *str, const char *delim, char **rest_ptr) {
    char *end;
    if (str == NULL) str = *rest_ptr;
    if (*str == '\0') {
        *rest_ptr = str;
        return NULL;
    }
    end = str + strcspn(str, delim);
    if (*end == '\0') {
        *rest_ptr = end;
        return str;
    }
    *end = '\0';
    *rest_ptr = end + 1;
    return str;
}

char *read_tsv_rec(const char *line, uint pos) {
    char *original, *token, *rest;

    original = strdup(line);
    rest = original;

    // for (uint i = 0; (token = strtok_r(rest, "\t", &rest)); i++) {
    for (uint i = 0; (token = tokenize(rest, "\t", &rest)); i++) {
        if (i == pos) {
            strip(token);
            break;
        }
    }

    if (token == NULL) {
        free(original);
        return NULL;
    } else {
        char *result = malloc(strlen(token) + 1);
        strcpy(result, token);
        free(original);
        return result;
    }
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
        item1->value = read_tsv_rec(line, 11);
        map_insert(&id2pangolin, item1);
    }
    free(line);
    fclose(stream);
    return id2pangolin;
}

void yaml_parser_parse_checked(yaml_parser_t *parser, yaml_event_t *event) {
    if (!yaml_parser_parse(parser, event)) {
        printf("Parser error %d\n", parser->error);
        exit(EXIT_FAILURE);
    }
}

char *get_name(yaml_parser_t *parser, yaml_event_t *event) {
    yaml_event_delete(event);
    yaml_parser_parse_checked(parser, event);
    char *tmp = (char *)event->data.scalar.value;
    char *result = malloc(sizeof (*result) * (strlen(tmp) + 1));
    strcpy(result, tmp);
    yaml_event_delete(event);
    return result;
}

void insert_children(yaml_parser_t *parser, yaml_event_t *event, map_t *pangolin2parent, char *name) {
    char *child;
    yaml_event_delete(event);
    yaml_parser_parse_checked(parser, event);
    yaml_event_delete(event);
    yaml_parser_parse_checked(parser, event);
    while (event->type != YAML_SEQUENCE_END_EVENT) {
        child = (char *)event->data.scalar.value;
        pair_t *pair = pair_create(child, name);

        pair_t *found = map_search(*pangolin2parent, pair);
        if (found != NULL) {
            map_delete(*pangolin2parent, found);
            pair_free(found);
        }

        map_insert(pangolin2parent, pair);
        yaml_event_delete(event);
        yaml_parser_parse_checked(parser, event);
    }
}

void insert_parent(yaml_parser_t *parser, yaml_event_t *event, map_t *pangolin2parent, char *name) {
    char *parent;
    yaml_event_delete(event);
    yaml_parser_parse_checked(parser, event);
    parent = (char *)event->data.scalar.value;
    pair_t *pair = pair_create(name, parent);

    pair_t *found = map_search(*pangolin2parent, pair);
    if (found != NULL) {
        map_delete(*pangolin2parent, found);
        pair_free(found);
    }

    map_insert(pangolin2parent, pair);
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
    yaml_event_delete(event);
    free(name);
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
    if(stream == NULL) fputs("Failed to open file!\n", stderr);

    yaml_parser_t parser;
    if(!yaml_parser_initialize(&parser)) fputs("Failed to initialize parser!\n", stderr);
    yaml_parser_set_input_file(&parser, stream);

    yaml_event_t event;
    yaml_parser_parse_checked(&parser, &event);
    while (event.type != YAML_STREAM_END_EVENT) {
        if (event.type == YAML_MAPPING_START_EVENT) {
            process_mapping(&parser, &event, &pangolin2parent);
        }
        yaml_event_delete(&event);
        yaml_parser_parse_checked(&parser, &event);
    }
    yaml_event_delete(&event);

    yaml_parser_delete(&parser);
    fclose(stream);

    return pangolin2parent;
}

pair_t *get_root(pair_t *pair, map_t pangolin2parent) {
    pair_t *pair_copy = pair_create(pair->key, pair->value);
    pair_t *result = NULL;
    do {
        strcpy(pair_copy->key, pair_copy->value);
        result = map_search(pangolin2parent, pair_copy);
        if (result == NULL) {
            pair_free(pair_copy);
            return NULL;
        }
        strcpy(pair_copy->value, result->value);
    } while (strcmp(pair_copy->key, pair_copy->value) != 0);
    pair_free(pair_copy);
    return result;
}

void add_root(map_t pangolin2parent, char *variant) {
    uint idx = 0;
    void *item = NULL;
    map_iterate(pangolin2parent, &idx, &item);

    pair_t *tmp = item;

    pair_t *root = get_root(tmp, pangolin2parent);
    // TODO: what if root will be NULL?
    strcpy(root->value, variant);

    pair_t *new_root = pair_create(variant, variant);
    map_insert(&pangolin2parent, new_root);
}

void isolate_subclades(map_t pangolin2parent, char **variants, uint num_variants) {
    pair_t *tmp;
    for (uint i=0; i<num_variants; i++) {
        pair_t *pair = pair_create(variants[i], variants[i]);

        tmp = map_search(pangolin2parent, pair);
        if (tmp != NULL) {
            map_delete(pangolin2parent, tmp);
            pair_free(tmp);
        }
        map_insert(&pangolin2parent, pair);
    }
}

char *get_variant(record_t *record, map_t id2pangolin, map_t pangolin2parent) {
    pair_t *pair;
    pair_t const *result;   // type declarations are read right-to-left -> pointer to constant pair_t

    pair = pair_create(record->id, "................");
    result = map_search(id2pangolin, pair);
    strcpy(pair->value, result->value);

    result = get_root(pair, pangolin2parent);
    if (result == NULL) {
        pair_free(pair);
        return NULL;
    }

    char *res = malloc(strlen(result->key) + 1);
    strcpy(res, result->key);
    pair_free(pair);
    return res;
}

void free_map_content(map_t *map) {
    uint idx = 0;
    pair_t *pair = NULL;
    map_iterate(*map, &idx, (void **)&pair);
    while (pair != NULL) {
        pair_free(pair);
        idx++;
        map_iterate(*map, &idx, (void **)&pair);
    }
}

//uint ***init_3d_array(uint x, uint y, uint z) {
//    uint ***res;
//    res = malloc(sizeof (**res) * x);
//    for (uint i=0; i<x; i++) {
//        res[i] = malloc(sizeof (*res) * y);
//        for (uint j=0; j<y; j++) {
//            res[i][j] = malloc(sizeof (res) * z);
//            for (uint k=0; k<z; k++) {
//                res[i][j][k] = 0;
//            }
//        }
//    }
//    return res;
//}

void array_3d_print(ndarray_t *array_3d, uint x, uint y, uint z, char **variants, char *alphabet, FILE *out) {
    // variant, pos, letter, count
    // TODO: get x, y, z from ndarray_t structure
    uint *c;
    for (uint j=0; j<y; j++) {
        for (uint i=0; i<x; i++) {
            for (uint k=0; k<z; k++) {
                c = ndarray_at(array_3d, i, j, k);
                fprintf(out, "%s\t%d\t%c\t%d\n", variants[j], i, alphabet[k], *c);
            }
        }
    }
}

//void array_3d_free(uint ***array_3d, uint x, uint y, uint z) {
//    for (uint i=0; i<x; i++) {
//        for (uint j=0; j<y; j++) {
//            free(array_3d[i][j]);
//        }
//        free(array_3d[i]);
//    }
//    free(array_3d);
//}

uint get_ref_size(sam_hdr_t *bam_header) {
    if (sam_hdr_nref(bam_header) != 1) {
        printf("BAM file is mapped to more than 1 reference. (%d)\n", bam_header->n_targets);
        exit(EXIT_FAILURE);
    }
    return bam_header->target_len[0];
}

void record_destroy(record_t *record) {
    free(record->variant);
    free(record->seq);
    free(record->id);
    free(record);
    record = NULL;
}

char *get_aln_seq(bam1_t *aln) {
    uint n = aln->core.l_qseq;
    char *result = malloc(n);
    for (uint i=0; i < n; i++)
        result[i] = seq_nt16_str[bam_seqi(bam_get_seq(aln), i)];
    return result;
}

void record_add_aln(record_t *record, bam1_t *aln) {
    char *ref_seq = record->seq;
    uint  ref_pos = aln->core.pos;
    char *aln_seq = get_aln_seq(aln);
    uint  aln_pos = 0;

    uint n_cigars = aln->core.n_cigar;
    uint32_t *cigar = bam_get_cigar(aln);

    for (uint i = 0; i < n_cigars; i++) {
        char op = bam_cigar_opchr(cigar[i]);
        uint32_t oplen = bam_cigar_oplen(cigar[i]);

        switch (op) // MIDNSHP=XB
        {
            case 'M':
                strncpy(&(ref_seq[ref_pos]), &aln_seq[aln_pos], oplen);
                ref_pos += oplen;
                aln_pos += oplen;
                break;
            case 'I':
                aln_pos += oplen;
                break;
            case 'D':
                for (uint j=0; j < oplen; j++)
                    ref_seq[ref_pos + j] = '-';
                ref_pos += oplen;
                break;
            case 'S':
                aln_pos += oplen;
                break;
            case 'H':
                // intentionally do nothing
                break;
            default:
                printf("Operator %c not implemented.", op);
        }
    }
    free(aln_seq);
    aln_seq = NULL;
}

record_t *record_read(samFile *bam_stream, sam_hdr_t *bam_header, bam1_t *bam_record, int *ret) {
    char *qname;
    qname = bam_get_qname(bam_record);
    uint ref_size = get_ref_size(bam_header);
    record_t *result = record_create(qname, ref_size);
    do {
        record_add_aln(result, bam_record);
        (*ret) = sam_read1(bam_stream, bam_header, bam_record);
        qname = bam_get_qname(bam_record);
    } while ((*ret) > 0 && strcmp(qname, result->id) == 0);
    return result;
}

void add_counts(ndarray_t *table, record_t *record, char **variants, const char *alphabet, uint num_variants, uint alphabet_size, uint ref_size) {
    uint j = 0;
    while (j < num_variants && strcmp(record->variant, variants[j]) != 0)
        j++;
    if (j == num_variants) return;

    uint *c;
    for (uint i=0; i<ref_size; i++) {
        for (uint k=0; k<alphabet_size; k++) {
            if (alphabet[k] == record->seq[i]) {
                c = ndarray_at(table, i, j, k);
                (*c)++;
            }
        }
    }
}
