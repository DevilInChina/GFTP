#include <iostream>
#include <csignal>
#include "Connector.h"
char buff[1024];

int main(int argc,char **argv) {
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    signal(SIGINT,sighandler);
    if(argc > 1){
        ftpServer s("192.168.2.100",1400);
        cout<<"start linsen "<<s._socket<<endl;
        s.beginListen();
    }else{
        ftpClient s(1400);
        s.beginProcess();
    }
#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}
