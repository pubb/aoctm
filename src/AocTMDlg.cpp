// AocTMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AocTM.h"
#include "AocTMDlg.h"
#include "playerdatabase.h"
#include "reportdlg.h"
#include "groupingdlg.h"
#include "progresswnd.h"
#include "Statdlg.h"
#include "feedlg.h"
#include "configchargedlg.h"
#include "configplayerdlg.h"
#include "renamer/renamer.h"
#include "gameinfodlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

#define	AOCTM_VERSION	(_T("1.1beta"))
#define AOCTM_BUILD	__AOCTM_BUILD()
inline CString __AOCTM_BUILD(void)
{
	CString date =_T(__DATE__);
	int curPos = 0;
	int month = Renamer::ToMonth(date.Tokenize(_T(" "), curPos));
	int day = _ttoi(date.Tokenize(_T(" "), curPos));
	CString year = date.Tokenize(_T(" "), curPos).Right(2);
	date.Format(_T("%s%.2d%.2d"), year, month, day);
	return date;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

BOOL CAboutDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();

	CString str;
	str.Format(_T("AocTM v%s build %s"), AOCTM_VERSION, AOCTM_BUILD);
	SetDlgItemText(IDC_VERSION, str);
	return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAocTMDlg dialog




CAocTMDlg::CAocTMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAocTMDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hAccelTable = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));
}

void CAocTMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REC_LIST, m_List);
	DDX_Control(pDX, IDC_DATETIME_FROM, m_From);
	DDX_Control(pDX, IDC_DATETIME_TO, m_To);
}

BEGIN_MESSAGE_MAP(CAocTMDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_VIEWHISTORY, &CAocTMDlg::OnViewHistory)
	ON_COMMAND(ID_STATISTIC, &CAocTMDlg::OnStatistic)
	ON_COMMAND(ID_GROUPING, &CAocTMDlg::OnGrouping)
	ON_COMMAND(ID_FEE, &CAocTMDlg::OnFee)
	ON_COMMAND(ID_CONFIG_CHARGE, &CAocTMDlg::OnConfigCharge)
	ON_COMMAND(ID_CONFIG_PLAYERINITIALIZE, &CAocTMDlg::OnConfigPlayer)
	ON_BN_CLICKED(IDC_REC_DELETE, &CAocTMDlg::OnBnClickedRecDelete)
	ON_NOTIFY(DTN_CLOSEUP, IDC_DATETIME_FROM, &CAocTMDlg::OnDtnDatetimechange)
	ON_NOTIFY(DTN_CLOSEUP, IDC_DATETIME_TO, &CAocTMDlg::OnDtnDatetimechange)
	ON_BN_CLICKED(IDC_REC_DISMISS, &CAocTMDlg::OnBnClickedRecDismiss)
	ON_COMMAND(ID_ACCELERATOR_LOAD, (AFX_PMSG)&CAocTMDlg::OnAcceleratorLoad)
	ON_NOTIFY(NM_DBLCLK, IDC_REC_LIST, &CAocTMDlg::OnNMDblclkRecList)
	ON_BN_CLICKED(IDC_SUMMARY, &CAocTMDlg::OnBnClickedSummary)
	ON_NOTIFY(NM_RCLICK, IDC_REC_LIST, &CAocTMDlg::OnNMRClickRecList)
	ON_COMMAND_RANGE(ID_SETWINNER_0, ID_SETWINNER_CLEAR, &CAocTMDlg::OnSetWinner)
	ON_COMMAND(ID__GROUPTHIS, &CAocTMDlg::GroupThis)
	ON_COMMAND(ID__REGROUPTHIS, &CAocTMDlg::OnRegroupThis)
END_MESSAGE_MAP()


// CAocTMDlg message handlers

