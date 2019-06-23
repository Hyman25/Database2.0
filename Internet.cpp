#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "Command.h"
using namespace std;

#ifdef _WIN32
#include <Winsock2.h>
#pragma comment(lib,"ws2_32.lib")

void LinkAsServer()
{
	WSADATA wsaData;
	SOCKET sockServer, sockClient;
	SOCKADDR_IN addrServer, addrClient;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
		cout << "————————————————" << endl;
		printf("本机IP[%d]:%s\n", i + 1, strIp);
	}

	cout << "使用端口：6000" << endl;
	cout << "————————————————" << endl;

	//Listen监听端
	listen(sockServer, SOMAXCONN);
	printf("服务器已启动:\n监听中...\n");

	//会阻塞进程，直到有客户端连接上来为止
	int len = sizeof(SOCKADDR);
	sockClient = accept(sockServer, (SOCKADDR*)& addrClient, &len);
	char recvBuf[1000] = {};//接受客户端返回的字符串

	cout << "————连接客户端————" << endl;

	while (true)
	{
		//接收并打印客户端数据
		if (recv(sockClient, recvBuf, 1000, 0) != SOCKET_ERROR)
			break;

		string buf(recvBuf);
		regex reg("\n");//不知道为什么有时候exit前面会有\n
		buf = regex_replace(buf, reg, "");

		cout << buf << endl;
		if (toUpper(buf) == "EXIT")
			break;

		streambuf* coutBuf = cout.rdbuf();
		ofstream of("out.txt");
		streambuf* fileBuf = of.rdbuf();
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
	closesocket(sockServer);
	WSACleanup();
}

void LinkAsClient() {
	WSADATA wsaData;
	SOCKADDR_IN addrServer;
	SOCKET sockClient;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//新建客户端socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//定义要连接的服务端地址
	std::string s;
	int port;
	cout << "————————————————" << endl;
	std::cout << "输入要连接的Ip地址与端口号：" << std::endl;
	std::cin >> s >> port; getchar();
	addrServer.sin_addr.S_un.S_addr = inet_addr(s.c_str());//目标IP(127.0.0.1是回送地址)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);//连接端口
	//连接到服务端
	int error = connect(sockClient, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

	if (error == SOCKET_ERROR) {
		cout << "LINK ERROR!断开连接!\n";
		return;
	}

	cout << "Connected!\n";

	cout << "————连接服务器————" << endl;
	//发送数据
	string message;
	while (true)
	{
		getline(cin, message, ';');
		send(sockClient, message.c_str(), message.size() + 1, 0);

		regex reg("\n");//不知道为什么有时候exit前面会有\n
		message = regex_replace(message, reg, "");
		if (toUpper(message) == "EXIT") break;

		char buf[1000] = {};
		error = recv(sockClient, buf, 1000, 0);

		if (error == SOCKET_ERROR) {
			cout << "ERROR!" << endl;
			break;
		}
		else {
			cout << message << " Done!" << endl;
			cout << buf; //<< std::endl;
		}

	}
	//关闭socket
	cout << "————已断开连接————" << endl;
	closesocket(sockClient);
	WSACleanup();
}

#elif  __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_MAC
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int LinkAsServer()
{
	int sockServer, sockClient;
	struct sockaddr_in addrServer, addrClient;
	memset(&addrServer, 0, sizeof(addrServer));

	sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	addrServer.sin_family = AF_INET;//IPV4
	addrServer.sin_addr.s_addr = inet_addr("127.0.0.1");
	addrServer.sin_port = htons(6000);//绑定端口6000	

	bind(sockServer, (struct sockaddr_in*)& addrServer, sizeof(struct sockaddr_in));

	cout << "————————————————" << endl;
	printf("本机IP:%s\n", "127.0.0.1");
	cout << "使用端口：6000" << endl;
	cout << "————————————————" << endl;

	//Listen监听端
	listen(sockServer, SOMAXCONN);
	printf("服务器已启动:\n监听中...\n");

	//会阻塞进程，直到有客户端连接上来为止
	auto len = sizeof(addrClient);
	sockClient = accept(sockServer, (struct sockaddr*)& addrClient, (socklen_t*)&len);
	char recvBuf[1000] = {};//接受客户端返回的字符串

	cout << "————连接客户端————" << endl;

	while (true)
	{
		//接收并打印客户端数据
		if (recv(sockClient, recvBuf, 1000, 0) != -1)
			break;

		string buf(recvBuf);
		regex reg("\n");//不知道为什么有时候exit前面会有\n
		buf = regex_replace(buf, reg, "");

		cout << buf << endl;
		if (toUpper(buf) == "EXIT")
			break;

		streambuf* coutBuf = cout.rdbuf();
		ofstream of("out.txt");
		streambuf* fileBuf = of.rdbuf();
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
	//close(sockClient);
	//close(sockServer);
	return 0;
}

void LinkAsClient() {
	int sockClient;
	struct sockaddr_in addrServer;
	memset(&addrServer, 0, sizeof(addrServer));
	//新建客户端socket
	sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//定义要连接的服务端地址
	std::string s;
	int port;
	cout << "————————————————" << endl;
	cout << "输入要连接的Ip地址与端口号：" << endl;
	cin >> s >> port; getchar();

	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = inet_addr(s.c_str());//目标IP
	addrServer.sin_port = htons(port);//连接端口
	//连接到服务端
	int error = connect(sockClient, (struct sockaddr*)& addrServer, sizeof(addrServer));

	if (error == -1) {
		cout << "ERROR!\n";
		return;
	}

	cout << "Connected!\n";

	cout << "————连接服务器————" << endl;
	//发送数据
	string message;
	while (true)
	{
		getline(cin, message, ';');
		send(sockClient, message.c_str(), message.size() + 1, 0);

		regex reg("\n");//不知道为什么有时候exit前面会有\n
		message = regex_replace(message, reg, "");
		if (toUpper(message) == "EXIT") break;

		char buf[1000] = {};
		error = recv(sockClient, buf, 1000, 0);

		if (error == -1) {
			cout << "ERROR!" << endl;
			break;
		}
		else {
			cout << message << " Done!" << endl;
			cout << buf; //<< std::endl;
		}

	}
	//关闭socket
	cout << "————已断开连接————" << endl;
	//close(sockClient);
}
#endif // 

#endif // 操作系统判断