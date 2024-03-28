


#include <iostream>
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <memory>


#include <Sock.h>
#include <Tcp.h>

#include "Application.h"

namespace server
{

	Application::Application(std::string host, int port):
		szHost{ host },
		nPort{ port },
		m_pServer{ new winpoxi::Tcp(szHost, nPort) },
		m_bTerminateFlag{ true },
		m_ulClientID{ 0 },
		m_pServerThread{ nullptr }
	{

		int err;
		if (0 != (err = winpoxi::Sock::startup()))
		{
			std::cout << "WSAStartup failed err: " << err << std::endl;
		}
		else
		{
			m_Clients.reserve(cnstMaxClients);
		}

	}

	Application::~Application()
	{
		if(m_pServer)
			delete m_pServer;

		m_pServer = nullptr;

		if (m_pServerThread)
			delete m_pServerThread;

		m_Clients.clear();
		winpoxi::Sock::cleanup();
	}

	bool Application::start()
	{
		if (m_pServer)
		{
			if (m_pServer->create())
			{
				if (nullptr != (m_pServerThread = new std::thread(&Application::listen, this)))
				{
					m_pServerThread->detach();
					return true;
				}
				else
				{
					std::cout << "Error creating server thread!!!" << std::endl;
				}
			}
			else
			{
				std::cout << "Server::create failed: " << m_pServer->getSocketError() << std::endl;
			}
		}
		else
		{
			std::cout << "No Server has been created!!!" << std::endl;
		}


		return false;
	}

	void Application::stop()
	{
		m_bTerminateFlag = true;

		if (m_pServer)
			m_pServer->close();			

		
		std::this_thread::sleep_for(std::chrono::milliseconds(cnstTimeoutRcv*2));
		std::cout << "Server stopped" << std::endl;

	}

	void Application::listen() 
	{
		std::cout << "Server listen" << std::endl;
		
		m_bTerminateFlag = !m_pServer->listen();
		while (!m_bTerminateFlag)
		{
			if (m_ulClientID < cnstMaxClients)
			{
				SOCKET newSck = m_pServer->accept();
				if (INVALID_SOCKET != newSck)
				{
					m_Clients.push_back(std::make_unique<winpoxi::Tcp>(szHost, nPort, newSck));

					auto pClient = m_Clients.back().get();
					
					pClient->setTimeoutRcv(cnstTimeoutRcv);
					pClient->setTimeoutRcv(cnstTimeoutSnd);

					addClientID();

					std::thread clientThread(&Application::client, this, pClient);
					clientThread.detach();
				}
			}
			else
			{
				std::cout << "No more client are admitted" << std::endl;
				std::cin.get();
				stop();
			}
		}
	}
}

