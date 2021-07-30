#include <stdlib.h>
#include "../src/functions.c"

int main() {
    const char *variants_file = "../data/variants.txt";
    char **variants = NULL;
    uint number = 0;

    load_variants(variants_file, &variants, &number);
    dealloc_variants(&variants, &number);
}
