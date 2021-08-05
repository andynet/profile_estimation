#include "../src/functions.c"

int main() {
    char *token;
    char *line = "A\t\tC\n";
    token = read_tsv_rec(line, 0);
    printf("%s\n", token);
    token = read_tsv_rec(line, 1);
    printf("%s\n", token);
    token = read_tsv_rec(line, 2);
    printf("%s\n", token);
}

