//
// Created by Administrator on 2020/6/15.
//
#include <stdio.h>
#ifdef  WIN32
#include  <winsock.h>
#else
#include <sys/socket.h>
#endif
#include "Connnector.h"
bool Connnector::CreateSocket(string ip, int port) {
    _socket = socket(AF_INET,SOCK_STREAM,0);
    if((ip.empty()) || (port<0))
    {
        //print_log()
        return -1;
    }

    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        //print_log();
        return -1;
    }

    int op=1;
    try {
        setsockopt(_socket,SOL_SOCKET,SO_REUSEADDR,(char*)&op,sizeof(op));
    }catch (int s){

    }

    struct sockaddr_in local;
    local.sin_family=AF_INET;
    local.sin_port=htons(port);
    local.sin_addr.s_addr=inet_addr(ip.c_str());

    if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
    {
        //print_log();
        return -1;
    }

    if(listen(sock,5)<0)
    {
        //print_log();
        return -1;
    }
    return sock;
}