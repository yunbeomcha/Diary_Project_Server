// ClientSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SocketServer.h"
#include "CClientSocket.h"
#include "CListenSocket.h"
#include "SocketServerDlg.h"
#include "afxdialogex.h"
#include <sstream>


CClientSocket::CClientSocket()
{
	m_client = _T("");
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket ��� �Լ�
void CClientSocket::SetListenSocket(CAsyncSocket* pSocket)
{
	m_pListenSocket = pSocket;
}
// CClientSocket ��� �Լ�


void CClientSocket::OnClose(int nErrorCode)
{
	CSocket::OnClose(nErrorCode);
	//����Ǿ� �ִ� Ŭ���̾�Ʈ�� ����Ǹ� ����Ʈ�ڽ��� �ش� ���̵��
	//�ش� ������ �Ҵ� �����ؾ��Ѵ�.
	CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->CloseClientSocket(this);
}

void CClientSocket::OnReceive(int nErrorCode)
{
	//���۸� �Ҵ�
	TCHAR strBuffer[1024];
	::ZeroMemory(strBuffer, sizeof(strBuffer));
	
	if (Receive(strBuffer, sizeof(strBuffer)) > 0) { //Ŭ�󿡼� send�� ������ �Ҵ�޴´�
		CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
		//���̵�,��¥ �������� send�� �߱⶧���� �����ڷ� �ɰ��� ���� �������ش� 
		CString str = CString(strBuffer);
		CString strId;
		CString strDate;
		AfxExtractSubString(strId, str, 0, ',');
		AfxExtractSubString(strDate, str, 1, ',');
		//����Ʈ�ڽ��� �̹� �ش� ���̵� �ִ����� �˱����� index ���
		int index = pMain->clientList->FindStringExact(-1, strId);
		// ���ٸ� ����Ʈ�ڽ��� �߰��Ѵ�
		if (index == LB_ERR){
			pMain->clientList->AddString(strId + m_client);
		}
		// �Խ��ǿ� �ش� ���� �߰��Ѵ�
		insertForum(strId, strDate);
		// �Խ��� ����
		pMain->ResetForum(CString("WEEK"));
	}
	CSocket::OnReceive(nErrorCode);
}

void CClientSocket::setODBC(SQLHDBC mh_odbc)
{
	//������ odbc���� �� Ŭ������ ������������� ��������
	this->mh_odbc = mh_odbc;
}
void CClientSocket::insertForum(CString strId, CString strDate)
{
	//insert ���� ���� ��������� ���ؼ� db���� ��ȸ��
	CString queryStr;
	queryStr.Format
	(L"insert into svrinfo (select id, date, title, contents from diaryinfo where id = '%s' and date = '%s');",
		strId, strDate);

	HSTMT h_statement;
	RETCODE ret_code;

	if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_STMT, mh_odbc, &h_statement)) {
		SQLSetStmtAttr(h_statement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);
		//���� ����
		ret_code = SQLExecDirect(h_statement, (SQLWCHAR*)(const wchar_t*)queryStr, SQL_NTS);
		//������ �ø��� �Ϸ� �ƴҽ� �ø��� ����
		if (ret_code == SQL_SUCCESS || ret_code == SQL_SUCCESS_WITH_INFO)
		{
			AfxMessageBox(_T("�ø��� �Ϸ�"), MB_ICONINFORMATION, 0);
		}
		else
		{
			AfxMessageBox(_T("�ø��� ����"));
		}
		SQLFreeHandle(SQL_HANDLE_STMT, h_statement);
	}
}

void CClientSocket::setCLIENT(CString m_client)
{
	//client�ּҰ� ������(Ŭ���̾�Ʈ ����� ����Ʈ���� ������� �Ǳ� ������ �ʿ�)
	this->m_client = m_client;
}
