// ViewDiary.cpp: 구현 파일
//

#include "stdafx.h"
#include "SocketServer.h"
#include "ViewDiary.h"
#include "afxdialogex.h"


// CViewDiary 대화 상자

IMPLEMENT_DYNAMIC(CViewDiary, CDialogEx)

CViewDiary::CViewDiary(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_staticContents(_T(""))
	, m_staticDate(_T(""))
	, m_staticTitle(_T(""))
	, m_staticID(_T(""))
{

}

CViewDiary::~CViewDiary()
{
}

void CViewDiary::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_CONTENTS, m_staticContents);
	DDX_Text(pDX, IDC_STATIC_DATE, m_staticDate);
	DDX_Text(pDX, IDC_STATIC_TITLE, m_staticTitle);
	DDX_Text(pDX, IDC_STATIC_ID, m_staticID);
}


BEGIN_MESSAGE_MAP(CViewDiary, CDialogEx)
END_MESSAGE_MAP()


// CViewDiary 메시지 처리기

void CViewDiary::setODBC(SQLHDBC mh_odbc)
{
	// TODO: 여기에 구현 코드 추가.
	this->mh_odbc = mh_odbc;
}


BOOL CViewDiary::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	setDiary();      // 데이터 세팅 함수 호출
	return TRUE;  
}


void CViewDiary::setDiary()
{
	m_staticDate = m_date;
	m_staticTitle = m_title;
	m_staticID = m_id;
	CString queryStr;
	queryStr.Format
	(L"select contents from svrinfo where id = '%s' and date = '%s';",                    //얻어온 값을 통해 데이터베이스에서 정보를 가져옴
		m_id, m_date);
	wchar_t contents[1000];
	unsigned short record_state[10];
	CString s;

	HSTMT h_statement;
	RETCODE ret_code;

	if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_STMT, mh_odbc, &h_statement)) {
		unsigned long record_num = 0;
		SQLSetStmtAttr(h_statement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)sizeof(contents), 0);
		SQLSetStmtAttr(h_statement, SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_ROWVER, SQL_IS_UINTEGER);
		SQLSetStmtAttr(h_statement, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, SQL_IS_UINTEGER);
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_NUMBER, (SQLPOINTER)10, SQL_IS_UINTEGER);
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_STATUS_PTR, record_state, 0);

		SQLBindCol(h_statement, 1, SQL_WCHAR, &contents, sizeof(wchar_t) * 1000, NULL);
		ret_code = SQLExecDirect(h_statement, (SQLWCHAR*)(const wchar_t*)queryStr, SQL_NTS);
		ret_code = SQLFetchScroll(h_statement, SQL_FETCH_NEXT, 0);
		if (ret_code != SQL_NO_DATA)
			m_staticContents = contents;                                  //일기 내용 세팅
		else
			m_staticContents = "";
		UpdateData(FALSE);
		SQLFreeHandle(SQL_HANDLE_STMT, h_statement);
	}
}
