//
// Created by Administrator on 2020/6/16.
//

#include "ftpServer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <ctime>
ftpServer::ftpServer(const string&ip,int port):Connector(){
    CreateSocket(ip,port);

    this->serverIp = ip;
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
    struct sockaddr_in sa;
    int iplen = sizeof(sa);
    string clientIP ;
    if(!getpeername(client, (struct sockaddr *)&sa, (socklen_t*)&iplen)) {
        clientIP = inet_ntoa(sa.sin_addr);
    }

    send_response(client,220,clientIP);
    serverStatus curStatus = IpConnectSucceed;
    string user,psd,workingPath,curPath;

    int dataPort;
    bool passiveMode = true;
    CurSocket dataSocketInProcess = INVALID_SOCKET;
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
                    if(checkUser(user,psd,workingPath)){
                        send_response(client,230);
                        curStatus = Logined;
                        curPath = workingPath;
                    }else{
                        send_response(client,530);
                        curStatus = IpConnectSucceed;
                    }
                }
            }break;
            case Logined:{
                if(cmds[0]=="PASV" || cmds[0]=="PORT"){
                    CurSocket sender = setPassiveMode(client,cmds,passiveMode,dataPort);
                    if(passiveMode) {/// pasv
                        if (sender != INVALID_SOCKET ) {
                            string ret = dealIp(serverIp) + "," + std::to_string(dataPort / 256) + "," +
                                         std::to_string(dataPort % 256);
                            send_response(client, 227, ret);
                            dataSocketInProcess = SocketAccept(sender);
                            CurClose(sender);
                        } else {
                            send_response(client, 425,"Cannot open data connection");
                        }
                    }else{//// port
                        if (sender != INVALID_SOCKET) {
                            send_response(client, 200, "PORT");
                            dataSocketInProcess = sender;
                        } else {
                            send_response(client, 552);
                        }
                    }
                }
                else if(cmds[0]=="LIST") {
                    if(dataSocketInProcess != INVALID_SOCKET) {
                        send_response(client, 150);
                        CMD_List(client, dataSocketInProcess, curPath + FILESEPERATOR + cmds[1]) ;

                        CurClose(dataSocketInProcess);
                        dataSocketInProcess = INVALID_SOCKET;
                    }else{
                        send_response(client, 426,"Data connection error");
                    }
                }else if(cmds[0]=="RETR"){
                    if(dataSocketInProcess != INVALID_SOCKET) {
                        CMD_Retr(client, dataSocketInProcess, curPath + FILESEPERATOR + cmds[1]) ;
                        CurClose(dataSocketInProcess);
                        dataSocketInProcess = INVALID_SOCKET;
                    }else{
                        send_response(client, 426,"Data connection error");
                    }
                }else if(cmds[0]=="STOR"){
                    if(dataSocketInProcess != INVALID_SOCKET) {
                        CMD_Stor(client, dataSocketInProcess, curPath + FILESEPERATOR + cmds[1]) ;
                        CurClose(dataSocketInProcess);
                        dataSocketInProcess = INVALID_SOCKET;
                    }else{
                        send_response(client, 426,"Data connection error");
                    }
                }else if(cmds[0]=="CWD"){
                    CMD_Cwd(client, dataSocketInProcess, cmds[1],workingPath,curPath);
                }else if(cmds[0]=="SIZE"){
                    CMD_Size(client,dataSocketInProcess,curPath+FILESEPERATOR+cmds[1]);
                }
            }break;
        }
    }
    CurClose(client);
}

CurSocket ftpServer::setPassiveMode(CurSocket client, vector<string>&cmds,bool &res,int &port) {
    CurSocket dataSocket = INVALID_SOCKET;
    if (cmds[0] == "PASV") {
        int po = getRandomPort();
        res = true;
        dataSocket = CreateSocket(serverIp, po, false);
        port = po;
    } else if (cmds[0] == "PORT") {
        res = false;
        dataSocket = SocketConnect(cmds[1], false);
    }
    return dataSocket;
}

