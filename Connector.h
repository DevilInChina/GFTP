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
    /**
     * @brief 根据ip和端口号创建一个socket并绑定在ip上，此ip需要为本地ip
     * @param ip
     * @param port
     * @param changeSelf 目前没有用处
     * @return
     */
    static CurSocket CreateSocket(const string &ip, int port, bool changeSelf = true);

    /**
     * @brief 解析字符串ipPortInfo后调用 CreateSocket
     * @param ipPortInfo (ip,ip,ip,ip,port,port)
     * @param changeSelf 目前没有用处
     * @return
     */
    static CurSocket CreateSocket(const string &ipPortInfo, bool changeSelf = true);

    /**
     * @brief 接受来自remote端的连接
     * @param sock
     * @return 建立好的socket 失败即为 INVALID_SOCKET
     */
    static CurSocket SocketAccept(CurSocket sock);

    /**
     * @brief 连接remote端，和socketAccept配套使用
     * @param ip
     * @param port
     * @param changeSelf
     * @return 失败即为 INVALID_SOCKET
     */
    static CurSocket SocketConnect(const string &ip, int port, bool changeSelf = true);

    /**
     * @brief 解析字符串后连接
     * @param ipPortInfo (ip,ip,ip,ip,port,port)
     * @param changeSelf
     * @return 失败即为 INVALID_SOCKET
     */
    static CurSocket SocketConnect(const string &ipPortInfo, bool changeSelf = true);

    /**
     * @brief 服务器向客户端发送返回值和信息
     * @param sock 和客户端建立好的连接
     * @param code 220,150 .......
     * @param resp 默认为空
     * @return 是否发送成功 -1不成功 0成功
     */
    static int send_response(CurSocket sock, int code, const string &resp = "");

    /**
     * @brief 从socket接收数据，接收之前清空buf 大小为 bufsize
     * @param sock
     * @param buf 提前分配空间
     * @param bufsize
     * @return 接收长度
     */
    static int recv_data(CurSocket sock, char *buf, unsigned long bufsize);

    /**
     * @brief 向socket发送大小为bufsize的数据，
     * @param sock
     * @param buff
     * @param bufsize
     * @return 发送长度
     */
    static int send_data(CurSocket sock, const char *buff, int bufsize);

    /**
     * @brief 发送传送字符串
     * @param sock
     * @param s
     * @return
     */
    static int send_data(CurSocket sock, const string &s);

    /**
     * @brief 先送出数据长度(int32)，后送出数据
     * @param sock
     * @param s
     * @param length
     * @return
     */
    static int sendBigData(CurSocket sock, const char *s, int length);

    /**
     * @brief 接收数据长度(int32)，分配空间后返回
     * @param sock
     * @param length 接收的数据长度
     * @return 接收到的数据，需要记得清理内存
     */
    static char *recvBigData(CurSocket sock, int &length);
    /**
     * @brief 发送一个int32，是sendBigData的第一步
     * @param sock
     * @param size
     * @return
     */
    static int sendSize(CurSocket sock, int size);

    /**
     * @brief 接收一个int32，是recvBigData的第一步
     * @param sock
     * @return
     */
    static int recvSize(CurSocket sock);

    /**
     * @brief recvBigData的第二步，分配空间，接收数据并返回。
     * @param sock
     * @param length
     * @return
     */
    static char *alloc_recv_data(CurSocket sock, int length);

    /**
     * @on_windows_brief 使用cmd命令读取文件大小，发送文件大小后使用共享内存将文件发送
     * @on_linux_brief 使用linux命令读取文件大小，发送文件大小后使用sendfile进行零拷贝发送。
     * @param sock
     * @param path
     * @param Encodes
     * @return
     * @todo 优化文件传送
     */
    static int sendFile(CurSocket sock, const string &path, int Encodes);

    /**
     * @on_windows_brief 接收文件大小后接收文件，使用共享内存机制直接写入路径
     * @on_linux_brief 接收文件大小后分配空间，将文件读取到内存中，写入路径后释放内存。
     * @param sock
     * @param path
     * @param Encodes
     * @return
     * @todo 优化文件接收
     */
    static int recvFile(CurSocket sock, const string &path, int Encodes);

    /**
     * @brief 将(ip,ip,ip,ip,port,port)解析为 ip port
     * @param ipPortInfo
     * @param ip
     * @param port
     */
    static void decoIPinfo(const string &ipPortInfo, string &ip, int &port);

    /**
     * @brief decoIPinfod的反函数
     * @param ipPortInfo
     * @param ip
     * @param port
     */
    static void encoIPinfo(string &ipPortInfo, const string &ip, int port);

    /**
     * @brief 使用MD5将字符串加密
     * @param a
     * @return
     */
    static string Encode(const string &a);

    /**
     * @brief 确定path中文件的大小
     * @param path
     * @param Encode (clent/server)+dataSocket 确定函数使用过程中产生的临时文件名字
     * @return -1表示文件不存在 -2表示是目录 其他为文件大小
     */
    static long long getFileSize(const string &path, int Encode);

    /**
     * @return 随机的一个端口号
     */
    static int getRandomPort();

    CurSocket _socket;
    const int BlockSize = 4096;

private:

    /**
     * @brief 使用命令确定是否为目录
     * @param path
     * @return
     */
    static int isDictionary(const string &path);

    /**
     * @brief 从使用命令生成的文件中读取文件大小
     * @param path
     * @return
     */
    static long long analyseSize(const string &path);
};


void sighandler(int );
#endif //GFTP_CONNECTOR_H
