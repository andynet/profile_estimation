#include "../src/functions.c"

int main() {
    map_t pangolin2parent = get_pangolin2parent("../data/lineages.yml");
    pair_t result;

    pair_t pair1 = { .key = "AA.1", .value = "B.1.177.15" };
    result = *(pair_t *) map_search(pangolin2parent, &pair1);

    pair_t pair2 = { .key = "A.2.5.2", .value = "A.2.5" };
    result = *(pair_t *) map_search(pangolin2parent, &pair2);

    free_map_content(&pangolin2parent);
    map_destroy(&pangolin2parent);
}
