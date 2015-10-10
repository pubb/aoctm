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
	void	SetDirty(bool dirty = true);

	CString	GetMapName(int id);		//called in recgame initialization, from map_id to map_name
	//pubb,14-09-28, use ID instead of CTime to simplify the algorithm, aiming to beautify RatingCurve with more accurate timespan
	INT_PTR	GetFirstGameID(CString name = NULL);
	INT_PTR	GetLastGameID(CString name = NULL);
	INT_PTR	GetFirstSameRecgame(CRecgame * rg);

	CString	GetRecPath(void);
	CString GetRecPathAOFE(void);

	bool Add(CRecgame * rg);

private:
	bool	m_bDirty;
	CLanguage	m_lang;
	CString m_str_table[100];
	CString	m_game_path, m_rec_path, m_rec_path_AOFE;

	CString GetAOCRegistry(void);
	bool LoadChatInfo(IPersistentInterface *engine);
};
