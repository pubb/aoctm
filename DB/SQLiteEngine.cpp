/********************************************************
*
*	Author : Huang Jie
*   2007.08.01
*
********************************************************/

#include "stdafx.h"
#include "SQLiteEngine.h"
#pragma warning(disable:4800)


#ifdef _UNICODE
#define SQLITE3_OPEN sqlite3_open16
#define SQLITE3_PREPARE(a, b, c, d)	sqlite3_prepare16_v2(a, b, -1, &c, (const void **)&d)
#define SQLITE3_BIND_TEXT(a, b, c) sqlite3_bind_text16(a, b, (LPCWSTR)c, -1, 0)
#define SQLITE3_COLUMN_TEXT sqlite3_column_text16
#else
#define SQLITE3_OPEN sqlite3_open
#define SQLITE3_PREPARE(a, b, c, d)	sqlite3_prepare(a, b, -1, &c, &d)
#define SQLITE3_BIND_TEXT(a, b, c) sqlite3_bind_text(a, b, (const char*)c, -1, 0)
#define SQLITE3_COLUMN_TEXT sqlite3_column_text
#endif

/* pubb, 07-08-02 
 * all 'NickName' to 'NickNames'
 * all 'PaidFee' to 'Record_PaidFee'
 * all 'ViewerName' to 'ViewerID', text to integer
 */
/* pubb, 07-08-11
 * add a 'playernum' in recgame table
 */
/* pubb, 07-08-23
 * all 'Payed' to 'Paid'
 */
//pubb, 07-08-02, move from .h file
const TCHAR* sqliteTableDefs[] = {
_T("create table t_Player \
(  \
	player_id integer primary key autoincrement, \
	fee	integer default 0, \
	initrating integer default ") DEF_RATING_STR _T(", \
	rating integer default ") DEF_RATING_STR _T(", \
	playcount integer default 0, \
	wincount integer default 0, \
	updatetime integer  \
);"),
_T("create table t_RecGame \
( \
	recgame_id integer primary key autoincrement, \
	filename text, \
	recordtime integer, \
	playtime integer, \
	playernum integer, \
	viewerid integer, \
	mapname text, \
	size integer, \
	hardlevel integer, \
	victory integer, \
	fulltechtree integer \
);"),
_T("create table t_PlayerInGame \
( \
	recgame_id integer not null, \
	name text not null, \
	team integer not null, \
	civ integer not null, \
	color integer not null, \
	positionx integer, \
	positiony integer, \
	feudaltime integer, \
	castletime integer, \
	imperialtime integer, \
	resigntime integer \
);"),
_T("create table t_ChatInfo \
( \
	recgame_id integer, \
	name text, \
	isingame integer, \
	chattime integer, \
	target integer, \
	content text \
);") ,
_T("create table t_PaidFee \
( \
	player_id integer not null, \
	paytime integer not null, \
	fee integer \
);"), 
_T("create table t_PlayerName \
( \
	name text unique not null, \
	player_id integer not null \
);")
};


SQLitePersisten::SQLitePersisten()
	: m_DB(0)
{
}

SQLitePersisten::~SQLitePersisten()
{
	if(m_DB != 0)
		Close();

}

bool 
SQLitePersisten::CreateNewDatabase(/*in*/ const TCHAR* openString)
{
	if(!Open(openString))
		return false;

	for(int i = 0; i < 6; i++)
	{
		sqlite3_stmt *stmt;
		const char* unused = 0;


		int ret = SQLITE3_PREPARE(m_DB, sqliteTableDefs[i], stmt, unused);

		if( ret != SQLITE_OK)
		{
			Close();
			return false;
		}

		if(sqlite3_step(stmt) != SQLITE_DONE)
		{
			sqlite3_finalize(stmt);
			Close();
			return false;
		}

		sqlite3_finalize(stmt);
	}
	
	Close();
	return true;
}

bool
SQLitePersisten::Open(const TCHAR* openString)
{
	int ret;

	ret = SQLITE3_OPEN(openString, &m_DB);

	if(ret != SQLITE_OK)
		return false;
	else
		return true;
}

void
SQLitePersisten::Close()
{
	if(m_DB)
		sqlite3_close(m_DB);

	m_DB = 0;
}

/* 07-08-04, pubb, changed back to GetAllPlayersID()
 * 'IDs' is allocated outside the routine and return all players' ID in it.
 * 'count' stores the max number of ready-to-load 'IDs', and return the actual number of loaded
 */
bool 
SQLitePersisten::GetAllPlayersID(/*out*/ int *PlayerIDs, /*in out*/ int& count)
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	const TCHAR* queryStr = _T("select player_id from t_Player");

	int ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return false;
	}

	int i = 0;

	while( sqlite3_step(stmt) != SQLITE_DONE && i < count)
	{
		PlayerIDs[i++] = sqlite3_column_int(stmt, 0);
	}

	count = i;

	sqlite3_finalize(stmt);

	return true;
}

