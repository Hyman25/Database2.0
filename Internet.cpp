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
		cout << endl << "��������������������������������" << endl;
		printf("����IP[%d]:%s\n", i + 1, strIp);
	}

	cout << "ʹ�ö˿ڣ�6000" << endl;
	cout << "��������������������������������" << endl;

	//Listen������
	listen(sockServer, 5);//5Ϊ�ȴ�������Ŀ
	printf("������������:\n������...\n");
	int len = sizeof(SOCKADDR);
	char sendBuf[1000] = {};//�������ͻ��˵��ַ���
	char recvBuf[1000] = {};//���ܿͻ��˷��ص��ַ���
	//���������̣�ֱ���пͻ�����������Ϊֹ
	sockClient = accept(sockServer, (SOCKADDR*)& addrClient, &len);
	cout << "�����������ӿͻ��ˡ�������" << endl;

	while (true)
	{
		//���ղ���ӡ�ͻ�������
		recv(sockClient, recvBuf, 1000, 0);
		cout << recvBuf << endl;
		if (memcmp(recvBuf, "exit", sizeof("exit")) == 0)
			break;
		streambuf* coutBuf = cout.rdbuf();

		ofstream of("out.txt");
		// ��ȡ�ļ�out.txt��������ָ��
		streambuf* fileBuf = of.rdbuf();

		// ����cout��������ָ��Ϊout.txt����������ָ��
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
	WSACleanup();
}

void LinkAsClient() {
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�½��ͻ���socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//����Ҫ���ӵķ���˵�ַ
	std::string s;
	int port;
	cout << "�����������ӷ�������������" << endl;
	std::cout << "����Ҫ���ӵ�Ip��ַ��˿ں�" << std::endl;
	std::cin >> s >> port; getchar();
	addrServer.sin_addr.S_un.S_addr = inet_addr(s.c_str());//Ŀ��IP(127.0.0.1�ǻ��͵�ַ)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);//���Ӷ˿�6000
	//���ӵ������
	int error = connect(sockClient, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

	if (!error) cout << "Connected!\n";
	else if (error == SOCKET_ERROR) {
		cout << "ERROR!\n";
		return;
	}
	cout << "��������������������������������" << endl;
	//��������
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
	//�ر�socket
	closesocket(sockClient);
	cout << "���������ѶϿ����ӡ�������" << endl;
	WSACleanup();
}