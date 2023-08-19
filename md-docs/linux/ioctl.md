
# ioctl

得到终端的行/列

```c
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main() {

    struct winsize size;

    if (!isatty(1)) {
        printf("1 is not a tty\n");
        exit(1);
    }
    if (ioctl(1, TIOCGWINSZ, &size) < 0) {
        perror("ioctl");
        exit(1);
    }

    printf("%d rows, %d columns\n",size.ws_row,size.ws_col);

    return 0;
}
```