
# hashtable

```c
#include <clib/clib.h>

int main(int argc, const char* argv[]) {
	HashTable table;
	ht_setup(&table, sizeof(int), sizeof(double), 10);

	int x = 6;
	double y = 9;
	ht_insert(&table, &x, &y);

	if (ht_contains(&table, &x)) {
		y = *(double*)ht_lookup(&table, &x);
		printf("%d's value is: %f\n", x, y);
	}

	ht_erase(&table, &x);
    
	ht_destroy(&table);
}
```

## 参考

- [hashtable](https://github.com/goldsborough/hashtable)
- [hashmap](https://github.com/DavidLeeds/hashmap)