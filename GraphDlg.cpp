// GraphDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "GraphDlg.h"

#include <math.h>

// CGraphDlg dialog

IMPLEMENT_DYNAMIC(CGraphDlg, CDialog)

CGraphDlg::CGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGraphDlg::IDD, pParent), m_pPlayers(NULL)
{
	if(::IsWindow(m_Graph.m_hWnd))
		m_Graph.DestroyWindow();
}

CGraphDlg::~CGraphDlg()
{
}

void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGraphDlg, CDialog)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CGraphDlg message handlers

int CGraphDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_pPlayers)
		return 0;

	//prepare data
	m_Graph.PrepareData(2, (int)m_pPlayers->GetCount());
	m_Graph.Series[0] = _T("Play Count");
	m_Graph.Series[1] = _T("Win Count");
	for(int i = 0; i < m_pPlayers->GetCount(); i++)
	{
		CPlayer * player = m_pPlayers->GetAt(i);
		m_Graph.Segments[i] = player->NickNames[0];
		m_Graph.Data[0][i] = player->PlayCount;
		m_Graph.Data[1][i] = player->WinCount;
	}

	CRect clRect;
	GetClientRect(clRect);
	m_Graph.ShowBar(clRect, RGB(255, 255, 255), _T("Play Count"), _T(""), this);
	
	m_Graph.ClearData();

	return 0;
}