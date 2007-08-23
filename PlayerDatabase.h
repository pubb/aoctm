#pragma once
#include "db/persistentinterface.h"

class CPlayerDatabase
	: public CArray <CPlayer *, CPlayer *>
{
public:
	CPlayerDatabase(void);
	~CPlayerDatabase(void);

	//pubb, 07-08-02, no need for SQL-based version
	/*
	void	CountPlay(CRecgame & rg);
	void	CountWin(CRecgame & rg);
	*/
	
	INT_PTR	Add(CPlayer * player);
	void	Add(CRecgame * rg);

	int	Load(IPersistentInterface * engine, bool reset = false);
	bool	Save(IPersistentInterface * engine);
	void	Reset(IPersistentInterface * engine);

	void	RemoveAll(void);

	void	Update(void);

	INT_PTR GetFirstSamePlayer(CString name);
	int	GetPlayCount(CString name);
	int GetWinCount(CString name);
	int GetRating(CString name);
	int GetPaidFee(CString name);

	int GetAllPaidFee(void);
	int GetAllPlayCount(void);

private:
	CString	m_ConfigFile;

	void	Revert(void);
	int		LoadInitial(void);
	void	UpdateRatings(CRecgame * rg);
	void	UpdateFee(void);
};
