// PlayerInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "PlayerInputDlg.h"


// CPlayerInputDlg dialog

IMPLEMENT_DYNAMIC(CPlayerInputDlg, CDialog)

CPlayerInputDlg::CPlayerInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayerInputDlg::IDD, pParent)
	, m_iRating(0)
	, m_Name(_T(""))
{

}

CPlayerInputDlg::~CPlayerInputDlg()
{
}

void CPlayerInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITRATING, m_iRating);
	DDV_MinMaxUInt(pDX, m_iRating, MIN_RATING, MAX_RATING);
	DDX_Text(pDX, IDC_EDITNAME, m_Name);
}


BEGIN_MESSAGE_MAP(CPlayerInputDlg, CDialog)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPINRATING, &CPlayerInputDlg::OnDeltaposSpinrating)
	ON_BN_CLICKED(IDOK, &CPlayerInputDlg::OnBnClickedOk)
END_MESSAGE_MAP()



BOOL CPlayerInputDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();

	//fred 
	m_iRating=INIT_RATING;

	CSpinButtonCtrl *pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_SPINRATING);
	pSpinCtrl->SetRange32(MIN_RATING,MAX_RATING);
	pSpinCtrl->SetPos32(m_iRating);

	return TRUE;
}

void CPlayerInputDlg::OnDeltaposSpinrating(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	//fred
	CSpinButtonCtrl *pSpinCtrl=(CSpinButtonCtrl*)GetDlgItem(IDC_SPINRATING);
	m_iRating=GetDlgItemInt(IDC_EDITRATING);
	if(m_iRating<MIN_RATING)
		m_iRating=MIN_RATING;
	if(m_iRating>MAX_RATING)
		m_iRating=MAX_RATING;
	if(pNMUpDown->iDelta==1) //up pushed
	{
		m_iRating=m_iRating+100-1;
		GetDlgItem(IDC_EDITRATING)->SetDlgItemInt(IDC_EDITRATING,m_iRating,false);
		pSpinCtrl->SetPos32(m_iRating);
		
	}else if(pNMUpDown->iDelta == -1) // up
	{
		m_iRating=m_iRating-100+1;
		GetDlgItem(IDC_EDITRATING)->SetDlgItemInt(IDC_EDITRATING,m_iRating,false);
		pSpinCtrl->SetPos32(m_iRating);
	}

	//end of fred
	*pResult = 0;
}

void CPlayerInputDlg::OnBnClickedOk()
{
	CString name;

	GetDlgItemText(IDC_EDITNAME, name);
	if(name.IsEmpty())
		return;

	OnOK();
}
