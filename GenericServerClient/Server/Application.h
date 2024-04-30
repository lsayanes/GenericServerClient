#pragma once

namespace server
{
	template<typename T>
	class Application
	{
	public:
		explicit Application(std::string &host, int port);
		virtual ~Application();
	
		std::string					szHost;
		int							nPort;
		
		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		bool start();
		void stop();

		static constexpr	uint32_t	cnstMaxClients{ 1024 };
		static constexpr	int			cnstTimeoutRcv{ 1000 * 1 };
		static constexpr	int			cnstTimeoutSnd{ 1000 * 1 };
		
	protected:

		T*									m_pServer;
		std::vector<std::unique_ptr<T>>		m_Clients;
		std::atomic<bool>					m_bTerminateFlag;
		std::atomic<uint32_t>				m_ulClientID;

		std::thread							*m_pServerThread;

		void listen();
		virtual void client(T *pSock) = 0;

		inline void			addClientID() { m_ulClientID++; }
		inline uint32_t		clientID() const { return m_ulClientID; };
		inline bool			isRunning() { return !m_bTerminateFlag; }
	};

	template<typename T>
	Application<T>::Application(std::string &host, int port) :
		szHost{ host },
		nPort{ port },
		m_pServer{ new T(szHost, nPort) },
		m_bTerminateFlag{ true },
		m_ulClientID{ 0 },
		m_pServerThread{ nullptr } 
	{
		
		int err;
		if (0 != (err = winpoxi::Sock::startup()))
			std::cout << "WSAStartup failed err: " << err << std::endl;
		else
			m_Clients.reserve(cnstMaxClients);
	}

	template<typename T>
	Application<T>::~Application()
	{
		if(m_pServer)
			delete m_pServer;

		m_pServer = nullptr;

		if (m_pServerThread)
			delete m_pServerThread;

		m_Clients.clear();
		winpoxi::Sock::cleanup();
	}

	template<typename T>
	bool Application<T>::start()
	{
		if (m_pServer)
		{
			if (m_pServer->create())
			{
				if (nullptr != (m_pServerThread = new std::thread(&Application<T>::listen, this)))
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

	template<typename T>
	void Application<T>::stop()
	{
		m_bTerminateFlag = true;

		if (m_pServer)
			m_pServer->close();


		std::this_thread::sleep_for(std::chrono::milliseconds(cnstTimeoutRcv*2));
		std::cout << "Server stopped" << std::endl;

	}

	template<typename T>
	void Application<T>::listen()
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
					m_Clients.push_back(std::make_unique<T>(szHost, nPort, newSck));

					auto pClient = m_Clients.back().get();

					pClient->setTimeoutRcv(cnstTimeoutRcv);
					pClient->setTimeoutRcv(cnstTimeoutSnd);

					Application<T>::addClientID();

					std::thread clientThread(&Application<T>::client, this, pClient);
					clientThread.detach();
				}
			}
			else
			{
				std::cout << "No more client are admitted" << std::endl;
				std::cin.get();
				Application<T>::stop();
			}
		}
	}

} //server