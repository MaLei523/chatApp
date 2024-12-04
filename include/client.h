#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <shared_mutex>
#include "message.h"

#include <GLFW/glfw3.h>

#define BUF_SIZE 100
#define CONTENT_SIZE 67
#define PORT 50000

namespace transmit {
    
    class Client {
    private:
        WSADATA wsaData;                // 用于 WinSock 初始化
        SOCKET clt_sock;             // 客户端套接字
        sockaddr_in ser_addr;         // 服务器地址
        int ser_addr_size = sizeof(ser_addr);  // 地址结构大小

        std::shared_ptr<std::vector<Message>> messages_arr_ptr; // 消息存储指针
        
    public:
        // 构造函数，初始化 WinSock
        Client() {
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
                return;
            }

            // 创建客户端套接字
            clt_sock = socket(PF_INET, SOCK_STREAM, 0);
            if (clt_sock == INVALID_SOCKET) {
                std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
                WSACleanup();
                return;
            }
        }

        // 析构函数，清理 WinSock
        ~Client() {
            closesocket(clt_sock);
            WSACleanup();
        }

        // 初始化服务器地址
        void InitAddr(u_short port = PORT, const std::string& ip = "127.0.0.1") {
            ser_addr.sin_family = AF_INET;
            ser_addr.sin_port = htons(port);
            ser_addr.sin_addr.s_addr = inet_addr(ip.c_str());
        }
        void connectToServer(){
            if (connect(clt_sock, (sockaddr*)&ser_addr, ser_addr_size) == SOCKET_ERROR) {
                std::cerr << "connect failed with error: " << WSAGetLastError() << std::endl;
                closesocket(clt_sock);
                WSACleanup();
                return;
            }else{
                std::cout<<"***connect success***"<<std::endl;
            }
            
        }
        // 监听服务器消息并接收数据
        void monitor(std::shared_ptr<std::vector<Message>> message_ptr,std::atomic<bool>& monitor_active) {
            messages_arr_ptr = message_ptr;
            // 持续监听数据
            while (!monitor_active.load())
            {
                std::this_thread::yield();
            }
            std::cout<<"***monitor on***"<<std::endl;
            while (monitor_active.load()) {

                char data[BUF_SIZE];
                int bytesReceived = recv(clt_sock, data, BUF_SIZE, 0);

                if (bytesReceived <= 0) {
                    std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
                    closesocket(clt_sock);
                    return;
                }

                // 反序列化接收到的消息
                Message recvMessage;
                recvMessage.deserialize(data);
                messages_arr_ptr->emplace_back(recvMessage);
            }
            
        }

        // 发送消息
        void sendMessage(const Message& message) {
            std::cout<<"***Message Init***"<<std::endl;
            char data[BUF_SIZE];
            message.serialize(data);

            if (send(clt_sock, data, BUF_SIZE, 0) <= 0) {
                std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
                closesocket(clt_sock);
            }
        }
    };
}

#endif
