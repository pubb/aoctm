// FeeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "FeeDlg.h"


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
END_MESSAGE_MAP()


// CFeeDlg message handlers
BOOL CFeeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_List.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, 120, -1);
	m_List.InsertColumn( 1, _T("PlayCount from July"), LVCFMT_LEFT, 120, -1);
	m_List.InsertColumn( 2, _T("PaidFee"), LVCFMT_LEFT, 70, -1);
	m_List.InsertColumn( 3, _T("RestFee"), LVCFMT_LEFT, 70, -1);

	int nItem, cost = theApp.Players.GetAllCostFee(), count = theApp.Players.GetAllPlayCountFromJuly();
	float eachcost = count == 0 ? 0 : (float)1.0 * cost / count;
	
	CString str;
	for(int i = 0; i < theApp.Players.GetCount(); i++)
	{
		CPlayer * player = theApp.Players[i];
		nItem = m_List.InsertItem(i, player->NickNames[0]);
		str.Format(_T("%d"), player->GetPlayCountFromJuly());
		m_List.SetItemText(nItem, 1, str);
		str.Format(_T("%d"), player->GetPaidFee());
		m_List.SetItemText(nItem, 2, str);
		str.Format(_T("%d"), (int)(player->GetPaidFee() - eachcost * player->GetPlayCountFromJuly()));
		m_List.SetItemText(nItem, 3, str);
	}

	return TRUE;
}