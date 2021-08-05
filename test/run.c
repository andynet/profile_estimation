#include "../src/functions.c"

int main() {
    char **variants = NULL;
    uint number = 0;

    load_variants("../data/variants.txt", &variants, &number);
    add_variant(&variants, &number, "other");
}