int 
SQLitePersisten::GetPlayerCount(void)
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	//const TCHAR* queryStr = _T("select player_id from t_Player");
	//huangjie, 07-08-03
	const TCHAR* queryStr = _T("select count(*) from t_Player");

	int ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return -1;
	}

	int i = 0;

	/*
	while( sqlite3_step(stmt) != SQLITE_DONE)
		i++;
	*/

	//huangjie, 07-08-03, when user count() function in SQL
	
	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		i = sqlite3_column_int(stmt, 0);
	}

	sqlite3_finalize(stmt);

	return i;
}

/* XXX, pubb, 07-08-11
 * CreatePlayer to SavePlayer
 * ignore ID value to avoid conflict and simplify
 */
bool 
SQLitePersisten::SavePlayer(/*inout*/ CPlayer& player)
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	//INSERT to t_Player
	/* allocate an auto-incremental 'player_id' automatically, and get it out again */
	const TCHAR* insertStr = _T("insert into t_Player (fee,	initrating, rating,	playcount, wincount, updatetime) values (?, ?, ?, ?, ?, ?)");

	int ret = SQLITE3_PREPARE(m_DB, insertStr, stmt, unused);
	
	if( ret != SQLITE_OK)
	{
		return false;
	}

	sqlite3_bind_int(stmt, 1, player.Fee);
	sqlite3_bind_int(stmt, 2, player.InitRating);
	sqlite3_bind_int(stmt, 3, player.Rating);
	sqlite3_bind_int(stmt, 4, player.PlayCount);
	sqlite3_bind_int(stmt, 5, player.WinCount);
	sqlite3_bind_int(stmt, 6, (int)player.UpdateTime.GetTime());

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	/* pubb, 07-08-10, BAD way
	 * use 'SELECT max(player_id) from t_Player' instead
	 */
	/*
	const TCHAR* queryStr = _T("select player_id from t_Player where updatetime = ?");
	ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return false;
	}

	sqlite3_bind_int(stmt, 1, (int)player.UpdateTime.GetTime());

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		player.ID = sqlite3_column_int(stmt, 0);
	}
	else
	{
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_finalize(stmt);
	*/
	const TCHAR* queryStr = _T("select max(player_id) from t_Player");
	ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return false;
	}

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		player.ID = sqlite3_column_int(stmt, 0);
	}
	else
	{
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_finalize(stmt);

	//INSERT to t_PlayerName
	int i = 0;

	for(i = 0; i < player.NickNames.GetSize(); i++)
	{
		sqlite3_stmt *stmt;
		const char* unused = 0;

		const TCHAR* insertStr = _T("insert into t_PlayerName values (?, ?)");

		int ret = SQLITE3_PREPARE(m_DB, insertStr, stmt, unused);

		if( ret != SQLITE_OK)
		{
			return false;
		}

		SQLITE3_BIND_TEXT(stmt, 1, player.NickNames[i]);

		sqlite3_bind_int(stmt, 2, player.ID);

		if(sqlite3_step(stmt) != SQLITE_DONE)
		{
			sqlite3_finalize(stmt);
			return false;
		}
		else
		{
			sqlite3_finalize(stmt);
		}
	}

	//INSERT to t_PaidFee
	for(i = 0; i < player.Record_PaidFee.GetSize(); i++)
	{
		sqlite3_stmt *stmt;
		const char* unused = 0;

		const TCHAR* insertStr = _T("insert into t_PaidFee values (?, ?, ?)");

		int ret = SQLITE3_PREPARE(m_DB, insertStr, stmt, unused);
		
		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, player.ID);
		sqlite3_bind_int(stmt, 2, (int)player.Record_PaidFee[i]->PayTime.GetTime());
		sqlite3_bind_int(stmt, 3, player.Record_PaidFee[i]->Money);

		if(sqlite3_step(stmt) != SQLITE_DONE)
		{
			sqlite3_finalize(stmt);
			return false;
		}
		else
		{
			sqlite3_finalize(stmt);
		}
	}

	return true;
}

//by mep 2007-08-14. Loading is slow.
bool 
SQLitePersisten::LoadPlayer(/*in out*/ CPlayer& player)
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	if(player.ID > 0)
	{
		const TCHAR* queryStr = _T("select fee, initrating, rating, playcount, wincount, updatetime from t_Player where player_id = ?");

		int ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);
		
		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, player.ID);

		if(sqlite3_step(stmt) != SQLITE_DONE)
		{
			player.Fee = sqlite3_column_int(stmt, 0);
			player.InitRating = sqlite3_column_int(stmt, 1);
			player.Rating = sqlite3_column_int(stmt, 2);
			player.PlayCount = sqlite3_column_int(stmt, 3);
			player.WinCount = sqlite3_column_int(stmt, 4);
			player.UpdateTime = (CTime)sqlite3_column_int(stmt, 5);
			sqlite3_finalize(stmt);
		}
		else
		{
			sqlite3_finalize(stmt);
			return false;
		}

		const TCHAR* queryStr2 = _T("select name from t_PlayerName where player_id = ?");

		ret = SQLITE3_PREPARE(m_DB, queryStr2, stmt, unused);

		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, player.ID);

		while(sqlite3_step(stmt) != SQLITE_DONE)
		{
			player.NickNames.Add((const TCHAR*)SQLITE3_COLUMN_TEXT(stmt, 0));
		}

		sqlite3_finalize(stmt);	

		//pubb, 07-08-11, check the NickNames, not null
		if(player.NickNames.GetCount() <= 0)
			return false;

		const TCHAR* queryStr3 = _T("select paytime, fee from t_PaidFee where player_id = ?");

		ret = SQLITE3_PREPARE(m_DB, queryStr3, stmt, unused);

		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, player.ID);

		while(sqlite3_step(stmt) != SQLITE_DONE)
		{
			CPaidFee * payedFee = new CPaidFee;
			payedFee->PayTime = (CTime)sqlite3_column_int(stmt, 0);
			payedFee->Money = sqlite3_column_int(stmt, 1);
			player.Record_PaidFee.Add(payedFee);
		}

		/* pubb, 07-09-04
		 * move it to CPlayerDatabase::Update() with no DB operations
		 */
