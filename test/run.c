#include "../src/functions.c"
#include "../src/ndarray.c"

int main() {
    ndarray_t *ndarray = ndarray_create(3, 2, 3, 4);

    uint *p = ndarray_at(ndarray, 1, 2, 3);
    *p = 3;
    printf("%d\n", ndarray->content[23]);
    p = ndarray_at(ndarray, 1, 2, 2);
    *p = 5;
    printf("%d\n", ndarray->content[22]);
    p = ndarray_at(ndarray, 0, 0, 0);
    *p = 8;
    printf("%d\n", ndarray->content[0]);

    ndarray_destroy(&ndarray);
}

