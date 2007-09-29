#pragma once

#include "player.h"

// CFeeInDlg dialog

class CFeeInDlg : public CDialog
{
	DECLARE_DYNAMIC(CFeeInDlg)

public:
	CFeeInDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFeeInDlg();

// Dialog Data
	enum { IDD = IDD_FEE_IN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDelete();
	virtual BOOL OnInitDialog();

	CPlayer * m_pPlayer;

private:
	void	Refresh();

	CListCtrl m_List;
	int	m_iMoney;
	const	static UINT	MIN_MONEY = 0, MAX_MONEY = 1000, INIT_MONEY = 50;

public:
	afx_msg void OnDeltaposMoneySpin(NMHDR *pNMHDR, LRESULT *pResult);
};
