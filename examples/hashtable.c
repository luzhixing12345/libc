#include "clib/hashtable.h"

#include <clib/clib.h>

HashTable table;

void base_type_hashtable() {
    /* Choose initial capacity of 10 */
    /* Specify the size of the keys and values you want to store once */
    ht_setup(&table, sizeof(int), sizeof(double), 10);
    printf("create hashtable<int, double>\n");

    ht_reserve(&table, 100);

    int x = 6;
    double y = 9;
    ht_insert(&table, &x, &y);

    if (ht_contains(&table, &x)) {
        y = *(double *)ht_lookup(&table, &x);
        printf("%d's value is: %f\n", x, y);
    }

    ht_erase(&table, &x);

    ht_clear(&table);
    ht_destroy(&table);
    printf("-------\n");
}

struct CustomKey {
    int key;
};

struct CustomType {
    int value;
};

void custom_type_hashtable() {
    ht_setup(&table, sizeof(struct CustomKey), sizeof(struct CustomType), 10);
    printf("create hashtable<struct CustomKey, struct CustomType>\n");

    struct CustomKey x;
    struct CustomType y;

    x.key = 6;
    y.value = 9;
    ht_insert(&table, &x, &y);

    if (ht_contains(&table, &x)) {
        y = *(struct CustomType *)ht_lookup(&table, &x);
        printf("%d's value is: %d\n", x.key, y.value);
    }

    ht_erase(&table, &x);
    ht_destroy(&table);
    printf("-------\n");
}

void iterate_hashtable() {
    ht_setup(&table, sizeof(int), sizeof(int), 10);
    printf("create hashtable<int, int>\n");

    for (int i = 0; i < 10; i++) {
        int x = i;
        int y = i;
        ht_insert(&table, &x, &y);
        printf("insert <%d, %d>\n", i, i);
    }

    int *k;
    int *v;

    ht_foreach(k, v, &table) {
        printf("iterate <%d, %d>\n", *k, *v);
    }

    ht_destroy(&table);
    printf("-------\n");
}

int main(int argc, const char *argv[]) {
    base_type_hashtable();
    custom_type_hashtable();
    iterate_hashtable();
    return 0;
}