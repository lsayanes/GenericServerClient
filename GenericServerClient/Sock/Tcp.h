#pragma once


namespace winposix
{
	struct Tcp : public Sock
	{
		Tcp(const std::string &strHost, int nPort);
		Tcp(const std::string &strHost, int nPort, SOCKET sck);
		Tcp(const Tcp&);
		
		Tcp(Tcp&&) = delete;
		Tcp& operator=(const Tcp&) = delete;
		Tcp& operator=(Tcp&&) = delete;
		
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