#if 0
		/* XXX, pubb, 07-08-25
		 * not a good place to do statics here.
		 * when LoadInitial(), there'll be no chance to get the statics information.
		 * add the function in PlayerDatabase::Reset() or PlayerDatabase::LoadInitial() if really want to do it here
		 */
		//by mep for statistic

		//select DISTINCT(t_PlayerInGame.recgame_id), t_PlayerInGame.civ, t_PlayerInGame.feudaltime, t_PlayerInGame.castletime, t_PlayerInGame.imperialtime
		//from t_PlayerInGame, t_PlayerName
		//where t_PlayerInGame.name in (select name from t_PlayerName where Player_id = ?);
		const TCHAR* queryStr4 = _T("select DISTINCT(t_PlayerInGame.recgame_id), t_PlayerInGame.civ, t_PlayerInGame.feudaltime, t_PlayerInGame.castletime, t_PlayerInGame.imperialtime")
		_T(" from t_PlayerInGame, t_PlayerName")
		_T(" where t_PlayerInGame.name in (select name from t_PlayerName where Player_id = ?)");

		ret = SQLITE3_PREPARE(m_DB, queryStr4, stmt, unused);

		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, player.ID);

		int feudcount = 0;
		int cstlcount = 0;
		int implcount = 0;

		player.MinFeudTime = player.MinCstl = player.MinImpl = 9999;
		player.AvgFeud = player.AvgCstl = player.AvgImpl = 0;

		while(sqlite3_step(stmt) != SQLITE_DONE)
		{
			int civ = sqlite3_column_int(stmt, 1);
			int ft = sqlite3_column_int(stmt, 2);
			int ct = sqlite3_column_int(stmt, 3);
			int it = sqlite3_column_int(stmt, 4);

			if( (civ > 0) && (civ < 19) )
				player.Civs[civ]++;

			if( ft != 0 )
			{
				feudcount++;
				player.AvgFeud += ft;

				if( player.MinFeudTime > ft)
					player.MinFeudTime = ft;
			}

			if( ct != 0 )
			{
				cstlcount++;
				player.AvgCstl += ct;

				if( player.MinCstl > ct )
					player.MinCstl = ct;
			}

			if( it != 0 )
			{
				implcount++;
				player.AvgImpl += it;

				if( player.MinImpl > it )
					player.MinImpl = it;
			}
			
		}

		if( feudcount != 0 )
		{
			player.AvgFeud = player.AvgFeud / feudcount;
		}

		if( cstlcount != 0 )
		{
			player.AvgCstl = player.AvgCstl / cstlcount;
		}

		if( implcount != 0 )
		{
			player.AvgImpl = player.AvgImpl / implcount;
		}

		sqlite3_finalize(stmt);
