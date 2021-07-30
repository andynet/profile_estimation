#include <stdlib.h>
#include "../src/functions.c"

int main() {
    char *str1, *str2;
    uint h1, h2;

    str1 = "test_string1";
    str2 = "test2_string2";
    h1 = hash(str1);
    h2 = hash(str2);

    h2 = hash(str1);
}
