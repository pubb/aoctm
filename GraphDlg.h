#pragma once

#include "mygraph.h"
#include "playerdatabase.h"

#include <map>

// CGraphDlg dialog
class CGraphDlg : public CDialog
{
	DECLARE_DYNAMIC(CGraphDlg)

public:
	CGraphDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGraphDlg();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Dialog Data
	enum { IDD = IDD_GRAPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	CPlayerDatabase	* m_pPlayers;

private:
	CMyGraph m_Graph;
	
	void ShowRatingCurve(CPlayer * players);
	void ShowRatingBar(void);
	void ShowPlayCountBar(void);
};
