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
		GetAt(i)->Initialize();
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

		//pubb, 07-09-04, for techical stat
		if(rg->Players[i].FeudTime > 0)
		{
			player->FeudCount++;
			player->TotalFeudTime += rg->Players[i].FeudTime;
			if(player->MinFeudTime > rg->Players[i].FeudTime)
				player->MinFeudTime = rg->Players[i].FeudTime;
		}
		if(rg->Players[i].CstlTime > 0)
		{
			player->CstlCount++;
			player->TotalCstlTime += rg->Players[i].CstlTime;
			if(player->MinCstlTime > rg->Players[i].CstlTime)
				player->MinCstlTime = rg->Players[i].CstlTime;
		}
		if(rg->Players[i].ImplTime > 0)
		{
			player->ImplCount++;
			player->TotalImplTime += rg->Players[i].ImplTime;
			if(player->MinImplTime > rg->Players[i].ImplTime)
				player->MinImplTime = rg->Players[i].ImplTime;
		}
		if( (rg->Players[i].Civ > 0) && (rg->Players[i].Civ < 19) )
			player->Civs[rg->Players[i].Civ]++;
	
#if 0
		//DEBUG, pubb, 07-09-15, Civ == 255, strange. 2007-06-02 21'14 - old(1), 2007-07-26 21'25 - mXp(1), 2007-07-26 21'55 - pubb(1)
		else
		{
			CString str;
			str.Format(_T("%s\n%s - %d"), rg->FileName, rg->Players[i].Name, rg->Players[i].Civ);
			AfxMessageBox(str);
		}
#endif
	}
			
	UpdateRatings(rg);
}

#if 0
/* Algorithm is from GameZone Rating System:
 * R_old is your current rating. 
 * R1 is the average rating of the winning players before the game. 
 * R2 is the average rating of the losing players before the game. 
 * f is a calculated point value between -1 and 1. f = max [ min [( R2 - R1 ) / 400 , 1 ], - 1 ] 
 * Winning Points = max [ 16 * ( 1 + f ), 1 ] 
 * Losing Points = min [ 16 * (- 1 - f ), - 1 ] ----the original formula from the web is wrong : 16 * ( -1 + f), corrected by onfire
 * For each win , your new rating = R_old + ( winning points / number of winners ) 
 * Here's how to calculate your new rating for a loss: 
 * For each loss , your new rating = R_old + ( losing points / number of losers ) 
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
 *		1. abs(R_winner - R_loser) is no greater than wlR_MAX for most of time;
 *		2. when R_winner is equal to R_loser (wlR_ZERO = 0), deltaR is deltaR_ZERO;
 *		3. when (R_winner - R_loser) is -wlR_MAX, deltaR is deltaR_MAX;
 *		4. when (R_winner - R_loser) is +wlR_MAX, deltaR is deltaR_MIN.
 *		5. for all conditions that abs(R_winner - R_loser) > wlR_MAX, deltaR is no less than deltaR_MIN and no greater than deltaR_MAX.
 * then we have 3 points:
 *		(-wlR_MAX, deltaR_MAX), (0, deltaR_ZERO), (wlR_MAX, deltaR_MIN)
 * According to Formula Langrange, the formula for delatR is (let wlR = R_winner - R_loser):
 * deltaR = (wlR - wlR_ZERO) * (wlR - wlR_MAX) * deltaR_MAX / ((-wlR_MAX) - wlR_ZERO) / ((-wlR_MAX) - wlR_MAX)
 *				+ (wlR - (-wlR_MAX)) * (wlR - wlR_MAX) * deltaR_ZERO / (wlR_ZERO - (-wlR_MAX)) / (wlR_ZERO - wlR_MAX)
 *				+ (wlR - (-wlR_MAX)) * (wlR - wlR_ZERO) * deltaR_MIN / (wlR_MAX - (-wlR_MAX)) / (wlR_MAX - wlR_ZERO)
 * deltaR = min(deltaR_MAX, max(deltaR_MIN, deltaR))
 */
void	CPlayerDatabase::UpdateRatings(CRecgame * rg)
{
	const int wlR_MAX = 100;
	const int wlR_ZERO = 0;
	const int wlR_MIN = -200;
	const int deltaR_ZERO = 10;
	const int deltaR_MAX = 50;
	const int deltaR_MIN = 1;

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
	
	//pubb, 07-08-26, change to use 'float' to calculate for more accuracy
	float wlR = (float)(R_winner - R_loser);
	/* XXX, pubb, 07-08-31
	 * if wlR is much greater than wlR_MAX (a very rare case),
	 * the function maybe curve up again.
	 * it'll be a very strange result since deltaR will be greater than 1 at that time
	 * so, do the following check...
	 */
	if(wlR >= wlR_MAX)
	{
		deltaR = 1;
	}
	else
	{
#if 1
		/* XXX, pubb, 07-08-31, not a good way to get the function */
		deltaR = (int)((wlR - wlR_ZERO) * (wlR - wlR_MAX) * deltaR_MAX / (wlR_MIN - wlR_ZERO) / (wlR_MIN - wlR_MAX)	
					+ (wlR - wlR_MIN) * (wlR - wlR_MAX) * deltaR_ZERO / (wlR_ZERO - wlR_MIN) / (wlR_ZERO - wlR_MAX)
					+ (wlR - wlR_MIN) * (wlR - wlR_ZERO) * deltaR_MIN / (wlR_MAX - wlR_MIN) / (wlR_MAX - wlR_ZERO));
#else
		/* XXX, pubb, 07-08-31, not a good way too */
		/* pubb, 07-08-31
		 * for y = ax^2+bx+c, c = deltaR_ZERO
		 * let '-b/2a = wlR_MAX',
		* then 'b = -2 * wlR_MAX * a'.
		* for point (x,y) = (-wlR_MAX, deltaR_MAX), a*(-wlR_MAX)^2 -2*wlR_MAX*a*(-wlR_MAX) + deltaR_ZERO = deltaR_MAX
		*/
		float a = (float)1.0 * (deltaR_MAX - deltaR_ZERO) / ((-wlR_MAX) * (-wlR_MAX) - 2 * wlR_MAX * (-wlR_MAX));
		// y = (1/3000) * x^2 -(4/30) * x + 10
		deltaR = (int)(a * wlR * wlR - 2 * wlR_MAX * a * wlR + deltaR_ZERO);
#endif
		deltaR = min(deltaR_MAX, max(deltaR_MIN, deltaR));
	}

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

//regegenerate RecgameDatabase by tranverse all the recgames in DB in order of RecordTime and recalculate player's playcount, wincount, rating, updatetime and the rest fee
void CPlayerDatabase::Update(CTime from, CTime to)
{
	//recaculate in order
	Revert();
	for(int i = 0; i < theApp.Recgames.GetCount(); i++)
	{
		CRecgame * rg = theApp.Recgames[i];
		if(rg->RecordTime < from)
			continue;
		if(rg->RecordTime > to)
			break;
		Add(rg);
	}
}

int CPlayerDatabase::GetAllCostFee(void)
{
	/* XXX, pubb, 07-08-28, not a good way */
	return 875;
}

void	CPlayerDatabase::GetRatings(CTime when)
{
	Update(CTime(0), when);
}