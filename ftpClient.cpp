//
// Created by Administrator on 2020/6/16.
//

#include "ftpClient.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <csignal>
#include <list>
list<ftpClient*>Awaking;
void sighandler(int s){
    switch (s){
        case SIGINT:{
            for(auto it :Awaking) {
                it->SendEnd();
            }
            exit(0);
        }
            break;
    }
}
ftpClient::ftpClient(int port):port(port) {
    curStatus = unLogin;
    Awaking.push_back(this);
}

int ftpClient::Connect(const string &ip) {
    int k = SocketConnect(ip,port);
    if(k==-1){
        return 0;
    }
    while (recv_data(_socket, (char *) &Retcode, maxSize) <= 0);
    print_reply(*(int *) (&Retcode));
    return 1;
}
ftpClient::~ftpClient() {
    if(_socket!=-1){
        SendEnd();
    }
}

void ftpClient::beginProcess() {
    string cmd,sig;
    cout<<"ftp<";
    while (true) {
        getline(cin,cmd);
        stringstream sin(cmd);
        vector<string>cmds;
        while (sin>>sig)cmds.push_back(sig);
        if(cmds.empty())continue;
        int cur = 0;
        if(cmds[0]=="bye" || cmds[0]=="quit") {
            /// tell server close
            sendDataAndResponse(_socket, "QUIT",cmds[1]);
            break;
        }

        switch (curStatus) {
            case unLogin: {
                if (cmds[0] == "open")
                    if (Connect(cmds[1])) {
                        curStatus = IpConnectSucceed;
                        cout << "Name (" << cmds[1] << "):";
                    } else {

                    }
            }break;
            case IpConnectSucceed:{
                send_data(_socket,"user");
                if(lastResponse!=220){
                    if(cmds[0]=="user"){
                        switch (cmds.size()){
                            case 2: {
                                sendDataAndResponse(_socket, "USER",cmds[1]);
                                if (lastResponse == 331) {
                                    cout << "Passwords:";
                                    curStatus = CheckingPsd;
                                }
                            }break;

                            case 3: {
                                sendDataAndResponse(_socket,"USER", cmds[1]);
                                string code = Encode(cmds[2]);
                                sendDataAndResponse(_socket,"PASS", code);
                                if (lastResponse == 230) {
                                    curStatus = Logined;
                                } else {
                                    curStatus = IpConnectSucceed;
                                }
                            }break;
                            default:
                                cout<<"usage: user username [password]\n";
                                break;
                        }
                    }
                }else {///login no toki hajime de username
                    sendDataAndResponse(_socket, "USER",cmds[0]);
                    if(lastResponse==331) {
                        cout << "Passwords:";
                        curStatus = CheckingPsd;
                    }
                }
            }break;
            case CheckingPsd:{
                string code = Encode(cmd);
                sendDataAndResponse(_socket,"PASS", code);
                if(lastResponse==230){
                    curStatus = Logined;
                }else{
                    curStatus = IpConnectSucceed;
                }
            }break;
            case Logined:{
                if(cmds.size()==1) cmds.emplace_back(".");
                if(cmds[0]=="ls"){
                    sendDataAndResponse(_socket,"LIST",cmds[1]);
                }
            }break;
        }
    }
    CurClose(_socket);
    _socket = -1;
}
int ftpClient::sendDataAndResponse(CurSocket sock, const string&cmd,const string&parameter){
    string data = cmd+" "+parameter;
    sendDataAndResponse(sock,data.c_str(),data.size());
}
int ftpClient::sendDataAndResponse(CurSocket sock, const char *buff, int buffsize) {
    int k = send_data(sock,buff,buffsize);

    if(k>0){
        while (recv_data(_socket,(char*)&Retcode,maxSize)<=0);
        print_reply(*(int*)(&Retcode));
    }
}
void ftpClient::SendEnd() {
    sendDataAndResponse(_socket,"QUIT","");
    CurClose(_socket);
    _socket = -1;
}
void ftpClient::print_reply(int status) {
    status= ntohl(status);
    lastResponse = status;
    switch(status)
    {
        case 200:
            printf("%d Command okay.",status);
            break;
        case 220:
            printf("220 welcome,server ready.\n");
            break;
        case 221:
            printf("221 bye!\n");
            break;
        case 226:
            printf("226 close data connection. requested fileaction successful.\n");
            break;
        case 230:
            printf("%d User logged in, proceed. Logged out if appropriate.",status);
            break;
        case 331:
            printf("331 Please specify the password.\n");
            break;
        case 502:
            printf("502 command execute failed.\n");
            break;
        case 530:
            printf("530 Not logged in.\n");
            break;
        case 550:
            printf("550 request action not taken.file unavailable.\n");
            break;
        case 553:
            printf("553 Could not file.\n");
            break;
        default:
            printf("%d unknown error.\n",status);
            break;
    }
}