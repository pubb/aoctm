// AocTM.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AocTM.h"
#include "AocTMDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAocTMApp

BEGIN_MESSAGE_MAP(CAocTMApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAocTMApp construction

CAocTMApp::CAocTMApp()
: Engine(NULL)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAocTMApp object

CAocTMApp theApp;


// CAocTMApp initialization

BOOL CAocTMApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("AOC Team Manager"));


	CFile f;
	if(!f.Open(DB_NAME, CFile::modeRead))
	{
		CFileDialog dbDlg(true, _T(".aoc"), DB_NAME, 0, _T("AOC Database File (*.aoc)|*.aoc|All Files (*.*)|*.*||"));
		if(dbDlg.DoModal() == IDCANCEL)
			return FALSE;
		DB_File = dbDlg.GetPathName();
	}
	else
	{
		f.Close();
		TCHAR	cwd[256];
		_tgetcwd(cwd, 256);
		DB_File = CString(cwd) + _T("\\") + DB_NAME;
	}

	Engine = PersistentEngineFactory::CreatePersistentEngine(_T("SQLite"));
	if(Engine == NULL)
		AfxMessageBox(_T("Database Engine Initialize FAILED."));


	Engine->CreateNewDatabase(DB_File);
/*
	while(!Engine->Open(DB_File))
	{
		CFileDialog dbDlg(true, _T(".aoc"), _T("doctors.aoc"), 0, _T("AOC (Database File) | *.aoc"));
		if(dbDlg.DoModal() == IDCANCEL)
		{
			//create a database
			Engine->CreateNewDatabase(DB_File);
			break;
		}
		 DB_File = dbDlg.GetPathName();
	}
*/

	//pubb, 07-08-04, open once, and close once during the program runs
	//huangjie, 07-08-03, we must open database explicit
	Engine-> Open(DB_File);

	Config.Load(Engine);

	Recgames.Initialize();
	Recgames.Load(Engine);
	/* pubb, 07-07-27
	 * load Players into PlayerDatabase from DB. only save back (and update) when exiting
	 */
	Players.Load(Engine);
	//pubb, 07-09-18, set related recgame db
	Players.m_pRecgameDB = &Recgames;

	/* pubb, 07-07-31
	 * for grouping
	 */
	ClearCurrentPlayers();

	CAocTMDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	
	//pubb, 07-08-04
	Players.Save(Engine);
	
	//pubb, 07-09-18
	Recgames.Save(Engine);

	Config.Save(Engine);

	//pubb, 07-09-18, explicitly clear memory
	Recgames.Free();
	Players.Free();

	/* pubb, 07-08-03 */
	Engine->Close();
	
	delete Engine;

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CAocTMApp::ClearCurrentPlayers(void)
{
	bCurrent = false;
	memset(CurrentPlayers, -1, sizeof(CurrentPlayers));
}