#endif
	}
	else if(player.NickNames.GetSize() > 0)
	{
		const TCHAR* queryStr0 = _T("select player_id from t_Player, t_PlayerName where t_Player.player_id = t_PlayerName.player_id and t_PlayerName.name = ?");

		int ret = SQLITE3_PREPARE(m_DB, queryStr0, stmt, unused);

		if( ret != SQLITE_OK)
		{
			return false;
		}

		SQLITE3_BIND_TEXT(stmt, 1, player.NickNames[0]);

		if(sqlite3_step(stmt) != SQLITE_DONE)
		{
			player.ID = sqlite3_column_int(stmt, 0);
		}
		else
		{
			sqlite3_finalize(stmt);	
			return false;
		}

		sqlite3_finalize(stmt);	

		const TCHAR* queryStr = _T("select fee, initrating, rating, playcount, wincount, updatetime from t_Player where player_id = ?");

		ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);
		
		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, player.ID);

		if(sqlite3_step(stmt) != SQLITE_DONE)
		{
			player.Fee = sqlite3_column_int(stmt, 0);
			player.InitRating = sqlite3_column_int(stmt, 1);
			player.Rating = sqlite3_column_int(stmt, 2);
			player.PlayCount = sqlite3_column_int(stmt, 3);
			player.WinCount = sqlite3_column_int(stmt, 4);
			player.UpdateTime = sqlite3_column_int(stmt, 5);
			sqlite3_finalize(stmt);
		}
		else
		{
			sqlite3_finalize(stmt);
			return false;
		}

		const TCHAR* queryStr2 = _T("select name from t_PlayerName where player_id = ?");

		ret = SQLITE3_PREPARE(m_DB, queryStr2, stmt, unused);

		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, player.ID);

		player.NickNames.RemoveAll();

		while(sqlite3_step(stmt) != SQLITE_DONE)
		{
			player.NickNames.Add((const TCHAR*)SQLITE3_COLUMN_TEXT(stmt, 0));
		}

		sqlite3_finalize(stmt);	

		const TCHAR* queryStr3 = _T("select paytime, fee from t_PaidFee where player_id = ?");

		ret = SQLITE3_PREPARE(m_DB, queryStr3, stmt, unused);

		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, player.ID);

		while(sqlite3_step(stmt) != SQLITE_DONE)
		{
			CPaidFee * payedFee = new CPaidFee;
			payedFee->PayTime = (CTime)sqlite3_column_int(stmt, 0);
			payedFee->Money = sqlite3_column_int(stmt, 1);
			player.Record_PaidFee.Add(payedFee);
		}

		/* pubb, 07-09-04
		 * do it in PlayerDatabase::Update() with no DB operations
		 */
#if 0
		//by mep for statistic
		//select DISTINCT(t_PlayerInGame.recgame_id), t_PlayerInGame.civ, t_PlayerInGame.feudaltime, t_PlayerInGame.castletime, t_PlayerInGame.imperialtime
		//from t_PlayerInGame, t_PlayerName
		//where t_PlayerInGame.name in (select name from t_PlayerName where Player_id = ?);

		const TCHAR* queryStr4 = _T("select DISTINCT(t_PlayerInGame.recgame_id), t_PlayerInGame.civ, t_PlayerInGame.feudaltime, t_PlayerInGame.castletime, t_PlayerInGame.imperialtime")
		_T(" from t_PlayerInGame, t_PlayerName")
		_T(" where t_PlayerInGame.name in (select name from t_PlayerName where Player_id = ?)");

		ret = SQLITE3_PREPARE(m_DB, queryStr4, stmt, unused);

		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, player.ID);

		int feudcount = 0;
		int cstlcount = 0;
		int implcount = 0;

		player.MinFeudTime = player.MinCstl = player.MinImpl = 9999;
		player.AvgFeud = player.AvgCstl = player.AvgImpl = 0;

		while(sqlite3_step(stmt) != SQLITE_DONE)
		{
			int civ = sqlite3_column_int(stmt, 1);
			int ft = sqlite3_column_int(stmt, 2);
			int ct = sqlite3_column_int(stmt, 3);
			int it = sqlite3_column_int(stmt, 4);

			if( (civ > 0) && (civ < 19) )
				player.Civs[civ]++;

			if( ft != 0 )
			{
				feudcount++;
				player.AvgFeud += ft;

				if( player.MinFeudTime > ft)
					player.MinFeudTime = ft;
			}

			if( ct != 0 )
			{
				cstlcount++;
				player.AvgCstl += ct;

				if( player.MinCstl > ct )
					player.MinCstl = ct;
			}

			if( it != 0 )
			{
				implcount++;
				player.AvgImpl += it;

				if( player.MinImpl > it )
					player.MinImpl = it;
			}
		}

		if( feudcount != 0 )
		{
			player.AvgFeud = player.AvgFeud / feudcount;
		}

		if( cstlcount != 0 )
		{
			player.AvgCstl = player.AvgCstl / cstlcount;
		}

		if( implcount != 0 )
		{
			player.AvgImpl = player.AvgImpl / implcount;
		}

		sqlite3_finalize(stmt);
#endif
	}
	else
	{
		//huangjie, 07-08-03, if ID = 0 and NickNames len = 0 then return false
		return false;
	}

	return true;
}

/* XXX, pubb, 07-08-10, CREATE-after-DELETE. why not using UPDATE? */
bool 
SQLitePersisten::UpdatePlayer(/*in*/ CPlayer& player)
{
	if(!BeginTx())
		return false;

	/* XXX, pubb, 07-08-10, check the existence of the player before DELETE */
	CPlayer temp;
	temp.ID = player.ID;
	if(LoadPlayer(temp) && !DeletePlayer(temp))
	{
		Rollback();
		return false;
	}
	
	if(!SavePlayer(player))
	{
		Rollback();
		return false;
	}

	Commit();

	return true;
}

