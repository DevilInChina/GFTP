//
// Created by Administrator on 2020/6/15.
//
#include <stdio.h>
using namespace std;

#include "Connector.h"
bool Connector::CreateSocket(const string &ip, int port) {
    if((ip.empty()) || (port<0))
    {
        //print_log()
        return -1;
    }

    _socket=socket(AF_INET,SOCK_STREAM,0);
    if(_socket<0 || (_socket==INVALID_SOCKET)){
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
        printf("connect error !\n");
        return -1;
        CurClose(_socket);
    }
    return _socket;
}

//strBaye的长度
class  MD5 {
#define shift(x, n) (((x) << (n)) | ((x) >> (32-(n))))//右移的时候，高位一定要补零，而不是补充符号位
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476
public:
    unsigned int strlength;
//A,B,C,D的临时变量
    unsigned int atemp;
    unsigned int btemp;
    unsigned int ctemp;
    unsigned int dtemp;
//常量ti unsigned int(abs(sin(i+1))*(2pow32))
    const unsigned int k[64] = {
            0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
            0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8,
            0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193,
            0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51,
            0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
            0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905,
            0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681,
            0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60,
            0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
            0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244,
            0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
            0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314,
            0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
//向左位移数
    const unsigned int s[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7,
                              12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                              4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10,
                              15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
    const char str16[17] = "0123456789abcdef";

    void mainLoop(unsigned int M[]) {
        unsigned int f, g;
        unsigned int a = atemp;
        unsigned int b = btemp;
        unsigned int c = ctemp;
        unsigned int d = dtemp;
        for (unsigned int i = 0; i < 64; i++) {
            if (i < 16) {
                f = F(b, c, d);
                g = i;
            } else if (i < 32) {
                f = G(b, c, d);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                f = H(b, c, d);
                g = (3 * i + 5) % 16;
            } else {
                f = I(b, c, d);
                g = (7 * i) % 16;
            }
            unsigned int tmp = d;
            d = c;
            c = b;
            b = b + shift((a + f + k[i] + M[g]), s[i]);
            a = tmp;
        }
        atemp = a + atemp;
        btemp = b + btemp;
        ctemp = c + ctemp;
        dtemp = d + dtemp;
    }

/*
*填充函数
*处理后应满足bits≡448(mod512),字节就是bytes≡56（mode64)
*填充方式为先加一个1,其它位补零
*最后加上64位的原来长度
*/
    unsigned int *add(const string &str) {
        unsigned int num = ((str.length() + 8) / 64) + 1;//以512位,64个字节为一组
        unsigned int *strByte = new unsigned int[num * 16];    //64/4=16,所以有16个整数
        strlength = num * 16;
        for (unsigned int i = 0; i < num * 16; i++)
            strByte[i] = 0;
        for (unsigned int i = 0; i < str.length(); i++) {
            strByte[i >> 2] |= (str[i]) << ((i % 4) * 8);//一个整数存储四个字节，i>>2表示i/4 一个unsigned int对应4个字节，保存4个字符信息
        }
        strByte[str.length() >> 2] |= 0x80 << (((str.length() % 4)) * 8);//尾部添加1 一个unsigned int保存4个字符信息,所以用128左移
        /*
        *添加原长度，长度指位的长度，所以要乘8，然后是小端序，所以放在倒数第二个,这里长度只用了32位
        */
        strByte[num * 16 - 2] = str.length() * 8;
        return strByte;
    }

    string changeHex(int a) {
        int b;
        string str1;
        string str = "";
        for (int i = 0; i < 4; i++) {
            str1 = "";
            b = ((a >> i * 8) % (1 << 8)) & 0xff;   //逆序处理每个字节
            for (int j = 0; j < 2; j++) {
                str1.insert(0, 1, str16[b % 16]);
                b = b / 16;
            }
            str += str1;
        }
        return str;
    }

    string getMD5(const string &source) {
        atemp = A;    //初始化
        btemp = B;
        ctemp = C;
        dtemp = D;
        unsigned int *strByte = add(source);
        for (unsigned int i = 0; i < strlength / 16; i++) {
            unsigned int num[16];
            for (unsigned int j = 0; j < 16; j++)
                num[j] = strByte[i * 16 + j];
            mainLoop(num);
        }
        return changeHex(atemp).append(changeHex(btemp)).append(changeHex(ctemp)).append(changeHex(dtemp));
    }
};
MD5 Encoder;
string Connector::Encode(const string &a) {
    return Encoder.getMD5(a);
}

int Connector::recv_data(CurSocket sock,char* buf,int bufsize) {
    memset(buf, 0, bufsize);
    int s;
    s = recv(sock, buf, bufsize, 0);
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
int Connector::send_data(CurSocket sock,const string&data){
    int ret =send(sock,data.c_str(),data.length(),0);
    return ret;
}
