#include <winsock2.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;

#define BUF_SIZE 100
#define CONTENT_SIZE 70
#define PORT 50000

class Message {
private:
    char from[16];    // 发送方，最大15字符 + '\0'
    char to[16];      // 接收方，最大15字符 + '\0'
    char content[68]; // 内容，最大67字符 + '\0'
public:
    // 设置发送方
    void setFrom(const string str_from) {
        if (str_from.size() > 15) {
            cerr << "Error: 'from' exceeds 15 characters." << endl;
            return;
        }
        strncpy(from, str_from.c_str(),15);
        from[15] = '\0'; // 确保以 '\0' 结尾
    }

    // 设置接收方
    void setTo(const string str_to) {
        if (str_to.size() > 15) {
            cerr << "Error: 'to' exceeds 15 characters." << endl;
            return;
        }
        strncpy(to, str_to.c_str(), 15);
        to[15] = '\0'; // 确保以 '\0' 结尾
    }

    // 设置内容
    void setContent(const string str_content) {
        if (str_content.size() > 67) {
            cerr << "Error: 'content' exceeds 67 characters." << endl;
            return;
        }
        strncpy(content, str_content.c_str(), sizeof(content) - 1);
        content[sizeof(content) - 1] = '\0'; // 确保以 '\0' 结尾
    }

    // 获取发送方
    string returnFrom(){
        return string(from);
    }

    // 获取接收方
    string returnTo(){
        return string(to);
    }

    // 获取内容
    string returnContent(){
        return string(content);
    }

    // 序列化
    void serialize(char* buffer) const {
        memcpy(buffer, this, sizeof(Message));
    }

    // 反序列化
    void deserialize(const char* buffer) {
        memcpy(this, buffer, sizeof(Message));
    }
    void swapFromTo(){
        swap(from,to);
    }
};

Message need_to_send;
unordered_map<string,SOCKET> clt_map;
int recvMessage(SOCKET& sock);
int sendMessage();
void recvConfirm(SOCKET clt_sock);

int main(){
    sockaddr_in ser_addr;
    memset(&ser_addr,0,sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(PORT);
    ser_addr.sin_addr.s_addr = INADDR_ANY;

    sockaddr_in clt_addr;
    int clt_addr_sz = sizeof(clt_addr);

    WSADATA WSAdata;
    if(WSAStartup(MAKEWORD(2,2),&WSAdata)!=0){
        cerr << "WSAStartup failed with error: " << WSAGetLastError() << endl;
        return 1;
    }
    SOCKET ser_sock = socket(PF_INET,SOCK_STREAM,0);
    if(ser_sock==INVALID_SOCKET){
        cerr << "socket failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;

    }

    int bind_check = bind(ser_sock,(sockaddr*)&ser_addr,sizeof(ser_addr));
    if(bind_check == SOCKET_ERROR){
        cerr << "bind failed with error: " << WSAGetLastError() << endl;
        closesocket(ser_sock);
        WSACleanup();
        return 1;

    }

    int listen_check = listen(ser_sock,2);
    if(listen_check == SOCKET_ERROR){
        cerr << "listen failed with error: " << WSAGetLastError() << endl;
        closesocket(ser_sock);
        WSACleanup();
        return 1;
    }
    cout << "Server listening on port " << PORT << "..." << endl;

    int max_fd = ser_sock;
    fd_set temp_fds,sock_fds;
    FD_ZERO(&sock_fds);
    FD_SET(ser_sock,&sock_fds);
    timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    while(1){
        temp_fds = sock_fds;
        vector<string> trash;
        int select_check = select(max_fd+1,&temp_fds,nullptr,nullptr,&timeout);
        if(select_check == SOCKET_ERROR){
            cerr<<"select failed"<<endl;
            break;
        }
        if(select_check == 0){
            cout<<"Server Idle No Connection"<<endl;
            continue;
        }
        if(FD_ISSET(ser_sock,&temp_fds)){
            SOCKET clt_sock = accept(ser_sock,(sockaddr*)&clt_addr,&clt_addr_sz);
            if(clt_sock == INVALID_SOCKET){
                cerr << "Accept failed." <<endl;
                continue;
            }
            cout << "New connections created" <<endl;

            FD_SET(clt_sock,&sock_fds);
            max_fd = max(clt_sock,ser_sock);
            recvConfirm(clt_sock);
        }
        for(pair<string,SOCKET> i : clt_map){
            if(FD_ISSET(i.second,&temp_fds)){
                if(recvMessage(i.second) == -1){
                    trash.emplace_back(i.first);
                    FD_CLR(i.second,&sock_fds);
                    closesocket(i.second);
                    continue;
                }
                int send_check = sendMessage();
                if(send_check==-1){
                    trash.emplace_back(need_to_send.returnTo());
                    FD_CLR(clt_map[need_to_send.returnTo()],&sock_fds);
                    closesocket(clt_map[need_to_send.returnTo()]);
                }else if(send_check == -2){
                    trash.emplace_back(need_to_send.returnFrom());
                    FD_CLR(clt_map[need_to_send.returnFrom()],&sock_fds);
                    closesocket(clt_map[need_to_send.returnFrom()]);
                }

            }

        }
        for(string i : trash){
            clt_map.erase(i);
        }

    }

    closesocket(ser_sock);
    WSACleanup();
    return 0;
}

int recvMessage(SOCKET& sock){
    char data[BUF_SIZE];
    if(recv(sock,data,BUF_SIZE,0) <= 0){
        cerr << "recv failed with error " << WSAGetLastError() << endl;
        return -1;
    }
    Message message;
    message.deserialize(data);
    need_to_send = message;
    return 1;
}

int sendMessage(){
    char data[BUF_SIZE];
    need_to_send.serialize(data);
    for(int i =-2;i<0;i++){
        if(send(clt_map[need_to_send.returnFrom()],data,BUF_SIZE,0)<=0){
            cerr << "send failed with error " << WSAGetLastError() << endl;
            return i;
        }
        need_to_send.swapFromTo();
    }
    return 1;
}

void recvConfirm(SOCKET clt_sock){
    char data[BUF_SIZE];
    recv(clt_sock,data,BUF_SIZE,0);

    Message check;
    check.deserialize(data);
    clt_map.emplace(check.returnFrom(),clt_sock);
}

