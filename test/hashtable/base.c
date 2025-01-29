#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <clib/clib.h>

void test_basic_operations() {
    HashTable table;
    ht_setup(&table, sizeof(int), sizeof(int), 10);

    int key = 42;
    int value = 99;
    ht_insert(&table, &key, &value);

    assert(ht_contains(&table, &key));
    int* found_value = (int*)ht_lookup(&table, &key);
    assert(found_value && *found_value == value);

    ht_erase(&table, &key);
    assert(!ht_contains(&table, &key));

    ht_destroy(&table);
    printf("test_basic_operations PASSED\n");
}

void test_iteration() {
    HashTable table;
    ht_setup(&table, sizeof(int), sizeof(int), 10);

    for (int i = 1; i <= 5; i++) {
        ht_insert(&table, &i, &i);
    }

    int* k;
    int* v;
    int count = 0;

    ht_foreach(k, v, &table) {
        printf("Iterate: key = %d, value = %d\n", *k, *v);
        count++;
    }

    assert(count == 5);
    ht_destroy(&table);
    printf("test_iteration PASSED\n");
}

void test_large_dataset() {
    HashTable table;
    ht_setup(&table, sizeof(int), sizeof(int), 10);

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        ht_insert(&table, &i, &i);
    }

    for (int i = 0; i < N; i++) {
        assert(ht_contains(&table, &i));
        int* val = (int*)ht_lookup(&table, &i);
        assert(val && *val == i);
    }

    ht_destroy(&table);
    printf("test_large_dataset PASSED\n");
}

void test_custom_struct() {
    typedef struct {
        int key;
    } CustomKey;

    typedef struct {
        int value;
    } CustomValue;

    HashTable table;
    ht_setup(&table, sizeof(CustomKey), sizeof(CustomValue), 10);

    CustomKey key = {6};
    CustomValue val = {9};

    ht_insert(&table, &key, &val);
    assert(ht_contains(&table, &key));

    CustomValue* found_val = (CustomValue*)ht_lookup(&table, &key);
    assert(found_val && found_val->value == val.value);

    ht_erase(&table, &key);
    assert(!ht_contains(&table, &key));

    ht_destroy(&table);
    printf("test_custom_struct PASSED\n");
}

int main() {
    test_basic_operations();
    test_iteration();
    test_large_dataset();
    test_custom_struct();
    printf("All tests PASSED!\n");
    return 0;
}
