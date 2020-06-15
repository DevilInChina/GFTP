//
// Created by Administrator on 2020/6/15.
//

#ifndef GFTP_CONNNECTOR_H
#define GFTP_CONNNECTOR_H

#include <string>
using namespace std;
class Connnector {
private:
    int _socket;
public:
    bool CreateSocket(string ip, int port);

};


#endif //GFTP_CONNNECTOR_H