bool 
SQLitePersisten::DeletePlayer(/*in*/ CPlayer& player)
{
	//by mep for performance
	if(!BeginTx())
		return false;

	sqlite3_stmt *stmt;
	const char* unused = 0;

	//pubb, 07-08-10, bad DELETE syntax, changed
	//const TCHAR* deleteStr = _T("delete t_Player where player_id = ?");
	const TCHAR* deleteStr = _T("delete from t_Player where player_id = ?");

	int ret = SQLITE3_PREPARE(m_DB, deleteStr, stmt, unused);
	
	if( ret != SQLITE_OK)
	{
		Rollback();
		return false;
	}

	sqlite3_bind_int(stmt, 1, player.ID);

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		Rollback();
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	//pubb, 07-08-10, bad DELETE syntax, changed
	//const TCHAR* deleteStr2 = _T("delete t_PlayerName where player_id = ?");
	const TCHAR* deleteStr2 = _T("delete from t_PlayerName where player_id = ?");

	ret = SQLITE3_PREPARE(m_DB, deleteStr2, stmt, unused);
	
	if( ret != SQLITE_OK)
	{
		Rollback();
		return false;
	}

	sqlite3_bind_int(stmt, 1, player.ID);

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		Rollback();
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	//pubb, 07-08-10, bad DELETE syntax, changed
	//const TCHAR* deleteStr3 = _T("delete t_PaidFee where player_id = ?");
	const TCHAR* deleteStr3 = _T("delete from t_PaidFee where player_id = ?");

	ret = SQLITE3_PREPARE(m_DB, deleteStr3, stmt, unused);
	
	if( ret != SQLITE_OK)
	{
		Rollback();
		return false;
	}

	sqlite3_bind_int(stmt, 1, player.ID);

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		Rollback();
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	Commit();

	return true;
}

/* pubb, 07-08-10
 * DELETE all items for t_Player and t_PlayerName
 * by MEPP
 */
bool SQLitePersisten::ClearPlayers(void)
{
	//by mep for performance
	if(!BeginTx())
		return false;

	sqlite3_stmt *stmt;
	const char* unused = 0;
	TCHAR* queryStr;

	queryStr = _T("delete from t_PlayerName");
	int ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);
	if( ret != SQLITE_OK)
	{
		Rollback();
		return false;
	}

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		Rollback();
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	queryStr = _T("delete from t_Player");
	ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);
	if( ret != SQLITE_OK)
	{
		Rollback();
		return false;
	}
	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		Rollback();
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	queryStr = _T("delete from t_PaidFee");
	ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);
	if( ret != SQLITE_OK)
	{
		Rollback();
		return false;
	}
	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		Rollback();
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	Commit();

	return true;
}

/* XXX, pubb, 07-08-04, added based on GetPlayerCount() */
int 
SQLitePersisten::GetRecgameCount(void)
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	const TCHAR* queryStr = _T("select count(*) from t_RecGame");

	int ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return -1;
	}

	int i = 0;

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		i = sqlite3_column_int(stmt, 0);
	}

	sqlite3_finalize(stmt);

	return i;
}

/* XXX, pubb, 07-08-04, rename to 'GetAllRecGamesID()'
 * RecGameIDs, allocated outside the routine and return all recgameIDs in DB.
 */
bool 
SQLitePersisten::GetAllRecGamesID(/*out*/ int* RecGameIDs, /*in out*/ int& count)
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	/* pubb, 07-08-10
	 * select PROCESSED games or AlL games
	 * ORDER BY RECORDTIME ASC
	 * wait for mepp to check...
	 */
	TCHAR* queryStr = _T("select recgame_id from t_RecGame order by recordtime asc");

	int ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return false;
	}

	int i = 0;

	while( sqlite3_step(stmt) != SQLITE_DONE && i < count)
	{
		RecGameIDs[i ++] = sqlite3_column_int(stmt, 0);
	}

	count = i;

	sqlite3_finalize(stmt);

	return true;
}

