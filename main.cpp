#include <iostream>
#include "Connector.h"
char buff[1024];
int main(int argc,char **argv) {
    Connector s;
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    if(argc > 1){
        int k = s.CreateSocket("192.168.2.100",1453);
    }else{
        SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sclient == INVALID_SOCKET)
        {
            printf("invalid socket!");
            return 0;
        }

        sockaddr_in serAddr;
        serAddr.sin_family = AF_INET;
        serAddr.sin_port = htons(1453);
        serAddr.sin_addr.S_un.S_addr = inet_addr("192.168.2.100");
        if(connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
        {  //连接失败
            printf("connect error !");
            closesocket(sclient);
            return 0;
        }

        string data;
        cin>>data;
        const char * sendData;
        sendData = data.c_str();   //string转const char*
        //char * sendData = "你好，TCP服务端，我是客户端\n";
        send(sclient, sendData, strlen(sendData), 0);
        //send()用来将数据由指定的socket传给对方主机
        //int send(int s, const void * msg, int len, unsigned int flags)
        //s为已建立好连接的socket，msg指向数据内容，len则为数据长度，参数flags一般设0
        //成功则返回实际传送出去的字符数，失败返回-1，错误原因存于error

        char recData[255];
        int ret = recv(sclient, recData, 255, 0);
        if(ret>0){
            recData[ret] = 0x00;
            printf(recData);
        }
        closesocket(sclient);
     }
    WSACleanup();
    return 0;
}
