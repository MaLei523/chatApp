#include <winsock2.h>
#include <iostream>
#include <vector>

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
};
string getInputPrompt(string prompt="");
void sendMessage(SOCKET& clt_sock,Message message);
void sendConfirm(SOCKET& clt_sock,Message message);
void recvMessage(SOCKET& clt_sock);

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
    Message message;
    message.setFrom(getInputPrompt("From:"));
    message.setTo(getInputPrompt("To:"));
    sendConfirm(clt_sock,message);

    int max_fd = clt_sock;
    fd_set read_fds,temp_fds;
    FD_ZERO(&read_fds);
    FD_SET(0,&read_fds);
    FD_SET(clt_sock,&read_fds);
    timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_sec = 0;

    while (1)
    {
        cout<<"Content:";
        temp_fds = read_fds;
        int select_check = select(max_fd+1,&temp_fds,nullptr,nullptr,&timeout);
        if(select_check == SOCKET_ERROR){
            cerr << "select failed with error: " << WSAGetLastError() << endl;
            break;
        }
        if(select_check == 0){
            cout<<"client Idle No Connection"<<endl;
            continue;
        }
        if(FD_ISSET(0,&temp_fds)){
            sendMessage(clt_sock,message);
        }
        if(FD_ISSET(clt_sock,&temp_fds)){
            recvMessage(clt_sock);
        }
    }
    WSACleanup();
    return 0;
}

void recvMessage(SOCKET& clt_sock) {
    Message recv_message;
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
    
    recv_message.deserialize(data);
    cout<<recv_message.returnFrom()<<":"<<recv_message.returnContent()<<endl;
}

void sendConfirm(SOCKET& clt_sock,Message message){
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
void sendMessage(SOCKET& clt_sock,Message message) {
    message.setContent(getInputPrompt());
    sendConfirm(clt_sock,message);
}


string getInputPrompt(string prompt){
    if(prompt!=""){
        cout<<prompt;
    }
    string str;
    getline(cin,str);
    return str;
}