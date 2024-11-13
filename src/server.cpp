#include <winsock2.h>
#include <iostream>
#include <string>
using namespace std;

int main(){

    SOCKADDR_IN ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(50000);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    SOCKADDR_IN clt_addr;
    int clt_sz = sizeof(clt_addr);

    WSADATA WSAdata;
    if(WSAStartup(MAKEWORD(2,2),&WSAdata)!=0){
        cout<<"WSA error"<<endl;
        return 1;
    }
    SOCKET ser_sock = socket(PF_INET,SOCK_STREAM,0);
    if(ser_sock==INVALID_SOCKET){
        cout<<"server socket error"<<endl;
        return 1;
        WSACleanup();
    }

    int bind_check = bind(ser_sock,(SOCKADDR*)&ser_addr,sizeof(ser_addr));
    if(bind_check == SOCKET_ERROR){
        cout<<"bind error"<<endl;
        return 1;
        WSACleanup();
    }

    int listen_check = listen(ser_sock,3);
    if(listen_check == SOCKET_ERROR){
        cout<<"listen error"<<endl;
        return 1;
        WSACleanup();       
    }

    while(1){
        SOCKET clt_sock = accept(ser_sock,(SOCKADDR*)&clt_addr,&clt_sz);
        if(clt_sock == INVALID_SOCKET){
            cout<<"accept error"<<endl;
            break;      
        }else{
            cout<<"accept successfully"<<endl;
        }
        string message;
        int max_len = 64;
        while(1){
            int recv_len = recv(clt_sock,message.data(),max_len,0);
            if(message=="exit"){
                break;
            }
            send(clt_sock,message.c_str(),recv_len,0);
        }
        closesocket(clt_sock);
    }
    closesocket(ser_sock);
    WSACleanup();
    return 0;
}