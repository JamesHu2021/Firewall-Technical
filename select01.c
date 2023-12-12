//
//  
//  MyFirewall02
//
//  Created by Huafang Zhang on 2023-12-12.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main(int argc, const char * argv[]) {
    int serverSocket, clientSockets[MAX_CLIENTS];
    fd_set readfds;
    char buffer[BUFFER_SIZE];

    // 创建服务器套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址结构
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    // 绑定服务器套接字到指定端口
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // 监听连接
    if (listen(serverSocket, 5) == -1) {
        perror("Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Firewall is running. Waiting for connections...\n");

    // 初始化客户端套接字数组
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clientSockets[i] = -1;
    }

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        int maxSocket = serverSocket;

        // 添加现有客户端套接字到文件描述符集合
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int clientSocket = clientSockets[i];

            if (clientSocket != -1) {
                FD_SET(clientSocket, &readfds);
                if (clientSocket > maxSocket) {
                    maxSocket = clientSocket;
                }
            }
        }

        // 使用 select 监视文件描述符
        int activity = select(maxSocket + 1, &readfds, NULL, NULL, NULL);
        if (activity == -1) {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        // 处理新的连接请求
        if (FD_ISSET(serverSocket, &readfds)) {
            struct sockaddr_in clientAddress;
            socklen_t clientAddressLength = sizeof(clientAddress);
            int newClientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

            if (newClientSocket == -1) {
                perror("Accept failed");
            } else {
                printf("New connection, socket fd is %d, ip is: %s, port is: %d\n",
                       newClientSocket, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

                // 添加新的客户端套接字到数组
                for (int i = 0; i < MAX_CLIENTS; ++i) {
                    if (clientSockets[i] == -1) {
                        clientSockets[i] = newClientSocket;
                        break;
                    }
                }
            }
        }

        // 处理现有客户端的数据
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int clientSocket = clientSockets[i];

            if (FD_ISSET(clientSocket, &readfds)) {
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

                if (bytesRead <= 0) {
                    // 客户端断开连接，移除套接字
                    printf("Client disconnected, socket fd %d\n", clientSocket);
                    close(clientSocket);
                    clientSockets[i] = -1;
                } else {
                    // 处理从客户端接收到的数据（在这里可以实现防火墙规则）
                    buffer[bytesRead] = '\0';
                    printf("Received data from client %d: %s\n", clientSocket, buffer);

                    // 在这里添加防火墙规则的处理逻辑
                    // ...

                    // 回显数据到客户端
                    send(clientSocket, buffer, strlen(buffer), 0);
                }
            }
        }
    }

    // 关闭服务器套接字
    close(serverSocket);

    return 0;
}
