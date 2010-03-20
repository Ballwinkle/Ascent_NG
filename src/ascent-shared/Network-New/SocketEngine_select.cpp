/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2010 Ascent Team <http://www.ascentemulator.net/>
 *
 * This software is  under the terms of the EULA License
 * All title, including but not limited to copyrights, in and to the AscentNG Software
 * and any copies there of are owned by ZEDCLANS INC. or its suppliers. All title
 * and intellectual property rights in and to the content which may be accessed through
 * use of the AscentNG is the property of the respective content owner and may be protected
 * by applicable copyright or other intellectual property laws and treaties. This EULA grants
 * you no rights to use such content. All rights not expressly granted are reserved by ZEDCLANS INC.
 *
 */

#include "Network.h"

#ifdef NETLIB_SELECT

SelectEngine::SelectEngine()
{
	m_running = true;
	new SocketDeleter;
}

SelectEngine::~SelectEngine()
{

}

void SelectEngine::AddSocket(BaseSocket * s)
{
	m_socketLock.Acquire();
	m_sockets.insert( make_pair( s->GetFd(), s ) );
	m_socketLock.Release();
}

void SelectEngine::RemoveSocket(BaseSocket * s)
{
	m_socketLock.Acquire();
	m_sockets.erase(s->GetFd());
	m_socketLock.Release();
}

void SelectEngine::MessageLoop()
{
	FD_SET readable;
	FD_SET writable;
	FD_SET exception;
	map<int, BaseSocket*>::iterator itr, it2;
	int nfds;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 50000;
	while(m_running)
	{
		if(!m_sockets.size())
		{
			Sleep(20);
			continue;
		}

		FD_ZERO(&readable);
		FD_ZERO(&writable);
		FD_ZERO(&exception);

        m_socketLock.Acquire();
		for(itr = m_sockets.begin(); itr != m_sockets.end(); ++itr)
		{
			FD_SET( itr->second->GetFd(), itr->second->Writable() ? &writable : &readable );
		}
		m_socketLock.Release();

		nfds = select(FD_SETSIZE, &readable, &writable, &exception, &timeout);
		if(nfds > 0)
		{
			m_socketLock.Acquire();
			for(it2 = m_sockets.begin(); it2 != m_sockets.end();)
			{
				itr = it2++;
				if( FD_ISSET( itr->second->GetFd(), &exception ) )
					itr->second->OnError(errno);
				else if( FD_ISSET( itr->second->GetFd(), &readable ) )
					itr->second->OnRead(0);
				else if( FD_ISSET( itr->second->GetFd(), &writable ) )
					itr->second->OnWrite(0);
			}
			m_socketLock.Release();
		}
	}
}

void SelectEngine::SpawnThreads()
{
	launch_thread(new SocketEngineThread(this));
}

void SelectEngine::Shutdown()
{
	m_running = false;
	m_socketLock.Acquire();
	BaseSocket * s;
	for(map<int, BaseSocket*>::iterator itr = m_sockets.begin(); itr != m_sockets.end(); )
	{
		s = itr->second;
		++itr;
		s->Delete();
	}
	m_socketLock.Release();

	/* shutdown the socket deleter */
	sSocketDeleter.Kill();

	/* delete the socket deleter */
	delete SocketDeleter::getSingletonPtr();

	/* delete us */
	delete this;
}

void SelectEngine::WantWrite(BaseSocket * s)
{

}

#endif
