#include "../src/functions.c"

int main() {
    uint num_variants = 0;
    char **variants = NULL;
    load_variants("../data/variants.txt", &variants, &num_variants);
    add_variant(&variants, &num_variants, "other");

    map_t pangolin2parent = get_pangolin2parent("../data/lineages.yml");
    add_root(pangolin2parent, "other");
    isolate_subclades(pangolin2parent, variants, num_variants);

    pair_t *pair, *root;
    pair = pair_create("B.1.258.20", "B.1.258.20");
    root = get_root(pair, pangolin2parent);
    pair_print(root);
    // cr_assert(strcmp(root->key, "B.1.258") == 0);

    strcpy(pair->value, "AY.2");
    root = get_root(pair, pangolin2parent);
    pair_print(root);
    // cr_assert(strcmp(root->key, "other") == 0);

    strcpy(pair->value, "Y.1");
    root = get_root(pair, pangolin2parent);
    pair_print(root);
    // cr_assert(strcmp(root->key, "B.1.177") == 0);

    strcpy(pair->value, "L.1");
    root = get_root(pair, pangolin2parent);
    pair_print(root);
    // cr_assert(strcmp(root->key, "other") == 0);

    strcpy(pair->value, "B.1.160");
    root = get_root(pair, pangolin2parent);
    pair_print(root);
    // cr_assert(strcmp(root->key, "B.1.160") == 0);

    pair_free(pair);
    free_map_content(&pangolin2parent);
    map_destroy(&pangolin2parent);
    dealloc_variants(&variants, &num_variants);
}

