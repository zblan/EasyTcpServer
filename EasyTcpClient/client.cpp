#include "EasyTcpClient.hpp"
//����c++��׼�߳̿� c++11��ʽ�����׼��  ���е������߳̿�pthread
#include<thread>

//������д��   ������ƽ̨�»��д���   ʹ��ָ��
void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuff[256] = {};
		scanf("%s", cmdBuff);
		if (0 == strcmp(cmdBuff, "exit"))
		{		
			client->Close();
			printf("�˳�cmdThread�߳� \n");
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
			printf("��֧�ֵ�����\n");
		}
	}
}

int main()
{
	EasyTcpClient client;
	client.InitSocket();
	client.Connect("127.0.0.1", 4567);

	//�߳� thread scanf��������   ʹ���߳��첽��ȡ����
	std::thread t1(cmdThread, &client);
	//�����̷߳���
	t1.detach();

	while (client.isRun()) {
		client.OnRun();
	}
	client.Close();
	
	getchar();
	return 0;
}
