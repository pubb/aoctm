// Report.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "Reportdlg.h"
#include "csvfile.h"

#include "graphdlg.h"

// CReport dialog

IMPLEMENT_DYNAMIC(CReportDlg, CDialog)



CReportDlg::CReportDlg(CWnd* pParent /*=NULL*/)
: CDialog(CReportDlg::IDD, pParent), m_pPlayerDB(NULL), m_FirstGame(FIRSTGAME_TIME), m_LastGame(CTime::GetCurrentTime())
{

}

CReportDlg::~CReportDlg()
{
}

void CReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ReportList, m_List);
}


BEGIN_MESSAGE_MAP(CReportDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CReportDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_ReportList, &CReportDlg::OnNMDblclkReportlist)
	ON_NOTIFY(NM_RCLICK, IDC_ReportList, &CReportDlg::OnNMRclickReportlist)
	ON_COMMAND_RANGE(ID__RATINGCURVE, ID__PLAYCOUNT, &CReportDlg::OnShowChart)
END_MESSAGE_MAP()

//Calculate and display the fee information for each player, default is OFF
//#define	_AocTM_FEE_

// CReport message handlers
BOOL CReportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_List.InsertColumn( 0, _T("Player Name"), LVCFMT_LEFT, 120, -1);
	m_List.InsertColumn( 1, _T("Play Count"), LVCFMT_LEFT, 70, -1);
	m_List.InsertColumn( 2, _T("Win Count"), LVCFMT_LEFT, 70, -1);
	m_List.InsertColumn( 3, _T("Win Rate"), LVCFMT_LEFT, 80, -1);
	m_List.InsertColumn( 4, _T("Ratings"), LVCFMT_LEFT, 65, -1);
	/* pubb, 07-08-28, move Fee functions to another dialog
	if(!m_bTemp)
	{
		m_List.InsertColumn( 5, _T("RestFee"), LVCFMT_LEFT, 60, -1);
//		m_List.InsertColumn( 6, _T("Cost"), LVCFMT_LEFT, 60, -1);
	}
	*/

	//by mep for statistic
	/*m_List.InsertColumn( 5, _T("Min Feud Time"), LVCFMT_LEFT, 85, -1);
	m_List.InsertColumn( 6, _T("Avg Feud Time"), LVCFMT_LEFT, 85, -1);
	m_List.InsertColumn( 7, _T("Min Cstl Time"), LVCFMT_LEFT, 80, -1);
	m_List.InsertColumn( 8, _T("Avg Cstl Time"), LVCFMT_LEFT, 80, -1);
	m_List.InsertColumn( 9, _T("Min Impl Time"), LVCFMT_LEFT, 80, -1);
	m_List.InsertColumn( 10, _T("Avg Impl Time"), LVCFMT_LEFT, 80, -1);*/

	if(m_pPlayerDB)
	{
		int nItem;
		CString str;
		int totalplays = 0, playcount, wincount, i;

		for(i = 0; i < m_pPlayerDB->GetCount(); i++)
		{
			playcount = m_pPlayerDB->GetAt(i)->PlayCount;
			if(!playcount)
				continue;
			nItem = m_List.InsertItem(i, m_pPlayerDB->GetAt(i)->NickNames[0]);
			str.Format(_T("%d"), playcount);
			m_List.SetItemText(nItem, 1, str);
			wincount = m_pPlayerDB->GetAt(i)->WinCount;
			str.Format(_T("%d"), wincount);
			m_List.SetItemText(nItem, 2, str);
			str.Format(_T("%d.%d%%"), (100 * wincount / playcount), (10000 * wincount / playcount) % 100);
			m_List.SetItemText(nItem, 3, str);
			str.Format(_T("%d"), m_pPlayerDB->GetAt(i)->Rating);
			m_List.SetItemText(nItem, 4, str);
			//pubb, 07-09-09, save index in CPlayerDatabase for retrieval
			m_List.SetItemData(nItem, i);

			/* pubb, 07-08-28, move Fee functions to another dialog
			if(!m_bTemp)
			{
				str.Format(_T("%d"), m_pPlayerDB->GetAt(i)->GetPaidFee() - m_pPlayerDB->GetAt(i)->GetCostFee());
				m_List.SetItemText(nItem, 5, str);
				//str.Format(_T("%d"), m_pPlayerDB->GetAt(i)->GetCostFee());
				//m_List.SetItemText(nItem, 6, str);
			}
			*/

			//by mep for statistic

			/*str.Format(_T("%d:%.2d"), m_pPlayerDB->GetAt(i)->MinFeudTime / 60, m_pPlayerDB->GetAt(i)->MinFeudTime % 60);
			m_List.SetItemText(nItem, 5, str);
			str.Format(_T("%d:%.2d"), m_pPlayerDB->GetAt(i)->AvgFeud / 60, m_pPlayerDB->GetAt(i)->AvgFeud % 60);
			m_List.SetItemText(nItem, 6, str);
			str.Format(_T("%d:%.2d"), m_pPlayerDB->GetAt(i)->MinCstl / 60, m_pPlayerDB->GetAt(i)->MinCstl % 60);
			m_List.SetItemText(nItem, 7, str);
			str.Format(_T("%d:%.2d"), m_pPlayerDB->GetAt(i)->AvgCstl / 60, m_pPlayerDB->GetAt(i)->AvgCstl % 60);
			m_List.SetItemText(nItem, 8, str);
			str.Format(_T("%d:%.2d"), m_pPlayerDB->GetAt(i)->MinImpl / 60, m_pPlayerDB->GetAt(i)->MinImpl % 60);
			m_List.SetItemText(nItem, 9, str);
			str.Format(_T("%d:%.2d"), m_pPlayerDB->GetAt(i)->AvgImpl / 60, m_pPlayerDB->GetAt(i)->AvgImpl % 60);
			m_List.SetItemText(nItem, 10, str);*/
		}
	}
	return true;
}

