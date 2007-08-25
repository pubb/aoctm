#include "StdAfx.h"
#include "PlayerDatabase.h"
#include "csvfile.h"
#include "AocTM.h"

CPlayerDatabase::CPlayerDatabase(void)
{
}

CPlayerDatabase::~CPlayerDatabase(void)
{
	for(int i = 0; i < GetCount(); i++)
		delete GetAt(i);
}

INT_PTR CPlayerDatabase::GetFirstSamePlayer(CString name)
{
	for(int i = 0; i < GetCount(); i++)
		for(int j = 0; j < GetAt(i)->NickNames.GetCount(); j++)
			if(name == GetAt(i)->NickNames[j])
				return i;
	return -1;
}

int	CPlayerDatabase::Load(IPersistentInterface * engine, bool reset)
{
	//clear PlayerDatabase before ADD
	RemoveAll();

	if(reset)
		return LoadInitial();

	if(!engine)
		return -1;

	int i, count = engine->GetPlayerCount();
	int * ids = new int [count];
	engine->GetAllPlayersID(ids, count);

	for(i = 0; i < count; i++)
	{
		CPlayer	* player = new CPlayer;
		player->ID = ids[i];
		if(!player->Load(engine) || player->NickNames[0].IsEmpty())
		{
			delete player;
			continue;
		}
		CArray<CPlayer *, CPlayer *>::Add(player);
	}
	
	delete [] ids;
	return i;
}

int	CPlayerDatabase::LoadInitial(void)
{
	if(m_ConfigFile.IsEmpty())
	{
		CFileDialog configDlg(true, _T(".csv"), _T("config.csv"), 0, _T("CSV (Comma delimited) | *.csv"));
		if(configDlg.DoModal() == IDCANCEL)
			return 0;
		m_ConfigFile = configDlg.GetPathName();
	}

	CStringArray	config;
	CCsvFile	csv(m_ConfigFile, CFile::modeRead);

	csv.Read(config);	//skip header
	int count;
	for(count = 0; csv.Read(config); count++)
	{
		CPlayer	*player = new CPlayer;
		player->InitRating = player->Rating = _ttoi(config[0]);
		CPaidFee *fee = new CPaidFee;
		fee->Money = _ttoi(config[1]);
		player->Record_PaidFee.Add(fee);
		for(int i = 2; i < config.GetCount(); i++)
			if(!config[i].IsEmpty())
				player->NickNames.Add(config[i]);
		Add(player);
		config.RemoveAll();
	}

	Update();

	return count;
}

