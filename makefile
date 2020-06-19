cc=g++ 
MingwLib=D:\\MinGW\\lib\\
WIN_LIB=${MingwLib}libws2_32.a #${MingwLib}libwsock32.a
linux:
	${cc} *.cpp -o Myftp -lpthread
windows:
	${cc} *.cpp -o Myftp ${WIN_LIB} -g
	.\Myftp.exe 192.168.2.100
