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
//与server端的struct datapackage变量类型顺序一致
struct DataPackage
{
	//long 在64位中是64字节    在32位只有4字节
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
	//创建版本号
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//----------------------------
	//1 建立socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock == INVALID_SOCKET) {
		printf("error, build socket faild");
	}
	else {
		printf("build success");
	}

	//2 连接服务器connect
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
		//3 输入请求命令
		char cmdBuff[128] = {};
		scanf("%s", cmdBuff);
		//4 处理请求
		if (0 == strcmp(cmdBuff, "exit"))
		{
			printf("get exit command");
			break;
		}
		else if (0 == strcmp(cmdBuff, "login"))
		{
			Login login = {"lanzhibo","123"};
			DataHeader dh = { sizeof(Login),  CMD_LOGIN };
			//5 向服务器发送请求命令
			send(_sock, (char*)&dh, sizeof(DataHeader), 0);
			//向服务器发送登陆信息
			send(_sock, (const char *)&login, sizeof(Login), 0);
			//接收服务器返回的信息
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
			//5 向服务器发送命令
			send(_sock, (char*)&dh, sizeof(DataHeader), 0);
			send(_sock,(char*)&logout, sizeof(LoginOut), 0);
			//接收服务器返回的数据
			DataHeader retHeader = {};
			LogoutResult logoutRet = {};
			recv(_sock, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(_sock, (char*)&logoutRet, sizeof(LogoutResult), 0);
			printf("LogoutResilt:%d \n", logoutRet.result);

		}
		else {
			printf("不支持的命令，请重新输入\n");
		}
		
	}
	
	//4 关闭套接字closesocket
	closesocket(_sock);
	//清除windows socket环境
	WSACleanup();
	getchar();
	return 0;
}