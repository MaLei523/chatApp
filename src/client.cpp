#include <winsock2.h>
#include <iostream>
#include <string>
using namespace std;

int main(){
    
    SOCKADDR_IN ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(50000);
    ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    WSADATA WSAdata;
    if(WSAStartup(MAKEWORD(2,2),&WSAdata)!=0){
        cout<<"WSA error"<<endl;
        return 1;
    }

    SOCKET clt_sock = socket(PF_INET,SOCK_STREAM,0);
    if(clt_sock==INVALID_SOCKET){
        cout<<"server socket error"<<endl;
        return 1;
        WSACleanup();
    }

    int connect_check = connect(clt_sock,(SOCKADDR*)&ser_addr,sizeof(ser_addr));
    if(connect_check == SOCKET_ERROR){
        cout<<"connect socket error"<<endl;
        return 1;
        WSACleanup();        
    }else{
        cout<<"connect successfully"<<endl;
    }

    while (1)
    {
        string message;
        cout<<"input:";
        getline(cin,message); 
        send(clt_sock,message.c_str(),message.size(),0);
        if(message=="exit"){
            break;
        }
        recv(clt_sock,message.data(),message.size(),0);
        cout<<"output:"<<message<<endl;
    }

    closesocket(clt_sock);
    WSACleanup();
    return 0;
}