bool ftpServer::checkUser(const string &user, const string &psd,string &wd) {
    ifstream fin(AUTH);
    string username,password;
    while (fin>>username>>password>>wd){
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
    return sock_data;
}

ftpServer::~ftpServer() {}

int ftpServer::CMD_List(CurSocket client, CurSocket dataSocket,const string&path) {
    string CMDS;
    string del;
    cout<<path<<endl;
#ifdef WIN32
    CMDS = "dir ";
    del = "del ";
#else
    CMDS="ls ";
    del = "rm ";
#endif
    string fileName = ".LIST" + to_string(client + dataSocket);
    string ClearCmd = del + fileName;
    CMDS += path;
    CMDS += " >> " + fileName;
    int k = system(CMDS.c_str());
    if(k==CMD_SUCCEED) {
        ifstream ff(fileName);
        string res;
        string temp;
#ifdef WIN32
        while (getline(ff, temp)) {
            if (!isdigit(temp[0]))continue;///not a file in windows
            stringstream ss(temp);
            for (int i = 0; i < 5; ++i) {
                ss >> temp;
            }
            if (temp != "." && temp != ".." && temp != fileName && !temp.empty() && temp != AUTH)
                res += temp + "\n";
        }
#else
        while (getline(ff,temp)) {
            if (temp != "." && temp != ".." && temp != fileName && !temp.empty() && temp!=AUTH)
                res += temp + "\n";
        }
#endif

        sendBigData(dataSocket, res.c_str(), res.length());
        send_response(client, 226);
        ff.close();
        system(ClearCmd.c_str());
        return 1;
    }else{
        sendBigData(dataSocket, nullptr, 0);
        send_response(client, 551,"File listing failed");
        system(ClearCmd.c_str());
        return 0;
    }
}
void trimDic(string &curPath){
    stringstream s(curPath);
    string f;
#ifdef WIN32
    getline(s,f,FILESEPERATOR);
#else
#endif
    string temp;
    vector<string>ss;
    while (getline(s,temp,FILESEPERATOR)){
        if(temp=="..") {
            if(!ss.empty()){
                ss.pop_back();
            }
        }else{
            if(temp!="." && !temp.empty()) {
                ss.push_back(temp);
            }
        }
    }
    for(auto const &it:ss){
        f+=FILESEPERATOR+it;
    }
    curPath = f;
}

int ftpServer::CMD_Cwd(CurSocket client, CurSocket dataSocket,const string&path,const string &mainPath,string &curPath) {
    string CMDS;
    string del;
#ifdef WIN32
    CMDS = "cd ";
    del = "del ";
#else
    CMDS="cd ";
    del = "rm ";
#endif
    if(path==".") {
        send_response(client, 250, "CWD COMMAND OK");
        return true;
    }else if (path.size()==1 && path[0]==FILESEPERATOR){
        curPath=mainPath;
        send_response(client, 250, "CWD COMMAND OK");
        return true;
    }
    CMDS+=curPath+FILESEPERATOR+path;
    int k = system(CMDS.c_str());
    cout<<k<<endl;
    if(k==CMD_SUCCEED) {
        curPath = curPath+FILESEPERATOR+path;
        trimDic(curPath);
        send_response(client, 250, "CWD COMMAND OK");
    }else{
        send_response(client, 550, "No such dic");
        return false;
    }
    return true;
}

long long ftpServer::CMD_Size(CurSocket client, CurSocket dataSocket,const string&path){
    long long ret = getFileSize(path,client+dataSocket);
    if(ret==-1){
        send_response(client,550,"Is a directory");
    }else if(ret==-2){
        send_response(client,550,"No such file");
    }else{
        send_response(client,213,to_string(ret));
    }
    return ret;
}
int ftpServer::CMD_Retr(CurSocket client, CurSocket dataSocket, const string &path) {
    long long ret = getFileSize(path,client+dataSocket);
    if(ret==-1){
        send_response(client,550,"Is a directory");
    }else if(ret==-2){
        send_response(client,550,"No such file");
    }else{
        send_response(client,150);
        sendFile(dataSocket,path,client+dataSocket);
    }

    send_response(client,226);
    return 1;
}
int ftpServer::CMD_Stor(CurSocket client, CurSocket dataSocket, const string &path) {
    long long ret = getFileSize(path,client+dataSocket);
    if(ret==-1){
        send_response(client,550,"Is a directory");
    }else if(ret==-2){///no file and not directory
        send_response(client,150);
        recvFile(dataSocket,path,client+dataSocket);
    }else{
        send_response(client,550,"File Exists");
    }

    send_response(client,226);
    return 1;
}
