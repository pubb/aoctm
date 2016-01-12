#pragma once
#include "xlistctrl\xlistctrl.h"


// CConfigPlayerDlg dialog

class CConfigPlayerDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigPlayerDlg)

public:
	CConfigPlayerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigPlayerDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG_PLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMClickPlayerList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedPlayerAdd();
	afx_msg void OnBnClickedPlayerDelete();

	DECLARE_MESSAGE_MAP()
public:
	CXListCtrl m_List;

private:
	bool	Dirty;

	//pubb, 07-10-25, check duplicate names to avoid error in DB save.
	CStringArray	names;
	bool	IsDuplicatedName(CString name);
public:
	afx_msg void OnBnClickedSave2csv();
};