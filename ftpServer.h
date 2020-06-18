//
// Created by Administrator on 2020/6/16.
//

#ifndef GFTP_FTPSERVER_H
#define GFTP_FTPSERVER_H
#include "Connector.h"

class ftpServer: public Connector {
public:
    ftpServer(const string &ip, int port);

    void beginListen();

    static void beginProcess(CurSocket client);

    static bool checkUser(const string &user, const string &psd, string &wd);

    static CurSocket setPassiveMode(CurSocket client, vector<string> &cmds, bool &res, int &po);

    ~ftpServer();

private:
    static int CMD_List(CurSocket client, CurSocket dataSocket, const string &path);

    static int
    CMD_Cwd(CurSocket client, CurSocket dataSocket, const string &path, const string &mainPath, string &curPath);

    static int CMD_Retr(CurSocket client, CurSocket dataSocket, const string &path);

    static int CMD_Stor(CurSocket client, CurSocket dataSocket, const string &path);

    static long long CMD_Size(CurSocket client, CurSocket dataSocket, const string &path);

#ifdef WIN32
    static DWORD WINAPI proc(LPVOID lpParamter);
    void BeginThread(CurSocket sClient);
#else
#endif

    enum serverStatus {
        unLogin, IpConnectSucceed, CheckingPsd, Logined
    };
    static string serverIp;
};
#endif //GFTP_FTPSERVER_H
