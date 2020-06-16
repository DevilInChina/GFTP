//
// Created by Administrator on 2020/6/16.
//

#include "ftpServer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
ftpServer::ftpServer(const string&ip,int port):Connector(){
    CreateSocket(ip,port);
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
void ftpServer::beginProcess(CurSocket client) {
    char revData[maxSize];
    send_response(client,220);
    curStatus = IpConnectSucceed;
    string user,psd;
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

            }break;
        }



    }
    CurClose(client);
}

int ftpServer::login(CurSocket client) {

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
ftpServer::~ftpServer() {}