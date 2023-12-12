//
//  main.c
//  MyFirewall
//
//  Created by Huafang Zhang on 2023-12-10.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>  // 添加此行，包含 <sys/select.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// 防火墙规则
typedef struct {
    char sourceIP[20];
    char destinationIP[20];
    int sourcePort;
    int destinationPort;
    int allow; // 1 for allow, 0 for deny
} FirewallRule;

// 检查规则是否匹配
int isRuleMatched(const FirewallRule* rule, const char* sourceIP, int sourcePort, const char* destinationIP, int destinationPort) {
    if (strcmp(rule->sourceIP, "*") != 0 && strcmp(rule->sourceIP, sourceIP) != 0) {
        return 0;
    }

    if (strcmp(rule->destinationIP, "*") != 0 && strcmp(rule->destinationIP, destinationIP) != 0) {
        return 0;
    }

    if (rule->sourcePort != -1 && rule->sourcePort != sourcePort) {
        return 0;
    }

    if (rule->destinationPort != -1 && rule->destinationPort != destinationPort) {
        return 0;
    }

    return 1;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    
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

    // 设置防火墙规则
    FirewallRule rules[] = {
        {"*", "*", 8080, -1, 1},
        {"192.168.1.2", "*", -1, 80, 0},
    };

    struct sockaddr_in clientAddress;  // 移动到更广泛的作用域

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        int maxSocket = serverSocket;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int clientSocket = clientSockets[i];

            if (clientSocket != -1) {
                FD_SET(clientSocket, &readfds);
                if (clientSocket > maxSocket) {
                    maxSocket = clientSocket;
                }
            }
        }

        int activity = select(maxSocket + 1, &readfds, NULL, NULL, NULL);
        if (activity == -1) {
            perror("Select error");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(serverSocket, &readfds)) {
            socklen_t clientAddressLength = sizeof(clientAddress);
            int newClientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

            if (newClientSocket == -1) {
                perror("Accept failed");
            } else {
                printf("New connection, socket fd is %d, ip is: %s, port is: %d\n",
                       newClientSocket, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

                for (int i = 0; i < MAX_CLIENTS; ++i) {
                    if (clientSockets[i] == -1) {
                        clientSockets[i] = newClientSocket;
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int clientSocket = clientSockets[i];

            if (FD_ISSET(clientSocket, &readfds)) {
                int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

                if (bytesRead <= 0) {
                    printf("Client disconnected, socket fd %d\n", clientSocket);
                    close(clientSocket);
                    clientSockets[i] = -1;
                } else {
                    buffer[bytesRead] = '\0';
                    printf("Received data from client %d: %s\n", clientSocket, buffer);

                    int allowConnection = 0;
                    for (size_t j = 0; j < sizeof(rules) / sizeof(FirewallRule); ++j) {
                        if (isRuleMatched(&rules[j], "*", inet_ntoa(clientAddress.sin_addr), -1, ntohs(clientAddress.sin_port))) {
                            allowConnection = rules[j].allow;
                            break;
                        }
                    }

                    if (allowConnection) {
                        send(clientSocket, buffer, strlen(buffer), 0);
                    } else {
                        printf("Connection blocked by firewall rule.\n");
                        close(clientSocket);
                        clientSockets[i] = -1;
                    }
                }
            }
        }
    }

    close(serverSocket);
    
//    printf("Hello, World!\n");
    return 0;
}
