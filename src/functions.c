#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../hashing/src/hashing.h"

typedef struct {
    char *key;
    char *value;
} item;

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
    item *i1 = (item *) item1;
    char *key = i1->key;
    ulong h = 0;
    int c;
    while ((c = *(key++)))
        h = c + (h << 6) + (h << 16) - h;   // http://www.cse.yorku.ca/~oz/hash.html
    return h;
}

int cmp(const void *item1, const void *item2) {
    item *i1 = (item *) item1;
    item *i2 = (item *) item2;

    return strcmp(i1->key, i2->key);
}

map_t get_id2pangolin(const char *filename) {
    map_t id2pangolin = map_create(1024, hash, cmp);

    FILE *stream = fopen(filename, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stream)) != -1) {
        item *item1 = malloc(sizeof (item));
        item1->key = read_tsv_rec(line, 0);
        item1->value = read_tsv_rec(line, 10);
        map_insert(&id2pangolin, item1);
    }
    return id2pangolin;
}