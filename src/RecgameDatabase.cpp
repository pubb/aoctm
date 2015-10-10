#include "StdAfx.h"
#include "RecgameDatabase.h"
#include "AocTM.h"

CRecgameDatabase::CRecgameDatabase(void)
: m_bDirty(false)
, m_rec_path_AOFE(_T(""))
{
}

CRecgameDatabase::~CRecgameDatabase(void)
{
}

bool	CRecgameDatabase::Initialize()
{
	m_game_path = GetAOCRegistry();

	if(m_game_path.IsEmpty())
		return false;

	m_rec_path = m_game_path + _T("\\SaveGame\\");
	m_rec_path_AOFE = m_game_path + _T("\\Games\\Forgotten Empires\\SaveGame\\");

	//pubb, 07-08-02, no use for upgrade msg, and change the check method too
	//Open Language database
	if(m_lang.Open(m_game_path + _T("\\language.dll")) 
		&& m_lang.Open(m_game_path + _T("\\language_x1.dll"))
		&& m_lang.Open(m_game_path + _T("\\language_x1_p1.dll"))
		&& m_lang.Open(m_game_path + _T("\\Games\\Forgotten Empires\\Data\\language_x1_p1.dll"))
)
	{

	//pubb, 07-08-02, no use now
	/*
	CString feudal_msg;
	CString castle_msg;
	CString imperial_msg;
	CString resign_msg;
	feudal_msg = m_lang.getString(FEUDAL_MSG_LANG_ID);
	castle_msg = m_lang.getString(CASTLE_MSG_LANG_ID);
	imperial_msg = m_lang.getString(IMPERIAL_MSG_LANG_ID);
	resign_msg = m_lang.getString(RESIGN_MSG_LANG_ID);

	if(!feudal_msg.IsEmpty()){
		m_feudal_msg   = feudal_msg;
		m_castle_msg   = castle_msg;
		m_imperial_msg = imperial_msg;
		m_resign_msg   = resign_msg;
	*/	
		//Load map names, the index in m_str_table is 'map_id' read from recgame header.
		m_str_table[9]  = m_lang.getString(10875);
		m_str_table[10] = m_lang.getString(10876);
		m_str_table[11] = m_lang.getString(10877);
		m_str_table[12] = m_lang.getString(10878);
		m_str_table[13] = m_lang.getString(10879);
		m_str_table[14] = m_lang.getString(10880);
		m_str_table[15] = m_lang.getString(10881);
		m_str_table[16] = m_lang.getString(10882);
		m_str_table[17] = m_lang.getString(10883);
		m_str_table[18] = m_lang.getString(10884);
		m_str_table[19] = m_lang.getString(10885);
		m_str_table[20] = m_lang.getString(10886);
		m_str_table[21] = m_lang.getString(10887);
		m_str_table[22] = m_lang.getString(10888);
		m_str_table[23] = m_lang.getString(10889);
		m_str_table[24] = m_lang.getString(10890);
		m_str_table[25] = m_lang.getString(10891);
		m_str_table[26] = m_lang.getString(10892);
		m_str_table[27] = m_lang.getString(10894);
		m_str_table[28] = m_lang.getString(10893);
		m_str_table[29] = m_lang.getString(10895);
		m_str_table[30] = m_lang.getString(10896);
		m_str_table[31] = m_lang.getString(10897);
		m_str_table[32] = m_lang.getString(10898);
		m_str_table[33] = m_lang.getString(10901);
		m_str_table[48] = m_lang.getString(10902);
	}
	/*
	else{
		m_feudal_msg   = "%s advanced to the Feudal Age.";
		m_castle_msg   = "%s advanced to the Castle Age.";
		m_imperial_msg = "%s advanced to the Imperial Age.";
		m_resign_msg   = "%s resigned.";
	}
	*/

	return true;
}

int	CRecgameDatabase::Load(IPersistentInterface * engine)
{
	if(!engine)
		return -1;

	RemoveAll();

	m_bDirty = false;

	int i, count = engine->GetRecgameCount();
	if(count <= 0)
		return 0;

	int * ids = new int [count];
	engine->GetAllRecGamesID(ids, count);

	for(i = 0; i < count; i++)
	{
		CRecgame	* rg = new CRecgame;
		rg->ID = ids[i];
		if(!rg->Load(engine))
		{
			delete rg;
			break;
		}
		rg->Loaded = true;
		CArray<CRecgame *, CRecgame *>::Add(rg);
	}
	
	delete [] ids;
	return i;
}

bool CRecgameDatabase::Save(IPersistentInterface *engine)
{
	//by mep for performance
	if(!engine || !m_bDirty)
		return false;

	//load chatinfo before clearrecgames() because we don't load it at startup.
	if(!LoadChatInfo(engine))
		return false;

	//pubb, 07-10-27, move to begin of the db operations
	if( !engine->BeginTx() )
		return false;

	if(!engine->ClearRecgames())
	{
		engine->Rollback();
		return false;
	}

	bool flag = true;
	for(int i = 0; i < GetCount(); i++)
	{
		//pubb, 07-08-12, mep's bug.
		if( !GetAt(i)->Save(engine) )
		{
			flag = false;
			break;
		}
	}

	if(flag)
	{
		engine->Commit();
	}
	else
	{
		engine->Rollback();
		return false;
	}

	SetDirty(false);

	return true;
}

