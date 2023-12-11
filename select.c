//
//  main.c
//  MyFirewall01
//
//  Created by Huafang Zhang on 2023-12-10.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    
    fd_set readfds;       // 用于存储要监视的文件描述符集合
    struct timeval timeout; // 用于设置超时时间

    // 清空并初始化文件描述符集合
    FD_ZERO(&readfds);

    // 添加标准输入文件描述符到集合中
    FD_SET(STDIN_FILENO, &readfds);

    while (1) {
        // 设置超时时间为 15 秒
        timeout.tv_sec = 15;
        timeout.tv_usec = 0;

        // 使用 select 监视文件描述符
        int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

        if (result == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        } else if (result == 0) {
            // 超时，标准输入未就绪
            printf("Timeout! No input from stdin.\n");
        } else {
            // 标准输入有输入
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                char buffer[256];
                fgets(buffer, sizeof(buffer), stdin);
                printf("Input from stdin: %s", buffer);
            }
        }

        // 清空文件描述符集合，以备下一次使用
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
    }

    
//    printf("Hello, World!\n");
    return 0;
}
