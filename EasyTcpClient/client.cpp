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
//��server�˵�struct datapackage��������˳��һ��
struct DataPackage
{
	//long ��64λ����64�ֽ�    ��32λֻ��4�ֽ�
	int age;
	char name[32];
};
enum CMD
{
	CMD_LOGIN,
	CMD_LOGINOUT
};
struct Login
{
	char userName[32];
	char passWord[32];
};
struct LoginOut
{
	char userName[32];
};
struct LoginResult
{
	int result;

};
struct LogoutResult
{
	int result;
};
struct DataHeader
{
	short dataLength;
	short cmd;
};
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
		printf("error, build socket faild");
	}
	else {
		printf("build success");
	}

	//2 ���ӷ�����connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR) {
		printf("error, connect socket faild");
	}
	else {
		printf("connect success");
	}

	while (true) {
		//3 ������������
		char cmdBuff[128] = {};
		scanf("%s", cmdBuff);
		//4 ��������
		if (0 == strcmp(cmdBuff, "exit"))
		{
			printf("get exit command");
			break;
		}
		else if (0 == strcmp(cmdBuff, "login"))
		{
			Login login = {"lanzhibo","123"};
			DataHeader dh = { sizeof(Login),  CMD_LOGIN };
			//5 �������������������
			send(_sock, (char*)&dh, sizeof(DataHeader), 0);
			//����������͵�½��Ϣ
			send(_sock, (const char *)&login, sizeof(Login), 0);
			//���շ��������ص���Ϣ
			DataHeader retHeader = {};
			LoginResult loginRet = {};
			recv(_sock, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(_sock, (char*)&loginRet, sizeof(LoginResult), 0);
			printf("LoginResult:%d \n", loginRet.result);
		}
		else if (0 == strcmp(cmdBuff, "logout"))
		{
			LoginOut logout = {"lanzhibo"};
			DataHeader dh = { sizeof(LoginOut),  CMD_LOGINOUT };
			//5 ���������������
			send(_sock, (char*)&dh, sizeof(DataHeader), 0);
			send(_sock,(char*)&logout, sizeof(LoginOut), 0);
			//���շ��������ص�����
			DataHeader retHeader = {};
			LogoutResult logoutRet = {};
			recv(_sock, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(_sock, (char*)&logoutRet, sizeof(LogoutResult), 0);
			printf("LogoutResilt:%d \n", logoutRet.result);

		}
		else {
			printf("��֧�ֵ��������������\n");
		}
		
	}
	
	//4 �ر��׽���closesocket
	closesocket(_sock);
	//���windows socket����
	WSACleanup();
	getchar();
	return 0;
}