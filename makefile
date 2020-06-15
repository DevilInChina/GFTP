cc=g++
MingwLib=D:\\MinGW\\lib\\
linux:
	${cc} main.cpp Connector.cpp -o main
windows:
	${cc} main.cpp Connector.cpp -o main ${MingwLib}libws2_32.a ${MingwLib}libwsock32.a

