

#include <string>
#include <optional>
#include <vector>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>


#include "Sock.h"
#include "Tcp.h"

namespace winpoxi
{

	Tcp::Tcp(const std::string strHost, int nPort) : Sock(strHost, nPort)
	{

	}

	Tcp::Tcp(const std::string strHost, int nPort, SOCKET sck) : Sock(strHost, nPort, sck)
	{
	}

	Tcp::~Tcp()
	{
	}

	bool Tcp::create() 
	{
		bool		bRet = false;

		m_sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (INVALID_SOCKET != m_sck)
		{
			int optval = 1;
			bRet = SOCKET_ERROR != setsockopt(m_sck, SOL_SOCKET, SO_REUSEADDR, (const char*)(&optval), sizeof(optval));
		}

		return bRet;

	}

	int Tcp::snd(void* pBuff, int nLen) const
	{

		return ::send(m_sck, static_cast<const char*>(pBuff), nLen, 0);
	}

	int Tcp::rcv(void* pBuff, int nLen)
	{
		/*
			FILE *p = fopen("tcp.txt", "a+");
			fprintf(p, "s:%d b:%p: l:%d:\n", m_sck, pBuff, nLen);
			fclose(p);
		*/

		return ::recv(m_sck, static_cast<char*>(pBuff), nLen, 0);

	}


	bool Tcp::connect()
	{
		struct	sockaddr_in		Address;

		Address.sin_family = AF_INET;
		Address.sin_port = htons(m_nPort);
		Address.sin_addr.s_addr = inet_addr(m_strHost.c_str());
		memset(&(Address.sin_zero), 0, 8);

		return (SOCKET_ERROR != ::connect(m_sck, (struct sockaddr*)&Address, sizeof(struct	sockaddr_in)));
	}

	bool Tcp::listen() const
	{
		struct sockaddr_in		me { 0 };
		int				nYes = 1;
		u_long			ulIP;

		ulIP = 0 == m_strHost.empty() ? INADDR_ANY : ::inet_addr(m_strHost.c_str());

		if (-1 != ::setsockopt(m_sck, SOL_SOCKET, SO_REUSEADDR | SO_DONTLINGER, (char*)&nYes, sizeof(int)))
		{
			me.sin_family = AF_INET;
			me.sin_port = htons(m_nPort);
			me.sin_addr.s_addr = htonl(ulIP);
			memset(&(me.sin_zero), '\0', 8);

			int nRet = ::bind(m_sck, (struct sockaddr*)&me, sizeof(struct sockaddr));

			if (-1 != nRet)
				return (0 == ::listen(m_sck, SOMAXCONN));
		}

		return false;
	}

	SOCKET Tcp::accept() const
	{
		int						nSize = sizeof(struct sockaddr_in);
		struct sockaddr_in		Their { 0 };

		SOCKET sckNew;
		if (SOCKET_ERROR != (sckNew = ::accept(m_sck, (struct sockaddr*)&Their, &nSize)))
		{
			//::send(sckNew, "Greetings humans!\x0", 18, 0);
			return sckNew;
		}

		return INVALID_SOCKET;
	}

	size_t Tcp::select() const
	{
		u_long iMode = 1;
		if (NO_ERROR == ioctlsocket(m_sck, FIONBIO, &iMode))
		{
			struct sockaddr_in		me { 0 };
			int				nYes = 1;
			u_long			ulIP;

			ulIP = 0 == m_strHost.empty() ? INADDR_ANY : ::inet_addr(m_strHost.c_str());

			if (-1 != ::setsockopt(m_sck, SOL_SOCKET, SO_REUSEADDR | SO_DONTLINGER, (char*)&nYes, sizeof(int)))
			{
				me.sin_family = AF_INET;
				me.sin_port = htons(m_nPort);
				me.sin_addr.s_addr = htonl(ulIP);
				memset(&(me.sin_zero), '\0', 8);

				int nRet = ::bind(m_sck, (struct sockaddr*)&me, sizeof(struct sockaddr));
				-1 != nRet && (nRet = ::listen(m_sck, 10) == -1);

				if (-1 != nRet)
				{
					fd_set read_fds{ 0 };
					std::vector<SOCKET> vctClientFds;
					
					FD_ZERO(&read_fds);
					FD_SET(m_sck, &vctClientFds);

					for (int cfd : vctClientFds)
						FD_SET(cfd, &vctClientFds);
					
					if (::select(FD_SETSIZE, &read_fds, nullptr, nullptr, nullptr) >= 0)
					{
						// new incoming connections
						if (FD_ISSET(m_sck, &read_fds)) 
						{

						}

					}
				}
			}
		}

		return 0;
	}

}//winpoxi