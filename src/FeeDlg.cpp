// FeeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "FeeDlg.h"
#include "csvfile.h"
#include "feeindlg.h"

// CFeeDlg dialog

IMPLEMENT_DYNAMIC(CFeeDlg, CDialog)

CFeeDlg::CFeeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFeeDlg::IDD, pParent)
{

}

CFeeDlg::~CFeeDlg()
{
}

void CFeeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
}


BEGIN_MESSAGE_MAP(CFeeDlg, CDialog)
	ON_BN_CLICKED(IDC_SAVE, &CFeeDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_CANCEL, &CFeeDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CFeeDlg::OnNMDblclkList)
END_MESSAGE_MAP()


// CFeeDlg message handlers
BOOL CFeeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_List.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, 120, -1);
	m_List.InsertColumn( 1, _T("PlayCount from July"), LVCFMT_LEFT, 120, -1);
	m_List.InsertColumn( 2, _T("Paid"), LVCFMT_LEFT, 70, -1);
	m_List.InsertColumn( 3, _T("Saving"), LVCFMT_LEFT, 70, -1);

	Refresh();

	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	return TRUE;
}
void CFeeDlg::OnBnClickedSave()
{
	CCsvFile	csv;
	if(csv.Open(_T("fee")))
	{
		csv.WriteList(m_List);
		OnOK();
	}
}

void CFeeDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CFeeDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	NMLISTVIEW *p = (NMLISTVIEW *)pNMHDR;
	if(p->iItem < 0)
		return;

	CFeeInDlg dlg;
	dlg.m_pPlayer = theApp.Players.GetAt(theApp.Players.GetFirstSamePlayer(m_List.GetItemText(p->iItem, 0)));
	dlg.DoModal();
	
	Refresh();
}

void CFeeDlg::Refresh(void)
{
	m_List.DeleteAllItems();

	//pubb, 07-09-04, to setup only to count plays with fee
	theApp.Players.Update(false, FIRSTGAME_FEE_TIME);

	int nItem, cost = theApp.Config.GetAllCostFee(), count = theApp.Players.GetAllPlayCount();
	float eachcost = count == 0 ? 0 : (float)1.0 * cost / count;
	int income = 0;

	CString str;
	for(int i = 0; i < theApp.Players.GetCount(); i++)
	{
		CPlayer * player = theApp.Players[i];
		if(player->IsComputer)
			continue;
		nItem = m_List.InsertItem(i, player->NickNames[0]);
		str.Format(_T("%d"), player->PlayCount);
		m_List.SetItemText(nItem, 1, str);
		income += player->GetPaidFee();
		str.Format(_T("%d"), player->GetPaidFee());
		m_List.SetItemText(nItem, 2, str);
		str.Format(_T("%d"), (int)(player->GetPaidFee() - eachcost * player->PlayCount));
		m_List.SetItemText(nItem, 3, str);
	}

	//pubb, 07-09-04, restore
	theApp.Players.Update(false);

	str.Format(_T("Money In: %d"), income);
	SetDlgItemText(IDC_MONEY_IN_TOTAL, str);
	str.Format(_T("Money Out: %d"), cost);
	SetDlgItemText(IDC_MONEY_OUT_TOTAL, str);
	str.Format(_T("Money Left: %d"), income - cost);
	SetDlgItemText(IDC_MONEY_LEFT_TOTAL, str);
	str.Format(_T("Play Count: %d"), count);
	SetDlgItemText(IDC_PLAYCOUNT_TOTAL, str);
}