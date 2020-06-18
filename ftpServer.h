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
    bool checkUser(const string&user,const string &psd,string &wd);
    CurSocket startDataConnection(CurSocket client);
    CurSocket setPassiveMode(CurSocket client, vector<string> &cmds,bool &res,int &po);
    ~ftpServer();
private:
    int CMD_List(CurSocket client,CurSocket dataSocket,const string&path);
    int CMD_Cwd(CurSocket client, CurSocket dataSocket,const string&path,const string &mainPath,string &curPath);
    enum serverStatus{
        unLogin,IpConnectSucceed,CheckingPsd,Logined
    };

    string serverIp;
};
#endif //GFTP_FTPSERVER_H
