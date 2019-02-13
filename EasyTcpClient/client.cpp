//�����������ڵĺ������
#define WIN32_LEAN_AND_MEAN
//Windows.h����ĺ��WinSock2����ĺ����ظ�
//Windows�����¿���
#include <Windows.h>
//windows�����¿�����������Ҫ�����socketͷ�ļ�
#include <WinSock2.h>
//windows�����½������붯̬���ӿ�  WSAStartup
//������ϵͳƽ̨�²���ʹ��  ���Խ�ws2_32.lib ���õ����� ���� ����������
//#pragma comment(lib, "ws2_32.lib")
#include<stdio.h>

//�ڴ����
struct DataPackage
{
	//long ��64λ����64�ֽ�    ��32λֻ��4�ֽ�
	int age;
	char name[32];
};
struct DataHeader
{
	short dataLength;
	short cmd;
};
enum CMD
{
	CMD_LOGIN,
	CMD_LOGINOUT,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
struct Login : public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};
struct LoginOut : public DataHeader
{
	LoginOut()
	{
		dataLength = sizeof(LoginOut);
		cmd = CMD_LOGINOUT;
	}
	char userName[32];
};
struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 1;
	}
	int result;
};
struct LogoutResult : public DataHeader
{
	LogoutResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock_id = 0;
	}
	int sock_id;
};

int process(SOCKET _cSock)
{
	//������,����Ϣ�ȷŵ�����������
	char szRecv[1024] = {};
	//5���տͻ�������
	int nLen = recv(_cSock, (char*)&szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		printf("��������Ͽ����ӣ����������\n");
		return -1;
	}
	switch (header->cmd) {
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginResult* login = (LoginResult*)szRecv;
			printf("�յ��������Ϣ��CMD_LOGIN_RESULT ���ݳ��ȣ�%d \n", _cSock, login->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogoutResult* logout = (LogoutResult*)szRecv;
			printf("�յ��������Ϣ��CMD_LOGOUT_RESULT ���ݳ��ȣ�%d \n", _cSock, logout->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin* user_join = (NewUserJoin*)szRecv;
			printf("�յ��������Ϣ��CMD_NEW_USER_JOIN ���ݳ��ȣ�%d \n", _cSock, user_join->dataLength);
		}
		break;
	}
}

int main()
{
	//�����汾��
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//----------------------------
	//1 ����socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock == INVALID_SOCKET) {
		printf("error, build socket faild \n");
	}
	else {
		printf("build success \n");
	}

	//2 ���ӷ�����connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		printf("error, connect socket faild \n");
	}
	else {
		printf("connect success \n");
	}

	while (true) {
		FD_SET fdReader;
		FD_ZERO(&fdReader);
		FD_SET(_sock, &fdReader);
		timeval t = { 1,0 };
		int ret = select(_sock, &fdReader, 0, 0, &t);
		if (ret < 0)
		{
			printf("select �������1\n");
			break;
		}
		if (FD_ISSET(_sock, &fdReader))
		{
			FD_CLR(_sock, &fdReader);
			if (-1 == process(_sock))
			{
				printf("select�������2\n");
				break;
			}
		}
		printf("���д�������ҵ��\n");
		Login login;
		strcpy(login.userName, "lanzhibo");
		strcpy(login.passWord, "123");
		send(_sock,(const char *)&login, sizeof(Login), 0);
		//Sleep(1000);
	}
	
	//4 �ر��׽���closesocket
	closesocket(_sock);
	//���windows socket����
	WSACleanup();
	getchar();
	return 0;
}