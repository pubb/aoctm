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
: CDialog(CReportDlg::IDD, pParent), m_pPlayerDB(NULL)
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
	ON_COMMAND_RANGE(ID__RATINGCURVE, ID__TECHSTAT, &CReportDlg::OnShowChart)
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
	m_List.InsertColumn( 5, _T("DeltaRatings"), LVCFMT_LEFT, 65, -1);
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

	if(m_pPlayerDB && m_pPlayerDB->m_pRecgameDB)
	{
		int nItem;
		CString str;
		int totalplays = 0, playcount, wincount, i;

		//pubb, 08-01-28, save 'dirty' state before temparory update
		//pubb, 07-09-18, to generate ratings before the report ( 5 minutes )
		INT_PTR id = m_pPlayerDB->m_pRecgameDB->GetFirstGameID();
		if(id < 0)	return false;
		
		theApp.Players.Update(false, CTime(0), m_pPlayerDB->m_pRecgameDB->GetAt(id)->RecordTime - CTimeSpan(0, 0, 5, 0));
		m_pPlayerDB->CopyPlayers();	//store ratings before this show up in InitRating
		//pubb, 07-09-22, restore the normal ratings
		theApp.Players.Update(false);
		m_pPlayerDB->Update(false);
		
		for(i = 0; i < m_pPlayerDB->GetCount(); i++)
		{
			playcount = m_pPlayerDB->GetAt(i)->PlayCount;
			if(!playcount)
				continue;
			//pubb, 07-10-25, ignore computer players
			if(m_pPlayerDB->GetAt(i)->IsComputer)
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
			str.Format(_T("%d"), m_pPlayerDB->GetAt(i)->Rating - m_pPlayerDB->GetAt(i)->InitRating);
			m_List.SetItemText(nItem, 5, str);

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

	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	return true;
}

void CReportDlg::OnBnClickedOk()
{
	if(!m_pPlayerDB)
		return;

	CFileDialog dlg(FALSE, _T(".csv"), _T("stat.csv"), OFN_OVERWRITEPROMPT, _T("CSV (Comma delimited, *.csv)|*.csv||"));
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
#ifdef	XLISTCTRL_OLD
		for(int j = 0; j < m_List.GetColumnCount(); j++)
#else
		for(int j = 0; j < m_List.GetColumns(); j++)
#endif
			a[j] = m_List.GetItemText(i, j);
		csv.WriteLine(a);
	}

	OnOK();
}

void CReportDlg::OnNMDblclkReportlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!m_pPlayerDB)
		return;

	CPlayerDatabase selected_players;
	selected_players.m_pRecgameDB = m_pPlayerDB->m_pRecgameDB;

	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(!pos)
		return;
	CPlayer * player = m_pPlayerDB->GetAt(m_List.GetItemData(m_List.GetNextSelectedItem(pos)));
	selected_players.Add(player);

	CGraphDlg dlg;
	dlg.m_nCommand = SHOW_RATINGCURVE;
	dlg.m_pPlayers = &selected_players;
	dlg.m_FirstGameID = m_pPlayerDB->m_pRecgameDB->GetFirstGameID(player->NickNames[0]);
	dlg.m_LastGameID = m_pPlayerDB->m_pRecgameDB->GetLastGameID(player->NickNames[0]);
	if(dlg.m_LastGameID >= dlg.m_FirstGameID)
		dlg.DoModal();
	else
		AfxMessageBox(_T("Strange Timeline, IGNORE!"));

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
	CPlayerDatabase selected_players;
	selected_players.m_pRecgameDB = m_pPlayerDB->m_pRecgameDB;

	POSITION pos = m_List.GetFirstSelectedItemPosition();
	while(pos)
	{		
		CPlayer * player = m_pPlayerDB->GetAt(m_List.GetItemData(m_List.GetNextSelectedItem(pos)));
		if(player->IsComputer)
			continue;
		selected_players.Add(player);
	}
	
	switch(command)
	{
	case ID__RATINGCURVE:
		command = SHOW_RATINGCURVE;
		break;
	case ID__TECHSTAT:
		command = SHOW_TECHSTAT;
		break;
	case ID__USEDCIVS:
		command = SHOW_USEDCIVS;
		break;
	}
	CGraphDlg dlg;
	dlg.m_nCommand = (CHART_COMMAND)command;
	dlg.m_pPlayers = &selected_players;
	dlg.m_FirstGameID = dlg.m_LastGameID = 0;
	for(int i = 0; i < selected_players.GetCount(); i++)
	{
		int id1 = m_pPlayerDB->m_pRecgameDB->GetFirstGameID(selected_players.GetAt(i)->NickNames[0]), id2 = m_pPlayerDB->m_pRecgameDB->GetLastGameID(selected_players.GetAt(i)->NickNames[0]);
		if(dlg.m_FirstGameID == 0 || dlg.m_LastGameID == 0)
		{
			dlg.m_FirstGameID = id1;
			dlg.m_LastGameID = id2;
			continue;
		}
		dlg.m_FirstGameID = min(dlg.m_FirstGameID, id1);
		dlg.m_LastGameID = max(dlg.m_LastGameID, id2);
	}
	if(dlg.m_LastGameID >= dlg.m_FirstGameID)
		dlg.DoModal();
	else
		AfxMessageBox(_T("Strange Timeline, IGNORE!"));
}