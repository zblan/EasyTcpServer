//�����������ڵĺ������
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
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
enum CMD
{
	CMD_LOGIN,
	CMD_LOGINOUT,
	CMD_ERROR
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
	//1 ����һ��socket
	//2 bind �����ڽ��տͻ������ӵ�����˿�
	//3 listen ��������˿�
	//4 accept �����ȴ����ܿͻ�������
	//5 send ��ͻ��˷���һ������
	//6 �ر��׽��� close socket

	//1 ����һ��socket
	//����һ��AF_INET���׽��֣�����ipv4
	//SOCK_STREAM ������������   ������ѡ�����������ĵȵ�
	//IPPROTO_TCP����tcp��   Ҳ����ѡ��udp��
	SOCKET _sock = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);

	//2 bind �����ڽ��տͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);  //host to net unsigned short �ֽ���ת��
	//���ð󶨵��Ǹ�IP��ַ��   һ̨�����кܶ��ַ
	_sin.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("ERROR,������˿�ʧ��\n");
	}
	else {
		printf("������˿ڳɹ�\n");
	}

	//3 listen ��������˿�
	//backlog 5  backlog�����ں�ʹ�ö����ֵ�����ȴ����У��ȴ�����    һ��С��30����
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("ERROR,��������˿�ʧ��\n");
	}
	else {
		printf("��������˿ڳɹ�\n");
	}

	//4 accept �����ȴ����ܿͻ�������
	//�ͻ��˵�ַ
	sockaddr_in clientAddr = {};
	int nAddrLen = (int)sizeof(clientAddr);
	SOCKET _cSock = INVALID_SOCKET;

	
	//ѭ��accept���
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (_cSock == INVALID_SOCKET)
	{
		printf("ERROR,���յ���Ч�ͻ�������\n");
	}
	printf("�¿ͻ��˼��룺ip = %s \n", inet_ntoa(clientAddr.sin_addr));

	while (true) 
	{
		DataHeader header = {};
		//5���տͻ�������
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0) {
			printf("kehuduan tuichu");
			break;
		}
		printf("�յ����%d ���ݳ��ȣ�%d \n", header.cmd, header.dataLength);
		switch (header.cmd) {
			case CMD_LOGIN:
			{
				Login login = {};
				recv(_cSock, (char*)&login, sizeof(Login), 0);
				//�����ж��û��������Ƿ���ȷ
				LoginResult loginRet = {0};
				send(_cSock, (char*)&header, sizeof(DataHeader), 0);
				send(_cSock, (char*)&loginRet, sizeof(LoginResult), 0);
			}
			break;
			case CMD_LOGINOUT:
			{
				LoginOut logout = {};
				recv(_cSock, (char*)&logout, sizeof(LoginOut), 0);
				//�����ж��û��������Ƿ���ȷ
				LogoutResult logoutRet = { 1 };
				send(_cSock, (char*)&header, sizeof(DataHeader), 0);
				send(_cSock, (char*)&logoutRet, sizeof(LogoutResult), 0);
			}
			break;
			default:
				header.cmd = CMD_ERROR;
				header.dataLength = 0;
				send(_cSock, (char*)&header, sizeof(header), 0);
			break;
		}
	}

	//8 �ر��׽��� close socket
	closesocket(_sock);

	//	���windows socket����
	WSACleanup();
	getchar();
	return 0;
}