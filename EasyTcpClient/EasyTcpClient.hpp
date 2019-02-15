#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_
//Windows.h里面的宏和WinSock2里面的宏有重复
//Windows环境下开发
#ifdef _WIN32
	//尽量避免早期的宏的引入
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	//windows环境下开发网络编程需要引入的socket头文件
	#include <WinSock2.h>
	//windows环境下进行引入动态链接库  WSAStartup
	//在其他系统平台下不能使用  可以将ws2_32.lib 配置到工程 属性 链接器里面
	//#pragma comment(lib, "ws2_32.lib")
#else
	#include<unistd.h>
	#include<arpa/inet.h>
	#include<string.h>
	#define SOCKET int
	#define INVALID_SOCKET  (SOCKET)(~0)
	#define SOCKET_ERROR            (-1)
#endif
#include<stdio.h>
#include "MessageHeader.hpp"
class EasyTcpClient
{
	SOCKET _sock;
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	// 虚析构函数 
	virtual ~EasyTcpClient()
	{
		Close();
	}
	//初始化socket
	void InitSocket()
	{
		//启动Win sock 2.x环境
#ifdef _WIN32
		//创建版本号
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif
		//1 建立socket
		if (_sock != INVALID_SOCKET)
		{
			//关闭之前连接
			printf("关闭旧连接<socket=%d>.... \n", _sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_sock == INVALID_SOCKET) {
			printf("error, build socket faild \n");
		}
		else {
			printf("build success \n");
		}
	}
	//连接服务器
	int Connect(const char* ip, unsigned short port)
	{
		if (_sock == INVALID_SOCKET)
		{
			InitSocket();
		}
		//2 连接服务器connect
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (ret == SOCKET_ERROR) {
			printf("错误, 连接服务器失败...... \n");
		}
		else {
			printf("连接服务器成功....... \n");
		}
		return ret;
	}

	//4 关闭套接字closesocket
	void Close()
	{
		if (_sock != INVALID_SOCKET) 
		{
			//清除Win sock 2.x环境	
#ifdef _WIN32
			closesocket(_sock);
			//清除windows socket环境
			WSACleanup();
#else
			close(_sock);
#endif
			_sock = INVALID_SOCKET;
		}	
	}
	
	//处理网络消息
	bool OnRun()
	{
		if (isRun())
		{
			fd_set fdReader;
			FD_ZERO(&fdReader);
			FD_SET(_sock, &fdReader);
			timeval t = { 1,0 };
			int ret = select(_sock, &fdReader, 0, 0, &t);
			if (ret < 0)
			{
				printf("<socket=%d>select 任务结束1\n", _sock);
				return false;
			}
			if (FD_ISSET(_sock, &fdReader))
			{
				FD_CLR(_sock, &fdReader);
				if (-1 == RecvData(_sock))
				{
					printf("<socket=%d>select任务结束2\n", _sock);
					return false;
				}
			}
			return true;
		}
		return false;
	}
	//是否工作中
	bool isRun()
	{
		return _sock != INVALID_SOCKET;
	}
	//接收数据 粘包 拆包
	int RecvData(SOCKET _cSock)
	{
		//缓冲区,把消息先放到缓冲区里面
		char szRecv[1024] = {};
		//5接收客户端数据
		int nLen = recv(_cSock, (char*)&szRecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)szRecv;
		if (nLen <= 0)
		{
			printf("与服务器断开连接，任务结束。\n");
			return -1;
		}
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		
		OnNetMsg(header);
		return 0;
	}

	//响应网络消息
	void OnNetMsg(DataHeader* header)
	{
		switch (header->cmd) {
			case CMD_LOGIN_RESULT:
			{
				//基类转换成子类
				LoginResult* login = (LoginResult*)header;
				printf("收到服务端消息：CMD_LOGIN_RESULT 数据长度：%d \n", login->dataLength);
			}
			break;
			case CMD_LOGOUT_RESULT:
			{
				LogoutResult* logout = (LogoutResult*)header;
				printf("收到服务端消息：CMD_LOGOUT_RESULT 数据长度：%d \n", logout->dataLength);
			}
			break;
			case CMD_NEW_USER_JOIN:
			{
				NewUserJoin* user_join = (NewUserJoin*)header;
				printf("收到服务端消息：CMD_NEW_USER_JOIN 数据长度：%d \n", user_join->dataLength);
			}
			break;
		}
	}

	//发送数据
	int SendData(DataHeader* header)
	{
		if (isRun() && header)
		{
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}
private:

};

#endif