/********************************************************
*
*	Author : Huang Jie
*   2007.08.01
*
********************************************************/

#pragma once

#include <afx.h>
#include <afxtempl.h>

#include "../player.h"
#include "../recgame.h"

class IPersistentInterface
{
public:
	virtual ~IPersistentInterface();
	virtual bool CreateNewDatabase(/*in*/ const TCHAR* openString) = 0;
	virtual bool Open(/*in*/ const TCHAR* openString) = 0;

	virtual bool ClearPlayers(void) = 0;
	virtual bool GetAllPlayersID(/*out*/ int * PlayerIDs, /*in out*/ int& count) = 0;
	virtual int	GetPlayerCount(void) = 0;
	virtual bool SavePlayer(/*inout*/ class CPlayer& player) = 0;
	virtual bool LoadPlayer(/*in out*/ CPlayer& player) = 0;
	virtual bool UpdatePlayer(/*in*/ CPlayer& player) = 0;
	virtual bool DeletePlayer(/*in*/ CPlayer& player) = 0;
	virtual void Close() = 0;

	virtual bool ClearRecgames(void) = 0;
	virtual bool GetAllRecGamesID(/*out*/ int* RecGameIDs, /*in out*/ int& count) = 0;
	virtual int	GetRecgameCount(void) = 0;
	virtual bool SaveRecGame(/*in*/ class CRecgame& recGame) = 0;
	virtual bool LoadRecGame(/*in out*/ CRecgame& recGame) = 0;
	//XXX pubb, 07-08-03, we need to update a recgame if we import a BAD recgame before. one occasion for BAD is 'no resign msg' in it so we can hardly decide who wins
	virtual bool UpdateRecGame(/*in*/ CRecgame & recGame) = 0;

	virtual bool LoadCharge(CArray<class CPaidFee *, class CPaidFee *> * charge, int id) = 0;
	virtual bool SaveCharge(CArray<CPaidFee *, CPaidFee *> * charge, int id) = 0;
	virtual bool DeleteCharge(int id) = 0;

	//by mep for performance
	virtual bool BeginTx() = 0;
	virtual bool Commit() = 0;
	virtual bool Rollback() = 0;
};

class PersistentEngineFactory
{
public:
	virtual IPersistentInterface* Create() = 0;
	static IPersistentInterface* CreatePersistentEngine(const TCHAR* engineName);
};

class EngineManager
{
	CArray <CString, CString&> m_EngineName;
	CArray <PersistentEngineFactory*, PersistentEngineFactory*> m_EngineFactory;
public:
	EngineManager();
	void RegisterEngine(const TCHAR* enginName, PersistentEngineFactory* factory);
	PersistentEngineFactory* FindEngineFactory(const TCHAR* enginName);
};

class SelfRegister
{
public:
	SelfRegister(const TCHAR* enginName, PersistentEngineFactory* factory);
	//07-08-03, pubb, add 'delete instance' in destruction
	~SelfRegister(void);
};
