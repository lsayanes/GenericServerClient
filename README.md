# GenericServerClient 
  (Windows only)


Not much to say here, but the idea is to use the same interface for TCP and UDP:

        Sock
         |
        / \
      Tcp  Udp

Server App (class ServerApp : public server::Application) It is listening on host:port, opens thread for each client and sends ACK for each message sent by the client:
  
   **Application Contructor**
	
	   Application::Application(std::string host, int port):
	  		szHost{ host },
	  		nPort{ port },
	  		m_pServer{ new winpoxi::Tcp(szHost, nPort) },
	  		m_bTerminateFlag{ true },
	  		m_ulClientID{ 0 },
	  		m_pServerThread{ nullptr }
	    {
	      [...]
	    }


**Server side**

    //start thread
  	bool Application::start()
  	{
  		if (m_pServer)
  		{
  			if (m_pServer->create())
  			{
  				if (nullptr != (m_pServerThread = new std::thread(&Application::listen, this)))
  				{
  					m_pServerThread->detach();
  					return true;
  				}
        }
        [...]
  	}
   
**Client**

The client side is created using the same interface:

	Tcp* pSock = new Tcp(szHost, nPort);

	if (pSock)
	{
		if (pSock->create())
		{
			if (pSock->connect())
				start(pSock);
			else
				std::cout << "connect failed: " << pSock->getSocketError() << std::endl;
		}
		else
		{
			std::cout << "create failed: " << pSock->getSocketError() << std::endl;
		}
		delete pSock;
	}

	Sock::cleanup();
