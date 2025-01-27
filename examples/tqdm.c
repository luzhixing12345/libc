#include <clib/clib.h>
#include <unistd.h>  // For usleep

int main() {
    int iter;
    tqdm progress;

    iter = 1000;
    init_tqdm(&progress, iter, 30);
    for (int i = 0; i < iter; i++) {
        update_tqdm(&progress, i);
        usleep(100000);
    }
    return (0);
}