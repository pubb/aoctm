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
	bool	GetDirty(void);

	void	RemoveAll(void);

	void	Revert(void);
	void	Update(bool changed = false, CTime from = CTime(0), CTime to = CTime::GetCurrentTime());

	INT_PTR GetFirstSamePlayer(CString name);

	int GetAllPaidFee(void);
	int GetAllPlayCount(void);
	void	GetRatings(CTime when = CTime::GetCurrentTime());

	void	CopyPlayers(bool current = true, CPlayerDatabase * source = NULL);
	void	CopyNickNames(void);
	static int	GetOddMoreRating(const int orig_rating, const int more, const int less);
	static int GetCooperateRating(const int rating1, const int rating2);

private:
	bool	m_bDirty;
	CString	m_ConfigFile;
	int		Charge;		//charge to Net Pub

	void	UpdateRatings(CRecgame * rg);
};