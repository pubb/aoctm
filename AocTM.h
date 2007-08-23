// AocTM.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "recgamedatabase.h"
#include "playerdatabase.h"
#include "db/sqliteengine.h"

// CAocTMApp:
// See AocTM.cpp for the implementation of this class
//

class CAocTMApp : public CWinApp
{
public:
	CAocTMApp();

	IPersistentInterface *Engine;
	CRecgameDatabase Recgames;
	CPlayerDatabase	Players;

	bool	OpenDatabase(void);

	int	CurrentPlayers[8];
	bool bCurrent;
	void ClearCurrentPlayers(void);

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

private:
	CString DB_File;

};

#define	DB_NAME	(_T("doctors.aoc"))

extern CAocTMApp theApp;