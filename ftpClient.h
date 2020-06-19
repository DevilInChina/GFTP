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

    /**
     * @brief 根据 passiveMode 状态建立主动/被动连接，同时通知服务器做好响应连接的准备
     * @param Server
     * @param cmds
     * @return
     */
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
    /**
     * @brief 发送一条命令并且接收一条回复，更新lastResponse的状态
     * @param sock
     * @param cmd
     * @param parameter
     * @param ret 从服务器端返回的字符串
     * @return
     */
    int sendDataAndResponse(CurSocket sock, const string &cmd, const string &parameter,string &ret);

    int sendDataAndResponse(CurSocket sock, const char *buff, int buffsize,string &ret);

    int Connect(const string &ip);

    /**
     * @brief 接收来自服务器的返回值和信息，未接收前处于阻塞状态。
     * @param _sock
     * @param ret
     * @return
     */
    int recResponse(CurSocket _sock,string &ret);


};


#endif //GFTP_FTPCLIENT_H
