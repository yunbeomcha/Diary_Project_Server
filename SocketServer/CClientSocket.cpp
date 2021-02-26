// ClientSocket.cpp : 구현 파일입니다.
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


// CClientSocket 멤버 함수
void CClientSocket::SetListenSocket(CAsyncSocket* pSocket)
{
	m_pListenSocket = pSocket;
}
// CClientSocket 멤버 함수


void CClientSocket::OnClose(int nErrorCode)
{
	CSocket::OnClose(nErrorCode);
	//연결되어 있던 클라이언트가 종료되면 리스트박스의 해당 아이디와
	//해당 소켓을 할당 해제해야한다.
	CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->CloseClientSocket(this);
}

void CClientSocket::OnReceive(int nErrorCode)
{
	//버퍼를 할당
	TCHAR strBuffer[1024];
	::ZeroMemory(strBuffer, sizeof(strBuffer));
	
	if (Receive(strBuffer, sizeof(strBuffer)) > 0) { //클라에서 send한 내용을 할당받는다
		CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
		//아이디,날짜 형식으로 send를 했기때문에 구분자로 쪼개어 각각 저장해준다 
		CString str = CString(strBuffer);
		CString strId;
		CString strDate;
		AfxExtractSubString(strId, str, 0, ',');
		AfxExtractSubString(strDate, str, 1, ',');
		//리스트박스에 이미 해당 아이디가 있는지를 알기위해 index 사용
		int index = pMain->clientList->FindStringExact(-1, strId);
		// 없다면 리스트박스에 추가한다
		if (index == LB_ERR){
			pMain->clientList->AddString(strId + m_client);
		}
		// 게시판에 해당 글을 추가한다
		insertForum(strId, strDate);
		// 게시판 갱신
		pMain->ResetForum(CString("WEEK"));
	}
	CSocket::OnReceive(nErrorCode);
}

void CClientSocket::setODBC(SQLHDBC mh_odbc)
{
	//설정한 odbc값을 이 클래스의 멤버변수값으로 저장해줌
	this->mh_odbc = mh_odbc;
}
void CClientSocket::insertForum(CString strId, CString strDate)
{
	//insert 쿼리 설정 멤버변수를 통해서 db에서 조회함
	CString queryStr;
	queryStr.Format
	(L"insert into svrinfo (select id, date, title, contents from diaryinfo where id = '%s' and date = '%s');",
		strId, strDate);

	HSTMT h_statement;
	RETCODE ret_code;

	if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_STMT, mh_odbc, &h_statement)) {
		SQLSetStmtAttr(h_statement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);
		//쿼리 실행
		ret_code = SQLExecDirect(h_statement, (SQLWCHAR*)(const wchar_t*)queryStr, SQL_NTS);
		//성공시 올리기 완료 아닐시 올리기 실패
		if (ret_code == SQL_SUCCESS || ret_code == SQL_SUCCESS_WITH_INFO)
		{
			AfxMessageBox(_T("올리기 완료"), MB_ICONINFORMATION, 0);
		}
		else
		{
			AfxMessageBox(_T("올리기 실패"));
		}
		SQLFreeHandle(SQL_HANDLE_STMT, h_statement);
	}
}

void CClientSocket::setCLIENT(CString m_client)
{
	//client주소값 가져옴(클라이언트 종료시 리스트에서 사라져야 되기 때문에 필요)
	this->m_client = m_client;
}
