// ConfigPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "ConfigPlayerDlg.h"
#include "csvfile.h"

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
	ON_BN_CLICKED(IDC_SAVE2CSV, &CConfigPlayerDlg::OnBnClickedSave2csv)
END_MESSAGE_MAP()


// CConfigPlayerDlg message handlers
BOOL CConfigPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;

	m_List.InsertColumn(0, _T("COM"), LVCFMT_LEFT, 20, -1);
	m_List.InsertColumn(1, _T("Player ID (check for COM)"), LVCFMT_LEFT, 150, -1);
	m_List.InsertColumn(2, _T("InitRating"), LVCFMT_LEFT, 50, -1);
	for(int i = 0; i < 8; i++)	//8 nicks shown at most
	{
		str.Format(_T("Nick%d"), i);
		m_List.InsertColumn(i + 3, str, LVCFMT_LEFT, 120, -1);
	}

	for(int i = 0; i < theApp.Players.GetCount(); i++)
	{
		CPlayer * player = theApp.Players[i];
		m_List.InsertItem(i, _T(""));
		//pubb, 07-10-25, set checked for COM players
		m_List.SetCheckbox(i, 0, player->IsComputer);
		m_List.SetItemText(i, 1, player->NickNames[0]);
		str.Format(_T("%d"), player->InitRating);
		m_List.SetItemText(i, 2, str);
		for(int j = 1; j < player->NickNames.GetCount(); j++)
		{
			m_List.SetItemText(i, j + 2, player->NickNames[j]);
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
	//XXX, pubb, 07-10-26, assuming Dirty is true
	//not a good way. better check whether Checkbox state is changed, but it's too costful for coding
	/*
	if(!Dirty)
	{
		OnOK();
		return;
	}
	*/

	//07-10-25, clear names buffer in case of multi OK pressed in one session
	names.RemoveAll();

	for(i = 0; i < m_List.GetItemCount(); i++)
	{
		index = m_List.GetItemData(i);
		if(m_List.GetItemText(i, 1).IsEmpty())
		{
			for(j = 0; j < 8 && m_List.GetItemText(i, j + 3).IsEmpty(); j++)
			{
			}
			if(j == 8)
			{
				AfxMessageBox(_T("There's Empty Name!"));
				return;
			}
			m_List.SetItemText(i, 0, m_List.GetItemText(i, j + 3));
			m_List.SetItemText(i, j + 3, _T(""));
		}
	
		str = m_List.GetItemText(i, 1);
		//pubb, 07-10-25, also save to names to check duplicate
		if(IsDuplicatedName(str))
			return;
		CPlayer * player = theApp.Players[index];
		//pubb, 07-10-26, COM checkbox
		switch(m_List.GetItemCheckedState(i, 0))
		{
		case 0:
			player->IsComputer = false;
			break;
		case 1:
			player->IsComputer = true;
			break;
		default:
			AfxMessageBox(_T("BAD checkbox state!"));
			return;
		}
		player->NickNames.RemoveAll();
		player->NickNames.Add(str);
		rating = _ttoi(m_List.GetItemText(i, 2));
		player->InitRating = (rating == 0 ? DEF_RATING : rating);
		for(j = 0; j < 8; j++)
		{
			str = m_List.GetItemText(i, j + 3);
			if(!str.IsEmpty())
			{
				//pubb, 07-10-25, also save to names to check duplicate
				if(IsDuplicatedName(str))
					return;
				player->NickNames.Add(str);
			}
		}
	}

	theApp.Players.Update(true);

	OnOK();
}

void CConfigPlayerDlg::OnNMClickPlayerList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (!pNMIA)
		return;
	int nItem = pNMIA->iItem;
	int	nSubItem = pNMIA->iSubItem;

	if(nSubItem != 0)
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
	//pubb, bugfix, dont know when comes the bug.
	m_List.InsertItem(index, _T(""));
	CString str;
	m_List.SetCheckbox(index, 0, player->IsComputer);
	m_List.SetItemText(index, 1, player->NickNames[0]);
	str.Format(_T("%d"), player->InitRating);
	m_List.SetItemText(index, 2, str);
	//end of insert
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

/*
 * pubb, 07-10-25, check duplicate name.
 * if found, report error by afxmessagebox, if not, add it to the names buffer for later check
 */
bool CConfigPlayerDlg::IsDuplicatedName(CString name)
{
	for(int i = 0; i < names.GetCount(); i++)
	{
		if(name == names[i])
		{
			AfxMessageBox(_T("Duplicate Name: ") + name);
			return true;
		}
	}
	names.Add(name);
	return false;
}

void CConfigPlayerDlg::OnBnClickedSave2csv()
{
	CCsvFile	csv;
	if(csv.Open(_T("players")))
	{
		csv.WriteList(m_List);
		OnOK();
	}
}