void CReportDlg::OnBnClickedOk()
{
	if(!m_pPlayerDB)
		return;

	CFileDialog dlg(FALSE, _T(".csv"), _T("stat.csv"), OFN_OVERWRITEPROMPT, _T("CSV (Comma delimited) | *.csv"));
	if(dlg.DoModal() == IDCANCEL)
		return;
	CString	csvfile = dlg.GetPathName();

	CCsvFile	csv(csvfile, CFile::modeWrite | CFile::modeCreate);

	CStringArray	a;
	const CHeaderCtrl	* header = m_List.GetHeaderCtrl();
	int i;

	HDITEM hdi;
    enum   { sizeOfBuffer = 256 };
    TCHAR  lpBuffer[sizeOfBuffer];
    
    hdi.mask = HDI_TEXT;
    hdi.pszText = lpBuffer;
    hdi.cchTextMax = sizeOfBuffer;

	a.SetSize(header->GetItemCount());

	for(i = 0; i < header->GetItemCount(); i++)
	{
		header->GetItem(i, &hdi);
		a[i] = lpBuffer;
	}
	csv.WriteLine(a);

	for(int i = 0; i < m_List.GetItemCount(); i++)
	{
		for(int j = 0; j < m_List.GetColumnCount(); j++)
			a[j] = m_List.GetItemText(i, j);
		csv.WriteLine(a);
	}

	OnOK();
}

void CReportDlg::OnNMDblclkReportlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!m_pPlayerDB)
		return;

	CGraphDlg dlg;
	dlg.m_pPlayers = m_pPlayerDB;
	dlg.DoModal();

	*pResult = 0;
}

void CReportDlg::OnNMRclickReportlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!m_pPlayerDB || !m_List.GetFirstSelectedItemPosition())
		return;

	CMenu menu, *pSubMenu;
	if (!menu.LoadMenu(IDR_REPORTPOPMENU))
		return;
	if (!(pSubMenu = menu.GetSubMenu(0)))
		return;
	CPoint pos;
	GetCursorPos(&pos);
	SetForegroundWindow();  

	::TrackPopupMenu(pSubMenu->m_hMenu, 0, pos.x, pos.y, 0, 
					 GetSafeHwnd(), NULL);
	PostMessage(WM_NULL, 0, 0);
	menu.DestroyMenu();

	*pResult = 0;
}

void CReportDlg::OnShowChart(UINT command)
{
	CArray<CPlayer *, CPlayer *> selected_players;
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	while(pos)
	{		
		selected_players.Add(m_pPlayerDB->GetAt(m_List.GetItemData(m_List.GetNextSelectedItem(pos))));
	}
	
	switch(command)
	{
	case ID__RATINGCURVE:
		command = SHOW_RATINGCURVE;
		break;
	case ID__PLAYCOUNT:
		command = SHOW_PLAYCOUNT;
		break;
	case ID__USEDCIVS:
		command = SHOW_USEDCIVS;
		break;
	}
	CGraphDlg dlg;
	dlg.m_nCommand = (CHART_COMMAND)command;
	dlg.m_pPlayers = &selected_players;
	dlg.m_FirstGame = m_FirstGame;
	dlg.m_LastGame = m_LastGame;
	dlg.DoModal();
}