#include "stdafx.h"
#include "SocketServer.h"
#include "CListenSocket.h"
#include "CClientSocket.h"
#include "SocketServerDlg.h"

CListenSocket::CListenSocket()
{
}

CListenSocket::~CListenSocket()
{
}

void CListenSocket::OnAccept(int nErrorCode)
{
	CClientSocket* pClient = new CClientSocket;
	//odbc값 설정
	pClient->setODBC(this->mh_odbc);

	if (Accept(*pClient)) {//accept 성공일때
		CString str;
		//pclient 변수에 소켓 설정해주고
		pClient->SetListenSocket(this);
		//포인터리스트에 잇는다
		m_ptrClientSocketList.AddTail(pClient);
		//현재 서버 다이알로그 가져옴
		CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
		//str에 pclient 숫자값 저장해주고
		str.Format(_T("(%d)"), (int)m_ptrClientSocketList.Find(pClient));
		//해당 클래스에 넘겨줌
		pClient->setCLIENT(str);
	}
	else {//accept 성공이 아닐때
		delete pClient; //할당한거 지우고 에러 메시지 출력
		AfxMessageBox(_T("ERROR : Failed can't accept new Client!"));
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

void CListenSocket::CloseClientSocket(CSocket* pClient)
{
	//소켓 리스트에 해당 클라이언트가 있는지 찾는다
	POSITION pos;
	pos = m_ptrClientSocketList.Find(pClient);

	if (pos != NULL) {
		//해당 소켓 정리
		if (pClient != NULL) {
			pClient->ShutDown();
			pClient->Close();
		}

		CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
		CString str1, str2;
		UINT indx = 0, posNum;
		pMain->clientList->SetCurSel(0);
		//리스트박스의 첫번째 인덱스부터 시작
		while (indx < pMain->clientList->GetCount()) {
			posNum = (int)m_ptrClientSocketList.Find(pClient);
			pMain->clientList->GetText(indx, str1);
			str2.Format(_T("%d"), posNum);
			//리스트박스 문자열에 해당 클라이언트의 숫자와 일치하는 값이 있다면 지운다
			if (str1.Find(str2) != -1) {
				AfxMessageBox(str1 + str2);
				pMain->clientList->DeleteString(indx);
				break;
			}
			indx++;
		}
		//포인터리스트에 이어져 있던 소켓도 지운다
		m_ptrClientSocketList.RemoveAt(pos);
		delete pClient;
	}
}

void CListenSocket::SendAllMessage(TCHAR* pszMessage)
{
	//혹시 데이터가 올려지지 않았을때를 대비해 만든 함수
	//에러 확인용 함수라 큰 기능은 없습니다.
	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;
	//소켓 포인터 리스트값들을 하나씩 돌아가면서 데이터가 전송되지 않았다면
	//에러메시지를 띄운다
	while (pos != NULL) {
		pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL) {

			int checkLenOfData = pClient->Send(pszMessage, lstrlen(pszMessage) * 2);
			if (checkLenOfData != lstrlen(pszMessage) * 2) {
				AfxMessageBox(_T("일부 데이터가 정상적으로 전송되지 못했습니다!"));
			}
		}
	}
}



void CListenSocket::setODBC(SQLHDBC mh_odbc)
{	//odbc 핸들러 얻음
	this->mh_odbc = mh_odbc;
}
