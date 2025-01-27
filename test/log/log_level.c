
#include <clib/clib.h>

#include <stdio.h>

int main(int argc, char **argv) {
    log_set_level(LOG_INFO);
    DEBUG("this is debug");
    INFO("this is info");
    WARNING("this is warning");
    ERR("this is error");
    return 0;
}
