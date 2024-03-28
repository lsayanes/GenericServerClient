#pragma once


namespace winpoxi
{
	struct Tcp : public Sock
	{
		Tcp(const std::string strHost, int nPort);
		Tcp(const std::string strHost, int nPort, SOCKET sck);
		virtual ~Tcp();

		bool		create();
		bool		connect();
		int			snd(void* pBuff, int nLen) const;
		int			rcv(void* pBuff, int nLen);

		bool listen() const;
		SOCKET accept() const;
		size_t select() const; //TODO
	};

}//sock