BOOL CAocTMDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//GdiplusStartup(&gdiplusToken,   &gdiplusStartupInput,   NULL);     //FRED

	m_List.InsertColumn(0, _T("No."), LVCFMT_LEFT, 30, -1);
	m_List.InsertColumn(1, _T("Time"), LVCFMT_LEFT, 110, -1);
	m_List.InsertColumn(2, _T("Map"), LVCFMT_LEFT, 50, -1);
	m_List.InsertColumn(3, _T("Duration"), LVCFMT_LEFT, 60, -1);

	int i = 4;
	m_List.InsertColumn(i, _T("Winner-1"), LVCFMT_LEFT, 100, -1);
	m_List.InsertColumn(i + 1, _T("Winner-2"), LVCFMT_LEFT, 100, -1);
	m_List.InsertColumn(i + 2, _T("Winner-3"), LVCFMT_LEFT, 100, -1);
	m_List.InsertColumn(i + 3, _T("Winner-4"), LVCFMT_LEFT, 100, -1);
	m_List.InsertColumn(i + 4, _T("Loser-1"), LVCFMT_LEFT, 100, -1);
	m_List.InsertColumn(i + 5, _T("Loser-2"), LVCFMT_LEFT, 100, -1);
	m_List.InsertColumn(i + 6, _T("Loser-3"), LVCFMT_LEFT, 100, -1);
	m_List.InsertColumn(i + 7, _T("Loser-4"), LVCFMT_LEFT, 100, -1);

	CTime t = CTime::GetCurrentTime() - CTimeSpan(7, 0, 0, 0);	//display recgames for recent 7 days
	m_From.SetTime(&t);
	t = CTime::GetCurrentTime() + CTimeSpan(1, 0, 0, 0);	//for future drag&drop
	m_To.SetTime(&t);

	Refresh();

	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAocTMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
//fred
#if 0
void CAocTMDlg::OnPaint()
{
	 Graphics   graphics(GetDC()->m_hDC);     
	 //Image   image(L"s1092909154.jpg"); 
	 Image   image(_T("..//s1092909154.jpg"),false); 
     graphics   .DrawImage(   &image,   0,   0   );   
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
#endif

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAocTMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/* pubb, 070724, the first function implementation */

void CAocTMDlg::OnDropFiles(HDROP hDropInfo)
{
	int	count =	DragQueryFile(hDropInfo,0xffffffff,NULL,0);

	TCHAR	strPath[4096];
	int	loadcount = 0, loadnew = 0;
	CRecgame *rg;
	CProgressWnd	wndProgress;

	//store and show the effect of this Drag&Drop
	CPlayerDatabase	players;
	CRecgameDatabase recgames;
	players.m_pRecgameDB = &recgames;

	wndProgress.Create(this, _T("Progress..."), TRUE);
	wndProgress.SetRange(0,count);
	wndProgress.SetText(_T("Loading Recorded Game files..."));

	//pubb, 07-09-22, no DB operations now
	//by mep for performance
	//theApp.Engine->BeginTx();

	for(int i = 0; i < count; i++)
	{
		wndProgress.StepIt();
		wndProgress.PeekAndPump();
		if (wndProgress.Cancelled())
		{
			break;
		}

		DragQueryFile(hDropInfo,i,strPath,4096);

		rg = new CRecgame;

		if(!rg->Read(strPath))
		{
			delete rg;
			continue;
		}
		
		if(rg->PlayTime < TIME_4_INCOMPLETE)	//if play time less than 20 min, consider it an INCOMPLETE game
		{
			delete rg;
			continue;
		}

		players.Add(rg);
		loadcount++;

		if(theApp.Recgames.Add(rg))
		{
			//pubb, 07-09-18, for temporary storage for one drag&drop
			recgames.Add(rg);
			loadnew++;
		}
		else
		{
			//pubb, 07-09-18, store it in temporary recgameDB when duplicate in mainDB
			recgames.Add(theApp.Recgames[theApp.Recgames.GetFirstSameRecgame(rg)]);
			delete rg;
		}
	}

	//pubb, 07-09-22
	//theApp.Engine->Commit();

	wndProgress.Hide();

	if(loadnew > 0)
		theApp.Players.Update(true);

	//pubb, 15-09-21, if load none, show mesg here instead of in CReportDlg::OnInitDialog()
	if(loadcount < count)
	{
		CString str;
		str.Format(_T("%d(new: %d) of %d Game(s) Loaded."), loadcount, loadnew, count);
		AfxMessageBox(str);
	}

	if(loadcount > 0)
	{
		players.CopyNickNames();
		ShowReport(&players);;
		Refresh();
		players.Free();
	}
	
	CDialog::OnDropFiles(hDropInfo);
}

int CAocTMDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style |= LVS_NOSORTHEADER;

	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles();

//	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);

	return 0;
}

void CAocTMDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	/*CGroupingDlg	dlg;
	
	dlg.DoModal();*/
	
	CDialog::OnRButtonUp(nFlags, point);

	CMenu menu, *pSubMenu;
	if (!menu.LoadMenu(IDR_MAINPOPMENU))
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

}

void CAocTMDlg::OnClose()
{
//	GdiplusShutdown(gdiplusToken); //fred

	CDialog::OnClose();
}

void CAocTMDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ShowReport(&theApp.Players);

	CDialog::OnLButtonDblClk(nFlags, point);
}

