#include <assert.h>
#include "NetworkObject.h"
#include "Session.h"

NetworkObject::NetworkObject()
{
	m_pSession = NULL;
	memset(m_szIP, 0, 32);
	m_usPort = 0;
}

NetworkObject::~NetworkObject()
{
}

VOID NetworkObject::Disconnect( BOOL bGracefulDisconnect )
{
	if( m_pSession )
	{
		m_pSession->Disconnect( bGracefulDisconnect );
	}
}

BOOL NetworkObject::Send( BYTE *pMsg, WORD wSize )
{
	if( !m_pSession || m_pSession->HasDisconnectOrdered() ) return FALSE;

	return m_pSession->Send( pMsg, wSize );
}

BOOL NetworkObject::SendEx( DWORD dwNumberOfMessages, BYTE **ppMsg, WORD *pwSize )
{
	if( !m_pSession || m_pSession->HasDisconnectOrdered() ) return FALSE;

	return m_pSession->SendEx( dwNumberOfMessages, ppMsg, pwSize );
}

VOID NetworkObject::Redirect( NetworkObject *pNetworkObject )
{
	assert( pNetworkObject != NULL && "NULL  Redirect"  );
	assert( m_pSession != NULL );

	m_pSession->BindNetworkObject( pNetworkObject );
}

char* NetworkObject::GetIP()
{
	if( m_pSession )
	{
		return m_pSession->GetIP();
	}
	else
	{
		return m_szIP;
	}
}

u_short NetworkObject::GetPort()
{
	if (m_pSession)
	{
		return m_pSession->GetPort();
	}
	else
	{
		return m_usPort;
	}
}




int NetworkObject::GetNetIdx()
{
	if( m_pSession )
	{
		return m_pSession->GetIndex();
	}
	else
	{
		return -1;
	}
}


VOID NetworkObject::SetSession(Session *pSession)
{
	if (NULL == pSession && NULL != m_pSession)
	{
		char* tmpIp = m_pSession->GetIP();
		memcpy_s(m_szIP, 32, tmpIp, 16);
		m_usPort = m_pSession->GetPort();
	}
	m_pSession = pSession;
}
