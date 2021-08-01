#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>
#include "../hashing/src/hashing.h"

typedef struct {
    char *key;
    char *value;
} pair_t;

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

map_t get_pangolin2parent(const char *filename) {
    map_t pangolin2parent = map_create(1024, hash, cmp);

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

    /* CODE HERE */
    do {
        if (!yaml_parser_parse(&parser, &event)) {
            printf("Parser error %d\n", parser.error);
            exit(EXIT_FAILURE);
        }

        switch(event.type)
        {
            case YAML_NO_EVENT: puts("No event!"); break;
                /* Stream start/end */
            case YAML_STREAM_START_EVENT: puts("STREAM START"); break;
            case YAML_STREAM_END_EVENT:   puts("STREAM END");   break;
                /* Block delimeters */
            case YAML_DOCUMENT_START_EVENT: puts("<b>Start Document</b>"); break;
            case YAML_DOCUMENT_END_EVENT:   puts("<b>End Document</b>");   break;
            case YAML_SEQUENCE_START_EVENT: puts("<b>Start Sequence</b>"); break;
            case YAML_SEQUENCE_END_EVENT:   puts("<b>End Sequence</b>");   break;
            case YAML_MAPPING_START_EVENT:  puts("<b>Start Mapping</b>");  break;
            case YAML_MAPPING_END_EVENT:    puts("<b>End Mapping</b>");    break;
                /* Data */
            case YAML_ALIAS_EVENT:  printf("Got alias (anchor %s)\n", event.data.alias.anchor); break;
            case YAML_SCALAR_EVENT: printf("Got scalar (value %s)\n", event.data.scalar.value); break;
        }
        if(event.type != YAML_STREAM_END_EVENT)
            yaml_event_delete(&event);
    } while(event.type != YAML_STREAM_END_EVENT);
    yaml_event_delete(&event);
    /* END new code */


    /* Cleanup */
    yaml_parser_delete(&parser);
    fclose(stream);

    return pangolin2parent;
}