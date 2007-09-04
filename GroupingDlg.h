#pragma once
#include "xlistctrl\xlistctrl.h"
#include "afxwin.h"
#include "playerdatabase.h"
#include "aoctmdlg.h"

// CGroupingDlg dialog

class CGroupingDlg : public CDialog
{
	DECLARE_DYNAMIC(CGroupingDlg)

public:
	CGroupingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGroupingDlg();

// Dialog Data
	enum { IDD = IDD_GROUPING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CXListCtrl m_Players;
	CXListCtrl m_Selected;
	CListCtrl m_Group1;
	CListCtrl m_Group2;

	void	InsertPlayers(CString name, int data);
	void	InsertSelected(CString name, int rating, int data);
	bool	UpdateSelected(int rating, int data);
	void	Refresh(void);

	virtual BOOL OnInitDialog(void);
	afx_msg void OnNMDblclkPlayers(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkSelected(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedInput();
	afx_msg void OnBnClickedKeep();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNMDblclkGroup1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkGroup2(NMHDR *pNMHDR, LRESULT *pResult);
	
	void OnExchange(int index1, int index2);

public:
	CAocTMDlg	* maindlg;
};
