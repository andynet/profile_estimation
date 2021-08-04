#include "../src/functions.c"

int main() {
    map_t pangolin2parent = map_create(32, hash, cmp);
    int pairs = 0;

    FILE *stream = fopen("../data/lineages.yml", "r");
    if(stream == NULL) fputs("Failed to open file!\n", stderr);

    yaml_parser_t parser;
    if(!yaml_parser_initialize(&parser)) fputs("Failed to initialize parser!\n", stderr);
    yaml_parser_set_input_file(&parser, stream);

    yaml_event_t event;
    yaml_parser_parse_checked(&parser, &event);
    while (event.type != YAML_STREAM_END_EVENT) {
        if (event.type == YAML_MAPPING_START_EVENT) {
            char *name = NULL;
            char *child = NULL;

            yaml_event_delete(&event);
            yaml_parser_parse_checked(&parser, &event);
            while (event.type != YAML_MAPPING_END_EVENT) {
                if (event.type == YAML_SCALAR_EVENT) {
                    char *value = (char *)event.data.scalar.value;
                    if (strcmp(value, "name") == 0) {
                        name = get_name(&parser, &event);
                    } else if (strcmp(value, "children") == 0) {
                        pair_t *pair = NULL;
                        yaml_event_delete(&event);
                        yaml_parser_parse_checked(&parser, &event);
                        yaml_event_delete(&event);
                        yaml_parser_parse_checked(&parser, &event);
                        while (event.type != YAML_SEQUENCE_END_EVENT) {
                            child = (char *)event.data.scalar.value;

                            pair = malloc(sizeof (*pair)); // there is a leak here
                            pair->key = malloc(sizeof (*child) * (strlen(child) + 1));
                            strcpy(pair->key, child);
                            pair->value = malloc(sizeof (*name) * (strlen(name) + 1));
                            strcpy(pair->value, name);

                            pairs++;

                            pair_t *found = map_search(pangolin2parent, pair);
                            if (found != NULL) {
                                map_delete(pangolin2parent, found);
                                // pair_free(found);
                                pairs--;
                            }

                            map_insert(&pangolin2parent, pair);
                            yaml_event_delete(&event);
                            yaml_parser_parse_checked(&parser, &event);
                            // print_map(pangolin2parent);
                            if (found != NULL) {
                                pair_free(found);
                            } else {
                                // free(pair);
                                // free(pair); // if found != NULL this was already deallocated
                            }
                        }
                    } else if (strcmp(value, "parent") == 0) {
                        insert_parent(&parser, &event, &pangolin2parent, name);
                    }
                }
                yaml_event_delete(&event);
                yaml_parser_parse_checked(&parser, &event);
            }
            yaml_event_delete(&event);

            free(name);
        }
        yaml_event_delete(&event);
        yaml_parser_parse_checked(&parser, &event);
    }
    yaml_event_delete(&event);

    yaml_parser_delete(&parser);
    fclose(stream);

    print_map(pangolin2parent);
    // end
    printf("%d\n", map_get_size(pangolin2parent));
    printf("#pairs\t%d\n", pairs);

    uint idx = 0;
    pair_t *pair = NULL;
    map_iterate(pangolin2parent, &idx, (void **)&pair);
    while (pair != NULL) {
        pair_free(pair);
        pairs--;
        idx++;
        map_iterate(pangolin2parent, &idx, (void **)&pair);
    }
    printf("#pairs\t%d\n", pairs);
    map_destroy(&pangolin2parent);
}