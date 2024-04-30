
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>


#pragma warning (disable : 4786)
#include <stdexcept>

#include <string.h>
#include <optional>

#include "Sock.h"
#include "Udp.h"

namespace winpoxi
{

	Udp::Udp(const std::string &strHost, int nPort) : Sock(strHost, nPort),
		m_SockAddrBind { 0 },
		m_SockAddrSndRcv{ 0 }
	{
	}

	Udp::Udp(const std::string &strHost, int nPort, SOCKET sck) : Sock(strHost, nPort, sck)
	{
	}

	Udp::~Udp()
	{
	}

	bool Udp::create()
	{
		m_sck = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		int nBind = -1;
		bool bRet = false;

		if (INVALID_SOCKET != m_sck)
		{
			char strServerIp[256] = "\x0";
			char strLocalIp[256] = "\x0";
			struct hostent* pServerName;
			struct hostent* pLocalName;

			int optval = 1;
			::setsockopt(m_sck, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof optval);

			memset((void*)&m_SockAddrSndRcv, '\0', sizeof(struct sockaddr_in));
			m_SockAddrSndRcv.sin_addr.s_addr = htonl(INADDR_ANY);

			if (!m_strHost.empty())
			{
				pServerName = ::gethostbyname(m_strHost.c_str());

				if (NULL != pServerName)
					strcpy(strServerIp, inet_ntoa(*((struct in_addr*)pServerName->h_addr)));
				else
					strcpy(strServerIp, m_strHost.c_str());

				m_SockAddrSndRcv.sin_addr.s_addr = inet_addr(strServerIp);
			}


			m_SockAddrSndRcv.sin_family = AF_INET;
			m_SockAddrSndRcv.sin_port = htons(static_cast<u_short>(m_nPort));

			memset((void*)&m_SockAddrBind, '\0', sizeof(struct sockaddr_in));

			m_SockAddrBind.sin_family = AF_INET;
			m_SockAddrBind.sin_port = htons(static_cast<u_short>(m_nPort));

			m_SockAddrBind.sin_addr.s_addr = INADDR_ANY;

			strcpy(strLocalIp, "0.0.0.0");
			pLocalName = ::gethostbyname(static_cast<const char*>(strLocalIp));

			if (pLocalName)
			{
				strcpy(strLocalIp, ::inet_ntoa(*((struct in_addr*)pLocalName->h_addr)));
				m_SockAddrBind.sin_addr.s_addr = ::inet_addr(strLocalIp);

				if (-1 == (nBind = ::bind(m_sck, (struct sockaddr*)&m_SockAddrBind, sizeof(struct sockaddr_in))))
				{
					close();
					throw std::exception(strerror(errno));
				}
				
				bRet = true;
			}
		}

		return bRet;

	}

	bool Udp::connect()
	{
		return true;
	}

	int	Udp::snd(void* pBuff, int nLen) const
	{
		return ::sendto(m_sck, static_cast<const char*>(pBuff), nLen, 0, (struct sockaddr*)&m_SockAddrSndRcv, sizeof(struct sockaddr_in));
	}
	
	int	Udp::rcv(void* pBuff, int nLen)
	{
		int nRet = 0;

		if (INVALID_SOCKET != m_sck)
		{
			int nSockLen = sizeof(struct sockaddr_in);
			nRet = ::recvfrom(m_sck, static_cast<char*>(pBuff), nLen, 0, (struct sockaddr*)&m_SockAddrSndRcv, &nSockLen);
		}

		return nRet;
	}

	void Udp::peerName(char* strHostIp)
	{
		strHostIp[0] = 0;
		if (INVALID_SOCKET != m_sck)
		{
			int nSockLen = sizeof(struct sockaddr_in);
			::getpeername(m_sck, (SOCKADDR*)&m_SockAddrSndRcv, &nSockLen);
			strcpy(strHostIp, inet_ntoa(m_SockAddrSndRcv.sin_addr)); //htons(SenderAddr.sin_port)
		}
	}



}