//
// Created by Administrator on 2020/6/15.
//
#include <cstdio>
#include <sstream>
#include <iostream>

using namespace std;

#include "Connector.h"

void Connector::dealIPinfo(const string &ipPortInfo,string &ip,int &port){
    string info = ipPortInfo;
    replace(info.begin(),info.end(),',','.');
    int cnt = 0;
    int len1 = 0;
    int len2 = 0;
    for(int i = 0 ; i < info.length() ; ++i){
        if(info[i]=='.')++cnt;
        if(cnt==4 && len1==0){
            len1 = i;
        }else if(cnt==5){
            len2 = i;
            break;
        }
    }

    ip = string(info.begin(),info.begin()+len1);
    string a (info.begin()+len1+1,info.begin()+len2);
    string b (info.begin()+len2+1,info.end());
    port = atoi(a.c_str());
    port<<=8;
    port+=atoi(b.c_str());
}
void Connector::encoIPinfo(string &ipPortInfo, const string &ip, int port) {
    ipPortInfo = ip;
    replace(ipPortInfo.begin(),ipPortInfo.end(),'.',',');
    ipPortInfo+=","+to_string(port/256);
    ipPortInfo+=","+to_string(port%256);
}
CurSocket Connector::CreateSocket(const string &ip, int port,bool changeSelf) {
    if((ip.empty()) || (port<0))
    {
        //print_log()
        return INVALID_SOCKET;
    }
    CurSocket sock =socket(AF_INET,SOCK_STREAM,0);

    if(sock==INVALID_SOCKET){
        //print_log();
        return INVALID_SOCKET;
    }

    int op=1;
    try {
        setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&op,sizeof(op));
    }catch (int s){

    }

    struct sockaddr_in local;
    local.sin_family=AF_INET;
    local.sin_port=htons(port);
    local.sin_addr.s_addr=inet_addr(ip.c_str());

    if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
    {
        //print_log();
        CurClose(sock);
        return INVALID_SOCKET;
    }

    if(listen(sock,5)<0)
    {
        //print_log();
        CurClose(sock);
        return INVALID_SOCKET;
    }
    if(changeSelf)_socket = sock;
    return sock;
}

