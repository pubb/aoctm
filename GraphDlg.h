#pragma once

#include "mygraph.h"
#include "playerdatabase.h"

#include <map>

typedef	enum {SHOW_RATINGCURVE = 0, SHOW_TECHSTAT, SHOW_USEDCIVS}	CHART_COMMAND;

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
	CHART_COMMAND	m_nCommand;
	CTime	m_FirstGame, m_LastGame;

private:
	CMyGraph m_Graph;
	
	void ShowRatingCurve(void);
	void ShowTechStatBar(void);
	void ShowUsedCivs(void);
	afx_msg void OnPaint();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	// Hide user control for curve view
	int HideUserControl(void);
	afx_msg void OnBnClickedButtonZoom();
	afx_msg void OnBnClickedButtonMark();
	afx_msg void OnBnClickedButtonMeasure();
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonReset();
public:
	//fred add for xgraph's Wnd
	CWnd * m_pXGraphWndHandle;
public:
	afx_msg void OnBnClickedButtonClear();
public:
	afx_msg void OnBnClickedButtonClearmeasure();
	virtual BOOL OnInitDialog();
};