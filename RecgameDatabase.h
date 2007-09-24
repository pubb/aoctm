#pragma once

#include "language.h"
#include "db/persistentinterface.h"

/*
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	CASTLE_MSG_LANG_ID = 3037;
const int	IMPERIAL_MSG_LANG_ID = 3038;
const int	RESIGN_MSG_LANG_ID = 3013;

const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
const int	FEUDAL_MSG_LANG_ID = 3036;
*/

class CRecgameDatabase
	: public CArray<CRecgame *, CRecgame *>
{
public:
	CRecgameDatabase(void);
	~CRecgameDatabase(void);

	bool Initialize(void);
	int	Load(IPersistentInterface * engine);
	bool	Save(IPersistentInterface * engine);
	void	RemoveAll(void);
	void	Free(void);

	CString	GetMapName(int id);		//called in recgame initialization, from map_id to map_name
	CTime	GetFirstGameTime(void);
	CTime	GetLatestGameTime(void);
	INT_PTR	GetFirstSameRecgame(CRecgame * rg);

	bool Add(CRecgame * rg);

private:
	bool	dirty;
	CLanguage	m_lang;
	CString m_str_table[100];
	CString	m_game_path, m_rec_path;

	CString GetAOCRegistry(void);
};
