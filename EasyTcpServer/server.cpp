//尽量避免早期的宏的引入
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
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
	//创建版本号
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//1 建立一个socket
	//2 bind 绑定用于接收客户端连接的网络端口
	//3 listen 监听网络端口
	//4 accept 阻塞等待接受客户端连接
	//5 send 向客户端发送一条数据
	//6 关闭套接字 close socket

	//1 建立一个socket
	//创建一个AF_INET的套接字，代表ipv4
	//SOCK_STREAM 面向数据流的   还可以选择面向蓝牙的等等
	//IPPROTO_TCP面向tcp的   也可以选择udp等
	SOCKET _sock = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);

	//2 bind 绑定用于接收客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);  //host to net unsigned short 字节序转换
	//设置绑定到那个IP地址上   一台机器有很多地址
	_sin.sin_addr.S_un.S_addr = INADDR_ANY; //inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("ERROR,绑定网络端口失败\n");
	}
	else {
		printf("绑定网络端口成功\n");
	}

	//3 listen 监听网络端口
	//backlog 5  backlog告诉内核使用多大数值创建等待队列，等待请求    一般小于30以内
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("ERROR,监听网络端口失败\n");
	}
	else {
		printf("监听网络端口成功\n");
	}

	//4 accept 阻塞等待接受客户端连接
	//客户端地址
	sockaddr_in clientAddr = {};
	int nAddrLen = (int)sizeof(clientAddr);
	SOCKET _cSock = INVALID_SOCKET;

	
	//循环accept多次
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (_cSock == INVALID_SOCKET)
	{
		printf("ERROR,接收到无效客户端连接\n");
	}
	printf("新客户端加入：ip = %s \n", inet_ntoa(clientAddr.sin_addr));

	while (true) 
	{
		DataHeader header = {};
		//5接收客户端数据
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0) {
			printf("kehuduan tuichu");
			break;
		}
		printf("收到命令：%d 数据长度：%d \n", header.cmd, header.dataLength);
		switch (header.cmd) {
			case CMD_LOGIN:
			{
				Login login = {};
				recv(_cSock, (char*)&login, sizeof(Login), 0);
				//忽略判断用户名密码是否正确
				LoginResult loginRet = {0};
				send(_cSock, (char*)&header, sizeof(DataHeader), 0);
				send(_cSock, (char*)&loginRet, sizeof(LoginResult), 0);
			}
			break;
			case CMD_LOGINOUT:
			{
				LoginOut logout = {};
				recv(_cSock, (char*)&logout, sizeof(LoginOut), 0);
				//忽略判断用户名密码是否正确
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

	//8 关闭套接字 close socket
	closesocket(_sock);

	//	清楚windows socket环境
	WSACleanup();
	getchar();
	return 0;
}