#include <criterion/criterion.h>
#include "../src/ndarray.h"
#include "../src/ndarray.c"

Test(core, empty_test) {}

Test(core, test_boundaries) {
    ndarray_t *ndarray = ndarray_create(3, 2, 3, 4);

    uint *p = ndarray_at(ndarray, 1, 2, 3);
    *p = 3;
    cr_assert(*p == ndarray->content[23]);
    p = ndarray_at(ndarray, 1, 2, 2);
    *p = 5;
    cr_assert(*p == ndarray->content[22]);
    p = ndarray_at(ndarray, 0, 0, 0);
    *p = 8;
    cr_assert(*p == ndarray->content[0]);

    ndarray_destroy(&ndarray);
}
