// FeeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "FeeDlg.h"
#include "csvfile.h"

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
END_MESSAGE_MAP()


// CFeeDlg message handlers
BOOL CFeeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//pubb, 07-09-04, to setup only to count plays with fee
	theApp.Players.m_bCountFee = true;
	theApp.Players.Update();

	m_List.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, 120, -1);
	m_List.InsertColumn( 1, _T("PlayCount from July"), LVCFMT_LEFT, 120, -1);
	m_List.InsertColumn( 2, _T("PaidFee"), LVCFMT_LEFT, 70, -1);
	m_List.InsertColumn( 3, _T("RestFee"), LVCFMT_LEFT, 70, -1);

	int nItem, cost = theApp.Players.GetAllCostFee(), count = theApp.Players.GetAllPlayCount();
	float eachcost = count == 0 ? 0 : (float)1.0 * cost / count;
	
	CString str;
	for(int i = 0; i < theApp.Players.GetCount(); i++)
	{
		CPlayer * player = theApp.Players[i];
		nItem = m_List.InsertItem(i, player->NickNames[0]);
		str.Format(_T("%d"), player->PlayCount);
		m_List.SetItemText(nItem, 1, str);
		str.Format(_T("%d"), player->GetPaidFee());
		m_List.SetItemText(nItem, 2, str);
		str.Format(_T("%d"), (int)(player->GetPaidFee() - eachcost * player->PlayCount));
		m_List.SetItemText(nItem, 3, str);
	}

	//pubb, 07-09-04, restore
	theApp.Players.m_bCountFee = false;
	theApp.Players.Update();

	return TRUE;
}
void CFeeDlg::OnBnClickedSave()
{
	CFileDialog dlg(FALSE, _T(".csv"), _T("fee.csv"), OFN_OVERWRITEPROMPT, _T("CSV (Comma delimited) | *.csv"));
	if(dlg.DoModal() == IDCANCEL)
		return;
	CString	csvfile = dlg.GetPathName();

	CCsvFile	csv(csvfile, CFile::modeWrite | CFile::modeCreate);

	CStringArray	a;
	const CHeaderCtrl	* header = m_List.GetHeaderCtrl();
	int i;

	HDITEM hdi;
    enum   { sizeOfBuffer = 256 };
    TCHAR  lpBuffer[sizeOfBuffer];
    
    hdi.mask = HDI_TEXT;
    hdi.pszText = lpBuffer;
    hdi.cchTextMax = sizeOfBuffer;

	a.SetSize(header->GetItemCount());

	for(i = 0; i < header->GetItemCount(); i++)
	{
		header->GetItem(i, &hdi);
		a[i] = lpBuffer;
	}
	csv.WriteLine(a);

	for(int i = 0; i < m_List.GetItemCount(); i++)
	{
		for(int j = 0; j < m_List.GetColumnCount(); j++)
			a[j] = m_List.GetItemText(i, j);
		csv.WriteLine(a);
	}

	OnOK();
}
void CFeeDlg::OnBnClickedCancel()
{
	OnCancel();
}
