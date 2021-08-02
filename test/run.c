#include <stdlib.h>
#include "../src/functions.c"

int main() {
    map_t pangolin2parent = get_pangolin2parent("../data/lineages_small.yml");
    print_map(pangolin2parent);
    pair_t result;

    pair_t pair1 = { .key = "A.2.5.1", .value = "A.2"};
    result = *(pair_t *) map_search(pangolin2parent, &pair1);
    pair_print(&pair1);

    pair_t pair2 = { .key = "A.2", .value = "A"};
    result = *(pair_t *) map_search(pangolin2parent, &pair2);
    pair_print(&pair2);

    pair_t pair3 = { .key = "A.2.2", .value = "A.2"};
    result = *(pair_t *) map_search(pangolin2parent, &pair3);
    pair_print(&pair3);
}
