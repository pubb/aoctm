// GroupingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ATM.h"
#include "GroupingDlg.h"
#include "grouping/grouping.h"
#include "playerinputdlg.h"

// CGroupingDlg dialog

IMPLEMENT_DYNAMIC(CGroupingDlg, CDialog)

CGroupingDlg::CGroupingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGroupingDlg::IDD, pParent)
{

}

CGroupingDlg::~CGroupingDlg()
{
}

void CGroupingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAYERS, m_Players);
	DDX_Control(pDX, IDC_SELECTED, m_Selected);
	DDX_Control(pDX, IDC_GROUP1, m_Group1);
	DDX_Control(pDX, IDC_GROUP2, m_Group2);
}


BEGIN_MESSAGE_MAP(CGroupingDlg, CDialog)
//	ON_WM_CREATE()
	ON_NOTIFY(NM_DBLCLK, IDC_PLAYERS, &CGroupingDlg::OnNMDblclkPlayers)
	ON_BN_CLICKED(IDOK, &CGroupingDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_SELECTED, &CGroupingDlg::OnNMDblclkSelected)
	ON_BN_CLICKED(IDC_RESET, &CGroupingDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_NEW, &CGroupingDlg::OnBnClickedInput)
	ON_BN_CLICKED(IDC_KEEP, &CGroupingDlg::OnBnClickedKeep)
END_MESSAGE_MAP()


// CGroupingDlg message handlers

BOOL CGroupingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Players.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 120, -1);

	for(int i = 0; i < theApp.Players.GetCount(); i++)
		InsertPlayers(theApp.Players[i]->NickNames[0], i);

	m_Selected.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 155, -1);
	m_Selected.InsertColumn(1, _T("Rating"), LVCFMT_LEFT, 155, -1);

	int	index;
	CString	str;
	if(theApp.bCurrent)
		for(int i = 0; i < 8 && theApp.CurrentPlayers[i] >= 0; i++)
		{
			index = theApp.CurrentPlayers[i];
			InsertSelected(theApp.Players[index]->NickNames[0], theApp.Players[index]->Rating, index);
		}

	return TRUE;
}

void CGroupingDlg::OnNMDblclkPlayers(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	NMLISTVIEW *p = (NMLISTVIEW *)pNMHDR;
	if(p->iItem < 0)
		return;

	CString	name = m_Players.GetItemText(p->iItem, 0);
	int	index = m_Players.GetItemData(p->iItem);
	CString	str;

	int selected = m_Selected.GetItemCount();
	
	if(selected >= 8)
		return;

	for(int i = 0; i < selected; i++)
		if(index == m_Selected.GetItemData(i))
			return;
	
	InsertSelected(name, theApp.Players[index]->Rating, index);
}

void CGroupingDlg::OnBnClickedOk()
{
	int ratings[8];
	int group1[4];
	int group2[4];
	int group1TotalScore;
	int group2TotalScore;
	int delta;

	int i, selected = m_Selected.GetItemCount();

	if(selected <= 0)
		return;

	//huangjie, 07-08-03, only even count is permitted
	if(selected %2 != 0)
	{
		AfxMessageBox(_T("even player count is permitted only"), MB_OK);
		return;
	}

	memset(group1, 0, sizeof(group1));
	memset(group2, 0, sizeof(group2));


	for(i = 0; i < selected; i++)
		ratings[i] = _ttoi(m_Selected.GetItemText(i, 1));
	
	Intermediator	inter(selected, ratings);
	inter.Grouping(group1, group2, group1TotalScore, group2TotalScore, delta);

	m_Group1.ResetContent();
	m_Group2.ResetContent();
		
	for(i = 0; i < 4 /* XXX, maybe less than */; i++)
		if(group1[i] > 0)
			m_Group1.AddString(m_Selected.GetItemText(group1[i] - 1, 0));
	for(i = 0; i < 4; i++)
		if(group2[i] > 0)
			m_Group2.AddString(m_Selected.GetItemText(group2[i] - 1, 0));

	CString	str;
	str.Format(_T("Group1: %d, Group2: %d, Delta Rating: %d"), group1TotalScore, group2TotalScore, delta);
	GetDlgItem(IDC_MSG)->SetWindowText(str);
}

void CGroupingDlg::OnNMDblclkSelected(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLISTVIEW *p = (NMLISTVIEW *)pNMHDR;

	if(p->iItem >= 0)
	{
		m_Selected.DeleteItem(p->iItem);
	}

	*pResult = 0;
}

void CGroupingDlg::OnBnClickedReset()
{
	m_Selected.DeleteAllItems();
	theApp.ClearCurrentPlayers();

	m_Group1.ResetContent();
	m_Group2.ResetContent();
	GetDlgItem(IDC_MSG)->SetWindowText(_T("Double click, Double click, Double click, ..."));
}

void CGroupingDlg::OnBnClickedInput()
{
	CPlayerInputDlg	dlg;
	if(dlg.DoModal() == IDOK)
	{
		CPlayer	*p = new CPlayer();
		p->NickNames.Add(dlg.m_Name);
		p->Rating = p->InitRating = dlg.m_iRating;
		
		INT_PTR index = theApp.Players.GetFirstSamePlayer(p->NickNames[0]);
		if(index < 0)
		{
			index = theApp.Players.Add(p);
			InsertPlayers(dlg.m_Name, (int)index);
		}
		else	//update the rating to the input one
		{
			theApp.Players[index]->Rating = dlg.m_iRating;
			if(UpdateSelected(dlg.m_iRating, (int)index))
				return;
		}
		InsertSelected(dlg.m_Name, dlg.m_iRating, (int)index);
	}
}

void CGroupingDlg::OnBnClickedKeep()
{
	int selected = m_Selected.GetItemCount();

	if(selected == 0)
		return;

	theApp.ClearCurrentPlayers();

	for(int i = 0; i < selected; i++)
		theApp.CurrentPlayers[i] = m_Selected.GetItemData(i);

	theApp.bCurrent = true;
}

void CGroupingDlg::InsertPlayers(CString name, int data)
{
	int index = m_Players.GetItemCount();
	m_Players.InsertItem(index, name);
	m_Players.SetItemData(index, data);	//to save index to the PlayerDB
}

void CGroupingDlg::InsertSelected(CString name, int rating, int data)
{
	int selected = m_Selected.GetItemCount();

	if(selected >= 8)
		return;

	CString	str;

	m_Selected.InsertItem(selected, name);
	str.Format(_T("%d"), rating);
	m_Selected.SetItemText(selected, 1, str);
	m_Selected.SetItemData(selected, data);
}

bool CGroupingDlg::UpdateSelected(int rating, int data)
{
	CString str;
	for(int i = 0; i < m_Selected.GetItemCount(); i++)
	{
		if(m_Selected.GetItemData(i) == data)
		{
			str.Format(_T("%d"), rating);
			m_Selected.SetItemText(i, 1, str);
			return true;
		}
	}
	return false;
}