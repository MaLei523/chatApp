#include <winsock2.h>
#include <iostream>
#include <vector>

using namespace std;
#define BUF_SIZE 64
#define PORT 50000

void recvMessage(SOCKET& clt_sock);
void sendMessage(SOCKET& clt_sock);
void getInput(vector<char>& x,string prompt);

string vcToStr(const vector<char>& vc);


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



    int max_fd = 0+1;
    while (1)
    {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(0,&read_fds);
        int select_check = select(max_fd,&read_fds,nullptr,nullptr,NULL);

        if(FD_ISSET(0,&read_fds)){

        }
    }


//----------------------------------------------------------------------------//



    WSACleanup();
    return 0;
}

void recvMessage(SOCKET& clt_sock) {
    vector<char> message(BUF_SIZE);
    int recv_len = recv(clt_sock, message.data(), BUF_SIZE, 0);
    
    if (recv_len == 0) {
        cout << "Server disconnected" << endl;
        closesocket(clt_sock);
        return;  
    }
    
    if (recv_len == SOCKET_ERROR) {
        cerr << "recv failed with error " << WSAGetLastError() << endl;
        return;
    }
    cout<<vcToStr(message)<<endl;
}

void sendMessage(SOCKET& clt_sock) {
    vector<char> message(BUF_SIZE);
    getInput(message, "input your message:");

    int send_check = send(clt_sock, message.data(), message.size(), 0);

    if (send_check == SOCKET_ERROR) {
        cerr << "send failed with error " << WSAGetLastError() << endl;
    } else if (send_check == 0) {
        cout << "Server disconnected" << endl;
        closesocket(clt_sock);
    }
}


void getInput(vector<char>& x,string prompt=""){
    if(prompt!=""){
        cout<<prompt;
    }
    string temp;
    getline(cin,temp);
    x.assign(temp.begin(),temp.end());
}

string vcToStr(const vector<char>& vc){
    string str(vc.begin(),vc.end());
    return str;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//