bool 
SQLitePersisten::SaveRecGame(/*in*/ CRecgame& recGame)
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	const TCHAR* insertStr = _T("insert into t_RecGame ")
		_T("(filename, recordtime, playtime, playernum, viewerid, mapname, size, hardlevel, victory, fulltechtree)")
		_T(" values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

	int ret = SQLITE3_PREPARE(m_DB, insertStr, stmt, unused);

	
	if( ret != SQLITE_OK)
	{
		return false;
	}


	SQLITE3_BIND_TEXT(stmt, 1, recGame.FileName);

	//XXX, pubb, 07-08-03, is it enough to store a 64b time_t to int? it's seconds from 1970.1.1.midnight
	sqlite3_bind_int(stmt, 2, (int)recGame.RecordTime.GetTime());
	//pubb, 07-08-03, CTimeSpan style, applied to other CTimeSpan STORE
	//sqlite3_bind_int(stmt, 3, (int)recGame.PlayTime.GetTime());
	sqlite3_bind_int(stmt, 3, (int)recGame.PlayTime.GetTotalSeconds());
	sqlite3_bind_int(stmt, 4, (int)recGame.PlayerNum);
	//pubb, 07-08-02, changed to ViewerID
	//SQLITE3_BIND_TEXT(stmt, 4, recGame.ViewerName);
	sqlite3_bind_int(stmt, 5, recGame.ViewerID);
	SQLITE3_BIND_TEXT(stmt, 6, recGame.Map.Name);
	sqlite3_bind_int(stmt, 7, recGame.Map.MapSizeLevel);
	sqlite3_bind_int(stmt, 8, recGame.Map.HardLevel);
	sqlite3_bind_int(stmt, 9, recGame.Map.Victory);
	sqlite3_bind_int(stmt, 10, recGame.Map.FullTechTree);

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	/* pubb, 07-08-11, BAD way
	 * use 'SELECT max(player_id) from t_Player' instead
	 */
	//const TCHAR* queryStr = _T("select recgame_id from t_RecGame where recordtime = ?");
	const TCHAR* queryStr = _T("select max(recgame_id) from t_RecGame");
	ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return false;
	}

	//sqlite3_bind_int(stmt, 1, (int)recGame.RecordTime.GetTime());

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		recGame.ID = sqlite3_column_int(stmt, 0);
	}
	else
	{
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_finalize(stmt);

	int i;
	for(i = 0; i < recGame.Players.GetSize(); i++)
	{
		const TCHAR* insertStr2 = 
			_T("insert into t_PlayerInGame ")
			_T("(recgame_id, name, team, civ, color, positionx, positiony, ")
			_T("feudaltime, castletime, imperialtime, resigntime)")
			_T(" values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

		int ret = SQLITE3_PREPARE(m_DB, insertStr2, stmt, unused);
		
		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, recGame.ID);
		SQLITE3_BIND_TEXT(stmt, 2, recGame.Players[i].Name);
		sqlite3_bind_int(stmt, 3, recGame.Players[i].Team);
		sqlite3_bind_int(stmt, 4, recGame.Players[i].Civ);
		sqlite3_bind_int(stmt, 5, recGame.Players[i].Color);
		sqlite3_bind_int(stmt, 6, recGame.Players[i].PositionX);
		sqlite3_bind_int(stmt, 7, recGame.Players[i].PositionY);
		sqlite3_bind_int(stmt, 8, (int)recGame.Players[i].FeudTime.GetTotalSeconds());
		sqlite3_bind_int(stmt, 9, (int)recGame.Players[i].CstlTime.GetTotalSeconds());
		sqlite3_bind_int(stmt, 10, (int)recGame.Players[i].ImplTime.GetTotalSeconds());
		sqlite3_bind_int(stmt, 11, (int)recGame.Players[i].ResignTime.GetTotalSeconds());

		if(sqlite3_step(stmt) != SQLITE_DONE)
		{
			sqlite3_finalize(stmt);
			return false;
		}
		else
		{
			sqlite3_finalize(stmt);
		}
	}

	for(i = 0; i < recGame.ChatB4Game.GetSize(); i++)
	{
		const TCHAR* insertStr2 = 
			_T("insert into t_ChatInfo ")
			_T("(recgame_id, name, isingame, chattime, target, content)")
			_T(" values (?, ?, 0, ?, ?, ?)");

		int ret = SQLITE3_PREPARE(m_DB, insertStr2, stmt, unused);
		
		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, recGame.ID);
		SQLITE3_BIND_TEXT(stmt, 2, recGame.ChatB4Game[i]->Name);
		sqlite3_bind_int(stmt, 3, (int)recGame.ChatB4Game[i]->Time.GetTotalSeconds());
		sqlite3_bind_int(stmt, 4, recGame.ChatB4Game[i]->Target);
		SQLITE3_BIND_TEXT(stmt, 5, recGame.ChatB4Game[i]->Content);


		if(sqlite3_step(stmt) != SQLITE_DONE)
		{
			sqlite3_finalize(stmt);
			return false;
		}
		else
		{
			sqlite3_finalize(stmt);
		}
	}

	for(i = 0; i < recGame.ChatInGame.GetSize(); i++)
	{
		const TCHAR* insertStr2 = 
			_T("insert into t_ChatInfo ")
			_T("(recgame_id, name, isingame, chattime, target, content)")
			_T(" values (?, ?, 1, ?, ?, ?)");

		int ret = SQLITE3_PREPARE(m_DB, insertStr2, stmt, unused);
		
		if( ret != SQLITE_OK)
		{
			return false;
		}

		sqlite3_bind_int(stmt, 1, recGame.ID);
		SQLITE3_BIND_TEXT(stmt, 2, recGame.ChatInGame[i]->Name);
		sqlite3_bind_int(stmt, 3, (int)recGame.ChatInGame[i]->Time.GetTotalSeconds());
		sqlite3_bind_int(stmt, 4, recGame.ChatInGame[i]->Target);
		SQLITE3_BIND_TEXT(stmt, 5, recGame.ChatInGame[i]->Content);

		if(sqlite3_step(stmt) != SQLITE_DONE)
		{
			sqlite3_finalize(stmt);
			return false;
		}
		else
		{
			sqlite3_finalize(stmt);
		}
	}
	return true;
}

