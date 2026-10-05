#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INITIAL_MAP_SIZE 128
#define LOAD_FACTOR_THRESHOLD 0.5

typedef enum {
    E_UNSET,
    E_ALIVE,
    E_DEAD
} EntryState;

typedef struct {
    EntryState state;
    unsigned int key;
    const char *value;
} Entry;

typedef struct {
    Entry *buckets;
    size_t size;
    size_t num_items;
} Map;

int hash_func(int key, size_t max) {
    return key % max;
}

void map_resize(Map *map, size_t new_size) {
    printf("[INFO] Resizing\n");
    Entry *new_buckets = calloc(new_size, sizeof(Entry));
    memset(new_buckets, 0, new_size * sizeof(Entry));

    for (size_t i = 0; i < map->size; i++) {
        Entry old_entry = map->buckets[i];
        if (old_entry.state == E_UNSET) {
            continue;
        }

        int hash = hash_func(old_entry.key, new_size);
        while (new_buckets[hash].state == E_ALIVE) {
            hash = (hash + 1) % new_size;
        }
        new_buckets[hash] = old_entry;
    }

    free(map->buckets);
    map->buckets = new_buckets;
    map->size = new_size;
}

void map_set(Map *map, unsigned int key, const char *value) {
    int index = hash_func(key, map->size);
    // probe until either entry is empty or key matches
    while (map->buckets[index].state == E_ALIVE && map->buckets[index].key != key) {
        index = (index + 1) % map->size;
    }

    // update value if entry already exists
    if (map->buckets[index].key == key && map->buckets[index].state == E_ALIVE) {
        map->buckets[index].value = value;
        return;
    }

    // resize if load factor threshold reached
    double load_factor = (double)map->num_items / map->size;
    if (load_factor > 0.5) {
        map_resize(map, map->size * 2);
    }

    // add entry
    map->buckets[index].state = E_ALIVE;
    map->buckets[index].key = key;
    map->buckets[index].value = value;
    map->num_items++;
}

void map_remove(Map *map, unsigned int key) {
    int index = hash_func(key, map->size);
    // collision checking
    while (map->buckets[index].state != E_UNSET && map->buckets[index].key != key) {
        index = (index + 1) % map->size;
    }

    if (map->buckets[index].state == E_ALIVE && map->buckets[index].key == key) {
        map->buckets[index].state = E_DEAD;
        map->buckets[index].key = 0;
        map->buckets[index].value = NULL;
        map->num_items--;
        return;
    }
}

const char * map_get(Map *map, unsigned int key) {
    int index = hash_func(key, map->size);
    // collision checking
    while (map->buckets[index].state != E_UNSET && map->buckets[index].key != key) {
        index = (index + 1) % map->size;
    }

    if (map->buckets[index].state == E_ALIVE && map->buckets[index].key == key) {
        return map->buckets[index].value;
    }

    return NULL;
}

Map * map_create() {
    Map *map = malloc(sizeof(Map));
    map->buckets = calloc(INITIAL_MAP_SIZE, sizeof(Entry));
    memset(map->buckets, 0, INITIAL_MAP_SIZE * sizeof(Entry));
    map->size = INITIAL_MAP_SIZE;
    map->num_items = 0;
    return map;
}

void map_free(Map *map) {
    free(map->buckets);
    free(map);
}

void map_print(Map *map) {
    printf("[ ");
    for (size_t i = 0; i < map->size; i++) {
        char state = 'N';
        switch (map->buckets[i].state) {
            case E_ALIVE:
                state = 'A';
                break;
            case E_DEAD:
                state = 'D';
                break;
            default:
                break;
        }
        printf("%c ", state);
    }
    printf("]\n");

    printf("[ ");
    for (size_t i = 0; i < map->size; i++) {
        printf("%d ", map->buckets[i].key);
    }
    printf("]\n");

    printf("[ ");
    for (size_t i = 0; i < map->size; i++) {
        printf("%s ", map->buckets[i].value);
    }
    printf("]\n");
}

int main(void) {
    Map *map = map_create();

    unsigned int key1 = 2856;
    unsigned int key2 = 73004;
    unsigned int key3 = 9;

    map_set(map, key1, "hi");
    map_set(map, key2, "lol");
    map_set(map, key3, "bye");

    map_set(map, 23094, "test1");
    map_set(map, 234778, "test2");
    map_set(map, 1234, "test3");
    map_set(map, 7934098, "test4");

    map_set(map, key1, "idk");

    map_remove(map, key3);
    map_remove(map, key3);

    const char *value1 = map_get(map, key1);
    printf("value1: %s\n", value1);

    const char *value2 = map_get(map, key2);
    printf("value2: %s\n", value2);

    map_remove(map, key2);
    value2 = map_get(map, key2);
    printf("value2: %s\n", value2);

    map_print(map);

    map_free(map);
    return 0;
}
