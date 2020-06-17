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
    dataTransferCmds.insert("ls");
    dataTransferCmds.insert("get");
    dataTransferCmds.insert("put");
}

int ftpClient::Connect(const string &ip) {
    int k = SocketConnect(ip,port);
    if(k==-1){
        return 0;
    }
    string s;
    recResponse(_socket,s);
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
    string ret;
    CurSocket DataSocket = -1;
    while (true) {
        getline(cin,cmd);
        stringstream sin(cmd);
        vector<string>cmds;
        while (sin>>sig)cmds.push_back(sig);
        if(cmds.empty())continue;
        int cur = 0;
        if(cmds[0]=="bye" || cmds[0]=="quit") {
            /// tell server close
            SendEnd();
            return;
        }

        if(curStatus==Logined){
            if(dataTransferCmds.count(cmds[0])) {
                vector<string> CMDS;
                CMDS.emplace_back((passiveMode ? "PASV" : "PORT"));
                if (!passiveMode) {/// port

                    CMDS.emplace_back("ima no ip");/// ip to port info
                } else {
                    CMDS.emplace_back("");
                }


                DataSocket = buildDataConnector(_socket, CMDS);
            }
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
                if(lastResponse!=220){
                    if(cmds[0]=="user"){
                        switch (cmds.size()){
                            case 2: {
                                sendDataAndResponse(_socket, "USER",cmds[1],ret);
                                if (lastResponse == 331) {
                                    cout << "Passwords:";
                                    curStatus = CheckingPsd;
                                }
                            }break;

                            case 3: {
                                sendDataAndResponse(_socket,"USER", cmds[1],ret);
                                string code = Encode(cmds[2]);
                                sendDataAndResponse(_socket,"PASS", code,ret);
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
                    sendDataAndResponse(_socket, "USER",cmds[0],ret);
                    if(lastResponse==331) {
                        cout << "Passwords:";
                        curStatus = CheckingPsd;
                    }
                }
            }break;
            case CheckingPsd:{
                string code = Encode(cmd);
                sendDataAndResponse(_socket,"PASS", code,ret);
                if(lastResponse==230){
                    curStatus = Logined;
                }else{
                    curStatus = IpConnectSucceed;
                }
            }break;
            case Logined:{
                if(cmd=="passive"){
                    passiveMode = !passiveMode;
                    cout<<"passive mode "<<(passiveMode?"On":"Off")<<"\n";
                }else if(cmds[0]=="ls"){
                    if (cmds.size() == 1) cmds.emplace_back(".");
                    sendDataAndResponse(_socket, "LIST", cmds[1], ret);
                    if(passiveMode) {
                        int length;
                        char *s = recvBigData(DataSocket, length);
                        cout << s << endl;
                        recResponse(_socket, ret);///recive 226
                    }else{

                    }
                }else if(cmds[0]=="get"){

                }else if(cmds[0]=="put"){

                }
            }break;
        }
    }
}

CurSocket ftpClient::buildDataConnector(CurSocket Server,vector<string>&cmds){
    CurSocket dataSocket= -1;

    if(passiveMode){
        string ret;
        int k = sendDataAndResponse(Server,"PASV",cmds[1],ret);
         dataSocket= SocketConnect(ret,false);
    }else{
        string ret;
        dataSocket = CreateSocket(cmds[1],false);
        if(dataSocket!=-1) {

            int k = sendDataAndResponse(Server, "PORT", cmds[1], ret);

        }
    }
    return dataSocket;
}


int ftpClient::sendDataAndResponse(CurSocket sock, const string&cmd,const string&parameter, string &ret){
    string data = cmd+" "+parameter;
    return sendDataAndResponse(sock,data.c_str(),data.size(),ret);
}

int ftpClient::recResponse(CurSocket sock,string &ret){
    char buffrecv[maxSize];
    while (recv_data(sock, buffrecv, sizeof(buffrecv)) <= 0);
    Retcode = *(int*)buffrecv;
    ret = buffrecv + sizeof(Retcode);
    Retcode= ntohl(Retcode);
    print_reply(Retcode,ret);
    return Retcode;
}

int ftpClient::sendDataAndResponse(CurSocket sock, const char *buff, int buffsize,string &ret) {
    int k = send_data(sock, buff, buffsize);
    if (k > 0) {
        recResponse(sock,ret);
    }
    return k;
}
void ftpClient::SendEnd() {
    string s;
    sendDataAndResponse(_socket,"QUIT","",s);
    CurClose(_socket);
    _socket = -1;
}
void ftpClient::print_reply(int status,const string&rep) {

    lastResponse = status;

    switch(status)
    {
        case 150:
            printf("%d Opening data connection.",status);
            break;
        case 200:
            printf("%d Command %s okay.",status,rep.c_str());
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
        case 227:{
            printf("%d Entering Passive Mode.(%s).\n",status,rep.c_str());
        }break;
        case 230:
            printf("%d User logged in, proceed. Logged out if appropriate.",status);
            break;
        case 331:
            printf("331 Please specify the password.\n");
            break;
        case 425:{
            printf("%d Cannot open passive connection.\n",status);
        }break;
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
CurSocket ftpClient::openDataConnector(CurSocket sock_ctl){
    CurSocket sock_listen=CreateSocket("0.0.0.0",DATAPORT,false);   //创建一个数据连接
    int ack=1;
    //给服务器发送一个确认，告诉服务器客户端创建好了一条数据链路
    if(send(sock_ctl,(char*)&ack,sizeof(ack),0)<0)
    {
        printf("client:ack write error:%d\n",errno);
        return -1;
    }
    int sock_data=SocketAccept(sock_listen);
    CurClose(sock_listen);
    return sock_data;
}