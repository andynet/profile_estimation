#include <stdarg.h>     /* for variadic functions */
#include <stdio.h>

struct ndarray {
    uint ndims;
    uint *dim_sizes;
    uint *content;
};
typedef struct ndarray ndarray_t;

// struct ndarray;
// typedef struct ndarray ndarray_t;

ndarray_t *ndarray_create(uint ndims, ...);
void       ndarray_destroy(ndarray_t **ndarray);
// void       ndarray_print(FILE *fp, ndarray_t *ndarray);

ndarray_t *ndarray_create(uint ndims, ...) {
    ndarray_t *array = malloc(sizeof *array);
    array->ndims = ndims;
    array->dim_sizes = malloc((sizeof *(array->dim_sizes)) * array->ndims);

    va_list args;
    uint content_size = 1;
    va_start(args, ndims);
    for (uint i = 0; i < ndims; i++) {
        array->dim_sizes[i] = va_arg(args, uint);
        content_size *= array->dim_sizes[i];
    }
    va_end(args);

    array->content = malloc((sizeof *(array->content)) * content_size);
    memset(array->content, 0, content_size);

    return array;
}

void ndarray_destroy(ndarray_t **ndarray) {
    free((**ndarray).content);
    free((**ndarray).dim_sizes);
    free(( *ndarray));
    (*ndarray) = NULL;
}

uint *ndarray_at(ndarray_t *array, ...) {
    va_list args;
    uint position = 0;
    va_start(args, array);
    for (uint i = 0; i < array->ndims; i++) {
        uint part_pos = 1;
        for (uint j = i+1; j < array->ndims; j++) part_pos *= array->dim_sizes[j];
        position += va_arg(args, uint) * part_pos;
    }
    // position += va_arg(args, uint) * array->dim_sizes[1] * array->dim_sizes[2];
    // position += va_arg(args, uint) * array->dim_sizes[2];
    // position += va_arg(args, uint);
    va_end(args);

    return &array->content[position];
}