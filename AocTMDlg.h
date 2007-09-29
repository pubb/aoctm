// AocTMDlg.h : header file
//

#pragma once
//pic control fred
 #include   <Gdiplus.h>   
#include "xlistctrl\xlistctrl.h"
#include "afxdtctl.h"
using   namespace   Gdiplus;   
//end 

// CAocTMDlg dialog
class CAocTMDlg : public CDialog
{
// Construction
public:
	CAocTMDlg(CWnd* pParent = NULL);	// standard constructor
//fred
	//pic
	GdiplusStartupInput   gdiplusStartupInput;     
    ULONG_PTR   gdiplusToken;     

// Dialog Data
	enum { IDD = IDD_AocTM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

private:
	void ShowReport(CPlayerDatabase * players);
	void Refresh(void);

public:
	afx_msg void OnViewHistory();
	afx_msg void OnStatistic();
	afx_msg void OnGrouping();
	afx_msg void OnFee();
	afx_msg void OnConfigCharge();
	afx_msg void OnConfigPlayer();
public:
	CXListCtrl m_List;
	CDateTimeCtrl m_From;
	CDateTimeCtrl m_To;
public:
	afx_msg void OnBnClickedRecDelete();
	afx_msg void OnDtnDatetimechange(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnBnClickedRecDismiss();
};
