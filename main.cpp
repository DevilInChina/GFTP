#include <iostream>
#include <csignal>
#include "ftpServer.h"
#include "ftpClient.h"
char buff[1024];

int main(int argc,char **argv) {
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    signal(SIGINT,sighandler);
    if(argc > 1){
        ftpServer s(argv[1],INFOPORT);
        cout<<"start linsen "<<s._socket<<endl;
        s.beginListen();
    }else{
        ftpClient s(INFOPORT);
        s.beginProcess();
    }
#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}
