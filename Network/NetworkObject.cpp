#include <assert.h>

#include "NetworkObject.h"
#include "Session.h"

NetworkObject::NetworkObject()
{
	m_pSession = NULL;
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
		return NULL;
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
