

#include <iostream>
#include <string>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>

#include <optional>

#include "Sock.h"
#include "Tcp.h"
#include "Udp.h"


namespace winposix
{
	
	Sock::Sock(const std::string &strHost, int nPort):
		m_sck{ INVALID_SOCKET },
		m_nPort{ nPort },
		m_strHost{ strHost }
	{
		
		struct			hostent* Host;

		Host = ::gethostbyname(strHost.c_str());
		if (NULL != Host)
			m_strHost = ::inet_ntoa(*((struct in_addr*)Host->h_addr));
	}

	Sock::Sock(const std::string &strHost, int nPort, SOCKET sk) : Sock(strHost, nPort)
	{
		m_sck = sk;
	}

	Sock::Sock(const Sock& cpy)
	{
		m_sck = cpy.m_sck;
		m_nPort = cpy.m_nPort;
		m_strHost = cpy.m_strHost;

		#if defined(_DEBUG)
			std::cout << "**** DEBUG: Sock Copia **** " << std::endl;
		#endif
	}

	Sock::~Sock()
	{
		down();
		close();
	}


	bool Sock::setTimeoutRcv(int nMillisec)
	{
		return SOCKET_ERROR != ::setsockopt(m_sck, SOL_SOCKET, SO_RCVTIMEO, (const char*)&nMillisec, sizeof(nMillisec)) ? true : false;
	}

	bool Sock::setTimeoutSnd(int nMillisec)
	{
		return SOCKET_ERROR != ::setsockopt(m_sck, SOL_SOCKET, SO_SNDTIMEO, (const char*)&nMillisec, sizeof(nMillisec)) ? true : false;
	}

	void Sock::close()
	{
		if(m_sck > 0)
			::closesocket(m_sck);
		
		m_sck = INVALID_SOCKET;
	}

	bool Sock::down(bool bSnd, bool bRcv)
	{
		bool	bRet = false;

		if (m_sck > 0)
		{
			int		nHow = -1;
			if (true == bSnd && true == bRcv)
				nHow = SD_BOTH;
			if (true == bSnd && false == bRcv)
				nHow = SD_SEND;
			if (false == bSnd && true == bRcv)
				nHow = SD_RECEIVE;

			if (-1 != nHow)
			{
				if (SOCKET_ERROR != ::shutdown(m_sck, nHow))
					bRet = true;
			}
		}

		return bRet;
	}

	const std::string Sock::getSocketError()
	{
		int nErr;
		socklen_t optlen = sizeof(nErr);
		if (SOCKET_ERROR == getsockopt(m_sck, SOL_SOCKET, SO_ERROR, (char*)&nErr, &optlen))
			return "Failed to retrieve socket error";
		

		if (0 == nErr)
			return "No error";

		return std::string(strerror(nErr));
	}


	int Sock::startup(uint8_t majv, uint8_t minv)
	{
		WSADATA	WSockData;
		return  ::WSAStartup(MAKEWORD(majv, minv), &WSockData);
	}

	void Sock::cleanup()
	{
		::WSACleanup();
	}



}; //winpoxi