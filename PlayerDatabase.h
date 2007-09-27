#pragma once
#include "db/persistentinterface.h"
#include "recgamedatabase.h"

class CPlayerDatabase
	: public CArray <CPlayer *, CPlayer *>
{
public:
	CPlayerDatabase(void);
	~CPlayerDatabase(void);
	void	Free(void);

	CRecgameDatabase	* m_pRecgameDB;
	//pubb, 07-08-02, no need for SQL-based version
	/*
	void	CountPlay(CRecgame & rg);
	void	CountWin(CRecgame & rg);
	*/
	
	INT_PTR	Add(CPlayer * player);
	void	Add(CRecgame * rg);

	bool	Load(IPersistentInterface * engine);
	bool	Save(IPersistentInterface * engine);

	void	SetDirty(bool d = true);

	void	RemoveAll(void);

	void	Update(CTime from = CTime(0), CTime to = CTime::GetCurrentTime());

	INT_PTR GetFirstSamePlayer(CString name);

	int GetAllPaidFee(void);
	int GetAllPlayCount(void);
	void	GetRatings(CTime when = CTime::GetCurrentTime());

	void	CopyNickNames(void);

private:
	bool	dirty;
	CString	m_ConfigFile;
	int		Charge;		//charge to Net Pub

	void	Revert(void);
	void	UpdateRatings(CRecgame * rg);
};
