// Config.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "ConfigChargeDlg.h"


// CConfigChargeDlg dialog

IMPLEMENT_DYNAMIC(CConfigChargeDlg, CDialog)

CConfigChargeDlg::CConfigChargeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigChargeDlg::IDD, pParent)
	, m_iCharge(0)
{

}

CConfigChargeDlg::~CConfigChargeDlg()
{
}

void CConfigChargeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHARGE_LIST, m_Charge_List);
	DDX_Text(pDX, IDC_CHARGE_MONEY, m_iCharge);
	DDV_MinMaxInt(pDX, m_iCharge, 0, 1000);
}


BEGIN_MESSAGE_MAP(CConfigChargeDlg, CDialog)
	ON_BN_CLICKED(IDC_CHARGE_ADD, &CConfigChargeDlg::OnBnClickedChargeAdd)
	ON_BN_CLICKED(IDC_CHARGE_DELETE, &CConfigChargeDlg::OnBnClickedChargeDelete)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CHARGE_MONEY_SPIN, &CConfigChargeDlg::OnDeltaposChargeMoneySpin)
END_MESSAGE_MAP()


// CConfigChargeDlg message handlers

BOOL CConfigChargeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Charge_List.InsertColumn(0, _T("Date"), LVCFMT_LEFT, 120, -1);
	m_Charge_List.InsertColumn(1, _T("Money"), LVCFMT_LEFT, 50, -1);
	Refresh();

	m_iCharge=INIT_MONEY;

	CSpinButtonCtrl *pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_CHARGE_MONEY_SPIN);
	pSpinCtrl->SetRange32(MIN_MONEY,MAX_MONEY);
	pSpinCtrl->SetPos32(m_iCharge);

	return TRUE;
}

void CConfigChargeDlg::OnBnClickedChargeAdd()
{
	CTime timeTime;
	DWORD	result = ((CDateTimeCtrl *)(GetDlgItem(IDC_CHARGE_DATE)))->GetTime(timeTime);

	CString	str;
	GetDlgItem(IDC_CHARGE_MONEY)->GetWindowText(str);
	int money = _ttoi(str);

	if(result == GDT_VALID && money != 0)
	{
	 	CPaidFee * fee = new CPaidFee();
		fee->Money = money;
		fee->PayTime = timeTime;
		theApp.Config.Charge.Add(fee);
	}
	theApp.Config.SetDirty(true);

	Refresh();
}

void CConfigChargeDlg::OnBnClickedChargeDelete()
{
	POSITION pos = m_Charge_List.GetFirstSelectedItemPosition();
	if(!pos)
		return;

	int nItem = m_Charge_List.GetNextSelectedItem(pos);

	delete theApp.Config.Charge.GetAt(nItem);
	theApp.Config.Charge.RemoveAt(nItem);
	theApp.Config.SetDirty(true);

	Refresh();
}

void CConfigChargeDlg::OnDeltaposChargeMoneySpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	//fred
	CSpinButtonCtrl *pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_CHARGE_MONEY_SPIN);
	m_iCharge=GetDlgItemInt(IDC_CHARGE_MONEY);
	if(m_iCharge<MIN_MONEY)
		m_iCharge=MIN_MONEY;
	if(m_iCharge>MAX_MONEY)
		m_iCharge=MAX_MONEY;
	if(pNMUpDown->iDelta==1) //up pushed
	{
		m_iCharge=m_iCharge+10-1;
		GetDlgItem(IDC_CHARGE_MONEY)->SetDlgItemInt(IDC_CHARGE_MONEY,m_iCharge,false);
		pSpinCtrl->SetPos32(m_iCharge);
		
	}else if(pNMUpDown->iDelta == -1) // up
	{
		m_iCharge=m_iCharge-10+1;
		GetDlgItem(IDC_CHARGE_MONEY)->SetDlgItemInt(IDC_CHARGE_MONEY,m_iCharge,false);
		pSpinCtrl->SetPos32(m_iCharge);
	}

	//end of fred
	*pResult = 0;
}

void CConfigChargeDlg::Refresh(void)
{
	m_Charge_List.DeleteAllItems();

	for(int i = 0; i < theApp.Config.Charge.GetCount(); i++)
	{
		m_Charge_List.InsertItem(i, theApp.Config.Charge.GetAt(i)->PayTime.Format(_T("%y-%m-%d")));
		CString str;
		str.Format(_T("%d"), theApp.Config.Charge.GetAt(i)->Money);
		m_Charge_List.SetItemText(i, 1, str);
	}
}