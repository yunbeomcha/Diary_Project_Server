#pragma once


// CViewDiary 대화 상자

class CViewDiary : public CDialogEx
{
	DECLARE_DYNAMIC(CViewDiary)

public:
	CViewDiary(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CViewDiary();
	SQLHDBC mh_odbc;
	CString m_date;
	CString m_title;
	CString m_id;
	void setODBC(SQLHDBC mh_odbc);
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	DECLARE_MESSAGE_MAP()
public:
	CString m_staticContents;
	CString m_staticDate;
	CString m_staticTitle;
	virtual BOOL OnInitDialog();
	void setDiary();
	CString m_staticID;
};
