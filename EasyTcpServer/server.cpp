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
#include<vector>
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
	//缓冲区,把消息先放到缓冲区里面
	char szRecv[1024] = {};
	//5接收客户端数据
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
		//取到消息   把消息对应到消息体里面
		Login* login = (Login*)szRecv;
		printf("收到命令：cmd_login 数据长度：%d, userName=%s, passWd = %s \n", login->dataLength, login->userName, login->passWord);
		//忽略判断用户名密码是否正确
		LoginResult loginRet;
		send(_cSock, (char*)&loginRet, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGINOUT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginOut* logout = (LoginOut*)szRecv;
		printf("收到命令：cmd_logout 数据长度：%d, userName=%s \n", logout->dataLength, logout->userName);

		//忽略判断用户名密码是否正确
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
		//select最后一个参数  设置时间t，将select变为非阻塞   查询等待时间，到了t时间没有请求，返回
		timeval t = {0,0};
		//伯克利 socket
		//nfds 第一个参数,是指fd_set集合中所有描述符(socket)范围，socket最大的值加1  在windows下面不产生意义，可以写0，在linux下面代表最大连接数加1
		int ret = select(_sock+1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			printf("select任务结束。 \n");
			break;
		}
		//判断当前socket是否在当前fdSet集合中
		if (FD_ISSET(_sock, &fdRead)) 
		{
			FD_CLR(_sock, &fdRead);
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
			g_clients.push_back(_cSock);
			printf("新客户端加入：ip = %s \n", inet_ntoa(clientAddr.sin_addr));
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

	//8 关闭套接字 close socket
	for (size_t n = g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
	}
	closesocket(_sock);

	//	清楚windows socket环境
	WSACleanup();
	getchar();
	return 0;
}