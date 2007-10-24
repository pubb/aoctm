#pragma once
#include "recgame.h"

// CGameInfoDlg dialog

class CGameInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CGameInfoDlg)

public:
	CGameInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGameInfoDlg();

// Dialog Data
	enum { IDD = IDD_GAMEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(void);

	DECLARE_MESSAGE_MAP()

public:
	CRecgame * m_Recgame;
};
