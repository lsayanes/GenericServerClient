#pragma once

namespace winpoxi
{
	struct Udp : public Sock
	{
		explicit Udp(const std::string strHost, int nPort);
		virtual ~Udp();

		bool create();

		int			snd(void* pBuff, int nLen) const;
		int			rcv(void* pBuff, int nLen);

		void		peerName(char* strHostIp, SOCKET sk = -1);

		bool connect();

		//TODO :check it..
		bool listen() const { return true; }
		SOCKET accept() const { return m_sck;  }

	private:
		struct sockaddr_in	m_SockAddrBind;
		struct sockaddr_in	m_SockAddrSndRcv;

	};

}//winpoxi