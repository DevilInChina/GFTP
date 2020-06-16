//
// Created by Administrator on 2020/6/16.
//

#ifndef GFTP_FTPSERVER_H
#define GFTP_FTPSERVER_H

#include "Connector.h"

class ftpServer: public Connector{
public:
    ftpServer(const string &ip,int port);
    void beginListen();
    void beginProcess(CurSocket client);
    int login(CurSocket client);
    bool checkUser(const string&user,const string &psd);
    ~ftpServer();
private:
    enum serverStatus{
        unLogin,IpConnectSucceed,CheckingPsd,Logined
    };
    serverStatus curStatus;
};
#endif //GFTP_FTPSERVER_H
