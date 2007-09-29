// FeeInoutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "FeeInDlg.h"


// CFeeInDlg dialog

IMPLEMENT_DYNAMIC(CFeeInDlg, CDialog)

CFeeInDlg::CFeeInDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFeeInDlg::IDD, pParent), m_iMoney(INIT_MONEY)
{

}

CFeeInDlg::~CFeeInDlg()
{
}

void CFeeInDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FEE_MONEY, m_iMoney);
	DDV_MinMaxUInt(pDX, m_iMoney, MIN_MONEY, MAX_MONEY);
	DDX_Control(pDX, IDC_FEE_LIST, m_List);
}


BEGIN_MESSAGE_MAP(CFeeInDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFeeInDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_DELETE, &CFeeInDlg::OnBnClickedDelete)
	ON_NOTIFY(UDN_DELTAPOS, IDC_FEE_SPIN, &CFeeInDlg::OnDeltaposMoneySpin)
END_MESSAGE_MAP()


BOOL CFeeInDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(!m_pPlayer)
		return FALSE;

	SetWindowText(m_pPlayer->NickNames[0]);

	m_List.InsertColumn(0, _T("Date"), LVCFMT_LEFT, 120, -1);
	m_List.InsertColumn(1, _T("Money"), LVCFMT_LEFT, 50, -1);
	Refresh();

	m_iMoney=INIT_MONEY;

	CSpinButtonCtrl *pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_FEE_SPIN);
	pSpinCtrl->SetRange32(MIN_MONEY,MAX_MONEY);
	pSpinCtrl->SetPos32(m_iMoney);

	return TRUE;
}

// CFeeInDlg message handlers

void CFeeInDlg::OnBnClickedOk()
{
	CTime timeTime;
	DWORD	result = ((CDateTimeCtrl *)(GetDlgItem(IDC_FEE_DATE)))->GetTime(timeTime);

	CString	str;
	GetDlgItem(IDC_FEE_MONEY)->GetWindowText(str);
	int money = _ttoi(str);

	if(result == GDT_VALID && money != 0)
	{
	 	CPaidFee * fee = new CPaidFee();
		fee->Money = money;
		fee->PayTime = timeTime;
		m_pPlayer->Record_PaidFee.Add(fee);
	}
	theApp.Players.SetDirty(true);

	Refresh();
}

void CFeeInDlg::OnBnClickedDelete()
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(!pos)
		return;

	int nItem = m_List.GetNextSelectedItem(pos);

	/* assume the order in m_List not changed */
	/*
	CString str = m_List.GetItemText(nItem, 0);
	int curPos= 0;
	CTime t(str.Tokenize(_T("-"),curPos), str.Tokenize(_T("-"), curPos), str.Tokenize(_T("-"), curPos), 0, 0, 0);

	for(int i = 0; i < a->GetCount(); i++)
	{
		if(a->GetAt(i)->PayTime - t < CTimeSpan(1, 0, 0, 0))		//assume paid only once a day
		{
			a->RemoveAt(i);
			break;
		}
	}
	*/
	delete m_pPlayer->Record_PaidFee.GetAt(nItem);
	m_pPlayer->Record_PaidFee.RemoveAt(nItem);
	theApp.Players.SetDirty(true);

	Refresh();
}

void CFeeInDlg::Refresh(void)
{
	m_List.DeleteAllItems();

	CArray<CPaidFee * , CPaidFee *> * fee = &m_pPlayer->Record_PaidFee;
	for(int i = 0; i < fee->GetCount(); i++)
	{
		m_List.InsertItem(i, fee->GetAt(i)->PayTime.Format(_T("%y-%m-%d")));
		CString str;
		str.Format(_T("%d"), fee->GetAt(i)->Money);
		m_List.SetItemText(i, 1, str);
	}
}

void CFeeInDlg::OnDeltaposMoneySpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	//fred
	CSpinButtonCtrl *pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_FEE_SPIN);
	m_iMoney=GetDlgItemInt(IDC_FEE_MONEY);
	if(m_iMoney<MIN_MONEY)
		m_iMoney=MIN_MONEY;
	if(m_iMoney>MAX_MONEY)
		m_iMoney=MAX_MONEY;
	if(pNMUpDown->iDelta==1) //up pushed
	{
		m_iMoney=m_iMoney+10-1;
		GetDlgItem(IDC_FEE_MONEY)->SetDlgItemInt(IDC_FEE_MONEY,m_iMoney,false);
		pSpinCtrl->SetPos32(m_iMoney);
		
	}else if(pNMUpDown->iDelta == -1) // up
	{
		m_iMoney=m_iMoney-10+1;
		GetDlgItem(IDC_FEE_MONEY)->SetDlgItemInt(IDC_FEE_MONEY,m_iMoney,false);
		pSpinCtrl->SetPos32(m_iMoney);
	}

	//end of fred
	*pResult = 0;
}
