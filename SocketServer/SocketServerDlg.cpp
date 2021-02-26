
// SocketServerDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "SocketServer.h"
#include "SocketServerDlg.h"
#include "CClientSocket.h"
#include "afxdialogex.h"
#include "ViewDiary.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "odbc32.lib")
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSocketServerDlg 대화 상자



CSocketServerDlg::CSocketServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SOCKETSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSocketServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}

BEGIN_MESSAGE_MAP(CSocketServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST1, &CSocketServerDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON_MONTH, &CSocketServerDlg::OnBnClickedButtonMonth)
	ON_BN_CLICKED(IDC_BUTTON_YEAR, &CSocketServerDlg::OnBnClickedButtonYear)
	ON_BN_CLICKED(IDC_BUTTON_WEEK, &CSocketServerDlg::OnBnClickedButtonWeek)
END_MESSAGE_MAP()


// CSocketServerDlg 메시지 처리기

BOOL CSocketServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	clientList = (CListBox*)GetDlgItem(IDC_CLIENT_LIST);

	if (m_ListenSocket.Create(5000, SOCK_STREAM)) { // 소켓생성
		if (!m_ListenSocket.Listen()) {
			AfxMessageBox(_T("ERROR:Listen() return False"));
		}
	}
	else {
		AfxMessageBox(_T("ERROR:Failed to create server socket!"));
	}
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	//ODBC 기술을 사용하기 위한 환경 구성. 구성된 환경 정보에 대한 핸들 값은 mh_environment
	if (SQL_ERROR != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &mh_environment))
	{
		//사용할 ODBC 버전 정보를 설정
		SQLSetEnvAttr(mh_environment, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
		SQLSetEnvAttr(mh_environment, SQL_ATTR_CP_MATCH, (SQLPOINTER)SQL_CP_RELAXED_MATCH, SQL_IS_INTEGER);

		//ODBC 함수를 사용하기 위한 정보 구성. 정보에 대한 핸들 값 mh_odbc에 저장
		if (SQL_ERROR != SQLAllocHandle(SQL_HANDLE_DBC, mh_environment, &mh_odbc))
		{
			RETCODE ret_code = SQLConnect(mh_odbc,
				(SQLWCHAR*)L"dsnTest", SQL_NTS,		// 접속할 DSN 설정
				(SQLWCHAR*)L"root", SQL_NTS,				// 접속에 사용할 ID
				(SQLWCHAR*)L"0000", SQL_NTS);	// 접속에 사용할 Password
			if (ret_code == SQL_SUCCESS || ret_code == SQL_SUCCESS_WITH_INFO)
			{
				//ODBC를 사용하여 DB접속에 성공한 경우
				m_connect_flag = 1;
				m_ListenSocket.setODBC(this->mh_odbc);
				ResetForum(CString("WEEK"));
				return 1;
			}
			else {
				//접속에 실패한 경우, 구성했던 메모리 제거
				if (mh_odbc != SQL_NULL_HDBC)
					SQLFreeHandle(SQL_HANDLE_DBC, mh_odbc);
				if (mh_environment != SQL_NULL_HENV)
					SQLFreeHandle(SQL_HANDLE_ENV, mh_environment);
			}

		}
		if (!m_connect_flag)
		{
			//연결에 실패한 경우 메세지 박스 출력 및 로그인 창 종료
			MessageBox(L"Server Connect Error");
			this->EndDialog(IDOK);
		}
	}
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CSocketServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSocketServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CSocketServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSocketServerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	//서버프로그램 종료시
	POSITION pos;
	//포인터리스트의 헤드 값부터 돌아감
	pos = m_ListenSocket.m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;
	while (pos != NULL) {
		//리스트 하나씩 돌아가면서
		pClient = (CClientSocket*)m_ListenSocket.m_ptrClientSocketList.GetNext(pos);
		//소켓들을 정리해줌
		if (pClient != NULL) {
			pClient->ShutDown();
			pClient->Close();

			delete pClient;
		}
	}
	//listen소켓또한 정리해줍니다.
	m_ListenSocket.ShutDown();
	m_ListenSocket.Close();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CSocketServerDlg::ResetForum(CString between)//week, year, month를 넣냐에 따라서 기간이 설정됨
{
	CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
	CString queryStr;
	CString str;
	//쿼리설정, 매개변수 between 값에 따라서 불러오는 데이터 양이 설정됨
	queryStr.Format(L"SELECT * FROM svrinfo WHERE date BETWEEN DATE_ADD(NOW(),INTERVAL -1 %s ) AND NOW() order by date desc;",between);

	unsigned short record_state[20];
	UserData diarydata[20];
	HSTMT h_statement;
	RETCODE ret_code;
	memset(diarydata, 0, sizeof(diarydata));
	pMain->m_List.ResetContent();
	if (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_STMT, mh_odbc, &h_statement)) {
		
		unsigned int record_num = 1;
		SQLSetStmtAttr(h_statement, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)15, SQL_IS_UINTEGER);
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)sizeof(diarydata), 0);
		SQLSetStmtAttr(h_statement, SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_ROWVER, SQL_IS_UINTEGER);
		SQLSetStmtAttr(h_statement, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, SQL_IS_UINTEGER);
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_NUMBER, (SQLPOINTER)20, SQL_IS_UINTEGER);
		SQLSetStmtAttr(h_statement, SQL_ATTR_ROW_STATUS_PTR, record_state, 0);
		//diartdata에서 첫번째 배열에 해당 쿼리문의 결과값들을 할당
		SQLBindCol(h_statement, 1, SQL_WCHAR, &diarydata[0].id, sizeof(wchar_t) * 30, NULL);
		SQLBindCol(h_statement, 2, SQL_WCHAR, &diarydata[0].date, sizeof(wchar_t) * 20, NULL);
		SQLBindCol(h_statement, 3, SQL_WCHAR, &diarydata[0].title, sizeof(wchar_t) * 100, NULL);
		//쿼리실행
		ret_code = SQLExecDirect(h_statement, (SQLWCHAR*)(const wchar_t*)queryStr, SQL_NTS);
		//받아온 row마다 돌아가면서 리스트 박스에 설정해줍니다.
		while (ret_code = SQLFetchScroll(h_statement, SQL_FETCH_NEXT, 0) != SQL_NO_DATA)
		{
			for (unsigned int i = 0; i < record_num; i++)
			{
				//읽어오는 도중 에러가 생겼는지 확인
				if (record_state[i] != SQL_ROW_DELETED &&
					record_state[i] != SQL_ROW_ERROR)
				{
						str.Format(L"%s    %s(%s)",
						diarydata[i].date, diarydata[i].title, diarydata[i].id);
				}
				//리스트에 추가
				pMain->m_List.InsertString(-1, str);
			}
		}
		//게시판 갱신합니다.
		pMain->UpdateData(FALSE);
		SQLFreeHandle(SQL_HANDLE_STMT, h_statement);
	}
}


