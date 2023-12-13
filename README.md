# Firewall-Technical
This repository keeps some c files about firewall/socket/service technicals


How to test MyFireware.c:

1> 在终端中运行你的服务器程序。

2> 在另一个终端中运行 telnet 命令，连接到你的服务器：
telnet localhost 8080
这里假设你的服务器运行在本地（localhost），端口是 8080。你可以根据你的实际情况修改主机和端口。

3> 如果连接成功，你应该会在服务器终端看到类似以下的输出：
New connection, socket fd is [some_socket_fd], ip is: [client_ip], port is: [client_port]
其中 [some_socket_fd] 是新建立连接的客户端套接字描述符，[client_ip] 是客户端的 IP 地址，[client_port] 是客户端的端口号。

4> 在客户端输入一些数据，按 Enter 键发送。
Hello, server!

5> 你应该会在服务器终端看到类似以下的输出：
Received data from client [some_socket_fd]: Hello, server!
这表明服务器成功接收了来自客户端的数据。

这是一个简单的测试流程，你可以根据你的需求和实际情况修改。你也可以编写一个简单的客户端程序，通过代码进行测试，模拟多个客户端的连接和数据交互。



=========================

在 macOS 上，默认情况下可能没有安装 telnet。你可以尝试使用 nc 命令（Netcat）来模拟一个简单的客户端连接。以下是使用 nc 进行测试的步骤：

1> 在一个终端中运行你的服务器程序。

2> 在另一个终端中运行以下命令：
nc localhost 8080
这会尝试连接到你的服务器，端口是 8080。如果 nc 命令没有安装，你可以通过 Homebrew 或其他包管理器安装。
# 使用 Homebrew 安装 nc
brew install netcat

3> 如果连接成功，你应该会在服务器终端看到类似以下的输出：
New connection, socket fd is [some_socket_fd], ip is: [client_ip], port is: [client_port]
其中 [some_socket_fd] 是新建立连接的客户端套接字描述符，[client_ip] 是客户端的 IP 地址，[client_port] 是客户端的端口号。

4> 在客户端输入一些数据，按 Enter 键发送。
Hello, server!

5> 你应该会在服务器终端看到类似以下的输出：
Received data from client [some_socket_fd]: Hello, server!
这样你就能够测试服务器代码是否能够正确接收来自客户端的连接和数据了。
