//
// Created by Administrator on 2020/6/16.
//

#ifndef GFTP_FTPCLIENT_H
#define GFTP_FTPCLIENT_H

#include "Connector.h"

class ftpClient:public Connector {

public:
    ftpClient(int port);

    ~ftpClient();

    void beginProcess();

    void print_reply(int status,const string&rep="");   //打印回复信息

    void SendEnd();

    bool Awake = true;

    CurSocket buildDataConnector(CurSocket Server,vector<string>&cmds);

private:
    enum Status {
        unLogin, IpConnectSucceed, CheckingPsd, Logined
    };
    string clientIp;
    int Retcode;
    int port;
    Status curStatus;
    int lastResponse;
    bool passiveMode = true;
    set<string>dataTransferCmds;
    set<string>totalCmds;
    int sendDataAndResponse(CurSocket sock, const string &cmd, const string &parameter,string &ret);

    int sendDataAndResponse(CurSocket sock, const char *buff, int buffsize,string &ret);

    int Connect(const string &ip);

    int recResponse(CurSocket _socket,string &ret);


};


#endif //GFTP_FTPCLIENT_H
