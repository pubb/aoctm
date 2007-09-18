#pragma once
#include "afxcmn.h"
#include "playerdatabase.h"
#include "xlistctrl/xlistctrl.h"

// CReport dialog

class CReportDlg : public CDialog
{
	DECLARE_DYNAMIC(CReportDlg)

public:
	CReportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CReportDlg();

// Dialog Data
	enum { IDD = IDD_REPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CPlayerDatabase	*m_pPlayerDB;
	CXListCtrl m_List;
	
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkReportlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickReportlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnShowChart(UINT command);

private:
	void CopyInitRatings();
};
