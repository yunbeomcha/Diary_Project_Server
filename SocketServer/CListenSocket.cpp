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
	//odbc�� ����
	pClient->setODBC(this->mh_odbc);

	if (Accept(*pClient)) {//accept �����϶�
		CString str;
		//pclient ������ ���� �������ְ�
		pClient->SetListenSocket(this);
		//�����͸���Ʈ�� �մ´�
		m_ptrClientSocketList.AddTail(pClient);
		//���� ���� ���̾˷α� ������
		CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
		//str�� pclient ���ڰ� �������ְ�
		str.Format(_T("(%d)"), (int)m_ptrClientSocketList.Find(pClient));
		//�ش� Ŭ������ �Ѱ���
		pClient->setCLIENT(str);
	}
	else {//accept ������ �ƴҶ�
		delete pClient; //�Ҵ��Ѱ� ����� ���� �޽��� ���
		AfxMessageBox(_T("ERROR : Failed can't accept new Client!"));
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

void CListenSocket::CloseClientSocket(CSocket* pClient)
{
	//���� ����Ʈ�� �ش� Ŭ���̾�Ʈ�� �ִ��� ã�´�
	POSITION pos;
	pos = m_ptrClientSocketList.Find(pClient);

	if (pos != NULL) {
		//�ش� ���� ����
		if (pClient != NULL) {
			pClient->ShutDown();
			pClient->Close();
		}

		CSocketServerDlg* pMain = (CSocketServerDlg*)AfxGetMainWnd();
		CString str1, str2;
		UINT indx = 0, posNum;
		pMain->clientList->SetCurSel(0);
		//����Ʈ�ڽ��� ù��° �ε������� ����
		while (indx < pMain->clientList->GetCount()) {
			posNum = (int)m_ptrClientSocketList.Find(pClient);
			pMain->clientList->GetText(indx, str1);
			str2.Format(_T("%d"), posNum);
			//����Ʈ�ڽ� ���ڿ��� �ش� Ŭ���̾�Ʈ�� ���ڿ� ��ġ�ϴ� ���� �ִٸ� �����
			if (str1.Find(str2) != -1) {
				AfxMessageBox(str1 + str2);
				pMain->clientList->DeleteString(indx);
				break;
			}
			indx++;
		}
		//�����͸���Ʈ�� �̾��� �ִ� ���ϵ� �����
		m_ptrClientSocketList.RemoveAt(pos);
		delete pClient;
	}
}

void CListenSocket::SendAllMessage(TCHAR* pszMessage)
{
	//Ȥ�� �����Ͱ� �÷����� �ʾ������� ����� ���� �Լ�
	//���� Ȯ�ο� �Լ��� ū ����� �����ϴ�.
	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;
	//���� ������ ����Ʈ������ �ϳ��� ���ư��鼭 �����Ͱ� ���۵��� �ʾҴٸ�
	//�����޽����� ����
	while (pos != NULL) {
		pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL) {

			int checkLenOfData = pClient->Send(pszMessage, lstrlen(pszMessage) * 2);
			if (checkLenOfData != lstrlen(pszMessage) * 2) {
				AfxMessageBox(_T("�Ϻ� �����Ͱ� ���������� ���۵��� ���߽��ϴ�!"));
			}
		}
	}
}



void CListenSocket::setODBC(SQLHDBC mh_odbc)
{	//odbc �ڵ鷯 ����
	this->mh_odbc = mh_odbc;
}
