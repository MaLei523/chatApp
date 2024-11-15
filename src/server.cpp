#include <winsock2.h>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
using namespace std;

#define BUF_SIZE 64
#define PORT 50000

void echo(SOCKET clt_sock);
string vcToStr(const vector<char>& vc);
int main(){
    SOCKADDR_IN ser_addr;
    memset(&ser_addr,0,sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(PORT);
    ser_addr.sin_addr.s_addr = INADDR_ANY;

    SOCKADDR_IN clt_addr;
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

    int bind_check = bind(ser_sock,(SOCKADDR*)&ser_addr,sizeof(ser_addr));
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
    while (1)
    {
        SOCKET clt_sock = accept(ser_sock,(SOCKADDR*)&clt_addr,&clt_addr_sz);
        if(clt_sock == INVALID_SOCKET){
            cerr << "accept failed with error: " << WSAGetLastError() << endl;
            continue;       
        }
        cout << "Client connected." <<endl;

        thread client_thread(echo,clt_sock);

        client_thread.detach();
        
    }
    
//----------------------------------------------------------------------------//
    closesocket(ser_sock);
    WSACleanup();
    return 0;
}

void echo(SOCKET clt_sock){
    vector<char> message(BUF_SIZE);
    vector<char> exit{'e','x','i','t'};

    int recv_len;
    while ((recv_len = recv(clt_sock,message.data(),BUF_SIZE,0))>0)
    {
        if(vector<char>(message.begin(),message.begin()+recv_len) == exit){
            cout<<"Time to exit"<<endl;
            break;
        }
        send(clt_sock,message.data(),recv_len,0);
    } 
    if (recv_len == 0) {
        cout << "Client disconnected" << std::endl;
    }
    else if (recv_len == SOCKET_ERROR) {
        cerr << "recv failed with error " << WSAGetLastError() << std::endl;
    }
    closesocket(clt_sock);
}
