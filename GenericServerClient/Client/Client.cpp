#include <iostream>
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <thread>
#include <string>
#include <chrono>
#include <vector>
#include <sstream>

#include <Sock.h>
#include <Tcp.h>
#include <Udp.h>

using namespace winposix;


const	std::string szHost{"localhost"};
const	int			nPort{ 2024 };


void start(Sock* pSock)
{

	pSock->setTimeoutRcv(1000);
	pSock->setTimeoutSnd(1000);

	srand(static_cast<unsigned int>(time(nullptr)));

	int nRcv, nSnd=1;
	int nIndex = 0;
	char strSnd[1024] = {0};
	char strRcv[1024] = {0};

	std::thread::id this_id = std::this_thread::get_id();

	std::hash<std::thread::id> hasher;
	size_t hashValue = hasher(this_id);

	uint64_t ullCnt = 0;

	
	while (nSnd > 0)
	{
		auto start = std::chrono::high_resolution_clock::now();	

		sprintf(strSnd, "Greetings server! this is %s (thread id %X) Msg Num : %llu;", szHost.c_str(), hashValue, ullCnt++);
		if ((nSnd = pSock->snd(strSnd, strlen(strSnd))) > 0)
		{
			std::cout << "Snd: " << strSnd << std::endl;

			strRcv[0] = 0;
			if ((nRcv = pSock->rcv(strRcv, sizeof(strRcv))) > 0)
			{
				strRcv[nRcv] = '\0';
				auto end = std::chrono::high_resolution_clock::now();

				std::stringstream ss(strRcv);
				std::string token;

				while (std::getline(ss, token, ';'))
				{
					std::chrono::duration<double, std::milli> elapsed = end - start;
					std::cout << "Rcv Num: " << nIndex << " :" << token << " Time: " << elapsed.count() << " ms" << std::endl;
					nIndex++;
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			std::cout << std::endl;
		}
	}
}

int main()
{

	int err;
	if (0 != (err = Sock::startup()))
	{
		std::cout << "WSAStartup failed err: " << err << std::endl;
		return 1;
	}

	Sock* pSock = new Udp(szHost, nPort);

	if (pSock)
	{
		if (pSock->create())
		{
			if (pSock->connect())
				start(pSock);
			else
				std::cout << "connect failed: " << pSock->getSocketError() << std::endl;
		}
		else
		{
			std::cout << "create failed: " << pSock->getSocketError() << std::endl;
		}


		delete pSock;
	}

	Sock::cleanup();
	return 0;
}

