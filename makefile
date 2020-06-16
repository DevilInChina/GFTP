cc=g++
MingwLib=D:\\MinGW\\lib\\
WIN_LIB=${MingwLib}libws2_32.a ${MingwLib}libwsock32.a
linux:
	${cc} main.cpp Connector.cpp -o main
windows:
	${cc} main.cpp Connector.cpp -o main $WIN_LIB

