//
// Created by Administrator on 2020/6/16.
//

#ifndef GFTP_FTPCLIENT_H
#define GFTP_FTPCLIENT_H

#include "Connector.h"

class ftpClient:public Connector{

public:
    ftpClient(int port);
    ~ftpClient();
    void beginProcess();
    void print_reply(int status);   //打印回复信息
    void SendEnd();
private:
    enum Status{
        unLogin,IpConnectSucceed,CheckingPsd,Logined
    };
    string ip;
    long long Retcode;
    int port;
    Status curStatus;
    int lastResponse;
    int sendDataAndResponse(CurSocket sock,const char*buff,int buffsize);
    int sendDataAndResponse(CurSocket sock, const string&cmd,const string&parameter);

    int Connect(const string&ip);

};


#endif //GFTP_FTPCLIENT_H
