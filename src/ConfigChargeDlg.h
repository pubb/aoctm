#pragma once
#include "afxcmn.h"


// CConfigChargeDlg dialog

class CConfigChargeDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigChargeDlg)

public:
	CConfigChargeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigChargeDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG_CHARGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_Charge_List;
	int m_iCharge;
	const	static UINT	MIN_MONEY = 0, MAX_MONEY = 1000, INIT_MONEY = 50;

	afx_msg BOOL OnInitDialog(void);
	afx_msg void OnBnClickedChargeAdd();
	afx_msg void OnBnClickedChargeDelete();
	afx_msg void OnDeltaposChargeMoneySpin(NMHDR *pNMHDR, LRESULT *pResult);

	void Refresh(void);
};
