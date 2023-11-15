#include <stdlib.h>
#include <stdio.h>

int main() {
    
    int *p = (int *)malloc(sizeof (char) * 1024);
    printf("finished malloc\n");
    free(p);
    printf("finshed free\n");
}
