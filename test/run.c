#include "../src/functions.c"

int main() {
    record_t *record;
    map_t id2pangolin = get_id2pangolin("../data/subset.meta.tsv");
    map_t pangolin2parent = get_pangolin2parent("../data/lineages.yml");

    record = record_create("hCoV-19/England/CAMB-755E9/2020", 3);
    record->variant = get_variant(record, id2pangolin, pangolin2parent);
    record_print(record);
    record_destroy(record);

    record = record_create("hCoV-19/Iran/Qom255194/2020", 3);
    record->variant = get_variant(record, id2pangolin, pangolin2parent);
    record_print(record);
    record_destroy(record);

    record = record_create("hCoV-19/Australia/NSW153/2020", 3);
    record->variant = get_variant(record, id2pangolin, pangolin2parent);
    record_print(record);
    record_destroy(record);

    record = record_create("hCoV-19/Guangzhou/GZMU0034/2020", 3);
    record->variant = get_variant(record, id2pangolin, pangolin2parent);
    record_print(record);
    record_destroy(record);

    free_map_content(&pangolin2parent);
    map_destroy(&pangolin2parent);

    free_map_content(&id2pangolin);
    map_destroy(&id2pangolin);
}

