//
// Created by Administrator on 2020/6/15.
//

#ifndef GFTP_CONNECTOR_H
#define GFTP_CONNECTOR_H

#include <string>
#include <string.h>

#ifdef  WIN32
#include  <winsock.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <zconf.h>
#endif

#ifdef  WIN32
#define CurSocket SOCKET
#define CurClose closesocket
#define socklen_t int
#else
#define CurSocket int
#define CurClose close
#endif

#ifndef SOCKADDR
#define SOCKADDR sockaddr
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET ((CurSocket)(~0))
#endif
using namespace std;
class Connector {
public:
    bool CreateSocket(const string &ip, int port);
    int SocketAccept();
    int SocketConnect(const string &ip,int port);
    int recv_data(CurSocket sock,char* buf,int bufsize)  ;
    int send_response(CurSocket sock,int code) ;
    int send_data(CurSocket sock,const char *buff,int bufsize);
    CurSocket _socket;
};
class ftpServer: public Connector{
public:
    ftpServer(const string &ip,int port);
    void beginListen();
    ~ftpServer();
private:
};
class ftpClient:public Connector{
public:
    ftpClient(const string&ip,int port);
    ~ftpClient();
    int Send(const string &data);
    int get();

private:
    string ip;
    int port;
};


#endif //GFTP_CONNECTOR_H
