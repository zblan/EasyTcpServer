#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_
//Windows.h����ĺ��WinSock2����ĺ����ظ�
//Windows�����¿���
#ifdef _WIN32
	//�����������ڵĺ������
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	//windows�����¿�����������Ҫ�����socketͷ�ļ�
	#include <WinSock2.h>
	//windows�����½������붯̬���ӿ�  WSAStartup
	//������ϵͳƽ̨�²���ʹ��  ���Խ�ws2_32.lib ���õ����� ���� ����������
	//#pragma comment(lib, "ws2_32.lib")
#else
	#include<unistd.h>
	#include<arpa/inet.h>
	#include<string.h>
	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif
#include<stdio.h>
#include "MessageHeader.hpp"
class EasyTcpClient
{
	SOCKET _sock;
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	// ���������� 
	virtual ~EasyTcpClient()
	{
		Close();
	}
	//��ʼ��socket
	void InitSocket()
	{
		//����Win sock 2.x����
#ifdef _WIN32
		//�����汾��
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		//1 ����socket
		if (_sock != INVALID_SOCKET)
		{
			//�ر�֮ǰ����
			printf("�رվ�����<socket=%d>.... \n", _sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_sock == INVALID_SOCKET) {
			printf("error, build socket faild \n");
		}
		else {
			printf("build success \n");
		}
	}
	//���ӷ�����
	int Connect(const char* ip, unsigned short port)
	{
		if (_sock == INVALID_SOCKET)
		{
			InitSocket();
		}
		//2 ���ӷ�����connect
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (ret == SOCKET_ERROR) {
			printf("����, ���ӷ�����ʧ��...... \n");
		}
		else {
			printf("���ӷ������ɹ�....... \n");
		}
		return ret;
	}

	//4 �ر��׽���closesocket
	void Close()
	{
		if (_sock != INVALID_SOCKET) 
		{
			//���Win sock 2.x����	
#ifdef _WIN32
			closesocket(_sock);
			//���windows socket����
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;
		}	
	}
	
	//����������Ϣ
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdReader;
			FD_ZERO(&fdReader);
			FD_SET(_sock, &fdReader);
			timeval t = { 1,0 };
			int ret = select(_sock, &fdReader, 0, 0, &t);
			if (ret < 0)
			{
				printf("<socket=%d>select �������1\n", _sock);
				return false;
			}
			if (FD_ISSET(_sock, &fdReader))
			{
				FD_CLR(_sock, &fdReader);
				if (-1 == RecvData(_sock))
				{
					printf("<socket=%d>select�������2\n", _sock);
					return false;
				}
			}
			return true;
		}
		return false;
	}
	//�Ƿ�����
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}
	//�������� ճ�� ���
	int RecvData(SOCKET _cSock)
	{
		//������,����Ϣ�ȷŵ�����������
		char szRecv[1024] = {};
		//5���տͻ�������
		int nLen = recv(_cSock, (char*)&szRecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)szRecv;
		if (nLen <= 0)
		{
			printf("��������Ͽ����ӣ����������\n");
			return -1;
		}
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		
		OnNetMsg(header);
		return 0;
	}

	//��Ӧ������Ϣ
	void OnNetMsg(DataHeader* header)
	{
		switch (header->cmd) {
			case CMD_LOGIN_RESULT:
			{
				//����ת��������
				LoginResult* login = (LoginResult*)header;
				printf("�յ��������Ϣ��CMD_LOGIN_RESULT ���ݳ��ȣ�%d \n", login->dataLength);
			}
			break;
			case CMD_LOGOUT_RESULT:
			{
				LogoutResult* logout = (LogoutResult*)header;
				printf("�յ��������Ϣ��CMD_LOGOUT_RESULT ���ݳ��ȣ�%d \n", logout->dataLength);
			}
			break;
			case CMD_NEW_USER_JOIN:
			{
				NewUserJoin* user_join = (NewUserJoin*)header;
				printf("�յ��������Ϣ��CMD_NEW_USER_JOIN ���ݳ��ȣ�%d \n", user_join->dataLength);
			}
			break;
		}
	}

	//��������
	int SendData(DataHeader* header)
	{
		if (isRun() && header)
		{
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}
private:

};

#endif