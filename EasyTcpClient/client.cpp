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
			//5 �������������������
			Login login;
			strcpy(login.userName, "lanzhibo");
			strcpy(login.passWord, "123456");
			send(_sock, (const char*)&login, sizeof(Login), 0);
			//���շ��������ص���Ϣ
			LoginResult loginRet = {};
			recv(_sock, (char*)&loginRet , sizeof(Login), 0);
			printf("LoginResult:%d \n", loginRet.result);
		}
		else if (0 == strcmp(cmdBuff, "logout"))
		{
			LoginOut logout;
			strcpy(logout.userName, "lanzhibo");
			//5 ���������������
			send(_sock,(const char*)&logout, sizeof(LoginOut), 0);
			//���շ��������ص�����
			LogoutResult logoutRet = {};
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