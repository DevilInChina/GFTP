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
    return _socket;
}
int Connector::SocketAccept() {
    struct sockaddr_in peer;
    int len=sizeof(peer);
    int connfd=accept(_socket,(struct sockaddr*)&peer,(socklen_t*)&len);
    if(connfd<0)
    {
        //print_log()
        return -1;
    }
    return connfd;
}
int Connector::SocketConnect(const string &ip, int port) {
    if(port<0 || ip.empty())
        return -1;
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(_socket == INVALID_SOCKET)
    {
        printf("invalid socket!");
        return -1;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    if(connect(_socket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {  //连接失败
        printf("connect error !");
        CurClose(_socket);
    }
    return _socket;
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

ftpServer::ftpServer(const string&ip,int port):Connector(){
    CreateSocket(ip,port);
}
void ftpServer::beginListen() {
    CurSocket sClient;
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);
    char revData[255];
    while (true){
        sClient = accept(_socket, (SOCKADDR *)&remoteAddr, (socklen_t*)&nAddrlen);
        if(sClient == INVALID_SOCKET)
        {
            continue;
        }
        printf("\n%d:%s \r\n",sClient, inet_ntoa(remoteAddr.sin_addr));

        int ret = recv(sClient, revData, 255, 0);
        if(ret > 0)
        {
            revData[ret] = 0x00;
            printf(revData);
        }

        const char * sendData = "hello!\n";
        send(sClient, sendData, strlen(sendData), 0);
        CurClose(sClient);
    }
}

ftpServer::~ftpServer() {}

ftpClient::ftpClient(const string&ip,int port):ip(ip),port(port) {

}

ftpClient::~ftpClient() {
}
int ftpClient::Send(const string &data) {
    const char * sendData;
    SocketConnect(ip,port);
    cout<<_socket<<endl;
    send(_socket, data.c_str(), data.length(), 0);
    char recData[255];
    int ret = recv(_socket, recData, 255, 0);
    if(ret>0){
        recData[ret] = 0x00;
        cout<<recData<<" "<<_socket<<endl;
    }else{
        cout<<"rec err"<<endl;
    }
    CurClose(_socket);
}