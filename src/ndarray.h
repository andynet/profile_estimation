#ifndef NDARRAY_H
#define NDARRAY_H

typedef unsigned int uint;
struct ndarray;
typedef struct ndarray ndarray_t;

ndarray_t *ndarray_create(uint ndims, ...);
void       ndarray_destroy(ndarray_t **ndarray);
uint      *ndarray_at(ndarray_t *array, ...);
uint       ndarray_ndims_get(ndarray_t *array);
uint      *ndarray_dim_sizes_get(ndarray_t *array);

#endif //NDARRAY_H
