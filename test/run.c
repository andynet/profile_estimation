#include "../src/functions.c"

int main() {
    map_t pangolin2parent = get_pangolin2parent("../data/lineages_small.yml");
    print_map(pangolin2parent);
    pair_t *pair;

    uint idx = 0;
    void *item = NULL;

    map_iterate(pangolin2parent, &idx, &item);
    idx++;
    while (item != NULL) {
        pair = (pair_t *)item;
        pair = get_root(pair, pangolin2parent);
        // cr_assert(strcmp(pair->key, pair->value) == 0);
        // cr_assert(strcmp(pair->key, "A") == 0);

        map_iterate(pangolin2parent, &idx, &item);
        idx++;
    }

    pair = pair_create("B", "B");
    pair = get_root(pair, pangolin2parent);
    // cr_assert(pair == NULL);
}

