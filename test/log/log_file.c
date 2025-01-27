
#include <clib/clib.h>

#include <stdio.h>

int main(int argc, char **argv) {
    log_init("log_file.txt");
    INFO("hello world");
    return 0;
}
