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
#include<vector>
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
struct LogoutResult: public DataHeader
{
	LogoutResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};
std::vector<SOCKET> g_clients;

int process(SOCKET _cSock)
{
	//������,����Ϣ�ȷŵ�����������
	char szRecv[1024] = {};
	//5���տͻ�������
	int nLen = recv(_cSock, (char*)&szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		printf("kehuduan tuichu");
		return -1;
	}
	switch (header->cmd) {
	case CMD_LOGIN:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		//ȡ����Ϣ   ����Ϣ��Ӧ����Ϣ������
		Login* login = (Login*)szRecv;
		printf("�յ����cmd_login ���ݳ��ȣ�%d, userName=%s, passWd = %s \n", login->dataLength, login->userName, login->passWord);
		//�����ж��û��������Ƿ���ȷ
		LoginResult loginRet;
		send(_cSock, (char*)&loginRet, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGINOUT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginOut* logout = (LoginOut*)szRecv;
		printf("�յ����cmd_logout ���ݳ��ȣ�%d, userName=%s \n", logout->dataLength, logout->userName);

		//�����ж��û��������Ƿ���ȷ
		LogoutResult logoutRet;
		send(_cSock, (char*)&logoutRet, sizeof(LogoutResult), 0);
	}
	break;
	default:
		header->cmd = CMD_ERROR;
		header->dataLength = 0;
		send(_cSock, (char*)&header, sizeof(header), 0);
		break;
	}
}

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

	while (true) 
	{
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		FD_SET(_sock, &fdExp);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdRead);
		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
		}
		//select���һ������  ����ʱ��t����select��Ϊ������   ��ѯ�ȴ�ʱ�䣬����tʱ��û�����󣬷���
		timeval t = {0,0};
		//������ socket
		//nfds ��һ������,��ָfd_set����������������(socket)��Χ��socket����ֵ��1  ��windows���治�������壬����д0����linux������������������1
		int ret = select(_sock+1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			printf("select��������� \n");
			break;
		}
		//�жϵ�ǰsocket�Ƿ��ڵ�ǰfdSet������
		if (FD_ISSET(_sock, &fdRead)) 
		{
			FD_CLR(_sock, &fdRead);
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
			g_clients.push_back(_cSock);
			printf("�¿ͻ��˼��룺ip = %s \n", inet_ntoa(clientAddr.sin_addr));
		}
		for (size_t n = 0; n < fdRead.fd_count; n++) 
		{
			if (-1 == process(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
	}

	//8 �ر��׽��� close socket
	for (size_t n = g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
	}
	closesocket(_sock);

	//	���windows socket����
	WSACleanup();
	getchar();
	return 0;
}