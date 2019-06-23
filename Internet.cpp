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
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//INADDR_ANY��ʾ�κ�IP
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);//�󶨶˿�6000
	bind(sockServer, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

	//  ����������Ļ�����
	char szHost[256];
	//  ȡ�ñ�����������
	::gethostname(szHost, 256);
	//  ͨ���������õ���ַ��Ϣ��һ�����������ж�����������IP��ַ
	hostent* pHost = ::gethostbyname(szHost);
	in_addr addr;
	for (int i = 0;; i++)
	{
		//��õ�ַ�������ֽ���
		char* p = pHost->h_addr_list[i];
		if (p == NULL)
		{
			break;
		}
		//  ����ַ������in_addr�ṹ����
		memcpy(&addr.S_un.S_addr, p, pHost->h_length);
		//  ��in_addrת��Ϊ�����ֽ���
		char* strIp = ::inet_ntoa(addr);
		//  ���
		cout << "��������������������������������" << endl;
		printf("����IP[%d]:%s\n", i + 1, strIp);
	}

	cout << "ʹ�ö˿ڣ�6000" << endl;
	cout << "��������������������������������" << endl;

	//Listen������
	listen(sockServer, SOMAXCONN);
	printf("������������:\n������...\n");

	//���������̣�ֱ���пͻ�����������Ϊֹ
	int len = sizeof(SOCKADDR);
	sockClient = accept(sockServer, (SOCKADDR*)& addrClient, &len);
	char recvBuf[1000] = {};//���ܿͻ��˷��ص��ַ���

	cout << "�����������ӿͻ��ˡ�������" << endl;

	while (true)
	{
		//���ղ���ӡ�ͻ�������
		if (recv(sockClient, recvBuf, 1000, 0) != SOCKET_ERROR)
			break;

		string buf(recvBuf);
		regex reg("\n");//��֪��Ϊʲô��ʱ��exitǰ�����\n
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
	cout << "���������ѶϿ����ӡ�������" << endl;
	//�ر�socket
	closesocket(sockClient);
	closesocket(sockServer);
	WSACleanup();
}

void LinkAsClient() {
	WSADATA wsaData;
	SOCKADDR_IN addrServer;
	SOCKET sockClient;

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�½��ͻ���socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//����Ҫ���ӵķ���˵�ַ
	std::string s;
	int port;
	cout << "��������������������������������" << endl;
	std::cout << "����Ҫ���ӵ�Ip��ַ��˿ںţ�" << std::endl;
	std::cin >> s >> port; getchar();
	addrServer.sin_addr.S_un.S_addr = inet_addr(s.c_str());//Ŀ��IP(127.0.0.1�ǻ��͵�ַ)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);//���Ӷ˿�
	//���ӵ������
	int error = connect(sockClient, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

	if (error == SOCKET_ERROR) {
		cout << "LINK ERROR!�Ͽ�����!\n";
		return;
	}

	cout << "Connected!\n";

	cout << "�����������ӷ�������������" << endl;
	//��������
	string message;
	while (true)
	{
		getline(cin, message, ';');
		send(sockClient, message.c_str(), message.size() + 1, 0);

		regex reg("\n");//��֪��Ϊʲô��ʱ��exitǰ�����\n
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
	//�ر�socket
	cout << "���������ѶϿ����ӡ�������" << endl;
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
	addrServer.sin_port = htons(6000);//�󶨶˿�6000	

	bind(sockServer, (struct sockaddr_in*)& addrServer, sizeof(struct sockaddr_in));

	cout << "��������������������������������" << endl;
	printf("����IP:%s\n", "127.0.0.1");
	cout << "ʹ�ö˿ڣ�6000" << endl;
	cout << "��������������������������������" << endl;

	//Listen������
	listen(sockServer, SOMAXCONN);
	printf("������������:\n������...\n");

	//���������̣�ֱ���пͻ�����������Ϊֹ
	auto len = sizeof(addrClient);
	sockClient = accept(sockServer, (struct sockaddr*)& addrClient, (socklen_t*)&len);
	char recvBuf[1000] = {};//���ܿͻ��˷��ص��ַ���

	cout << "�����������ӿͻ��ˡ�������" << endl;

	while (true)
	{
		//���ղ���ӡ�ͻ�������
		if (recv(sockClient, recvBuf, 1000, 0) != -1)
			break;

		string buf(recvBuf);
		regex reg("\n");//��֪��Ϊʲô��ʱ��exitǰ�����\n
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
	cout << "���������ѶϿ����ӡ�������" << endl;
	//�ر�socket
	//close(sockClient);
	//close(sockServer);
	return 0;
}

void LinkAsClient() {
	int sockClient;
	struct sockaddr_in addrServer;
	memset(&addrServer, 0, sizeof(addrServer));
	//�½��ͻ���socket
	sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//����Ҫ���ӵķ���˵�ַ
	std::string s;
	int port;
	cout << "��������������������������������" << endl;
	cout << "����Ҫ���ӵ�Ip��ַ��˿ںţ�" << endl;
	cin >> s >> port; getchar();

	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.s_addr = inet_addr(s.c_str());//Ŀ��IP
	addrServer.sin_port = htons(port);//���Ӷ˿�
	//���ӵ������
	int error = connect(sockClient, (struct sockaddr*)& addrServer, sizeof(addrServer));

	if (error == -1) {
		cout << "ERROR!\n";
		return;
	}

	cout << "Connected!\n";

	cout << "�����������ӷ�������������" << endl;
	//��������
	string message;
	while (true)
	{
		getline(cin, message, ';');
		send(sockClient, message.c_str(), message.size() + 1, 0);

		regex reg("\n");//��֪��Ϊʲô��ʱ��exitǰ�����\n
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
	//�ر�socket
	cout << "���������ѶϿ����ӡ�������" << endl;
	//close(sockClient);
}
#endif // 

#endif // ����ϵͳ�ж