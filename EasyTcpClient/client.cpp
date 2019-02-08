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
struct LoginResult
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
		}else {
			//5 ���������������
			send(_sock, cmdBuff, strlen(cmdBuff)+1, 0);
		}
		//6 ���շ�������Ϣ recv
		char recvBuf[256] = {};
		int nlen = recv(_sock, recvBuf, 256, 0);
		if (nlen > 0) {
			DataPackage* dp = (DataPackage*)recvBuf;
			printf("���յ����ݣ�����=%d������=%s \n",dp->age, dp->name);
		}
	}
	
	//4 �ر��׽���closesocket
	closesocket(_sock);
	//���windows socket����
	WSACleanup();
	getchar();
	return 0;
}