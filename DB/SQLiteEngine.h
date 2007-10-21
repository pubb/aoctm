/********************************************************
*
*	Author : Huang Jie
*   2007.08.01
*
********************************************************/

#pragma once

#include "PersistentInterface.h"
#include "sqlite3.h"

class SQLitePersisten: public IPersistentInterface
{
	sqlite3* m_DB;
public:
	SQLitePersisten();
	virtual ~SQLitePersisten();
	virtual bool CreateNewDatabase(/*in*/ const TCHAR* openString);
	virtual bool Open(/*in*/ const TCHAR* openString);

	virtual bool GetAllPlayersID(/*out*/ int * PlayerIDs, /*in out*/ int& count);
	virtual int	GetPlayerCount(void);

	virtual bool ClearPlayers(void);
	//pubb, 07-08-11, name changed
	//virtual bool CreatePlayer(/*inout*/ CPlayer& player);
	virtual bool SavePlayer(/*inout*/ CPlayer& player);
	virtual bool LoadPlayer(/*in out*/ CPlayer& player);
	virtual bool UpdatePlayer(/*in*/ CPlayer& player);
	virtual bool DeletePlayer(/*in*/ CPlayer& player);
	virtual void Close();

	virtual bool ClearRecgames(void);
	virtual bool GetAllRecGamesID(/*out*/ int* RecGameIDs, /*in out*/ int& count);
	virtual int	GetRecgameCount(void);
	virtual bool SaveRecGame(/*in*/ CRecgame& recGame);
	virtual bool LoadRecGame(/*in out*/ CRecgame& recGame);
	/* XXX, pubb, 07-08-03, 
	 * we need to update a recgame if we import a BAD recgame before. 
	 * one occasion for BAD is 'no resign msg' in it so we can hardly decide who wins.
	 * not used now, but better with it
	 */
	virtual bool UpdateRecGame(/*in*/ CRecgame & recGame);	//recGame.ID is set to the preferred position with the same ID in the DB.

	virtual bool LoadCharge(CArray<CPaidFee *, CPaidFee *> * charge, int id);
	virtual bool SaveCharge(CArray<CPaidFee *, CPaidFee *> * charge, int id);
	virtual bool DeleteCharge(int id);
	virtual bool LoadChatInfo(CRecgame & recGame);
	//by mep for performance
	virtual bool BeginTx();
	virtual bool Commit();
	virtual bool Rollback();
};

class SQLiteEnginFactory : public PersistentEngineFactory
{
public:
	SQLiteEnginFactory();
	virtual IPersistentInterface* Create();
};