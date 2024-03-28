#pragma once

namespace server
{
	struct Application
	{
		std::string					szHost;
		int							nPort;

		explicit Application(std::string host, int port);
		
		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		virtual ~Application();

		bool start();
		void stop();

		static constexpr	uint32_t	cnstMaxClients{ 1024 };
		static constexpr	int			cnstTimeoutRcv{ 1000 * 1 };
		static constexpr	int			cnstTimeoutSnd{ 1000 * 1 };

	protected:
		winpoxi::Sock*									m_pServer;
		std::vector<std::unique_ptr<winpoxi::Sock>>		m_Clients;
		std::atomic<bool>								m_bTerminateFlag;
		std::atomic<uint32_t>							m_ulClientID;

		std::thread										*m_pServerThread;

		void listen();
		virtual void client(winpoxi::Sock *pSock) = 0;

		inline void			addClientID() { m_ulClientID++; }
		inline uint32_t		clientID() const { return m_ulClientID; };
		inline bool			isRunning() { return !m_bTerminateFlag; }

	};
} //server