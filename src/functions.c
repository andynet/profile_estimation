#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