//Load recgame from DB, acoording to the recGame.ID
bool 
SQLitePersisten::LoadRecGame(/*in out*/ CRecgame& recGame)
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	//XXX, pubb, 07-08-11, no chance to 'load by filename', so no need this code segment. commented out
	//XXX, pubb, 07-08-11, why?? changed
	//if(recGame.FileName != "" && !recGame.FileName.IsEmpty())
	/*
	if(!recGame.FileName.IsEmpty())
			const TCHAR* queryStr0 = _T("select recgame_id from t_RecGame where filename = ?");

		int ret = SQLITE3_PREPARE(m_DB, queryStr0, stmt, unused);

		if( ret != SQLITE_OK)
		{
			return false;
		}

		SQLITE3_BIND_TEXT(stmt, 1, recGame.FileName);

		if(sqlite3_step(stmt) != SQLITE_DONE)
		{
			recGame.ID = sqlite3_column_int(stmt, 0);
		}
		else
		{
			sqlite3_finalize(stmt);	
			return false;
		}

		sqlite3_finalize(stmt);	
	}
	*/

	if(recGame.ID <= 0)
		return false;

	const TCHAR* queryStr = _T("select filename, recordtime, playtime, playernum, viewerid, mapname, size, ")
		_T(" hardlevel, victory, fulltechtree")
		_T(" from t_RecGame where recgame_id = ?");

	int ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);	
	if( ret != SQLITE_OK)
	{
		return false;
	}

	sqlite3_bind_int(stmt, 1, recGame.ID);

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		recGame.FileName = (const TCHAR*)SQLITE3_COLUMN_TEXT(stmt, 0);
		recGame.RecordTime = (CTime)sqlite3_column_int(stmt, 1);
		//pubb, 07-08-03, changed to use CTimeSpan(seconds), applied to other LOAD, too
		recGame.PlayTime = CTimeSpan(sqlite3_column_int(stmt, 2));
		recGame.PlayerNum = sqlite3_column_int(stmt, 3);
		//pubb, 07-08-02, changed to ViewerID as int
		recGame.ViewerID = sqlite3_column_int(stmt, 4);
		recGame.Map.Name = (const TCHAR*)SQLITE3_COLUMN_TEXT(stmt, 5);
		recGame.Map.MapSizeLevel = sqlite3_column_int(stmt, 6);
		recGame.Map.HardLevel = sqlite3_column_int(stmt, 7);
		recGame.Map.Victory = sqlite3_column_int(stmt, 8);
		recGame.Map.FullTechTree = sqlite3_column_int(stmt, 9);

		sqlite3_finalize(stmt);
	}
	else
	{
		sqlite3_finalize(stmt);
		return false;
	}

	const TCHAR* queryStr2 = _T("select name, team, civ, color, positionx, positiony, ")
		_T("feudaltime, castletime, imperialtime, resigntime ")
		_T(" from t_PlayerInGame where recgame_id = ?");

	ret = SQLITE3_PREPARE(m_DB, queryStr2, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return false;
	}

	sqlite3_bind_int(stmt, 1, recGame.ID);

	//pubb, 07-08-11, removeall before ADD, because the constructor of recGame called SetSize(9) for Players
	recGame.Players.RemoveAll();
	while(sqlite3_step(stmt) != SQLITE_DONE)
	{
		CPlayerInGame playerInGame;
		playerInGame.Name = (const TCHAR*)SQLITE3_COLUMN_TEXT(stmt, 0);
		playerInGame.Team = sqlite3_column_int(stmt, 1);
		playerInGame.Civ = sqlite3_column_int(stmt, 2);
		playerInGame.Color = sqlite3_column_int(stmt, 3);
		playerInGame.PositionX = sqlite3_column_int(stmt, 4);
		playerInGame.PositionY = sqlite3_column_int(stmt, 5);
		playerInGame.FeudTime = CTimeSpan(sqlite3_column_int(stmt, 6));
		playerInGame.CstlTime = CTimeSpan(sqlite3_column_int(stmt, 7));
		playerInGame.ImplTime = CTimeSpan(sqlite3_column_int(stmt, 8));
		playerInGame.ResignTime = CTimeSpan(sqlite3_column_int(stmt, 9));
		recGame.Players.Add(playerInGame);
	}

	sqlite3_finalize(stmt);

	const TCHAR* queryStr3 = _T("select name, chattime, target, content")
		_T(" from t_ChatInfo where recgame_id = ? and isingame = 0");

	ret = SQLITE3_PREPARE(m_DB, queryStr3, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return false;
	}

	sqlite3_bind_int(stmt, 1, recGame.ID);

	while(sqlite3_step(stmt) != SQLITE_DONE)
	{
		CChatInfo * chatInfo = new CChatInfo;
		chatInfo->Name = (const TCHAR*)SQLITE3_COLUMN_TEXT(stmt, 0);
		chatInfo->Time = CTimeSpan(sqlite3_column_int(stmt, 1));
		chatInfo->Target = sqlite3_column_int(stmt, 2);
		chatInfo->Content = (const TCHAR*)SQLITE3_COLUMN_TEXT(stmt, 3);
		recGame.ChatB4Game.Add(chatInfo);
	}
	sqlite3_finalize(stmt);

	const TCHAR* queryStr4 = _T("select name, chattime, target, content")
		_T(" from t_ChatInfo where recgame_id = ? and isingame = 1");

	ret = SQLITE3_PREPARE(m_DB, queryStr4, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return false;
	}

	sqlite3_bind_int(stmt, 1, recGame.ID);

	while(sqlite3_step(stmt) != SQLITE_DONE)
	{
		CChatInfo * chatInfo = new CChatInfo;
		chatInfo->Name = (const TCHAR*)SQLITE3_COLUMN_TEXT(stmt, 0);
		chatInfo->Time = CTimeSpan(sqlite3_column_int(stmt, 1));
		chatInfo->Target = sqlite3_column_int(stmt, 2);
		chatInfo->Content = (const TCHAR*)SQLITE3_COLUMN_TEXT(stmt, 3);
		recGame.ChatInGame.Add(chatInfo);
	}
	sqlite3_finalize(stmt);
	
	return true;
}

