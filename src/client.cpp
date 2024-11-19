#include <winsock2.h>
#include <iostream>
#include <vector>
#include <thread>
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
void recvLoop(SOCKET clt_sock);
void sendLoop(SOCKET clt_sock,Message message);
void sendMessage(SOCKET clt_sock,Message message);
string getInputPrompt(string prompt="");
int send_state = 1;
int recv_state = 1;
string user;
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
    user = getInputPrompt("From");

    Message message;
    message.setFrom(user);
    message.setTo(getInputPrompt("To"));
    message.setContent("");
    cout<<string(67,'-')<<endl;
    sendMessage(clt_sock,message);

    thread send_thread(sendLoop,clt_sock,message);
    send_thread.detach();
    thread recv_thread(recvLoop,clt_sock);
    recv_thread.detach();

    while(send_state==1&&recv_state==1);
    WSACleanup();
    return 0;

}
//接收Message
void recvLoop(SOCKET clt_sock) {
    while (send_state == 1){
        char data[BUF_SIZE];
        if (recv(clt_sock, data, BUF_SIZE, 0) <= 0) {
            cerr << "recv failed with error " << WSAGetLastError() << endl;
            recv_state = 0;
            closesocket(clt_sock);
            return;  
        }
        Message recv_message;
        recv_message.deserialize(data);

        if(recv_message.returnFrom()!=user){
            cout <<"\033[46m\033[30m"<<recv_message.returnFrom() << ":" << recv_message.returnContent()<<"\033[0m"<< endl;
        }else{
            cout <<"\033[42m\033[30m"<<recv_message.returnFrom() << ":" << recv_message.returnContent()<<"\033[0m"<< endl;
        }
    }
    

}
void sendLoop(SOCKET clt_sock,Message message){
    while (recv_state == 1)
    {
        message.setContent(getInputPrompt());
        sendMessage(clt_sock,message);
    }
}
//发送已经准备好的Message
void sendMessage(SOCKET clt_sock,Message message){
    char data[BUF_SIZE];
    message.serialize(data);
    if (send(clt_sock,data,BUF_SIZE, 0) <= 0) {
        cerr << "send failed with error " << WSAGetLastError() << endl;
        send_state = 0;
        closesocket(clt_sock);
        return;
    } 
}

string getInputPrompt(string prompt){
    string str;
    while(str.empty()){
        if(prompt!=""){
            cout<<prompt<<":";
        }
        getline(cin,str);
        cout<<"\033[A\033[0G\033[K";
    }
    return str;
}