bool	CPlayerDatabase::Save(IPersistentInterface * engine)
{
	if(!engine)
		return false;

	//pubb, 07-08-23, will do BeginTx in it itself
	engine->ClearPlayers();

	//XXX, pubb, 07-08-11, clear table and then reinsert everything
	//by mep for performance
	if(!engine->BeginTx())
		return false;

	
	bool flag = true;
	for(int i = 0; i < GetCount(); i++)
	{
		//pubb, 07-08-12, mep's bug. and 'break' the loop if error occurs.
		//if( GetAt(i)->Save(engine) )
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

	return true;
}

void	CPlayerDatabase::RemoveAll(void)
{
	for(int i = 0; i < GetCount(); i++)
		delete GetAt(i);
	CArray <CPlayer *, CPlayer *>::RemoveAll();
}

void	CPlayerDatabase::Reset(IPersistentInterface * engine)
{
	Load(engine, true);
}

void	CPlayerDatabase::Revert(void)
{
	for(int i = 0; i < GetCount(); i++)
	{
		CPlayer * player = GetAt(i);
		player->Rating = player->InitRating;
		player->PlayCount = player->WinCount = 0;
	}
}


INT_PTR	CPlayerDatabase::Add(CPlayer * player)
{
	if(GetFirstSamePlayer(player->NickNames[0]) >= 0)
		return -1;
	return CArray<CPlayer *, CPlayer *>::Add(player);
}

void CPlayerDatabase::Add(CRecgame * rg)
{
	CPlayer * player;

	for(int i = 1; i <= rg->PlayerNum; i++)
	{
		if(rg->Players[i].Name.IsEmpty())
			continue;

		INT_PTR index = GetFirstSamePlayer(rg->Players[i].Name);
		if(index >= 0)
			player = GetAt(index);
		else
		{
			player = new CPlayer;
			player->NickNames.Add(rg->Players[i].Name);
			CArray<CPlayer *, CPlayer *>::Add(player);
		}
		player->PlayCount++;
		if(!rg->IsLoser(i))
			player->WinCount++;
		player->UpdateTime = rg->RecordTime;
	}
			
	UpdateRatings(rg);
}

#if 0
/* Algorithm is from GameZone Rating System:
 * R old is your current rating. 
 * R1 is the average rating of the winning players before the game. 
 * R2 is the average rating of the losing players before the game. 
 * f is a calculated point value between -1 and 1. f = max [ min [( R2 - R1 ) / 400 , 1 ], - 1 ] 
 * Winning Points = max [ 16 * ( 1 + f ), 1 ] 
 * Losing Points = min [ 16 * (- 1 - f ), - 1 ] ----the original formula from the web is wrong : 16 * ( -1 + f), corrected by onfire
 * For each win , your new rating = R old + ( winning points / number of winners ) 
 * Here's how to calculate your new rating for a loss: 
 * For each loss , your new rating = R old + ( losing points / number of losers ) 
 *
 * Not suitable for us. We need deltaR to be related to (Rw - Rl)
 */

void	CPlayerDatabase::UpdateRatings(CRecgame & rg)
{
	/* for simplicity, consider the condition of 2 teams */
	int	winteam, R_winner = 0, R_loser = 0;
	int	deltaR, winning_point, losing_point;
	int i,  winner_count = 0, loser_count = 0;
	INT_PTR	index;

	winteam = rg.GetWinnerTeam();
	if(winteam == 0)	//'an incomplete game'.
		return;

	for(i = 1; i <= 8; i++)
	{
		index = GetFirstSamePlayer(rg.player_name[i]);
		if(index < 0)
			continue;
		
		if(rg.player_team[i] ==  winteam)
		{
			winner_count++;
			R_winner += GetAt(index)->Rating;
		}
		else
		{
			loser_count++;
			R_loser += GetAt(index)->Rating;
		}
	}
	if(winner_count == 0 || loser_count == 0)
		return;

	R_winner /= winner_count;
	R_loser /= loser_count;

	deltaR = max(min((R_loser - R_winner) * 10 / 4, 1000), -1000);
	winning_point = max(16 * (1000 + deltaR), 1000) / 1000;
	losing_point = min(16 * (-1000 - deltaR), -1000) / 1000;

	for(i = 1; i <= 8; i++)
	{
		index = GetFirstSamePlayer(rg.player_name[i]);
		if(index < 0)
			continue;

		if(rg.player_resign[i])
			GetAt(index)->Rating += losing_point;
		else
			GetAt(index)->Rating += winning_point;
	}
}
#else

/* my algorithm:
 * R_winner is the average rating of the winner team,
 * R_loser is the average rating of the loser team,
 * R_old is the current rating for one player,
 * R_new is the new rating of the player after UpdateRating() calls
 * Assume:
 * for winner, R_new = R_old + deltaR;
 * for loser, R_new = R_old - deltaR.
 *
 * I use this formula:
 *		deltaR = a * POWER(R_winner - R_loser, 2) + b * (R_winner - R_loser) + c
 * and assume:
 *		1. abs(R_winner - R_loser) is no greater than DELTA_MAX for most of time;
 *		2. when R_winner is equal to R_loser (DELTA_ZERO = 0), deltaR is R_ZERO;
 *		3. when (R_winner - R_loser) is -DELTA_MAX, deltaR is R_MAX;
 *		4. when (R_winner - R_loser) is +DELTA_MAX, deltaR is R_MIN.
 *		5. for all conditions that abs(R_winner - R_loser) > DELTA_MAX, deltaR is no less than R_MIN and no greater than R_MAX.
 * then we have 3 points:
 *		(-DELTA_MAX, R_MAX), (0, R_ZERO), (DELTA_MAX, R_MIN)
 * According to Formula Langrange, the formula for delatR is (let R = R_winner - R_loser:
 *		deltaR = ((R - DELTA_ZERO) * (R - DELTA_MAX) * R_MAX / (R_MAX - R_ZERO) / (R_MAX - R_MIN))
 *					+ (R - (-DELTA_MAX)) * (R - DELTA_MAX) * R_ZERO /(R_ZERO - R_MAX) / (R_ZERO - R_MIN)
 *					+ (R - (-DELTA_MAX)) * (R - DELTA_ZERO) * R_MIN / (R_MIN - R_MAX) / (R_MIN - R_ZERO)
 *		deltaR = min(R_MAX, max(R_MIN, deltaR))
 */
void	CPlayerDatabase::UpdateRatings(CRecgame * rg)
{
	const int DELTA_MAX = 200;
	const int DELTA_ZERO = 0;
	const int R_ZERO = 10;
	const int R_MAX = 50;
	const int R_MIN = 1;

	/* for simplicity, consider the condition of 2 teams */
	int	winteam, R_winner = 0, R_loser = 0;
	int	deltaR;
	int i,  winner_count = 0, loser_count = 0;
	INT_PTR	index;

	winteam = rg->GetWinnerTeam();
	if(winteam == 0)	//'an incomplete game'.
		return;

	for(i = 1; i <= 8; i++)
	{
		index = GetFirstSamePlayer(rg->Players[i].Name);
		if(index < 0)
			continue;
		
		if(rg->Players[i].Team ==  winteam)
		{
			winner_count++;
			R_winner += GetAt(index)->Rating;
		}
		else
		{
			loser_count++;
			R_loser += GetAt(index)->Rating;
		}
	}
	if(winner_count == 0 || loser_count == 0)
		return;

	R_winner /= winner_count;
	R_loser /= loser_count;
	
	int R = R_winner - R_loser;
	deltaR = (R - DELTA_ZERO) * (R - DELTA_MAX) * R_MAX / ((-DELTA_MAX) - DELTA_ZERO) / ((-DELTA_MAX) - DELTA_MAX)
				+ (R - (-DELTA_MAX)) * (R - DELTA_MAX) * R_ZERO / (DELTA_ZERO - (-DELTA_MAX)) / (DELTA_ZERO - DELTA_MAX)
				+ (R - (-DELTA_MAX)) * (R - DELTA_ZERO) * R_MIN / (DELTA_MAX - (-DELTA_MAX)) / (DELTA_MAX - DELTA_ZERO);
	deltaR = min(R_MAX, max(R_MIN, deltaR));

	for(i = 1; i <= 8; i++)
	{
		index = GetFirstSamePlayer(rg->Players[i].Name);
		if(index < 0)
			continue;

		if(rg->IsLoser(i))
			GetAt(index)->Rating -= deltaR;
		else
			GetAt(index)->Rating += deltaR;
	}
}
#endif

int	CPlayerDatabase::GetPlayCount(CString name)
{
	INT_PTR index = GetFirstSamePlayer(name);
	if(index >= 0)
		return GetAt(index)->PlayCount;
	return 0;
}

int CPlayerDatabase::GetWinCount(CString name)
{
	INT_PTR index = GetFirstSamePlayer(name);
	if(index >= 0)
		return GetAt(index)->WinCount;
	return 0;
}

int CPlayerDatabase::GetRating(CString name)
{
	INT_PTR index = GetFirstSamePlayer(name);
	if(index >= 0)
		return GetAt(index)->Rating;
	return 0;
}

int CPlayerDatabase::GetPaidFee(CString name)
{
	INT_PTR index = GetFirstSamePlayer(name);
	if(index >= 0)
		return GetAt(index)->GetPaidFee();
	else
		return 0;
}

int CPlayerDatabase::GetAllPaidFee(void)
{
	int fee = 0;
	for(int i = 0; i < GetCount(); i++)
		for(int j = 0; j < GetAt(i)->Record_PaidFee.GetCount(); j++)
			fee += GetAt(i)->Record_PaidFee[j]->Money;

	return fee;
}

int CPlayerDatabase::GetAllPlayCount(void)
{
	int count = 0;
	for(int i = 0; i < GetCount(); i++)
		count += GetAt(i)->PlayCount;

	return count;
}

/* a player's is equal to that what he paid subtracted by his playcount times cost of each game. 
 * and the cost of each game is calculated from all paid divided by all playcounts.
 * updated each time a new game inserted into database.
 */
void CPlayerDatabase::UpdateFee(void)
{
	int eachcost = 0;
	int count = GetAllPlayCount();
	
	if(count != 0)
		eachcost = GetAllPaidFee() * 10000 / count;

	for(int i = 0; i < GetCount(); i++)
		GetAt(i)->Fee = GetAt(i)->GetPaidFee() - eachcost * GetAt(i)->PlayCount / 10000;
}

//regegenerate RecgameDatabase by tranverse all the recgames in DB in order of RecordTime and recalculate player's playcount, wincount, rating, updatetime and the rest fee
void CPlayerDatabase::Update(void)
{
	//recaculate in order
	Revert();
	for(int i = 0; i < theApp.Recgames.GetCount(); i++)
		Add(theApp.Recgames[i]);

	UpdateFee();
}