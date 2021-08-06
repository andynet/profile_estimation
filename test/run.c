#include "../src/functions.c"

int main() {
    map_t pangolin2parent = get_pangolin2parent("../data/lineages_small.yml");
    pair_t *pair, *root;

    uint idx = 0;
    void *item = NULL;

    map_iterate(pangolin2parent, &idx, &item);
    idx++;
    while (item != NULL) {
        pair = (pair_t *)item;
        root = get_root(pair, pangolin2parent);
//        cr_assert(strcmp(root->key, root->value) == 0);
//        cr_assert(strcmp(root->key, "A") == 0);

        map_iterate(pangolin2parent, &idx, &item);
        idx++;
    }

    pair = pair_create("B", "B");
    root = get_root(pair, pangolin2parent);
//    cr_assert(root == NULL);
    pair_free(pair);

    free_map_content(&pangolin2parent);
    map_destroy(&pangolin2parent);
}

