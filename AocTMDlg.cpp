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
	str.Format(_T("AocTM v%s build %s"), AOCTM_VERSION, CTime::GetCurrentTime().Format(_T("%y%m%d")));
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
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIME_FROM, &CAocTMDlg::OnDtnDatetimechange)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIME_TO, &CAocTMDlg::OnDtnDatetimechange)
	ON_BN_CLICKED(IDC_REC_DISMISS, &CAocTMDlg::OnBnClickedRecDismiss)
	ON_COMMAND(ID_ACCELERATOR_LOAD, (AFX_PMSG)&CAocTMDlg::OnAcceleratorLoad)
	ON_NOTIFY(NM_DBLCLK, IDC_REC_LIST, &CAocTMDlg::OnNMDblclkRecList)
	ON_BN_CLICKED(IDC_SUMMARY, &CAocTMDlg::OnBnClickedSummary)
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
	bool loadnew = false;
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

		if(theApp.Recgames.Add(rg))
		{
			//pubb, 07-09-18, for temporary storage for one drag&drop
			recgames.Add(rg);
			if(!loadnew)
				loadnew = true;
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

	if(loadnew)
		theApp.Players.Update();

	players.CopyNickNames();
	ShowReport(&players);;
	Refresh();

	players.Free();

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
	// TODO: Add your command handler code here
	ShowReport(&theApp.Players);
}

void CAocTMDlg::OnStatistic()
{
	// TODO: Add your command handler code here
	Statdlg dlg;
	dlg.m_pPlayerDB = &theApp.Players;
	dlg.DoModal();
}

void CAocTMDlg::OnGrouping()
{
	// TODO: Add your command handler code here
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
	str.Format(_T("%3d games (%s to %s)"), theApp.Recgames.GetCount(), theApp.Recgames.GetFirstGameTime().Format(_T("%Y-%m-%d %H:%M:%S")), theApp.Recgames.GetLatestGameTime().Format(_T("%Y-%m-%d %H:%M:%S")));
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
			m_List.SetItemText(j, team_position, str);
#ifdef	XLISTCTRL_OLD
			//m_List.SetItemTextColor(j, team_position, player_color[k]);
#else
			m_List.SetItemColors(j, team_position, player_color[k], background_color);
#endif
		}
		//save recgame index in DB
		m_List.SetItemData(j++, i);
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
	theApp.Players.Update();

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

bool CAocTMDlg::OnAcceleratorLoad()
{
	CString strPath = theApp.Recgames.GetRecPath();

	if(strPath.IsEmpty())
		return false;

	TCHAR oldpath[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, oldpath);
	::SetCurrentDirectory(strPath);

	bool loadnew = false;
	CFileFind finder;
	BOOL bWorking = finder.FindFile(_T("*.mgx"));
	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		//07-10-11, pubb, to speedup, analyze the filename before process
		{
			if(Renamer::Parse(finder.GetFileName()) <= theApp.Recgames.GetLatestGameTime())
				continue;
		}
		
		CRecgame *rg = new CRecgame;

		if(!rg->Read(finder.GetFilePath()))
		{
			delete rg;
			continue;
		}
		
		if(rg->PlayTime < TIME_4_INCOMPLETE)	//if play time less than 20 min, consider it an INCOMPLETE game
		{
			delete rg;
			continue;
		}

		if(theApp.Recgames.Add(rg))
		{
			if(!loadnew)
				loadnew = true;
		}
		else
		{
			delete rg;
		}
	}

	if(loadnew)
	{
		theApp.Players.Update();
		Refresh();
	}

	::SetCurrentDirectory(oldpath);
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