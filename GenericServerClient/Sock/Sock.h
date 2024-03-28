#pragma once


namespace winpoxi
{

	class Sock
	{
	public:
		Sock(const std::string strHost, int nPort);
		Sock(const std::string strHost, int nPort, SOCKET sk);
		Sock(const Sock&);

		//Sock(const Sock&) = delete;
		Sock(Sock&&) = delete;
		Sock& operator=(const Sock&) = delete;
		Sock& operator=(Sock&&) = delete;

		virtual ~Sock();

		void				close();
		bool				setTimeoutRcv(int nMillisec);
		bool				setTimeoutSnd(int nMillisec);
		bool				down(bool bSnd = true, bool bRcv = true);
		const std::string	getSocketError();

		virtual bool		create() = 0;
		virtual bool		connect() = 0;

		virtual int			snd(void* pBuff, int nLen) const = 0;
		virtual int			rcv(void* pBuff, int nLen) = 0;

		virtual bool listen() const = 0;
		virtual SOCKET accept() const = 0;
		virtual size_t select() const = 0;


		inline bool isOk() const { return (m_sck > 0); }
		inline const char* host() const { return m_strHost.c_str(); }
		inline int	port() const { return m_nPort; }


		static int	startup(uint8_t majv = 2, uint8_t minv = 2);
		static void cleanup();

	protected:
		SOCKET			m_sck;
		int				m_nPort;
		std::string		m_strHost;

	};

} //winpoxi