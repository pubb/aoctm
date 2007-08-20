#pragma once
#include "afxcmn.h"
#include "playerdatabase.h"
#include "xlistctrl/xlistctrl.h"

// Statdlg dialog

class Statdlg : public CDialog
{
	DECLARE_DYNAMIC(Statdlg)

public:
	Statdlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~Statdlg();

// Dialog Data
	enum { IDD = IDD_STATDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CXListCtrl m_List;
	CPlayerDatabase	*m_pPlayerDB;
};
