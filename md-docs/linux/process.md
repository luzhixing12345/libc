
# process

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    char *msg;
    int n;
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        n = 6;
        msg = "child process\n";
    } else {
        n = 3;
        msg = "parent process\n";
    }

    while (n--) {
        printf(msg);
        sleep(1);
    }

    return 0;
}
```

process

在正常的Linux(debian)系统中跑是这样, 孤儿进程的 ppid = 1

![20230428010317](https://raw.githubusercontent.com/learner-lu/picbed/master/20230428010317.png)

在 wsl2 中 initd 的 pid 是3126

![20230428011248](https://raw.githubusercontent.com/learner-lu/picbed/master/20230428011248.png)

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    char *msg;
    int n;
    pid_t pid;

    for (int i=0;i<10;i++) {
        pid = fork();
        if (pid == 0) {
            printf("child [%d]: pid = %d, parent pid = %d\n", i, getpid(), getppid());
            sleep(1);
            break;
        }
    }

    return 0;
}
```

乱序

```bash
child [0]: pid = 4770, parent pid = 4769
child [1]: pid = 4771, parent pid = 4769
child [3]: pid = 4773, parent pid = 4769
child [2]: pid = 4772, parent pid = 4769
child [6]: pid = 4776, parent pid = 4769
child [4]: pid = 4774, parent pid = 4769
child [5]: pid = 4775, parent pid = 4769
child [7]: pid = 4777, parent pid = 4769
child [8]: pid = 4778, parent pid = 4769
child [9]: pid = 4779, parent pid = 4769
```