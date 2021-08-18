#ifndef NDARRAY_H
#define NDARRAY_H

struct ndarray;
typedef struct ndarray ndarray_t;

ndarray_t *ndarray_create(uint ndims, ...);
void       ndarray_destroy(ndarray_t **ndarray);
uint      *ndarray_at(ndarray_t *array, ...);

#endif //NDARRAY_H
