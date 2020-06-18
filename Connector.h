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
#include <random>
#include <fstream>
#define GETRAND(x) (rand()%(x))
#ifdef  WIN32
#include  <winsock.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/sendfile.h>
#include <zconf.h>
#include <fcntl.h>
#endif

#ifdef  WIN32
#define CurSocket SOCKET
#define CurClose closesocket
#define socklen_t int
#define FILESEPERATOR '\\'
#else
#define CurSocket int
#define CurClose close
#define SOCKET_ERROR (-1)
#define FILESEPERATOR '/'
#endif

#ifndef SOCKADDR
#define SOCKADDR sockaddr
#endif

#ifndef CMD_SUCCEED
#define CMD_SUCCEED 0
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET ((CurSocket)(~0))
#endif
#define INFOPORT 1521
#define DATAPORT 1520
#define AUTH ".AUTH"
#define maxSize 255
using namespace std;
class Connector {
public:
    static CurSocket CreateSocket(const string &ip, int port, bool changeSelf = true);

    static CurSocket CreateSocket(const string &ipPortInfo, bool changeSelf = true);

    static CurSocket SocketAccept(CurSocket sock);

    static CurSocket SocketConnect(const string &ipPortInfo, bool changeSelf = true);

    static CurSocket SocketConnect(const string &ip, int port, bool changeSelf = true);

    static int recv_data(CurSocket sock, char *buf, unsigned long bufsize);

    static int send_response(CurSocket sock, int code, const string &resp = "");

    static int send_data(CurSocket sock, const char *buff, int bufsize);

    static int send_data(CurSocket sock, const string &s);

    static int sendBigData(CurSocket sock, const char *s, int length);

    static char *recvBigData(CurSocket sock, int &length);
    static char *alloc_recv_data(CurSocket sock, int length);
    static int sendFile(CurSocket sock,const string &path,int Encodes);

    static int recvFile(CurSocket sock,const string &path,int Encodes);

    static void dealIPinfo(const string &ipPortInfo, string &ip, int &port);

    static void encoIPinfo(string &ipPortInfo, const string &ip, int port);

    static string Encode(const string &a);


    static long long getFileSize(const string &path,int Encode);

    static int getRandomPort();

    CurSocket _socket;
    const int BlockSize = 4096;

private:
    static int sendSize(CurSocket sock, int size);

    static int recvSize(CurSocket sock);

    static int isDictionary(const string &path);

    static long long analyseSize(const string &path);
};


void sighandler(int );
#endif //GFTP_CONNECTOR_H
