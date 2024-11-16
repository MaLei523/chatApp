#include <winsock2.h>
#include <iostream>
#include <vector>

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
void fillData(Message& message,string& from,string& to,string& content);
void getInputPrompt(string& str,string prompt);
void recvMessage(SOCKET& clt_sock);
void sendMessage(SOCKET& clt_sock,string& from,string& to);
void sendConfirm(SOCKET& clt_sock,string& from);
void getInput(vector<char>& x,string prompt);



int main(){
    sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(PORT);
    ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ser_addr_sz = sizeof(ser_addr);

    WSADATA WSAdata;
    if(WSAStartup(MAKEWORD(2,2),&WSAdata)!=0){
        cerr << "WSAStartup failed with error: " << WSAGetLastError() << endl;
        return 1;
    }

    SOCKET clt_sock = socket(PF_INET,SOCK_STREAM,0);
    if(clt_sock==INVALID_SOCKET){
        cerr << "socket failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }
    if(connect(clt_sock,(sockaddr*)&ser_addr,ser_addr_sz)==SOCKET_ERROR){
        cerr << "connect failed with error: " << WSAGetLastError() << endl;
        closesocket(clt_sock);
        WSACleanup();
        return 1;      
    }
    string from;
    string to;
    getInputPrompt(from,"Enter your username");
    getInputPrompt(to,"Enter your friend");
    sendConfirm(clt_sock,from);
    int max_fd = clt_sock;
    while (1)
    {
        cout<<"Input:";
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(0,&read_fds);
        FD_SET(clt_sock,&read_fds);

        int select_check = select(max_fd+1,&read_fds,nullptr,nullptr,NULL);
        if(select_check == SOCKET_ERROR){
            cerr << "connect failed with error: " << WSAGetLastError() << endl;
            break;
        }
        if(FD_ISSET(0,&read_fds)){
            sendMessage(clt_sock,from,to);
        }
        if(FD_ISSET(clt_sock,&read_fds)){
            recvMessage(clt_sock);
        }
    }
    WSACleanup();
    return 0;
}

void recvMessage(SOCKET& clt_sock) {
    Message message;
    char data[BUF_SIZE];

    int recv_len = recv(clt_sock, data, BUF_SIZE, 0);
    
    if (recv_len == 0) {
        cout << "Server disconnected" << endl;
        closesocket(clt_sock);
        return;  
    }
    
    if (recv_len == SOCKET_ERROR) {
        cerr << "recv failed with error " << WSAGetLastError() << endl;
        return;
    }
    
    message.deserialize(data);
    cout<<string(message.from)<<":"<<string(message.content)<<endl;
}

void sendConfirm(SOCKET& clt_sock,string& from){
    Message message;
    from.copy(message.from,14);
    message.from[14] = '\0';
    char data[BUF_SIZE];
    int send_check = send(clt_sock,data,BUF_SIZE, 0);
    if (send_check == SOCKET_ERROR) {
        cerr << "send failed with error " << WSAGetLastError() << endl;
    } else if (send_check == 0) {
        cout << "Server disconnected" << endl;
        closesocket(clt_sock);
    }
}
void sendMessage(SOCKET& clt_sock,string& from,string& to) {
    string content;
    getline(cin,content);

    Message message;
    fillData(message,from,to,content);

    char data[BUF_SIZE];
    message.serialize(data);

    int send_check = send(clt_sock,data,BUF_SIZE, 0);

    if (send_check == SOCKET_ERROR) {
        cerr << "send failed with error " << WSAGetLastError() << endl;
    } else if (send_check == 0) {
        cout << "Server disconnected" << endl;
        closesocket(clt_sock);
    }
}
void fillData(Message& message,string& from,string& to,string& content){
    from.copy(message.from,14);
    to.copy(message.to,14);
    content.copy(message.content,69);
    message.from[14] = '\0';
    message.to[14] = '\0';
    message.content[69] = '\0';
}

void getInputPrompt(string& str,string prompt=""){
    cout<<prompt<<":";
    getline(cin,str);
}

void getInput(vector<char>& x){
    string temp;
    getline(cin,temp);
    x.assign(temp.begin(),temp.end());
}
