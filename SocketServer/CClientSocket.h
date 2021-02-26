#pragma once

class CClientSocket : public CSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();
	SQLHDBC mh_odbc;
	CAsyncSocket *m_pListenSocket;
	void SetListenSocket(CAsyncSocket* pSocket);
	void OnClose(int nErrorCode);
	void OnReceive(int nErrorCode);

	void setODBC(SQLHDBC mh_odbc);
	void insertForum(CString strId, CString strDate);
//	void ResetForum();
	CString m_client;
	void setCLIENT(CString m_client);
};