void CAocTMDlg::ShowReport(CPlayerDatabase * players)
{
	CReportDlg	dlg;
	dlg.m_pPlayerDB = players;
	dlg.DoModal();
}

void CAocTMDlg::OnViewHistory()
{
	ShowReport(&theApp.Players);
}

void CAocTMDlg::OnStatistic()
{
	Statdlg dlg;
	dlg.m_pPlayerDB = &theApp.Players;
	dlg.DoModal();
}

void CAocTMDlg::OnGrouping()
{
	CGroupingDlg	dlg;
	dlg.maindlg = this;
	
	dlg.DoModal();
}

void CAocTMDlg::OnFee(void)
{
	CFeeDlg	dlg;
	dlg.DoModal();
}

void CAocTMDlg::Refresh(void)
{
	CString str;

	//pubb, 14-12-13, to save selected iteam position, in order to restore display
	POSITION pos, saved_selected_pos;
	pos = saved_selected_pos = m_List.GetFirstSelectedItemPosition();
	int selected_recid;
	if(saved_selected_pos)
		selected_recid = m_List.GetItemData(m_List.GetNextSelectedItem(pos));

	//pubb, 14-12-11, bugfix for empty database situation.
	if(theApp.Recgames.IsEmpty())
	{
		str.Format(_T("Empty database."));
		SetDlgItemText(IDC_TITLE, str);
		m_List.DeleteAllItems();
		m_List.UpdateWindow();
		return;
	}

	INT_PTR id1 = theApp.Recgames.GetFirstGameID(), id2 = theApp.Recgames.GetLastGameID();
	if(id1 < 0 || id2 < 0)
	{
		AfxMessageBox(_T("Database Error!"));
		return;
	}
	str.Format(_T("%3d games (%s to %s)"), theApp.Recgames.GetCount(), theApp.Recgames[id1]->RecordTime.Format(_T("%Y-%m-%d %H:%M:%S")), theApp.Recgames[id2]->RecordTime.Format(_T("%Y-%m-%d %H:%M:%S")));
	SetDlgItemText(IDC_TITLE, str);

	m_List.SetRedraw(false);
	m_List.DeleteAllItems();

	CTime from, to;
	CRecgame * rec;
	m_From.GetTime(from);
	m_To.GetTime(to);
	for(int i = 0, j = 0; i < theApp.Recgames.GetCount(); i++)
	{
		rec = theApp.Recgames[i];
		if(rec->RecordTime < from || rec->RecordTime > to)
			continue;

		str.Format(_T("%d"), j + 1);
		m_List.InsertItem(j, str);
		m_List.SetItemText(j, 1, rec->RecordTime.Format(_T("%y-%m-%d %H:%M:%S")));
		m_List.SetItemText(j, 2, rec->Map.Name);
		m_List.SetItemText(j, 3, rec->PlayTime.Format(_T("%H:%M:%S")));

		//fill players table
		int winner = rec->GetWinnerTeam();
		//07-10-20, pubb, no winner / loser condition
		COLORREF	background_color = ::GetSysColor(COLOR_WINDOW);
		if(winner == 0)
		{
			background_color = RGB(220,220,220);
			winner = 1;
		}
		int win_index, lose_index, k;
		for(k = 1, win_index = lose_index = 0; k <= 8 && k < rec->Players.GetCount() && !rec->Players[k].Name.IsEmpty(); k++)
		{
			str.Format(_T("[%s]%s"), civ_name[rec->Players[k].Civ], rec->Players[k].Name);
			int team_position;
			if(rec->Players[k].Team == winner)
			{
				team_position = 4 + win_index++;
			}
			else
			{
				team_position = 8 + lose_index++;
			}
			//14-12-08, pubb, bugfix for strange team condition when more than 4 players in one team
			//16-01-12, pubb, set background color to be red for this situation, not ignore it.
			if( win_index > 4 || lose_index > 4)
			{
//				str.Format(_T("Recgame %s, More than 4 players in one team, r u sure?\n Ignore it now!"), rec->RecordTime.Format(_T("%y-%m-%d %H:%M:%S")));
//				AfxMessageBox(str);
//				continue;
				background_color = RGB(220, 0, 0);
			}
			m_List.SetItemText(j, team_position, str);
#ifdef	XLISTCTRL_OLD
			//m_List.SetItemTextColor(j, team_position, player_color[k]);
#else
			//pubb, 07-10-31, it should use Player->Color
			m_List.SetItemColors(j, team_position, player_color[rec->Players[k].Color], background_color);
#endif
		}
		//save recgame index in DB
		m_List.SetItemData(j++, i);
	}

	//pubb, 14-12-13, ensure the selected rec be selected after refresh if existed in current view.
	if(saved_selected_pos)
	{
		int i;
		for(i = 0; i < m_List.GetItemCount(); i++)
		{
			if(m_List.GetItemData(i) == selected_recid)
			{
				m_List.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
				m_List.EnsureVisible(i, true);
				break;
			}
		}
	}
	m_List.SetRedraw();
	m_List.UpdateWindow();
}

