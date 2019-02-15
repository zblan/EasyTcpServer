#include "EasyTcpClient.hpp"
//引入c++标准线程库 c++11正式加入标准库  还有第三方线程库pthread
#include<thread>

//传引用写法   在其他平台下会有错误   使用指针
void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuff[256] = {};
		scanf("%s", cmdBuff);
		if (0 == strcmp(cmdBuff, "exit"))
		{		
			client->Close();
			printf("退出cmdThread线程 \n");
			break;
		}
		else if (0 == strcmp(cmdBuff, "login"))
		{
			Login login;
			strcpy(login.userName, "lanzhibo");
			strcpy(login.passWord, "123");
			client->SendData(&login);
 		}
		else if (0 == strcmp(cmdBuff, "logout"))
		{
			LoginOut logout;
			strcpy(logout.userName, "lanzhibo");
			client->SendData(&logout);
		}
		else
		{
			printf("不支持的命令\n");
		}
	}
}

int main()
{
	EasyTcpClient client;
	client.InitSocket();
	client.Connect("127.0.0.1", 4567);

	//线程 thread scanf是阻塞的   使用线程异步获取输入
	std::thread t1(cmdThread, &client);
	//与主线程分离
	t1.detach();

	while (client.isRun()) {
		client.OnRun();
	}
	client.Close();
	
	getchar();
	return 0;
}
