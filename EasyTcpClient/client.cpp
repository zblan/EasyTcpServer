//尽量避免早期的宏的引入
#define WIN32_LEAN_AND_MEAN
//Windows.h里面的宏和WinSock2里面的宏有重复
//Windows环境下开发
#include <Windows.h>
//windows环境下开发网络编程需要引入的socket头文件
#include <WinSock2.h>
//windows环境下进行引入动态链接库  WSAStartup
//在其他系统平台下不能使用  可以将ws2_32.lib 配置到工程 属性 链接器里面
//#pragma comment(lib, "ws2_32.lib")
#include<stdio.h>

//内存对齐
struct DataPackage
{
	//long 在64位中是64字节    在32位只有4字节
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
	//缓冲区,把消息先放到缓冲区里面
	char szRecv[1024] = {};
	//5接收客户端数据
	int nLen = recv(_cSock, (char*)&szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0) {
		printf("与服务器断开连接，任务结束。\n");
		return -1;
	}
	switch (header->cmd) {
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginResult* login = (LoginResult*)szRecv;
			printf("收到服务端消息：CMD_LOGIN_RESULT 数据长度：%d \n", _cSock, login->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogoutResult* logout = (LogoutResult*)szRecv;
			printf("收到服务端消息：CMD_LOGOUT_RESULT 数据长度：%d \n", _cSock, logout->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin* user_join = (NewUserJoin*)szRecv;
			printf("收到服务端消息：CMD_NEW_USER_JOIN 数据长度：%d \n", _cSock, user_join->dataLength);
		}
		break;
	}
}

int main()
{
	//创建版本号
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//----------------------------
	//1 建立socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock == INVALID_SOCKET) {
		printf("error, build socket faild \n");
	}
	else {
		printf("build success \n");
	}

	//2 连接服务器connect
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
			printf("select 任务结束1\n");
			break;
		}
		if (FD_ISSET(_sock, &fdReader))
		{
			FD_CLR(_sock, &fdReader);
			if (-1 == process(_sock))
			{
				printf("select任务结束2\n");
				break;
			}
		}
		printf("空闲处理其他业务\n");
		Login login;
		strcpy(login.userName, "lanzhibo");
		strcpy(login.passWord, "123");
		send(_sock,(const char *)&login, sizeof(Login), 0);
		//Sleep(1000);
	}
	
	//4 关闭套接字closesocket
	closesocket(_sock);
	//清除windows socket环境
	WSACleanup();
	getchar();
	return 0;
}