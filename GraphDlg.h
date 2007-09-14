#pragma once

#include "mygraph.h"
#include "playerdatabase.h"

#include <map>

typedef	enum {SHOW_RATINGCURVE = 0, SHOW_PLAYCOUNT, SHOW_USEDCIVS}	CHART_COMMAND;

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
	CArray<CPlayer *, CPlayer *>	* m_pPlayers;
	CHART_COMMAND	m_nCommand;
	CTime	m_FirstGame, m_LastGame;

private:
	CMyGraph m_Graph;
	
	void ShowRatingCurve(void);
	void ShowPlayCountBar(void);
	void ShowUsedCivs(void);
};