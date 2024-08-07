#include <stdio.h>

int main() {
    char buffer[100];
    
    printf("Enter some text: ");
    
    // 使用阻塞 I/O 读取标准输入
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        printf("You entered: %s", buffer);
    } else {
        perror("Error reading from stdin");
    }
    
    return 0;
}

