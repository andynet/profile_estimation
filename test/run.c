#include <stdlib.h>
#include "../src/functions.c"

int main() {
    map_t pangolin2parent = get_pangolin2parent("../data/lineages.yml");
    print_map(pangolin2parent);
    pair_t result;

    pair_t pair1 = { .key = "B.1.1.13", .value = "A" };
    result = *(pair_t *) map_search(pangolin2parent, &pair1);
    pair_print(&result);

    pair_t pair2 = { .key = "A.2.5.2", .value = "A.2.5" };
    result = *(pair_t *) map_search(pangolin2parent, &pair2);
    pair_print(&result);
}