void	CRecgameDatabase::RemoveAll(void)
{
	for(int i = 0; i < GetCount(); i++)
		delete GetAt(i);
	CArray <CRecgame *, CRecgame *>::RemoveAll();
}

CString CRecgameDatabase::GetAOCRegistry(void)
{
	HKEY hKey;
	DWORD vartype;
	TCHAR buff[1024];
	DWORD buffsize = 1024;
	CString path;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		         _T("SOFTWARE\\Microsoft\\Microsoft Games\\Age of Empires II: The Conquerors Expansion\\1.0"),
		         0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		AfxMessageBox(_T("Error reading Registry!"));
		return _T("");
	}
	RegQueryValueEx(hKey, _T("EXE Path"), NULL, &vartype, (LPBYTE) &buff[0], &buffsize);
	RegCloseKey(hKey);

	path = buff;

	return path;
}

CString CRecgameDatabase::GetMapName(int id)
{
	if( (id >= 9 && id <= 33) || id == 48)
		return m_str_table[id];
	else
		if(id == 44)
			return _T("Custom unknown");
	return _T("");
}

bool	CRecgameDatabase::Add(CRecgame * rg)
{
	//pubb, 07-09-22, no DB operations now
	//if(!rg->Loaded || !theApp.Engine || GetFirstSameRecgame(rg) >= 0)
	if(!rg->Loaded)
		return false;
	int index = GetFirstSameRecgame(rg);
	if(index >= 0)
	{
		if(GetAt(index)->GetWinnerTeam() > 0 || rg->GetWinnerTeam() == 0)
			return false;
		delete GetAt(index);
		this->SetAt(index, rg);
	}
	else
	{
		//store it in order of RecordTime in memory. each LOAD-from-DB is also in RecordTime order.
		int i;
		for(i = 0; i < GetCount(); i++)
			if(GetAt(i)->RecordTime >= rg->RecordTime)
				break;
		InsertAt(i, rg);
	}

	SetDirty(true);

	return true;
}

/* pubb, 07-08-03, search the database, compare the player_name, playtime, recordtime and so on, if matched, return the index */
INT_PTR	CRecgameDatabase::GetFirstSameRecgame(CRecgame * rg)
{
	CRecgame * rg_db;

	INT_PTR	i;
	int j;

	//search from top to bottom because most of time, the duplicate one is near the top.
	for(i = GetCount() - 1; i >= 0; i--)
	{
		rg_db = GetAt(i);

		/* pubb, 07-08-03,
		 * if 2 games with
		 * 1.the same players, 
		 * 2. started in (TIME_4_INCOMPLETE - 5min) (12 min in this time), (for the same recgame with different Viewer, maybe the computers' system time is not syncronized)
		 * then, consider it a duplicate
		 * we ASSUME the RecordTime is absolutely CORRECT.
		 */
		if(rg_db->PlayerNum != rg->PlayerNum || _abs64((rg_db->RecordTime - rg->RecordTime).GetTotalSeconds()) >= TIME_4_INCOMPLETE - 5 * 60)
			continue;

		//speedup! because the game DB is sorted in RecordTime, so if it's a newer recgame, just return -1
		if(rg_db->RecordTime < rg->RecordTime && (rg->RecordTime - rg_db->RecordTime).GetTotalSeconds() >= TIME_4_INCOMPLETE - 5 * 60)
			return -1;

		for(j = 0; j <= rg_db->PlayerNum; j++)
			if(rg_db->Players[j].Name != rg->Players[j].Name)
				goto next;
		if(j > rg_db->PlayerNum)
		{
			return i;
		}
next:	;
	}
	return -1;
}

INT_PTR	CRecgameDatabase::GetLastGameID(CString name)
{
	if(GetCount() <= 0)
		return -1;

	if(name.IsEmpty())
		return GetCount() - 1;
	else
	{
		for(int i = GetCount() - 1; i >= 0; i--)
		{
			if(GetAt(i)->IsPlayerInGame(name))
				return i;
		}
		return -1;
	}
}

INT_PTR	CRecgameDatabase::GetFirstGameID(CString name)
{
	if(GetCount() <= 0)
		return -1;

	if(name.IsEmpty())
		return 0;
	else
	{
		for(int i = 0; i < GetCount(); i++)
		{
			if(GetAt(i)->IsPlayerInGame(name))
				return i;
		}
		return -1;
	}
}

void	CRecgameDatabase::Free(void)
{
	for(int i = 0; i < GetCount(); i++)
		delete GetAt(i);
}

void	CRecgameDatabase::SetDirty(bool dirty)
{
	if(m_bDirty != dirty)
		m_bDirty = dirty;
}

CString	CRecgameDatabase::GetRecPath(void)
{
	return m_rec_path;
}

//pubb, 07-10-24, load chatinfo before clearrecgames(), because we don't load it at startup
bool	CRecgameDatabase::LoadChatInfo(IPersistentInterface *engine)
{
	if(!engine->BeginTx())
		return false;

	for(int i = 0; i < GetCount(); i++)
	{
		if(!GetAt(i)->LoadChatInfo(engine))
		{
			engine->Rollback();
			return false;
		}
	}
	engine->Commit();
	return true;
}

CString CRecgameDatabase::GetRecPathAOFE(void)
{
	return m_rec_path_AOFE;
}
