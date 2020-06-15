#include <iostream>
#include "Connnector.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    Connnector s;
    s.CreateSocket("127.0.0.1",1453);
    return 0;
}
