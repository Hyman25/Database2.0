#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Winsock2.h>
#include <iostream>
#include <string>
#include "Command.h"
using namespace std;
#pragma comment(lib,"ws2_32.lib")

WSADATA wsaData;
SOCKET sockServer;
SOCKADDR_IN addrServer;
SOCKET sockClient;
SOCKADDR_IN addrClient;

void LinkAsServer()
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	sockServer = socket(AF_INET, SOCK_STREAM, 0);
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//INADDR_ANY表示任何IP
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);//绑定端口6000
	bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));

	//  存放主机名的缓冲区
	char szHost[256];
	//  取得本地主机名称
	::gethostname(szHost, 256);
	//  通过主机名得到地址信息，一个主机可能有多个网卡，多个IP地址
	hostent *pHost = ::gethostbyname(szHost);
	in_addr addr;
	for (int i = 0;; i++)
	{
		//获得地址（网络字节序）
		char *p = pHost->h_addr_list[i];
		if (p == NULL)
		{
			break;
		}
		//  将地址拷贝到in_addr结构体中
		memcpy(&addr.S_un.S_addr, p, pHost->h_length);
		//  将in_addr转换为主机字节序
		char *strIp = ::inet_ntoa(addr);
		//  输出
		cout << endl << "————————————————" << endl;
		printf("本机IP[%d]:%s\n", i + 1, strIp);
	}

	cout << "使用端口：6000" << endl;
	cout << "————————————————" << endl;
	

	//Listen监听端
	listen(sockServer, 5);//5为等待连接数目
	printf("服务器已启动:\n监听中...\n");
	int len = sizeof(SOCKADDR);
	char sendBuf[1000];//发送至客户端的字符串
	char recvBuf[1000];//接受客户端返回的字符串
	//会阻塞进程，直到有客户端连接上来为止
	sockClient = accept(sockServer, (SOCKADDR*)&addrClient, &len);
	cout << "————连接客户端————" << endl;
	while (true) 
	{
		//接收并打印客户端数据
		recv(sockClient, recvBuf, 100, 0);
		cout << recvBuf << endl;
		if (memcmp(recvBuf, "exit",sizeof("exit"))==0)
			break;
		string s(recvBuf);
		Command com(s);
		com.operate();
		send(sockClient, "done", 5, 0);
	}
	cout << "————已断开连接————" << endl;
	//关闭socket
	closesocket(sockClient);
	WSACleanup();
}

void LinkAsClient(){
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//新建客户端socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//定义要连接的服务端地址
	std::string s;
	int port;
	std::cout << "输入要连接的Ip地址与端口号" << std::endl;
	std::cin >> s >> port; getchar();
	addrServer.sin_addr.S_un.S_addr = inet_addr(s.c_str());//目标IP(127.0.0.1是回送地址)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);//连接端口6000
	//连接到服务端
	int error=connect(sockClient, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

	if (!error) cout << "Connected!\n";
	else if (error == SOCKET_ERROR) {
		cout << "ERROR!\n";
		return;
	}
	//发送数据
	char message[2000]; //= "HelloSocket!";
	while (true)
	{
		std::cin.getline(message, ';');
		send(sockClient, message, strlen(message) + 1, 0);
		char buf[100];
		if (memcmp(message, "exit", 4) == 0)
		{
			break;
		}
		recv(sockClient, buf, 100, MSG_PEEK);
		std::cout << buf << std::endl;
	}
	//关闭socket
	closesocket(sockClient);
	std::cout << "断开连接。" << std::endl;
	WSACleanup();
}