void CSocketServerDlg::OnLbnSelchangeList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_List.GetCurSel();
	CString strTmp;

	if (nIndex != LB_ERR)
	{
		m_List.GetText(nIndex, strTmp);           //게시판 클릭시에 해당 스트링값을 얻음
		UpdateData(false);
	}
	strTmp.Replace(_T("    "), _T("@"));          // 스트링 형식의 구분문자를 @로 바꿔 나눠지는 부분을 통일함
	strTmp.Replace(_T("("), _T("@"));
	strTmp.Replace(_T(")"), _T(""));
	CString date, title, id;                       //@로 끊어진 문자정보에서 값들을 나눠서 얻음
	AfxExtractSubString(date, strTmp, 0, '@');
	AfxExtractSubString(title, strTmp, 1, '@');
	AfxExtractSubString(id, strTmp, 2, '@');
	
	CViewDiary dlg = new CViewDiary;                 // 일기를 보여주는 클래스에 값들을 저장하고 다이얼로그 생성
	dlg.setODBC(mh_odbc);
	dlg.m_date = date;
	dlg.m_title = title;
	dlg.m_id = id;

	dlg.DoModal();
}


void CSocketServerDlg::OnBnClickedButtonMonth()
{
	ResetForum(CString("MONTH"));//한달 기간으로 게시판 조회
}


void CSocketServerDlg::OnBnClickedButtonYear()
{
	ResetForum(CString("YEAR"));//1년 기간으로 게시판 조회
}


void CSocketServerDlg::OnBnClickedButtonWeek()
{
	ResetForum(CString("WEEK"));//1주 기간으로 게시판 조회
}
