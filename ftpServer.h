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
    bool checkUser(const string&user,const string &psd);
    CurSocket startDataConnection(CurSocket client);
    CurSocket setPassiveMode(CurSocket client, vector<string> &cmds,bool &res,int &po);
    ~ftpServer();
private:
    int CMD_List(CurSocket client,CurSocket dataSocket,const string&path);
    enum serverStatus{
        unLogin,IpConnectSucceed,CheckingPsd,Logined
    };
    serverStatus curStatus;
    string serverIp;
};
#endif //GFTP_FTPSERVER_H
