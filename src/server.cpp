#include <winsock2.h>
#include <iostream>
#include <vector>
#include <thread>
#include <deque>
#include <mutex>
#include <unordered_map>
using namespace std;

#define BUF_SIZE 100
#define CONTENT_SIZE 70
#define PORT 50000

struct Message {
    char from[15];      // 固定大小的 from 字段，最多 14 个字符 + 1 个结尾的 '\0'
    char to[15];        // 固定大小的 to 字段，最多 14 个字符 + 1 个结尾的 '\0'
    char content[70];  // 固定大小的 content 字段,69+1

    // 序列化为二进制数据
    void serialize(char* buffer) const {
        memcpy(buffer, this, sizeof(Message));
    }

    void deserialize(const char* buffer) {
        memcpy(this, buffer, sizeof(Message));
    }

};

Message need_to_send;
unordered_map<string,SOCKET> clt_arr;
int recvMessage(SOCKET& sock);
void sendMessage();


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
//----------------------------------------------------------------------------//

    int max_fd = ser_sock;
    fd_set temp_fds,sock_fds;
    FD_ZERO(&sock_fds);
    FD_SET(ser_sock,&sock_fds);
    timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    while(1){
        temp_fds = sock_fds;
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
            SOCKET clt_scok = accept(ser_sock,(sockaddr*)&clt_addr,&clt_addr_sz);
            if(clt_scok == INVALID_SOCKET){
                cerr << "Accept failed." <<endl;
            }else{
                cout << "New connections created" <<endl;
            }
            char data[BUF_SIZE];
            recv(clt_scok,data,BUF_SIZE,0);
            Message check;
            check.deserialize(data);
            cout<<string(check.from)<<endl;
            cout<<string(check.to)<<endl;
            clt_arr.emplace(string(check.from),clt_scok);
            FD_SET(clt_scok,&sock_fds);

            max_fd = max(clt_scok,ser_sock);

        }
        for(pair<string,SOCKET> i : clt_arr){
            if(FD_ISSET(i.second,&temp_fds)){
                int recv_check = recvMessage(i.second);
                if(recv_check <= 0){
                    closesocket(i.second);
                    FD_CLR(i.second,&sock_fds);
                    continue;
                }
                sendMessage();
            }
        }

    }
//----------------------------------------------------------------------------//
    closesocket(ser_sock);
    WSACleanup();
    return 0;
}

int recvMessage(SOCKET& sock){
    char data[BUF_SIZE];
    int recv_len = recv(sock,data,BUF_SIZE,0);
    if(recv_len == 0){
        cerr<<"client disconnect"<<endl;
        return 0;
    }
    if(recv_len == SOCKET_ERROR){
        cerr << "recv failed with error " << WSAGetLastError() << endl;
        return -1;
    }
    Message message;
    message.deserialize(data);
    need_to_send = message;
    return 1;
}

void sendMessage(){
    string from(need_to_send.from);
    SOCKET sock = clt_arr[from];
    char data[BUF_SIZE];
    need_to_send.serialize(data);
    recv(sock,data,BUF_SIZE,0);

}