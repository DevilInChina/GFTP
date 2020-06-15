//
// Created by Administrator on 2020/6/15.
//
#include <stdio.h>
using namespace std;
#include <iostream>
#include "Connector.h"
bool Connector::CreateSocket(const string &ip, int port) {
    if((ip.empty()) || (port<0))
    {
        //print_log()
        return -1;
    }

    _socket=socket(AF_INET,SOCK_STREAM,0);
    if(_socket<0 || (_socket==INVALID_SOCKET))
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

    if(bind(_socket,(struct sockaddr*)&local,sizeof(local))<0)
    {
        //print_log();
        return -1;
    }

    if(listen(_socket,5)<0)
    {
        //print_log();
        return -1;
    }
    cout<<" "<<_socket<<" "<<errno<<endl;
    SOCKET sClient;
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);
    char revData[255];
    while (1){
        sClient = accept(_socket, (SOCKADDR *)&remoteAddr, &nAddrlen);
        if(sClient == INVALID_SOCKET)
        {
            continue;
        }
        printf("get:%s \r\n", inet_ntoa(remoteAddr.sin_addr));

        //接收数据
        int ret = recv(sClient, revData, 255, 0);
        if(ret > 0)
        {
            revData[ret] = 0x00;
            printf(revData);
        }

        //发送数据
        const char * sendData = "hello!\n";
        send(sClient, sendData, strlen(sendData), 0);
        closesocket(sClient);
    }
    return _socket;
}
int Connector::SocketAccept() {
    struct sockaddr_in peer;
    int len=sizeof(peer);
    int connfd=accept(_socket,(struct sockaddr*)&peer,&len);
    if(connfd<0)
    {
        //print_log()
        return -1;
    }
    return connfd;
}
int Connector::SocketConnect(const string &ip, int port) {
    if(ip.empty() || (port<0))
    {
        //print_log()
        return -1;
    }

    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        //print_log()
        return -1;
    }

    struct sockaddr_in peer;
    peer.sin_family=AF_INET;
    peer.sin_port=htons(port);
    peer.sin_addr.s_addr=inet_addr(ip.c_str());

    if(connect(sock,(struct sockaddr*)&peer,sizeof(peer))<0)
    {
        //print_log()
        return -1;
    }

    return sock;
}

int Connector::recv_data(CurSocket sock,char* buf,int bufsize){
    memset(buf,0,bufsize);
    int s=recv(sock,buf,bufsize,0);
    if(s<=0)
        return -1;
    return s;
}

int Connector::send_response(CurSocket sock, int code){
    int stat_code=htonl(code);
    if(send(sock,(char*)&stat_code,sizeof(stat_code),0)<0)
    {
        //print_log()
        return -1;
    }
    return 0;
}
int Connector::send_data(CurSocket sock,const char*buff,int buffsize){
    int ret =send(sock,buff,buffsize,0);
    return ret;
}

ftpServer::ftpServer(CurSocket sock):Connector(){
    _socket = sock;
}
ftpServer::~ftpServer() {}