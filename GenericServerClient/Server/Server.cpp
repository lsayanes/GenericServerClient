


#include <iostream>
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <memory>
//#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <fstream>

#include <Sock.h>
#include <Tcp.h>
#include <Udp.h>

#include "Application.h"

template<typename T>
class ServerApp : public server::Application<T>
{
	std::mutex											m_mxt{};
	std::atomic<std::time_t>							m_lastRequest;

public:
	explicit ServerApp(std::string &host, int port) : 
		server::Application<T>(host, port),
		m_lastRequest{ std::time(nullptr) } 
	{
	}

	virtual ~ServerApp()
	{
	}

	inline bool isTimeToDie() const
	{
		return (std::time(nullptr) - m_lastRequest > 59);
	}

	void client(T* pSock) final
	{
		
		uint64_t ullCnt{ 0 };
		int nRcv{ 1 };
		char strRcv[1024] = { 0 };
		char strSnd[1024] = { 0 };

		std::string szClientName{ "Client_" };
		szClientName += std::to_string(ServerApp<T>::clientID());

		std::cout << std::endl << "* New client name:" << szClientName << " host: " << pSock->host() << " conected" << std::endl << std::endl;

		while (ServerApp<T>::isRunning())
		{
			m_mxt.lock();
			if ((nRcv = pSock->rcv(strRcv, sizeof(strRcv) - 1)) > 0)
			{
				strRcv[nRcv] = '\0';

				std::stringstream ss(strRcv);
				std::string token;

				while (std::getline(ss, token, ';'))
				{
					std::cout << "Rcv Client: " << szClientName << " Msg: " << token << std::endl;
					sprintf(strSnd, "ACK Num:%llu;", ullCnt);
					pSock->snd(strSnd, strlen(strSnd));
					std::cout << "Snd: " << strSnd << std::endl;

					++ullCnt;
				}

				m_lastRequest = std::time(nullptr);
			}
			std::cout << std::endl;
			m_mxt.unlock();
		}
	}
};




int main()
{

	std::string host = "localhost";

	ServerApp<winposix::Tcp> app(host, 2024);
	
	if (app.start())
	{

		while (!app.isTimeToDie())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(ServerApp<winposix::Tcp>::cnstTimeoutRcv));
		}

		std::cout << "closing..." << std::endl;
		app.stop();
	}

	return 0;
}

