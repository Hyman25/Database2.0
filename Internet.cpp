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
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//INADDR_ANY��ʾ�κ�IP
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);//�󶨶˿�6000
	bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));

	//  ����������Ļ�����
	char szHost[256];
	//  ȡ�ñ�����������
	::gethostname(szHost, 256);
	//  ͨ���������õ���ַ��Ϣ��һ�����������ж�����������IP��ַ
	hostent *pHost = ::gethostbyname(szHost);
	in_addr addr;
	for (int i = 0;; i++)
	{
		//��õ�ַ�������ֽ���
		char *p = pHost->h_addr_list[i];
		if (p == NULL)
		{
			break;
		}
		//  ����ַ������in_addr�ṹ����
		memcpy(&addr.S_un.S_addr, p, pHost->h_length);
		//  ��in_addrת��Ϊ�����ֽ���
		char *strIp = ::inet_ntoa(addr);
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
	char sendBuf[1000];//�������ͻ��˵��ַ���
	char recvBuf[1000];//���ܿͻ��˷��ص��ַ���
	//���������̣�ֱ���пͻ�����������Ϊֹ
	sockClient = accept(sockServer, (SOCKADDR*)&addrClient, &len);
	cout << "�����������ӿͻ��ˡ�������" << endl;
	while (true) 
	{
		//���ղ���ӡ�ͻ�������
		recv(sockClient, recvBuf, 100, 0);
		cout << recvBuf << endl;
		if (memcmp(recvBuf, "exit",sizeof("exit"))==0)
			break;
		string s(recvBuf);
		Command com(s);
		com.operate();
		send(sockClient, "done", 5, 0);
	}
	cout << "���������ѶϿ����ӡ�������" << endl;
	//�ر�socket
	closesocket(sockClient);
	WSACleanup();
}

void LinkAsClient(){
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�½��ͻ���socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//����Ҫ���ӵķ���˵�ַ
	std::string s;
	int port;
	std::cout << "����Ҫ���ӵ�Ip��ַ��˿ں�" << std::endl;
	std::cin >> s >> port; getchar();
	addrServer.sin_addr.S_un.S_addr = inet_addr(s.c_str());//Ŀ��IP(127.0.0.1�ǻ��͵�ַ)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);//���Ӷ˿�6000
	//���ӵ������
	int error=connect(sockClient, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

	if (!error) cout << "Connected!\n";
	else if (error == SOCKET_ERROR) {
		cout << "ERROR!\n";
		return;
	}
	//��������
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
	//�ر�socket
	closesocket(sockClient);
	std::cout << "�Ͽ����ӡ�" << std::endl;
	WSACleanup();
}