// ConfigPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "ConfigPlayerDlg.h"


// CConfigPlayerDlg dialog

IMPLEMENT_DYNAMIC(CConfigPlayerDlg, CDialog)

CConfigPlayerDlg::CConfigPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigPlayerDlg::IDD, pParent), Dirty(false)
{

}

CConfigPlayerDlg::~CConfigPlayerDlg()
{
}

void CConfigPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYER_LIST, m_List);
}


BEGIN_MESSAGE_MAP(CConfigPlayerDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CConfigPlayerDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_PLAYER_LIST, &CConfigPlayerDlg::OnNMClickPlayerList)
	ON_BN_CLICKED(IDC_PLAYER_ADD, &CConfigPlayerDlg::OnBnClickedPlayerAdd)
	ON_BN_CLICKED(IDC_PLAYER_DELETE, &CConfigPlayerDlg::OnBnClickedPlayerDelete)
END_MESSAGE_MAP()


// CConfigPlayerDlg message handlers
BOOL CConfigPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;

	m_List.InsertColumn(0, _T("Player ID"), LVCFMT_LEFT, 120, -1);
	m_List.InsertColumn(1, _T("InitRating"), LVCFMT_LEFT, 50, -1);
	for(int i = 0; i < 8; i++)	//8 nicks shown at most
	{
		str.Format(_T("Nick%d"), i);
		m_List.InsertColumn(i + 2, str, LVCFMT_LEFT, 120, -1);
	}

	for(int i = 0; i < theApp.Players.GetCount(); i++)
	{
		m_List.InsertItem(i, theApp.Players[i]->NickNames[0]);
		str.Format(_T("%d"), theApp.Players[i]->InitRating);
		m_List.SetItemText(i, 1, str);
		for(int j = 1; j < theApp.Players[i]->NickNames.GetCount(); j++)
		{
			m_List.SetItemText(i, j + 1, theApp.Players[i]->NickNames[j]);
		}
		m_List.SetItemData(i, i);
	}

	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
#ifdef	XLISTCTRL_OLD
	m_List.SetEditable();
#endif

	return TRUE;
}
void CConfigPlayerDlg::OnBnClickedOk()
{
	int i, j;
	INT_PTR index;
	int rating;
	CString str;

#ifdef	XLISTCTRL_OLD
	m_List.EndEdit();
#endif

	if(!Dirty)
	{
		OnOK();
		return;
	}

	for(i = 0; i < m_List.GetItemCount(); i++)
	{
		index = m_List.GetItemData(i);
		if(m_List.GetItemText(i, 0).IsEmpty())
		{
			for(j = 0; j < 8 && m_List.GetItemText(i, j + 2).IsEmpty(); j++)
			{
			}
			if(j == 8)
			{
				AfxMessageBox(_T("Empty Name?"));
				return;
			}
			m_List.SetItemText(i, 0, m_List.GetItemText(i, j + 2));
			m_List.SetItemText(i, j + 2, _T(""));
		}
	
		CPlayer * player = theApp.Players[index];
		player->NickNames.RemoveAll();
		player->NickNames.Add(m_List.GetItemText(i, 0));
		rating = _ttoi(m_List.GetItemText(i, 1));
		player->InitRating = (rating == 0 ? DEF_RATING : rating);
		for(j = 0; j < 8; j++)
		{
			str = m_List.GetItemText(i, j + 2);
			if(!str.IsEmpty())
			{
				player->NickNames.Add(str);
			}
		}
	}

	theApp.Players.Update();
	theApp.Players.SetDirty();

	OnOK();
}

void CConfigPlayerDlg::OnNMClickPlayerList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (!pNMIA)
		return;
	int nItem = pNMIA->iItem;
	int	nSubItem = pNMIA->iSubItem;

#ifdef	XLISTCTRL_OLD
	m_List.StartEdit(nItem, nSubItem);
#else
	m_List.SetEdit(nItem, nSubItem);
#endif

	Dirty = true;

	*pResult = 0;
}

void CConfigPlayerDlg::OnBnClickedPlayerAdd()
{
#ifdef	XLISTCTRL_OLD
	m_List.EndEdit();
#endif

	CPlayer * player = new CPlayer;
	player->NickNames.Add(_T(""));	//to pass 'CPlayerDatabase::Add()'

	int index = m_List.GetItemCount();
	m_List.InsertItem(index, _T("new player"));
	m_List.SetItemData(index, (DWORD)theApp.Players.Add(player));
	m_List.EnsureVisible(index, true);

	Dirty = true;
}

//XXX, delete one player per time
void CConfigPlayerDlg::OnBnClickedPlayerDelete()
{
#ifdef	XLISTCTRL_OLD
	m_List.EndEdit();
#endif

	POSITION pos = m_List.GetFirstSelectedItemPosition();
	int nItem;
	if(pos)
		nItem = m_List.GetNextSelectedItem(pos);
	else
		return;

	INT_PTR index = m_List.GetItemData(nItem);
	m_List.DeleteItem(nItem);

	int	data;
	for(int i = 0; i < m_List.GetItemCount(); i++)
	{
		data = m_List.GetItemData(i);
		if(data > index)
		{
			m_List.SetItemData(i, data - 1);
		}
	}
	delete theApp.Players[index];
	theApp.Players.RemoveAt(index);
	
	Dirty = true;
}