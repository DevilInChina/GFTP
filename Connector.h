//
// Created by Administrator on 2020/6/15.
//

#ifndef GFTP_CONNECTOR_H
#define GFTP_CONNECTOR_H

#include <string>
#include <cstring>
#include <thread>
#include <map>
#include <functional>
#include <algorithm>
#include <vector>
#include <set>
#ifdef  WIN32
#include  <winsock.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
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
#define SOCKET_ERROR (-1)
#endif

#ifndef SOCKADDR
#define SOCKADDR sockaddr
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET ((CurSocket)(~0))
#endif
#define INFOPORT 1521
#define DATAPORT 1520
using namespace std;
class Connector {
public:
    CurSocket CreateSocket(const string &ip, int port, bool changeSelf = true);

    CurSocket CreateSocket(const string &ipPortInfo, bool changeSelf = true);

    CurSocket SocketAccept(CurSocket sock);

    CurSocket SocketConnect(const string &ipPortInfo, bool changeSelf = true);

    CurSocket SocketConnect(const string &ip, int port, bool changeSelf = true);

    int recv_data(CurSocket sock, char *buf, unsigned long bufsize);

    int send_response(CurSocket sock, int code, const string &resp = "");

    int send_data(CurSocket sock, const char *buff, int bufsize);

    int send_data(CurSocket sock, const string &s);

    int sendBigData(CurSocket sock, const char*s, int length);

    char* recvBigData(CurSocket sock,int &length);

    string Encode(const string &a);

    CurSocket _socket;
    const int maxSize = 255;
private:
    int sendSize(CurSocket sock, int size);

    int recvSize(CurSocket sock);
};


void sighandler(int );
#endif //GFTP_CONNECTOR_H