void CAocTMDlg::OnConfigCharge(void)
{
	CConfigChargeDlg	dlg;
	dlg.DoModal();
}

void CAocTMDlg::OnConfigPlayer(void)
{
	CConfigPlayerDlg	dlg;
	dlg.DoModal();
}

void CAocTMDlg::OnBnClickedRecDelete()
{
	if(GetKeyState(VK_SHIFT)<0)
	{
		if(AfxMessageBox(_T("Delete ALL?"), MB_OKCANCEL) == IDCANCEL)
			return;

		theApp.Recgames.RemoveAll();
	}
	else
	{
		POSITION pos = m_List.GetFirstSelectedItemPosition();
		if(!pos)
			return;

		if(AfxMessageBox(_T("Permanent delete, r u sure?"), MB_OKCANCEL) == IDCANCEL)
			return;

		while(pos)
		{
			int nItem = m_List.GetNextSelectedItem(pos);
			INT_PTR index = m_List.GetItemData(nItem);

			int	data;
			for(int i = 0; i < m_List.GetItemCount(); i++)
			{
				data = m_List.GetItemData(i);
				if(data > index)
				{
					m_List.SetItemData(i, data - 1);
				}
			}

			delete theApp.Recgames[index];
			theApp.Recgames.RemoveAt(index);
		}
	}

	//07-10-11, do update() when one or more recgames are deleted.
	theApp.Players.Update(true);

	//XXX, can't delete list before remove recgame DB, see Andytalk::LogDlg.cpp
	//but still bugs, so comment it, let Refresh() do it.
#if 0
	pos = m_List.GetFirstSelectedItemPosition();
	while(pos)
	{
		m_List.DeleteItem(m_List.GetNextSelectedItem(pos));
	}
#else
	Refresh();
#endif

	theApp.Recgames.SetDirty();
}

void CAocTMDlg::OnDtnDatetimechange(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);

	Refresh();

	*pResult = 0;
}
void CAocTMDlg::OnBnClickedRecDismiss()
{
	OnCancel();
}

void CAocTMDlg::LoadRecFile(bool aofe, int & loadnew, int & loadcount, int & readcount)
{
	CString strPath = (aofe?theApp.Recgames.GetRecPathAOFE():theApp.Recgames.GetRecPath());

	if(strPath.IsEmpty())
		return;

	TCHAR oldpath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, oldpath);
	::SetCurrentDirectory(strPath);

	CFileFind finder;
	INT_PTR id = theApp.Recgames.GetLastGameID();
	//pubb, 15-09-21, bugfix for empty db loading
	CTime DBLatestGameTime;
	DBLatestGameTime = (id < 0 ? CTime(0) : theApp.Recgames[id]->RecordTime);
	BOOL bWorking = finder.FindFile(_T("*.mg?"));
	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		//08-02-10, pubb, compare with the game time before importing new games.
		//07-10-11, pubb, to speedup, analyze the filename before process
		{
			CString file = finder.GetFileName();
			if((Renamer::Parse2Playtime(file)) <= DBLatestGameTime)
				continue;
		}
		
		CRecgame *rg = new CRecgame;

		if(!rg->Read(finder.GetFilePath()))
		{
			delete rg;
			continue;
		}
		readcount++;

		if(rg->PlayTime < TIME_4_INCOMPLETE)	//if play time less than 20 min, consider it an INCOMPLETE game
		{
			delete rg;
			continue;
		}

		loadcount++;
		if(theApp.Recgames.Add(rg))
		{
			loadnew++;
		}
		else
		{
			delete rg;
		}
	}
	
	::SetCurrentDirectory(oldpath);
	return;
}

int CAocTMDlg::OnAcceleratorLoad()
{
	//pubb, 14-10-07, bugfix for stopping after loading aoe. '||' problem of C language implementation.
	//pubb, 15-09-21, show load count when some recs aren't loaded successfully
	int	readcount = 0, loadcount = 0, loadnew = 0;
	
	LoadRecFile(false, loadnew, loadcount, readcount);
	LoadRecFile(true, loadnew, loadcount, readcount);

	if(loadcount < readcount)
	{
		CString str;
		str.Format(_T("%d(new: %d) of %d Game(s) Loaded."), loadcount, loadnew, readcount);
		AfxMessageBox(str);
	}

	if(loadnew > 0)
	{
		theApp.Players.Update(true);
		Refresh();
	}

	return loadnew;
}