CurSocket Connector::CreateSocket(const string &ipPortInfo,bool changeSelf){
    string ip;
    int port;
    dealIPinfo(ipPortInfo,ip,port);
    return CreateSocket(ip,port,changeSelf);
}
CurSocket Connector::SocketAccept(CurSocket sock) {
    struct sockaddr_in peer;
    int len=sizeof(peer);
    CurSocket connfd=accept(sock,(struct sockaddr*)&peer,(socklen_t*)&len);
    return connfd;
}
CurSocket Connector::SocketConnect(const string &ip, int port,bool changeSelf) {
    if(port<0 || ip.empty())
        return INVALID_SOCKET;
    CurSocket ret = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ret == INVALID_SOCKET)
    {
        printf("invalid socket!");
        return INVALID_SOCKET;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    if(connect(ret, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {  //连接失败
        printf("%s %d connect error !\n",ip.c_str(),port);
        return INVALID_SOCKET;
    }
    if(changeSelf)_socket = ret;
    return ret;
}

CurSocket Connector::SocketConnect(const string &ipPortInfo,bool changeSelf) {
    string ip;
    int port;
    dealIPinfo(ipPortInfo,ip,port);

    return SocketConnect(ip,port,changeSelf);
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

int Connector::recv_data(CurSocket sock,char* buf,unsigned long bufsize) {
    memset(buf, 0, bufsize);
    int s;
    s = recv(sock, buf, bufsize, 0);
    return s;
}
int Connector::sendSize(CurSocket sock, int size){
    int sender = htonl(size);
    if(send(sock,(char*)&sender, sizeof(sender),0)<0)
    {
        return -1;
    }
    return 0;
}
int Connector::recvSize(CurSocket sock){
    int recv;
    while (recv_data(sock, (char*)&recv, sizeof(recv)) <= 0){}

    recv = ntohl(recv);
    return recv;
}
int Connector::send_response(CurSocket sock, int code,const string&resp){
    int stat_code=htonl(code);
    char bufint[maxSize];
    bufint[4] = 0;
    int *res = (int*)bufint;
    *res = stat_code;
    memcpy(bufint+4,resp.c_str(), sizeof(char)*resp.length());
    string ret = string(bufint)+resp;
    if(send(sock,bufint, sizeof(stat_code)+resp.length(),0)<0)
    {
        return -1;
    }
    return 0;
}
int Connector::send_data(CurSocket sock,const char*buff, int buffsize){
    int ret =send(sock,buff,buffsize,0);
    return ret;
}
int Connector::send_data(CurSocket sock,const string&data){
    int ret =send(sock,data.c_str(),data.length(),0);
    return ret;
}

int Connector::sendBigData(CurSocket sock, const char*s, int length) {
    sendSize(sock,length);
    return send_data(sock,s,length);
}
char* Connector::recvBigData(CurSocket sock, int&length) {
    length = recvSize(sock);
    char *ret = new char[length + 1];

    recv_data(sock, ret, length);
    ret[length ] = 0;
    return ret;
}

int Connector::getRandomPort() {
    int po = (GETRAND(65535)) + 1024;
    po = min(65534, po);
    return po;
}

int Connector::isDictionary(const string &path) {
    return system(("cd "+path).c_str())==CMD_SUCCEED;
}

long long Connector::analyseSize(const string &path) {
    ifstream ff(path);
    string res;
    string temp;
#ifdef WIN32
    while (getline(ff, temp)) {
        if (!isdigit(temp[0]))continue;///not a file in windows
        stringstream ss(temp);
        for (int i = 0;ss >> temp && i < 2; ++i) {}
        return atoll(temp.c_str());
    }
#else
    while (getline(ff,temp)) {
        stringstream ss(temp);
        for (int i = 0;ss>>temp && i < 4; ++i) {
            cout<<temp<<endl;
        }
        return atoll(temp.c_str());
    }
#endif

}

long long Connector::getFileSize(const string &path,int Encode) {
    int k = isDictionary(path);
    if(k){
        return -1;//
    }
    string fileName = ".SIZE" + to_string(Encode);
    string CMDS;
    string del;
#ifdef WIN32
    CMDS = "dir ";
    del = "del ";
#else
    CMDS="ls -l ";
    del = "rm ";
#endif
    CMDS+=path+" >> "+fileName;
    k = system(CMDS.c_str());
    long long ret= 0;
    if(k!=CMD_SUCCEED){/// no such file
        ret = -2;
    }else{
        ret = analyseSize(fileName);
    }
    del+=fileName;
    system(del.c_str());
    return ret;
}
#define FLASH_SIZE        1024*1024*4
#define COPY_DATA        "Hello"
string toLinuxPath(const string &path){
    string ret = path;
    replace(ret.begin(),ret.end(),FILESEPERATOR,'/');
    return ret;
}
#ifdef WIN32
int Connector::sendFile(CurSocket sock, const string &paths,int Encodes) {
    int buff_size = getFileSize(paths,Encodes);
    if(buff_size<0){
        return 0;
    }
    string path = toLinuxPath(paths);

    HANDLE dump_file_descriptor = CreateFile(path.c_str(),
                                             GENERIC_READ | GENERIC_WRITE,
                                             FILE_SHARE_READ,
                                             NULL,
                                             OPEN_ALWAYS, // open exist or create new, overwrite file
                                             FILE_ATTRIBUTE_NORMAL,
                                             NULL);

    if (dump_file_descriptor == INVALID_HANDLE_VALUE) {
        return 0;
    }
    HANDLE shared_file_handler = CreateFileMapping(
            dump_file_descriptor, // Use paging file - shared memory
            nullptr,                 // Default security attributes
            PAGE_READWRITE,       // Allow read and write access
            0,                    // High-order DWORD of file mapping max size
            buff_size,            // Low-order DWORD of file mapping max size
            path.c_str());    // Name of the file mapping object
    if (shared_file_handler)
    {
        // map memory file view, get pointer to the shared memory
        LPVOID lp_base = MapViewOfFile(
                shared_file_handler,  // Handle of the map object
                FILE_MAP_READ,  // Read and write access
                0,                    // High-order DWORD of the file offset
                0,                    // Low-order DWORD of the file offset
                buff_size);           // The number of bytes to map to view



        // copy data to shared memory
        //memcpy(lp_base, &share_buffer, sizeof(share_buffer));
        sendBigData(sock,(char*)lp_base,buff_size);
        //puts((char*)lp_base);
        //FlushViewOfFile(lp_base, buff_size); // can choose save to file or not

        // process wait here for other task to read data
        //cout << share_buffer << endl;

        // close shared memory file
        UnmapViewOfFile(lp_base);
        CloseHandle(shared_file_handler);
        CloseHandle(dump_file_descriptor);
    }
    else {
        CloseHandle(dump_file_descriptor);
        return 0;
    }
    return 1;
}

int Connector::recvFile(CurSocket sock, const string &paths,int Encodes) {
    int buff_size;

    if(buff_size<0){
        return 0;
    }
    string path = toLinuxPath(paths);
    HANDLE dump_file_descriptor = CreateFile(path.c_str(),
                                             GENERIC_READ | GENERIC_WRITE,
                                             FILE_SHARE_READ|FILE_SHARE_WRITE ,
                                             NULL,
                                             OPEN_ALWAYS, // open exist or create new, overwrite file
                                             FILE_ATTRIBUTE_NORMAL,
                                             NULL);

    if (dump_file_descriptor == INVALID_HANDLE_VALUE){
        return 0;
    }
    HANDLE shared_file_handler = CreateFileMapping(
            dump_file_descriptor, // Use paging file - shared memory
            nullptr,                 // Default security attributes
            PAGE_READWRITE,       // Allow read and write access
            0,                    // High-order DWORD of file mapping max size
            buff_size,            // Low-order DWORD of file mapping max size
            path.c_str());    // Name of the file mapping object
    if (shared_file_handler)
    {
        // map memory file view, get pointer to the shared memory
        LPVOID lp_base = MapViewOfFile(
                shared_file_handler,  // Handle of the map object
                FILE_MAP_ALL_ACCESS,  // Read and write access
                0,                    // High-order DWORD of the file offset
                0,                    // Low-order DWORD of the file offset
                buff_size);           // The number of bytes to map to view

        // copy data to shared memory
        char *share_buffer = recvBigData(sock,buff_size);
        memcpy(lp_base, share_buffer, buff_size);
        delete share_buffer;
        //FlushViewOfFile(lp_base, buff_size); // can choose save to file or not

        // process wait here for other task to read data
        //cout << share_buffer << endl;

        // close shared memory file
        UnmapViewOfFile(lp_base);
        CloseHandle(shared_file_handler);
        CloseHandle(dump_file_descriptor);
        //unlink(path.c_str());
    }
    else {
        CloseHandle(dump_file_descriptor);
        return 0;
    }
    return 1;
}
#else
int Connector::sendFile(CurSocket sock, const string &paths,int Encodes){
    int fileSize = getFileSize(paths,Encodes);
    int file_fd = open(paths.c_str(),O_RDONLY);
    cout<<fileSize<<" "<<file_fd<<" "<<sock<<endl;
    sendSize(sock,fileSize);
    cout<<sendfile(sock,file_fd, nullptr,fileSize)<<endl;
    close(file_fd);
}
int Connector::recvFile(CurSocket sock, const string &paths,int Encodes){
    int fileSize;
    char *s = recvBigData(sock,fileSize);
    fstream f(paths,ios::out);
    f.write(s,fileSize);
    delete s;
    f.close();
}
#endif