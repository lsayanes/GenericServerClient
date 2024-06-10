#pragma once

namespace winposix
{
	struct Udp : public Sock
	{
		Udp(const std::string &strHost, int nPort);
		Udp(const std::string &strHost, int nPort, SOCKET sck);
		virtual ~Udp();

		bool create();

		int			snd(void* pBuff, int nLen) const;
		int			rcv(void* pBuff, int nLen);

		void		peerName(char* strHostIp);

		bool connect();

		bool listen() const { return true; }
		SOCKET accept() const;
		size_t select() const { return 0; }

	private:
		struct sockaddr_in	m_SockAddrBind;
		struct sockaddr_in	m_SockAddrSndRcv;

	};

}//winpoxi