bool
SQLitePersisten::UpdateRecGame(/*in*/ CRecgame & recGame)
{
	//search the DB for recGame.ID, and replace it with recGame

	BeginTx();
	
	//delete record games by id
	sqlite3_stmt *stmt;
	const char* unused = 0;

	//pubb, 07-08-10, bad DELETE syntax, changed
	//const TCHAR* deleteStr = _T("delete t_RecGame where recgame_id = ?");
	const TCHAR* deleteStr = _T("delete from t_RecGame where recgame_id = ?");

	int ret = SQLITE3_PREPARE(m_DB, deleteStr, stmt, unused);
	
	if( ret != SQLITE_OK)
	{
		Rollback();
		return false;
	}

	sqlite3_bind_int(stmt, 1, recGame.ID);

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		Rollback();
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	//pubb, 07-08-10, bad DELETE syntax, changed
	//const TCHAR* deleteStr2 = _T("delete t_PlayerInGame where recgame_id = ?");
	const TCHAR* deleteStr2 = _T("delete from t_PlayerInGame where recgame_id = ?");

	ret = SQLITE3_PREPARE(m_DB, deleteStr2, stmt, unused);
	
	if( ret != SQLITE_OK)
	{
		Rollback();
		return false;
	}

	sqlite3_bind_int(stmt, 1, recGame.ID);

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		Rollback();
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	//pubb, 07-08-10, bad DELETE syntax, changed
	//const TCHAR* deleteStr3 = _T("delete t_ChatInfo where recgame_id = ?");
	const TCHAR* deleteStr3 = _T("delete from t_ChatInfo where recgame_id = ?");

	ret = SQLITE3_PREPARE(m_DB, deleteStr3, stmt, unused);
	
	if( ret != SQLITE_OK)
	{
		Rollback();
		return false;
	}

	sqlite3_bind_int(stmt, 1, recGame.ID);

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		Rollback();
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	//huangjie, 07-08-03, resave record game
	if(SaveRecGame(recGame))
	{
		Commit();
	}
	else
	{
		Rollback();
		return false;
	}

	return true;
}

/* pubb, 07-08-10
 * DELETE all recgames in DB
 * by MEPP
 */
bool SQLitePersisten::ClearRecgames(void)
{
	sqlite3_stmt *stmt;
	const char* unused = 0;
	TCHAR* queryStr;

	queryStr = _T("delete from t_RecGame");
	int ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);
	if( ret != SQLITE_OK)
	{
		return false;
	}
	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	queryStr = _T("delete from t_PlayerInGame");
	ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);
	if( ret != SQLITE_OK)
	{
		return false;
	}
	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	queryStr = _T("delete from t_ChatInfo");
	ret = SQLITE3_PREPARE(m_DB, queryStr, stmt, unused);
	if( ret != SQLITE_OK)
	{
		return false;
	}
	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	return true;
}

bool 
SQLitePersisten::BeginTx()
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	const TCHAR* beginStr = _T("begin");

	int ret = SQLITE3_PREPARE(m_DB, beginStr, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return false;
	}

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}
	
	return true;
}

bool 
SQLitePersisten::Commit()
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	const TCHAR* commitStr = _T("commit");

	int ret = SQLITE3_PREPARE(m_DB, commitStr, stmt, unused);
	
	if( ret != SQLITE_OK)
	{
		return false;
	}

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	return true;
}

bool 
SQLitePersisten::Rollback()
{
	sqlite3_stmt *stmt;
	const char* unused = 0;

	const TCHAR* rollbackStr = _T("rollback");

	int ret = SQLITE3_PREPARE(m_DB, rollbackStr, stmt, unused);

	if( ret != SQLITE_OK)
	{
		return false;
	}

	if(sqlite3_step(stmt) != SQLITE_DONE)
	{
		sqlite3_finalize(stmt);
		return false;
	}
	else
	{
		sqlite3_finalize(stmt);
	}

	return true;
}

SQLiteEnginFactory::SQLiteEnginFactory()
{
}

IPersistentInterface*
SQLiteEnginFactory::Create()
{
	return new SQLitePersisten;
}

SQLiteEnginFactory factory;

SelfRegister sqliteSelfRegister(_T("SQLite"), &factory);