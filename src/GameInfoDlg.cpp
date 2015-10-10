// GameInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "GameInfoDlg.h"


// CGameInfoDlg dialog

IMPLEMENT_DYNAMIC(CGameInfoDlg, CDialog)

CGameInfoDlg::CGameInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGameInfoDlg::IDD, pParent)
{

}

CGameInfoDlg::~CGameInfoDlg()
{
}

void CGameInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGameInfoDlg, CDialog)
END_MESSAGE_MAP()


// CGameInfoDlg message handlers

BOOL CGameInfoDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();

	if(!m_Recgame)
		return false;

	SetWindowText(_T("recgame ") + m_Recgame->RecordTime.Format(_T("%Y-%m-%d %H'%M'%S")));

	if(!m_Recgame->LoadChatInfo(theApp.Engine))
		return false;

	CString str;
	for(int i = 0; i < m_Recgame->ChatB4Game.GetCount(); i++)
	{
		CChatInfo * chat = m_Recgame->ChatB4Game[i];
		str += m_Recgame->Players[chat->Player_Num].Name + _T(": ") + chat->Content + _T("\r\n");
	}
	SetDlgItemText(IDC_CHATB4GAME, str);

	str.Empty();
	for(int i = 0; i < m_Recgame->ChatInGame.GetCount(); i++)
	{
		CChatInfo * chat = m_Recgame->ChatInGame[i];
		str += chat->Time.Format(_T("%H'%M'%S : ")) + m_Recgame->Players[chat->Player_Num].Name + _T(": ") + chat->Content + _T("\r\n");
	}
	SetDlgItemText(IDC_CHATINGAME, str);
	
	return true;
}