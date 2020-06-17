//
// Created by Administrator on 2020/6/16.
//

#include "ftpServer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
#include <random>
#define GETRAND(x) (rand()%(x))
ftpServer::ftpServer(const string&ip,int port):Connector(){
    CreateSocket(ip,port);

    this->serverIp = ip;
    curStatus = unLogin;
}
void ftpServer::beginListen() {
    CurSocket sClient;
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);

    while (true){
        sClient = accept(_socket, (SOCKADDR *)&remoteAddr, (socklen_t*)&nAddrlen);
        if(sClient == INVALID_SOCKET)
        {
            continue;
        }else{
            beginProcess(sClient);////Thread or Fork
        }
    }
}
string dealIp(const string &ip){
    string s = ip;
    replace(s.begin(),s.end(),'.',',');
    return s;
}
void ftpServer::beginProcess(CurSocket client) {
    char revData[maxSize];
    send_response(client,220);
    curStatus = IpConnectSucceed;
    string user,psd;
    int dataPort;
    bool passiveMode = true;
    while (true) {
        int c = recv_data(client,revData,maxSize);
        if(c<=0)continue;
        string s = revData,stemp;
        vector<string>cmds;
        stringstream sin(s);
        while (sin>>stemp)cmds.push_back(stemp);
        if(cmds[0]=="QUIT"){
            send_response(client, 200);
            break;
        }
        cout<<curStatus<<" "<<s<<endl;
        CurSocket dataSocket;
        switch (curStatus) {
            case IpConnectSucceed: {
                if(cmds[0]=="USER") {
                    user = cmds[1];
                    curStatus = CheckingPsd;
                    send_response(client,331);
                }
            }break;
            case CheckingPsd:{
                if(cmds[0]=="PASS") {
                    psd = cmds[1];
                    if(checkUser(user,psd)){
                        send_response(client,230);
                        curStatus = Logined;
                    }else{
                        send_response(client,530);
                        curStatus = IpConnectSucceed;
                    }
                }
            }break;
            case Logined:{
                if(cmds[0]=="PASV" || cmds[0]=="PORT"){
                    cout<<cmds[0]<<endl;
                    CurSocket sender = setPassiveMode(client,cmds,passiveMode,dataPort);
                    if(passiveMode) {/// pasv
                        if (sender != -1) {
                            string ret = dealIp(serverIp) + "," + std::to_string(dataPort / 256) + "," +
                                         std::to_string(dataPort % 256);
                            send_response(client, 227, ret);
                            dataSocket= SocketAccept(sender);
                            CurClose(sender);
                        } else {
                            dataSocket = -1;
                            send_response(client, 425);
                        }
                    }else{//// port
                        if (sender != -1) {
                            send_response(client, 200, "PORT");
                            CurClose(sender);
                        } else {
                            send_response(client, 552);
                        }
                    }
                }
                else if(cmds[0]=="LIST"){
                    if(passiveMode) {
                        send_response(client, 150);
                        CMD_List(client, dataSocket, cmds[1]);
                        CurClose(dataSocket);
                        dataSocket = -1;
                        send_response(client, 226);
                    }
                }
            }break;
        }
    }
    CurClose(client);
}

CurSocket ftpServer::setPassiveMode(CurSocket client, vector<string>&cmds,bool &res,int &port) {
    CurSocket dataSocket = -1;
    if (cmds[0] == "PASV") {
        int po = (GETRAND(65535)) + 1024;
        po = min(65534, po);
        res = true;
        dataSocket = CreateSocket(serverIp, po, false);

        port = po;
    } else if (cmds[0] == "PORT") {
        res = false;
         dataSocket = SocketConnect(cmds[1], false);
    }
    return dataSocket;
}

bool ftpServer::checkUser(const string &user, const string &psd) {
    ifstream fin("data.txt");
    string username,password;
    while (fin>>username>>password){
        if(username==user){
            if(Encode(password)==psd){
                return true;
            }
        }
    }
    return false;
}

CurSocket ftpServer::startDataConnection(CurSocket client)
{
    char buf[1024];
    struct sockaddr_in client_addr;
    socklen_t len=sizeof(client_addr);
    getpeername(client,(struct sockaddr*)&client_addr,&len);
#ifdef WIN32
    char *ip = inet_ntoa(client_addr.sin_addr);
    CurSocket sock_data=SocketConnect(ip,client_addr.sin_port, false);
    free(ip);
#else
    inet_ntop(AF_INET,&client_addr.sin_addr,buf,sizeof(buf));
    CurSocket sock_data=SocketConnect(buf,client_addr.sin_port, false);
#endif
    if(sock_data<0)
    {
        return -1;
    }
    return sock_data;
}

ftpServer::~ftpServer() {}

int ftpServer::CMD_List(CurSocket client, CurSocket dataSocket,const string&path) {
    string CMDS;
    string del;
#ifdef WIN32
    CMDS="dir ";
    del = "del ";
#else
    CMDs="ls ";
    del = "rm ";
#endif
    string fileName = ".LIST"+to_string(client+dataSocket);
    string ClearCmd = del+fileName;
    CMDS+=path;
    CMDS+=" >> "+fileName;
    system(CMDS.c_str());
    ifstream ff(fileName);
    string res;
#ifdef WIN32
    string temp;
    while (getline(ff,temp)) {
        if (!isdigit(temp[0]))continue;///not a file in windows
        stringstream ss(temp);
        for (int i = 0; i < 5; ++i) {
            ss >> temp;
        }
        if (temp != "." && temp != ".." && temp != fileName && !temp.empty())
            res += temp + "\n";
    }
#else
    while (getline(ff,temp)) {
        if (temp != "." && temp != ".." && temp != fileName)
            res += temp + "\n";
    }
#endif
    cout<<res<<endl;
    sendBigData(dataSocket,res.c_str(),res.length());
    ff.close();
    system(ClearCmd.c_str());
    return 1;
}