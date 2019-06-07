#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <Winsock2.h>
#include <iostream>
#include <string>
#include <fstream>
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
	bind(sockServer, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

	//  存放主机名的缓冲区
	char szHost[256];
	//  取得本地主机名称
	::gethostname(szHost, 256);
	//  通过主机名得到地址信息，一个主机可能有多个网卡，多个IP地址
	hostent* pHost = ::gethostbyname(szHost);
	in_addr addr;
	for (int i = 0;; i++)
	{
		//获得地址（网络字节序）
		char* p = pHost->h_addr_list[i];
		if (p == NULL)
		{
			break;
		}
		//  将地址拷贝到in_addr结构体中
		memcpy(&addr.S_un.S_addr, p, pHost->h_length);
		//  将in_addr转换为主机字节序
		char* strIp = ::inet_ntoa(addr);
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
	char sendBuf[1000] = {};//发送至客户端的字符串
	char recvBuf[1000] = {};//接受客户端返回的字符串
	//会阻塞进程，直到有客户端连接上来为止
	sockClient = accept(sockServer, (SOCKADDR*)& addrClient, &len);
	cout << "————连接客户端————" << endl;

	while (true)
	{
		//接收并打印客户端数据
		recv(sockClient, recvBuf, 1000, 0);
		cout << recvBuf << endl;
		if (memcmp(recvBuf, "exit", sizeof("exit")) == 0)
			break;
		streambuf* coutBuf = cout.rdbuf();

		ofstream of("out.txt");
		// 获取文件out.txt流缓冲区指针
		streambuf* fileBuf = of.rdbuf();

		// 设置cout流缓冲区指针为out.txt的流缓冲区指针
		cout.rdbuf(fileBuf);
		string s(recvBuf);
		Command com(s);
		com.operate();
		of.flush();
		of.close();
		cout.rdbuf(coutBuf);

		ifstream fin;
		fin.open("out.txt");
		string tmp, contents;
		int i = 0;
		while (!fin.eof())
		{
			getline(fin, tmp);
			if (!tmp.empty()) {
				cout << tmp << endl;
				contents += tmp + "\n";
			}
		}
		fin.close();

		send(sockClient, contents.c_str(), contents.size() + 1, 0);
	}
	cout << "————已断开连接————" << endl;
	//关闭socket
	closesocket(sockClient);
	WSACleanup();
}

void LinkAsClient() {
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//新建客户端socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//定义要连接的服务端地址
	std::string s;
	int port;
	cout << "————连接服务器————" << endl;
	std::cout << "输入要连接的Ip地址与端口号" << std::endl;
	std::cin >> s >> port; getchar();
	addrServer.sin_addr.S_un.S_addr = inet_addr(s.c_str());//目标IP(127.0.0.1是回送地址)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);//连接端口6000
	//连接到服务端
	int error = connect(sockClient, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

	if (!error) cout << "Connected!\n";
	else if (error == SOCKET_ERROR) {
		cout << "ERROR!\n";
		return;
	}
	cout << "————————————————" << endl;
	//发送数据
	string message;
	while (true)
	{
		getline(cin, message, ';');
		char* tmp = const_cast<char*>(message.c_str());
		send(sockClient, message.c_str(), message.size() + 1, 0);
		char buf[1000] = {};
		if (memcmp(tmp, "exit", 4) == 0)
		{
			break;
		}
		error = recv(sockClient, buf, 1000, 0);

		if (error == SOCKET_ERROR) cout << "ERROR!" << endl;
		else {
			cout << message << " Done!" << endl;
			cout << buf; //<< std::endl;
		}
	}
	//关闭socket
	closesocket(sockClient);
	cout << "————已断开连接————" << endl;
	WSACleanup();
}