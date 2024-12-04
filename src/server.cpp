#include <winsock2.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
// #include <sqlite3.h>
#include "message.h"

#define BUF_SIZE 100
#define CONTENT_SIZE 67
#define PORT 50000

namespace transmit{
    class Server{
    private:
        Message need_to_send;
        std::unordered_map<std::string,SOCKET> clt_map;
        WSADATA WSAdata;
        SOCKET ser_sock;
        sockaddr_in ser_addr;
    public:
        Server(){
            if(WSAStartup(MAKEWORD(2,2),&WSAdata)!=0){
                std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
                return;
            }
            ser_sock = socket(PF_INET,SOCK_STREAM,0);
            if(ser_sock==INVALID_SOCKET){
                std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
                WSACleanup();
                return;
            }            
        }
        ~Server(){
            closesocket(ser_sock);
            WSACleanup();            
        }

        void InitAddr(u_short port = PORT) {
            ser_addr.sin_family = AF_INET;
            ser_addr.sin_port = htons(PORT);
            ser_addr.sin_addr.s_addr = INADDR_ANY;
        }

        void listenON(){
            int bind_check = bind(ser_sock,(sockaddr*)&ser_addr,sizeof(ser_addr));
            if(bind_check == SOCKET_ERROR){
                std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
                closesocket(ser_sock);
                WSACleanup();
                return;

            }
            //listen list 只能有两个
            int listen_check = listen(ser_sock,2);
            if(listen_check == SOCKET_ERROR){
                std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
                closesocket(ser_sock);
                WSACleanup();
                return;
            }
            std::cout <<"^_^ 服务器监听开始 ^_^"<< std::endl;            
        }
        void monitorON(){
            int max_fd = ser_sock;
            fd_set temp_fds, sock_fds;
            FD_ZERO(&sock_fds);
            FD_SET(ser_sock, &sock_fds);
            
            timeval timeout;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            while (true) {
                temp_fds = sock_fds;
                std::vector<std::string> trash;  // 存储需要关闭连接的客户端

                int select_check = select(max_fd + 1, &temp_fds, nullptr, nullptr, &timeout);
                if (select_check == SOCKET_ERROR) {
                    std::cerr << "select failed" << std::endl;
                    break;
                }

                if (select_check == 0) {
                    std::cout << "\033[1K\r-_- 服务器闲置 -_-";
                    continue;
                }

                // 处理新的连接
                if (FD_ISSET(ser_sock, &temp_fds)) {
                    sockaddr_in clt_addr;
                    int clt_addr_sz = sizeof(clt_addr);
                    SOCKET clt_sock = accept(ser_sock, (sockaddr*)&clt_addr, &clt_addr_sz);
                    if (clt_sock == INVALID_SOCKET) {
                        std::cerr << "Accept failed." << std::endl;
                        continue;
                    }

                    std::cout <<"\n"<<"^_^ 建立新连接成功 ^_^" << std::endl;
                    FD_SET(clt_sock, &sock_fds);
                    max_fd = std::max(clt_sock, ser_sock);
                    recvConfirm(clt_sock);
                }

                // 处理已连接的客户端
                for (auto& i : clt_map) {
                    if (FD_ISSET(i.second, &temp_fds)) {
                        if (recvMessage(i.second) == -1) {
                            trash.push_back(i.first);
                            FD_CLR(i.second, &sock_fds);
                            closesocket(i.second);
                            continue;
                        }

                        int send_check = sendMessage();
                        if (send_check == -1) {
                            trash.push_back(i.first);
                            FD_CLR(i.second, &sock_fds);
                            closesocket(i.second);
                        } else if (send_check == -2) {
                            trash.push_back(i.first);
                            FD_CLR(i.second, &sock_fds);
                            closesocket(i.second);
                        }
                    }
                }

                // 清理掉被关闭的客户端
                for (const std::string& i : trash) {
                    clt_map.erase(i);
                }
            }        
        
        }
        int recvMessage(SOCKET& sock){
            char data[BUF_SIZE];
            if(recv(sock,data,BUF_SIZE,0) <= 0){
                std::cerr << "*_* 接收错误-错误代码:" <<WSAGetLastError()<<" *_*"<< std::endl;
                return -1;
            }
            Message message;
            message.deserialize(data);
            std::cout<<message.getFrom()<<" "<<message.getTo()<<" :"<<message.getContent()<<std::endl;
            need_to_send = message;
            return 1;
        }

        int sendMessage(){
            char data[BUF_SIZE];
            need_to_send.serialize(data);
            for(int i =-2;i<0;i++){
                if(send(clt_map[need_to_send.getFrom()],data,BUF_SIZE,0)<=0){
                    std::cerr << "*_* 发送错误-错误代码:" <<WSAGetLastError()<<" *_*"<< std::endl;
                    return i;
                }
                need_to_send.swapFromTo();
            }
            return 1;
        }

        void recvConfirm(SOCKET clt_sock){
            char data[BUF_SIZE];
            if(recv(clt_sock,data,BUF_SIZE,0)<=0){
                std::cerr << "*_* 接收错误-错误代码:" <<WSAGetLastError()<<" *_*"<< std::endl;
            }

            Message check;
            check.deserialize(data);
            clt_map.emplace(check.getFrom(),clt_sock);
        }


    };
}

int main(){
    transmit::Server chat_app;
    chat_app.InitAddr();
    chat_app.listenON();
    chat_app.monitorON();
    return 0;
    
}