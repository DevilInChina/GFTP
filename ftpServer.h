//
// Created by Administrator on 2020/6/16.
//

#ifndef GFTP_FTPSERVER_H
#define GFTP_FTPSERVER_H
#include "Connector.h"

class ftpServer: public Connector {
public:
    /**
     * @brief 创建一个ip，port的socket，并且绑定ip(invoke CreateSocket)
     * @param ip 随参数传入，同时设置serverIp(static)
     * @param port 默认情况为 INFOPORT
     */
    ftpServer(const string &ip, int port);

    /**
     * @brief 在while(true)中进行监听，每次开辟线程（windows）/进程（linux）进行处理
     */
    void beginListen();

    /**
     * @brief 进行密码验证，传入的密码是加密后的密码。本地保存的密码为明码。
     * @param user
     * @param psd
     * @param wd working_directory 验证成功返回保存在文件中的工作目录。
     * @return 是否验证成功
     */
    static bool checkUser(const string &user, const string &psd, string &wd);

    /**
     * @brief 设置主动/被动模式，每次进行数据传输前被调用。
     * @param client
     * @param cmds
     * @param res
     * @param po
     * @return
     */
    static CurSocket setPassiveMode(CurSocket client, vector<string> &cmds, bool &res, int &po);

    /**
     * @breif 根据系统开辟线程或进程进行处理
     * @param sClient
     */
    void BeginThread(CurSocket sClient);

    ~ftpServer();

private:
    static int CMD_List(CurSocket client, CurSocket dataSocket, const string &path);

    static int
    CMD_Cwd(CurSocket client, CurSocket dataSocket, const string &path, const string &mainPath, string &curPath);

    static int CMD_Retr(CurSocket client, CurSocket dataSocket, const string &path);

    static int CMD_Stor(CurSocket client, CurSocket dataSocket, const string &path);

    static long long CMD_Size(CurSocket client, CurSocket dataSocket, const string &path);

    static int CMD_Dele(CurSocket client, CurSocket dataSocket, const string &path);
    /**
        * @brief 和客户端交互的主要程序
        * @param client
        */
    static void beginProcess(CurSocket client);


#ifdef WIN32
    static DWORD WINAPI proc(LPVOID lpParamter);
#else
#endif

    enum serverStatus {
        unLogin, IpConnectSucceed, CheckingPsd, Logined
    };
    static string serverIp;
};
#endif //GFTP_FTPSERVER_H