BOOL   CAocTMDlg::PreTranslateMessage(MSG*   pMsg)
{   
	if(m_hAccelTable)
	{
		if(::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
		{
			return(TRUE);
		}   
	}   
	return   CDialog::PreTranslateMessage(pMsg);   
}

void CAocTMDlg::OnNMDblclkRecList(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(!pos)
		return;
	
	CGameInfoDlg dlg;
	dlg.m_Recgame = theApp.Recgames[m_List.GetItemData(m_List.GetNextSelectedItem(pos))];
	dlg.DoModal();

	*pResult = 0;
}

void CAocTMDlg::OnBnClickedSummary()
{
	int index;
	CPlayerDatabase	players;
	CRecgameDatabase recgames;
	players.m_pRecgameDB = &recgames;

	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(pos)
	{
		while(pos)
		{
			index = m_List.GetItemData(m_List.GetNextSelectedItem(pos));
			players.Add(theApp.Recgames[index]);
			recgames.Add(theApp.Recgames[index]);
		}
	}
	else
	{
		for(int i = 0; i < m_List.GetItemCount(); i++)
		{
			index = m_List.GetItemData(i);
			players.Add(theApp.Recgames[index]);
			recgames.Add(theApp.Recgames[index]);
		}
	}

	players.CopyNickNames();
	ShowReport(&players);;
	
	players.Free();
}

//pubb, 14-12-13, add right click handler to set winner manually.
void CAocTMDlg::OnNMRClickRecList(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(!pos)
		return;

	CMenu menu, *pSubMenu;
	if (!menu.LoadMenu(IDR_RECPOPMENU))
		return;
	if (!(pSubMenu = menu.GetSubMenu(0)))
		return;
	CPoint point;
	GetCursorPos(&point);
	SetForegroundWindow();  

	::TrackPopupMenu(pSubMenu->m_hMenu, 0, point.x, point.y, 0, 
					 GetSafeHwnd(), NULL);
	PostMessage(WM_NULL, 0, 0);
	menu.DestroyMenu();

	*pResult = 0;
}

void CAocTMDlg::OnSetWinner(UINT command)
{
	int team;
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	switch(command)
	{
	case ID_SETWINNER_0:
		team = 0;
		break;
	case ID_SETWINNER_1:
		team = 1;
		break;
	case ID_SETWINNER_2:
		team = 2;
		break;
	case ID_SETWINNER_CLEAR:
		team = -1;
		break;
	default:
		//probably unable to run here
		team = theApp.Recgames[m_List.GetItemData(m_List.GetNextSelectedItem(pos))]->GetWinnerTeam();
	}
	pos = m_List.GetFirstSelectedItemPosition();
	while(pos)
	{
		theApp.Recgames[m_List.GetItemData(m_List.GetNextSelectedItem(pos))]->ManualWinnerTeam = team;
	}
	Refresh();
}

//pubb, 14-12-13, call grouping according to players in one played game.
void CAocTMDlg::GroupThis()
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(!pos)
		return;
	CRecgame * rec = theApp.Recgames[m_List.GetItemData(m_List.GetNextSelectedItem(pos))];
	if(!rec)
		return;
	int index;
	theApp.ClearCurrentPlayers();
	for(int i = 0, j = 0; i < rec->Players.GetCount(); i++)
	{
		index = theApp.Players.GetFirstSamePlayer(rec->Players[i].Name);
		if(index < 0)
			continue;
		theApp.CurrentPlayers[j++] = index;
	}

	theApp.bCurrent = true;
	OnGrouping();
}

//pubb, 14-12-13, call grouping to see what's the best grouping for a played game.
void CAocTMDlg::OnRegroupThis()
{
	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(!pos)
		return;
	CRecgame * rec = theApp.Recgames[m_List.GetItemData(m_List.GetNextSelectedItem(pos))];
	if(!rec)
		return;
	int index;
	theApp.ClearCurrentPlayers();
	for(int i = 0, j = 0; i < rec->Players.GetCount(); i++)
	{
		index = theApp.Players.GetFirstSamePlayer(rec->Players[i].Name);
		if(index < 0)
			continue;
		theApp.CurrentPlayers[j++] = index;
	}

	theApp.bCurrent = true;

	theApp.Players.Update(false, CTime(0), rec->RecordTime - CTimeSpan(0, 0, 5, 0));
	OnGrouping();
	theApp.Players.Update();
}
