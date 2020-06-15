#include <iostream>
#include "Connector.h"
char buff[1024];
int main(int argc,char **argv) {
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    if(argc > 1){
        ftpServer s("192.168.2.100",1453);
        cout<<"start linsen "<<s._socket<<endl;
        s.beginListen();
    }else{
        ftpClient s("192.168.2.100",1453);
        string data;
        while (true){
            cin>>data;
            s.Send(data);
            cout<<"senddone"<<endl;
        }
    }
#ifdef WIN32
    WSACleanup();
#endif
    return 0;
}
