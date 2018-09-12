#pragma once

namespace ReBot
{
	class CBSHanlder : public INetConnectorCallBacker
	{
	public:
		CBSHanlder();
		~CBSHanlder();


		//call back event while net listener heart beat.
		virtual		INT32				OnConnectorHeartBeart() ;
		//call back event while received new message.
		virtual		UINT32				OnReceieved(INT32 n32NetSessionID, BYTE *pbData, UINT32 un32DataLen) ;
		//call back event while disconnect from server.
		virtual VOID					OnDisconnect(INT32 n32NetSessionID) ;
		//call backer event while connected to server.
		virtual	INT32					OnConnected(INT32 n32NetSessionID) ;
		//call backer event while connect fail.
		virtual	INT32					OnConnectFail(INT32 n32NetSessionID, INT32 n32ErrorCode);

	private:
		INetConnector *m_piNetConnector;
	};

}

