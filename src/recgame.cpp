/* pubb, 07-07-23 
 * Originated from Bari's aocrec
 * Changed to Unicode compatible version
 */

#include "stdafx.h"
#include "recgame.h"
#include "AocTM.h"

#include "zconf.h"
#include "zlib.h"

#include "renamer/renamer.h"

//pubb, 07-10-31, change it to start from blue, since no one use player_color[0]
/*
COLORREF	player_color[9] =
{
 	RGB(0, 0, 0), 
	RGB(0, 0, 192), 
	RGB(224, 0, 0),
	RGB(0, 192, 0),
	RGB(192, 192, 0),
	RGB(0, 192, 192),
	RGB(192, 0, 128),
	RGB(128, 128, 128),
	RGB(255, 128, 0)
};
*/
COLORREF	player_color[8] =
{
	RGB(0, 0, 192), 
	RGB(224, 0, 0),
	RGB(0, 192, 0),
	RGB(192, 192, 0),
	RGB(0, 192, 192),
	RGB(192, 0, 128),
	RGB(128, 128, 128),
	RGB(255, 128, 0)
};

#ifdef _DEBUG
/*
#undef THIS_FILE
static TCHAR *THIS_FILE = _T(__FILE__);
*/
#define new DEBUG_NEW
#endif

//huangjie 07-08-03, copy constructor and operator =
CMapInfo::CMapInfo(CMapInfo& rh)
{
	Name = rh.Name;
	MapSizeLevel = rh.MapSizeLevel;					
	HardLevel = rh.HardLevel;					
	Victory = rh.Victory;								
	FullTechTree = rh.FullTechTree;	
}

CMapInfo& 
CMapInfo::operator = (CMapInfo& rh)
{
	Name = rh.Name;
	MapSizeLevel = rh.MapSizeLevel;					
	HardLevel = rh.HardLevel;					
	Victory = rh.Victory;								
	FullTechTree = rh.FullTechTree;	
	return *this;
}

CChatInfo::CChatInfo(unsigned char * chat, CTimeSpan time)
{
	//format: "@#nNAME: msg", for example, "@#4pubb: 1"
	CString str(chat);
	Player_Num = _ttoi(str.Mid(2, 1));
	int index = str.Find(':');
	if(index >= 3)	//"@#0-- ..." is a game tip msg
		Content = str.Mid(index + 2);
	Time = time;
	Target = 0;	//not used now
}

CChatInfo::CChatInfo(void)
{
}

//huangjie, 07-08-03, copy constructor
CChatInfo::CChatInfo(CChatInfo& rh)
{
	Player_Num = rh.Player_Num;
	Time = rh.Time;
	Target = rh.Target;
	Content = rh.Content;
}

//huangjie, 07-08-03, operator =
CChatInfo&
CChatInfo::operator = (CChatInfo& rh)
{
	Player_Num = rh.Player_Num;
	Time = rh.Time;
	Target = rh.Target;
	Content = rh.Content;

	return *this;
}

CPlayerInGame::CPlayerInGame(void)
: Name(_T("")), Team(-1), Civ(0), Color(0), PositionX(0), PositionY(0), 
  FeudTime(0), CstlTime(0), ImplTime(0),ResignTime(0),
  number_key(0)
{
	//pubb, 14-10-09, better implementation.
	memset(name_key, 0, 256);
}

CPlayerInGame::CPlayerInGame(CPlayerInGame & player)
{
	Name = player.Name;
	Team = player.Team;
	Civ = player.Civ;
	Color = player.Color;
	PositionX = player.PositionX;
	PositionY = player.PositionY;

	FeudTime = player.FeudTime;
	CstlTime = player.CstlTime;
	ImplTime = player.ImplTime;
	ResignTime = player.ResignTime;

	/* pubb, 07-08-02, copy from original, not sure for their meanings */
	//pubb, 14-10-09, better implementation.
	memcpy(name_key, player.name_key, 256);
	number_key = number_key;
}

//huangjie, 07-08-03, copy constructor and operator = 
CPlayerInGame&
CPlayerInGame::operator = (CPlayerInGame& rh)
{
	Name = rh.Name;
	Team =rh.Team;
	Civ = rh.Civ;
	Color = rh.Color;
	PositionX = rh.PositionX;
	PositionY = rh.PositionY;

	FeudTime = rh.FeudTime;
	CstlTime = rh.CstlTime;
	ImplTime = rh.ImplTime;
	ResignTime = rh.ResignTime;

	/* pubb, 07-08-02, copy from original, not sure for their meanings */
	//pubb, 14-10-09, better implementation.
	memcpy(name_key, rh.name_key, 256);
	number_key = number_key;

	return *this;
}

CRecgame::CRecgame()
: ID(0), Loaded(false), bSpecialRecordTime(false)
{
	//pubb, 07-10-23, moved from getGameData()
	ViewerID = 0;
	ManualWinnerTeam = -1;

	map_load_flg = 0;
	for(int j = 0; j <= 8 ; j++){
		player_index[j] = 0;
		player_resign[j] = true;	//assume everyone to be loser
	}

	map_size.cx = 0;
	map_size.cy = 0;
}

CRecgame::~CRecgame()
{
	/* pubb, 07-08-02, free chatinfo */
	int i;
	for(i = 0; i < ChatB4Game.GetCount(); i++)
		delete ChatB4Game[i];
	for(i = 0; i < ChatInGame.GetCount(); i++)
		delete ChatInGame[i];
}

bool CRecgame::Read(CString file)
{
	m_header_len = 0;
	m_body_len   = 0;

	if(!readFile(file))		//0 for failed
		return false;

	//prepare the space for later load. they are all accessed through 'Players[i]' directly
	Players.SetSize(9);

	//Analyze the filename and retrieve corrected Game Time from it.
	int index = file.ReverseFind('\\');
	FileName = file.Mid(index + 1);	//only FILENAME
	
	RecordTime = Renamer::Parse2Playtime(FileName);

	if(RecordTime == CTime(0))		//if the filename was renamed manually, then use 'file create time' for RecordTime
	{
		CFileStatus status;
		CFile::GetStatus(file, status);
		RecordTime = status.m_mtime;
		bSpecialRecordTime = true;
	}

	Loaded = getGameData();

	ClearMem();

	return true;
}

bool CRecgame::Load(IPersistentInterface * engine)
{
	if(!engine->LoadRecGame(*this))
		return false;
	FillWinner();
	return true;
}

bool CRecgame::Save(IPersistentInterface * engine)
{
	if(!engine || !Loaded)
		return false;

	engine->SaveRecGame(*this);
	return true;
}

void CRecgame::ClearMem(void)
{
	if(m_header_len != 0){
		delete[] m_pt_header;
		m_header_len = 0;
	}
	if(m_body_len != 0){
		delete[] m_pt_body;
		m_body_len = 0;		
	}
}

int CRecgame::readFile(CString filename)
{
	CFile recgame;				
	CFile tmp;					
	CString tmp_file;			
	CString fileExt;			
	unsigned int compresslen;	
	Bytef buff[0x20000];		
	z_stream strm;
	int code;

	
	if(m_header_len != 0){
		delete[] m_pt_header;
		m_header_len = 0;
	}
	if(m_body_len != 0){
		delete[] m_pt_body;
		m_body_len = 0;		
	}

	if(!(recgame.Open(filename, CFile::modeRead))){
		return 0;
	}
	recgame.Read(&compresslen, sizeof(int));

	
	if(recgame.GetLength() < compresslen || compresslen < 9){
		recgame.Close();
		return 0;
	}

	m_body_len = (unsigned long)(recgame.GetLength() - compresslen);

	fileExt = filename.Right(3);
	fileExt.MakeLower();
	if(fileExt != _T("mgl")){
		recgame.Seek(4, CFile::current);
		compresslen -= 8;
	}
	else{
		compresslen -= 4;
	}
	
	TCHAR *comp_buff = new TCHAR [compresslen];
	
	recgame.Read(comp_buff, compresslen);
	
	tmp_file = filename + _T(".tmp1");
	if(!(tmp.Open(tmp_file, CFile::modeCreate | CFile::modeReadWrite))){
		
		delete[] comp_buff;
		recgame.Close();
		return 0;
	}
	
	strm.zalloc    = (alloc_func)Z_NULL;
	strm.zfree     = (free_func)Z_NULL;
	strm.next_in   = (Bytef *)comp_buff;
	strm.avail_in  = compresslen;
	strm.avail_out = 0;
	
	code = inflateInit2(&strm, -15);
	m_header_len = 0;

	
	while (code == Z_OK && strm.avail_out == 0)
	{
		strm.next_out = buff;
		strm.avail_out = sizeof(buff);
		
		code = inflate(&strm, Z_SYNC_FLUSH);
		
		switch (code)
		{
		case Z_OK: 
			tmp.Write(buff, sizeof(buff));
			m_header_len += sizeof(buff);
			break;
		case Z_STREAM_END: 
			tmp.Write(buff, sizeof(buff) - strm.avail_out);
			m_header_len += sizeof(buff) - strm.avail_out;
			break;
		case Z_DATA_ERROR: 
			break;
		default: 
			break;
		}
	}
	
	inflateEnd(&strm);

	delete[] comp_buff;

	m_pt_header = new unsigned char [m_header_len];
	
	tmp.SeekToBegin();
	
	tmp.Read(m_pt_header, m_header_len);
	
	tmp.Close();
	
	CFile::Remove(tmp_file);

	
	m_pt_body = new unsigned char [m_body_len];
	
	recgame.Read(m_pt_body, m_body_len);
	
	recgame.Close();

	if(code != Z_STREAM_END){
		return 0;
	}
	return 1;
}

/* pubb, 07-08-02, no use */
/*
void CRecgame::getHeader(unsigned char **pt_header, unsigned int *length)
{
	*length = m_header_len;
	if(m_header_len != 0){
		*pt_header = m_pt_header;
	}
	else{
		pt_header = NULL;
	}
}

void CRecgame::getBody(unsigned char **pt_body, unsigned int *length)
{
	*length = m_body_len;
	if(m_body_len != 0){
		*pt_body = m_pt_body;
	}
	else{
		pt_body = NULL;
	}
}
*/

/* Fill the playernames from the rec.
 * Names are listed in the order of what lists in game lobby.
 * For a restored game, the order is maybe not the same as what the game begins with, so, there's a player_index array storing the order when beginning. 
 * The player_index will be used to decide the player's team, because the team information is stored in the order of begining (XXX).
 * For a '2in1' case, the controlling players will point to a single index in the play_index array.
 */

bool	CRecgame::SetPlayersName(int pos)
{
	int i;
	int player_name_len;
	unsigned char t_player_name[256];

	for(i = 0; i <= MAX_PLAYERS_INGAME; i++){
		player_index[i] = *((int*)&m_pt_header[pos]);
		if(player_index[i] > MAX_PLAYERS_INGAME) return false;

		pos += 8;
		
		player_name_len = *(int*)(&m_pt_header[pos]);
		//pubb, 16-01-12, strange player_name_len, but dont panic but continue, referring to game 01-06-13-18`40`33
		//if(player_name_len <= 0)	return false;
		if(player_name_len < 0) return false;
		if(player_name_len == 0)
		{
			pos += 4;
			continue;
		}

		pos += 4;

		//XXX, pubb, 07-10-31, this code segment should be considered specially for '2 v 1' condition
		if(player_index[i] != 0){
			
			memset(t_player_name, 0, 256);
			memcpy(t_player_name, &m_pt_header[pos], player_name_len);
			Players[i].Name = t_player_name;

			if(strlen(Players[player_index[i]].name_key) == 0)
			{
				memcpy(Players[player_index[i]].name_key, t_player_name, player_name_len);
				Players[player_index[i]].number_key = i;
			}
		}
		pos += player_name_len;
	}
	return true;
}

/* Fill players' Civ and Color.
 * The players' civ and color are stored in the order of game lobby order when the game begins whether it is a restored game or not.
 */
bool CRecgame::SetPlayersCivColor(int pos, int end_pos)
{
	int civ, color;
	int player_name_len, str_len, length;

	for(int j = 1; pos < end_pos; pos++)
	{
		player_name_len = strlen(Players[j].name_key);
		if(memcmp(&m_pt_header[pos], Players[j].name_key, player_name_len) == 0){
			str_len = *((short*)&m_pt_header[pos-2]) - 1;

			if(str_len == player_name_len){
				pos += (player_name_len + 1);
				//XXX, pubb, 07-11-04, 'length' is actually either 0 or 255(-1). just ignore it
				//so make 'pos' increases by 816
				//pubb, 14-06-27, restore original pos++ code for wrongly interpreting to a restored game
				pos += 807;
				length = *((short*)&m_pt_header[pos]);
				//length = 0;
				pos += 4;
				pos += length * 8;
				pos += 5;
				//*/
				//pos += 816;

				civ = m_pt_header[pos];
				pos += 1;
				pos += 3;
				//pubb, 07-10-31, 0 for 'blue'
				color = m_pt_header[pos];

				//pubb, 14-12-10, new method to fill color/civ information. leave the id controlling an cooperating player blank, until fill/copy its infor from the active player later.
				//there'll be some cases that more than 1 players have the same no..
				//if k1,k2 are controlling the same player j, then player_index[k1],player_index[k2] will both be equal to j, then player k1,k2 will be set to the same civ and color.
#if 0
				for(int k = 1; k < 9; k++)
				{
					if(player_index[k] == j)
					{
						Players[k].Civ = civ;
						Players[k].Color = color;
					}
				}
#else
				int index = Players[j].number_key;
				if(index != 0)
				{
					Players[index].Civ = civ;
					Players[index].Color = color;
				}
#endif

				j++;
				
				if(j > MAX_PLAYERS_INGAME){
					break;
				}
			}
		}
	}
	return true;
}

/* pubb, 09-03-29, rewrite the team setting algorithm
 * player team can be 'not set'(0), random(1), or set (team1=2,team2=3,...,team8=9)
 * XXX: only support 2 teams situation.
 * the team information are stored in the order what the game begins, whether a restored game or not.
 */
/* pubb, 14-12-11, bugfix for '2in1' condition since 14-10-08 patch.
 * pubb, 14-10-08, bugfix for restored game situation. bug example: 20141001-22:51:22, famin exchanged with web.
 */
/* pubb, 16-01-12, for the game which team members differ in count more than 1 (like 3v1, 4v2), just ignore it as a joke,
 */
bool	CRecgame::SetPlayersTeam(int pos)
{
	int j;
	int team = 0, team1 = 0, team2 = 0, index = 0;
	int team1count = 0, team2count = 0;

	//set the team number according the player1's team
	for(j = 0; j <= MAX_PLAYERS_INGAME; j++)
	{
		switch(m_pt_header[pos+j] - 1)
		{
		case 1:
		case 2:	
			team1 = m_pt_header[pos+j] - 1;
			break;
		case 0:
			continue;
		default:
			team2 = m_pt_header[pos+j] - 1;
			break;
		}
		if(team1 != 0)
			break;
	}

	/* pubb, 07-07-26, player_team as integer */
	bool	found;
	int	shift = 0;
	for(j = 1; j <= MAX_PLAYERS_INGAME; j++)
	{
		//pubb, 14-12-11, team info only meanful to one player when '2 in 1' condition. use number_key as index.
		//pubb, 07-08-04, set Players.Team
		//pubb, 09-01-04, skip null players
		for(found = false, index = 1; index <= PlayerNum; index++)
		{
			/*
			 * pubb, 14-12-11, number_key stores the current real position of players, whether a restored game or not.
			 * but for a '2in1' situation, the cooperating player will have no number_key stored, so the position has to be shifted forward to read the correct team information.
			 * and the cooperating player will have no team information filled. it'll be copied later.
			 */
			if(Players[index].number_key == j)
			{
				team = m_pt_header[pos + index + shift - 1] - 1;
				//pubb, 07-10-31, if not set team (that's '-' in the game), then set it manually
				//pubb, 09-01-04, buggy data (08-07-30-22:40), two in team 1, two in team '4'. Set it definitely to be team 1 and team 2
				//pubb, 09-03-29, rewritten according to preset team1 and team2 variables
				switch(team)
				{
				case 0:
					if(team1 == 0)	//no one set team
					{
						Players[j].Team = 1; team1count++;
						team1 = 1;
						team2 = 2;
					}
					else
					{
						Players[j].Team = 2; team2count++;	//usually there're only 2 players in this condition
					}
					break;
				default:
					if(team == team1)
					{
						Players[j].Team = 1; team1count++;
					}
					else
					{
						Players[j].Team = 2; team2count++;
					}
				}
				found = true;
				break;
			}
		}
		// not found a number_key equal to j, indicates to a '2in1' situation, so shift forward for next team reading.
		if(!found)
			shift++;
		//pubb, 14-12-10, leave '2 as 1' player's team blank until fill/copy its whole infor later.
		//pubb, 09-01-04, '2 as 1' condition will also be invalid team, set it manually
		//pubb, 14-10-07, bugfix move this code segment after team decision
	}
	//pubb, 16-01-12, ignore the game of 2v4 or 3v1 as a joke.
	team1count -= team2count; if(team1count > 1 || team1count < -1) return false;
	return true;
}

void	CRecgame::CopyCooperatingPlayerInfo(void)
{
	int j;
	
	for(j = 0; j <= MAX_PLAYERS_INGAME; j++)
	{
		if(player_index[j] != 0 && Players[j].Team < 0)
		{
			Players[j].Civ = Players[player_index[j]].Civ;
			Players[j].Color = Players[player_index[j]].Color;
			Players[j].Team = Players[player_index[j]].Team;
		}
	}
}

CString	CRecgame::GetMapName(void)
{
	//pubb, 07-11-07, make them a group of string for any possibilities. end with 'null'
	//pubb, 07-11-05, use CHAR instead of TCHAR to do correct compare
	/*
	char maptype_jp[15] = "マップの種類";	
	char maptype_en[15] = "Map Type";		
	char maptype_sp[15] = "Tipo de mapa";	
	char maptype_de[15] = "Kartentyp";		
	char maptype_cn[15] = "ｵﾘﾍｼﾀ獎ﾍ";		
	char maptype_c2[15] = "ｦaｹﾏﾃ�ｧO";		
	char maptype_kr[15] = "ﾁ�ｵｵ ﾁｾｷ\xf9";	
	*/
	char maptype_str[][32] = 
	{ "マップの種類",	//jp
	  "Map Type",		//en
	  "Tipo de mapa",	//sp
	  "Kartentyp",		//de
	  "ｵﾘﾍｼﾀ獎ﾍ",		//cn
	  "ｦaｹﾏﾃ�ｧO",		//tw
	  "ﾁ�ｵｵ ﾁｾｷ\xf9",	//kr
	  "ｵﾘﾍｼﾀ牾�",		//cn2
	  NULL
	};
	unsigned char map_buff[100];
	int i, j;

	for(i = m_header_len - 18000; i > 100000; i--)
	{
		//pubb, 07-11-07, fix a bug incurred by chinese version of aoc
		//pubb, 07-11-05, rearranged the code section for better view
		if((m_pt_header[i-1] == ':' && m_pt_header[i] == ' ') || (m_pt_header[i-1] == 0xa1 && m_pt_header[i] == 'G') || (m_pt_header[i-1] == 0xa3 && m_pt_header[i] == 0xba /* "｣ｺ" */))
		{ 
			//pubb, 07-11-07, to find 'map type'
			bool found = false;
			for(int t = 0; maptype_str[t][0] != NULL; t++)
			{
				char * p = maptype_str[t];
				int len = (int)strlen(p);
				if(memcmp(&m_pt_header[i - 1 - len], p, len) == 0)
				{
					found = true;
					break;
				}
			}
			if(found)
			{
				i = i + 1;
				memset(map_buff,'\0',100);
				for(j = 0; j < 100; j++){
					if(m_pt_header[i+j] == 0x0A){
						break;
					}
					map_buff[j] = m_pt_header[i+j];
				}
				break;
			}
		}
		else if(m_pt_header[i] == 'A')
		{
			if(m_pt_header[i-3] == 'G' && m_pt_header[i-2] == 'A' && m_pt_header[i-1] == 'I')
			{
				map_buff[0] = NULL;
				break;		
			}
		}
	}
	return (map_buff[0] == NULL ? Map.Name : CString(map_buff));
}

bool	CRecgame::GetPlayerDataPos(int & trigger_pos, int & player_data_pos, unsigned long tail)
{
	int i;
	//XXX, little endian
	const INT64 MAGIC1 = 0x3FF999999999999A;
	const INT32 MAGIC2 = 0xFFFFFF9D;
	const unsigned long MAX_INDEX = 18000;

	for(i = tail - sizeof(INT64) + 1 /* 15-11-06, pubb, reserve room for INT64 pointer */; i > MAX_INDEX; i--)
	{
		if(*(INT64 *)(m_pt_header + i) == MAGIC1 && trigger_pos == 0)
			trigger_pos = i + 8;
		if(*(INT32 *)(m_pt_header + i) == MAGIC2 && trigger_pos != 0)
		{
			player_data_pos = i;
			return true;
		}
	}
	return false;
}

//pubb, 14-10-09, rearrange players' name/civ/color/team and mapname code to seperated functions as above.
bool CRecgame::getGameData(void)
{
	char gaia[15]       = "GAIA";

	long i,j;
	int pos;

	int trigger_pos = 0;
	int player_data_pos = 0;
	int error = 0;

	int include_ai;
	short num_chat;
	int chat_len_work;
	short num_rule;
	int num_data;
	int num_float;
	int trigger_num;
	int effect_num;
	int selected_num;
	int condition_num;
	int chat_num;
	int length;
	int rec_wk;
	int type;
	int command;
	int command_len;
	int time_cnt;
	short player_id;
	short research_id;
	short unit_type_id;
	int object_id;
	short unit_num;
	/*
	int hour;
	int minute;
	int second;
	*/
	//CStringArray  str_chat;

	unsigned char age_flag[9] ={0,0,0,0,0,0,0,0,0};
	int chat_len;
	unsigned char player_number;
	unsigned char chat_msg[256];
	//CString str_edit;
	CString alt_map_name;
	CString str_work;
	CString str_advance;

	//pubb, 07-08-02, map_id not public now.
	int map_id = -1;

	//pubb, 07-11-07, no use now
	/*
	//pubb, 07-11-05, use CHAR instead of TCHAR to do correct compare
	int maptype_len_jp  = (int)strlen(maptype_jp);
	int maptype_len_en  = (int)strlen(maptype_en);
	int maptype_len_sp  = (int)strlen(maptype_sp);
	int maptype_len_de  = (int)strlen(maptype_de);
	int maptype_len_cn  = (int)strlen(maptype_cn);
	int maptype_len_c2  = (int)strlen(maptype_c2);
	int maptype_len_kr  = (int)strlen(maptype_kr);
	*/

	if(m_header_len == 0){
		return false;
	}
	
	//pubb, 15-09-19, magic words searching changed to be a function, enabling loop search in case that the magic words appear for multiple times.
	player_data_pos = m_header_len + 1;
	do
	{
		if(!GetPlayerDataPos(trigger_pos, player_data_pos, player_data_pos - 1)) return false; 
		
		pos = player_data_pos;
	
		pos += 12;
	
		//pubb, 07-11-05, for 'custom' map, if read the map name out from rec, then set it.
		//set mapname, standard or 'custom' for map_id = 44
		map_id = *((int*)&m_pt_header[pos]);
		Map.Name = (map_id == 44 ? GetMapName() : theApp.Recgames.GetMapName(map_id));
	} while(Map.Name.IsEmpty());
		
	pos += 4;
	pos += 8;
	
	//15-09-18, pubb, check the return value (bool). there're buggy records in some occasion.
	if(!SetPlayersName(pos)) return false;	

	pos = 0x0C;
	
	include_ai = *((int*)&m_pt_header[pos]);
	pos += 4;
	
	if(include_ai == 1){
		pos = 0x12;
		num_chat = *((short*)&m_pt_header[pos]);
		pos += 6;
		
		for(i = 0; i < num_chat; i++){
			chat_len_work = *((int*)&m_pt_header[pos]);
			pos += 4;
			pos += chat_len_work;
		}
		pos += 6;
		
		for(i = 0; i < 8; i++){
			pos += 10;
			num_rule = *((short*)&m_pt_header[pos]);
			pos += 6;
			
			pos += (400 * num_rule);
		}
		pos += 5544;
	}

	pos += 45;
	rec_wk = m_pt_header[pos];
	if(rec_wk < 1 && rec_wk > 8){
		ViewerID = 0;
	}
	rec_wk = Players[rec_wk].number_key;
	if(rec_wk < 1 && rec_wk > 8){
		ViewerID = 0;
	}
	else{
		ViewerID = rec_wk;
	}

	pos += 2;
	PlayerNum = m_pt_header[pos] - 1;
	pos += 63;
	map_size.cx = *((int*)&m_pt_header[pos]);
	pos += 4;
	map_size.cy = *((int*)&m_pt_header[pos]);
	pos += 4;
	num_data = *((int*)&m_pt_header[pos]);
	pos += 4;
	
	for(i = 0; i < num_data; i++){
		pos += 1275;
		pos += (map_size.cx * map_size.cy);
		num_float = *((int*)&m_pt_header[pos]);
		if(num_float != 0x29){
			error = 1;
			break;
		}
		pos += (4 * num_float);
		pos += 4;
		pos += 4;
	}
	pos += 2;

	
	memset(map_data, 0x41, 57600);
	if(error == 0){
		
		for(i = 0; i < map_size.cx * map_size.cy; i++){
			map_data[i] = m_pt_header[pos + i*2];
		}
		map_load_flg = 1;
	}
	pos += map_size.cx * map_size.cy * 2;
	pos += 128;
	pos += map_size.cx * map_size.cy;
	pos += 15;

	
	pos += PlayerNum;
	
	pos += 36;
	pos += 5;
	
	pos += 7;

	
	pos += 821;

	
	//FIXME, pubb, 07-10-31, this code segment should be considered specially for '2 v 1' condition
	//it actually fills the real players' data. if 2 player control 1 real player, the latter one will have no data found in this code segment
	/*
	j = 1;
	
	for(; pos < trigger_pos; pos++){

		player_name_len = (int)strlen(Players[j].name_key);
		
		ret = memcmp(&m_pt_header[pos], Players[j].name_key, player_name_len);
		
		if(ret == 0){
			str_len = *((short*)&m_pt_header[pos-2]) - 1;

			if(str_len == player_name_len){
				pos += (player_name_len + 1);
				pos += 807;
				length = *((short*)&m_pt_header[pos]);
				pos += 4;
				pos += length * 8;
				pos += 5;
				Players[j].Civ = m_pt_header[pos];
				pos += 1;
				pos += 3;
				//'color + 1', 0 for blue
				Players[j].Color = m_pt_header[pos] + 1;
				j++;
				
				if(j > PlayerNum){
					break;
				}
			}
		}
	}
	*/

	//15-09-18, pubb, check the return value (bool)
	if(!SetPlayersCivColor(pos, trigger_pos)) return false;

	pos = trigger_pos;

	
	pos += 1;
	
	trigger_num = *((int*)&m_pt_header[pos]);
	//if there're 1 or more trigger, that'll be a scenario map.
	if(trigger_num > 0){
		Map.Name = _T("scenario");
	}

	pos += 4;

	
	for(i = 0; i < trigger_num; i++)
	{
		pos += 18;
		length = *((int*)&m_pt_header[pos]); 
		pos += 4;
		pos += length;
		length = *((int*)&m_pt_header[pos]); 
		pos += 4;
		pos += length;
		effect_num = *((int*)&m_pt_header[pos]);
		pos += 4;
		
		for(j = 0; j < effect_num; j++){
			pos += 24;
			selected_num = *((int*)&m_pt_header[pos]);
			if(selected_num == -1){
				selected_num = 0;
			}
			pos += 4;
			pos += 72;
			length = *((int*)&m_pt_header[pos]); 
			pos += 4;
			pos += length;
			length = *((int*)&m_pt_header[pos]); 
			pos += 4;
			pos += length;
			pos += (selected_num * 4);
		}
		pos += (effect_num * 4); 
		condition_num = *((int*)&m_pt_header[pos]);
		pos += 4;
		
		for(j = 0; j < condition_num; j++){
			pos += 72;
		}
		pos += (condition_num * 4); 
	}
	pos += (trigger_num * 4); 

	if(!SetPlayersTeam(pos)) return false;

	CopyCooperatingPlayerInfo();

	pos += 8;
	pos += 19;

	
	chat_num = *((int*)&m_pt_header[pos]);
	pos += 4;
	
	for(j = pos; j < (long)m_header_len; j++){
		if(m_pt_header[j] == '@'   && m_pt_header[j+1] == '#' &&
		   m_pt_header[j+2] >= '1' && m_pt_header[j+2] <= '8' ){
			chat_len = *(int*)(&m_pt_header[j-4]);
			
			memset(chat_msg, 0, 256);
			memcpy(chat_msg, &m_pt_header[j], chat_len);
//			str_edit += CString(chat_msg) + _T("\r\n");

			/* pubb, 07-08-02, copy chAocTMsg. it's stored in format like "pubb:1" */
			CChatInfo * chat = new CChatInfo(chat_msg, 0);
			ChatB4Game.Add(chat);
		}
	}
	//pubb, 07-08-02, no need now */
	//AnalyzeChat(str_edit, ChatB4Game);

	/* body decoding
	 * containing a serie of operations with meaningful type of 1, 2 or 4.
	 * type 1 for command
	 * type 2 for sync
	 * type 4 for game start and chat in game
	 */
	 //	str_edit = _T("");

	pos = 0;
	time_cnt = 0;

	for(; pos < (long)m_body_len - 3;){
		//pubb, 07-11-07, maybe no need now because i've fixed a bug for decoding. but i still left it here for a time.
		//XXX, pubb, 07-10-26, invalid 'pos' value returned after a time of loop for a certain rec
		if(pos < 0)
			break;

		type = *((int*)&m_pt_body[pos]);
		pos += 4;

		switch(type){
		case 4: 
			command = *((int*)&m_pt_body[pos]);
			pos += 4;
			if(command == 500){		//game start with unknown 20 bytes
				pos += 20;
			}
			else if(command == -1){		//chat in game	
				chat_len = *((int*)&m_pt_body[pos]);
				pos += 4;

				/* pubb, 07-08-02, commented because we have our own method dealing with Upgrade Time msg */
				/*
				for(i=0; i<9; i++){
					hour   = Players[i].FeudTime.GetHour();
					minute = ((int)(Players[i].FeudTime/1000/60))%60;
					second = ((int)(Players[i].FeudTime/1000))%60;

					if(Players[i].FeudTime != 0 &&
						Players[i].FeudTime < time_cnt &&
						age_flag[i] < 1){
//						str_advance.Format((LPCTSTR)("@#1(%02d:%02d:%02d) " + m_feudal_msg + "\r\n" ),
//							            hour, minute, second, info->player_name[i]);
//						str_edit = str_edit + str_advance;
//						age_flag[i] = 1;
					}
				}
				for(i=0; i<9; i++){
					hour   = (int)(Players[i].CstlTime/1000/3600);
					minute = ((int)(Players[i].CstlTime/1000/60))%60;
					second = ((int)(Players[i].CstlTime/1000))%60;

					if(Players[i].CstlTime != 0 &&
						Players[i].CstlTime < time_cnt &&
						age_flag[i] < 2){
//						str_advance.Format((LPCTSTR)("@#1(%02d:%02d:%02d) " + m_castle_msg + "\r\n" ),
//							            hour, minute, second, info->player_name[i]);
//						str_edit = str_edit + str_advance;
//						age_flag[i] = 2;
					}
				}
				for(i=0; i<9; i++){
					hour   = (int)(Players[i].ImplTime/1000/3600);
					minute = ((int)(Players[i].ImplTime/1000/60))%60;
					second = ((int)(Players[i].ImplTime/1000))%60;

					if(Players[i].ImplTime != 0 &&
						Players[i].ImplTime < time_cnt &&
						age_flag[i] < 3){
//						str_advance.Format((LPCTSTR)("@#1(%02d:%02d:%02d) " + m_imperial_msg + "\r\n" ),
//							            hour, minute, second, info->player_name[i]);
//						str_edit = str_edit + str_advance;
//						age_flag[i] = 3;
					}

				}
				*/

				memset(chat_msg, 0, 256);
				memcpy(chat_msg, &m_pt_body[pos], chat_len);
				
				//07-10-20, pubb, store ChatInGame
				if(CString(chat_msg).Find(_T(":")) >= 3)	//if it's a game tip msg or wrong format, then skip
				{
					CChatInfo * chat = new CChatInfo(chat_msg, Timecnt2CTimeSpan(time_cnt));
					ChatInGame.Add(chat);
				}

				/*
				hour   = (int)(time_cnt/1000/3600);
				minute = ((int)(time_cnt/1000/60))%60;
				second = ((int)(time_cnt/1000))%60;
				str_work.Format(_T("(%02d:%02d:%02d) %s"), hour, minute, second, chat_msg);
				str_chat.Add(str_work);

				str_edit = str_edit + CString(chat_msg) + _T("\r\n");
				*/
				pos += chat_len;
			}
			else{ 
				pos += 0;
			}
			break;

		case 2: 
			time_cnt += *((int*)&m_pt_body[pos]);

			pos += 4;
			command = *((int*)&m_pt_body[pos]);
			pos += 4;
			if(command == 0){
				pos += 28;
			}
			else{
				pos += 0;
			}
			pos += 12;	//for player's view position x (4 bytes), position y (4 bytes) and player_index (4 bytes)
			break;

		case 1: 
			command_len = *((int*)&m_pt_body[pos]);
			pos += 4;
			command = *((short*)&m_pt_body[pos]);

			switch(command){
			case 0x010B: 
			case 0x020B:
			case 0x030B:
			case 0x040B:
			case 0x050B:
			case 0x060B:
			case 0x070B:
			case 0x080B:	//player_index << 8 | resign command (0x0B)
				pos += 2;
				player_number = *((unsigned char*)&m_pt_body[pos]);		//maybe different to player_index in cooperation mode
				pos += 2;
				
				/* pubb, 07-08-02, commented because we have our own method dealing with resign msg */
				/*
				hour   = (int)(time_cnt/1000/3600);
				minute = ((int)(time_cnt/1000/60))%60;
				second = ((int)(time_cnt/1000))%60;
				str_work.Format(_T("(%02d:%02d:%02d) ") + m_resign_msg,
					            hour, minute, second, player_name[player_number]);
				str_edit = str_edit + _T("@#1") + str_work + _T("\r\n");
				*/

				/* pubb, 07-08-02
				 * if resigntime is not zero, the the player had resigned at that time
				 */
				Players[player_number].ResignTime = Timecnt2CTimeSpan(time_cnt);

				pos += (command_len - 4);
				break;
			case 0x65: 
				pos += 8;
				player_id = *((short*)&m_pt_body[pos]);
				pos += 2;
				research_id = *((short*)&m_pt_body[pos]);
				pos += 2;
				
				/* Upgrade time, for Persians, it's shorter */
				if(research_id == 101){
					Players[player_id].FeudTime = Timecnt2CTimeSpan(time_cnt + 130 * 1000);
				}
				
				if(research_id == 102){
					if(Players[player_id].Civ == 0x08){
						Players[player_id].CstlTime = Timecnt2CTimeSpan(time_cnt + 144 * 1000);
					}
					else{
						Players[player_id].CstlTime = Timecnt2CTimeSpan(time_cnt + 160 * 1000);
					}
				}
				
				if(research_id == 103){
					if(Players[player_id].Civ == 0x08){
						Players[player_id].ImplTime = Timecnt2CTimeSpan(time_cnt + 162 * 1000);
					}
					else{
						Players[player_id].ImplTime = Timecnt2CTimeSpan(time_cnt + 190 * 1000);
					}
				}

				pos += (command_len - 12);
				break;

			case 0x77: 
				pos += 4;
				object_id = *((int*)&m_pt_body[pos]);
				pos += 4;
				unit_type_id = *((short*)&m_pt_body[pos]);
				pos += 2;
				unit_num = *((short*)&m_pt_body[pos]);

				pos += (command_len - 10);
				break;

			case 0x166: 
				pos += 2;
				player_id = *((short*)&m_pt_body[pos]);
				pos += 10;
				unit_type_id = *((short*)&m_pt_body[pos]);
				pos += 2;
				pos += (command_len - 14);
				break;

			default:
				pos += command_len;
				break;
			}
			
			pos += 4;
			break;

		default:
			pos += 0;
			break;

		}

	}
	//pubb, 07-08-02, convert to CTimeSpan
	PlayTime = Timecnt2CTimeSpan(time_cnt);
	
	//pubb, 07-10-24, adjust RecordTime when it's from file modified time
	//XXX, assuming that real time is 3/4 of game time
	if(bSpecialRecordTime)
		RecordTime -= CTimeSpan(PlayTime.GetTotalSeconds() * 3 / 4);

	//pubb, 07-08-02, no need now
	//AnalyzeChat(str_edit, ChatInGame);

	/* 07-08-02, pubb, commented them out since it seems no use of them */
	/*
	switch(PlayerNum){ 
	case 2:
		team_num = _T("1vs1");
		break;
	case 3:
		if(player_team[1] == 0 && player_team[2] == 0 && player_team[3] == 0 ){
			team_num = _T("FFA");
		}
		break;
	case 4:
		team_num = _T("2vs2");
		break;
	case 5:
		if(player_team[1] == 0 && player_team[2] == 0 &&
		   player_team[3] == 0 && player_team[4] == 0 &&
		   player_team[5] == 0 ){
			team_num = _T("FFA");
		}
		break;
	case 6:
		if(player_team[1] == 0 && player_team[2] == 0 &&
		   player_team[3] == 0 && player_team[4] == 0 &&
		   player_team[5] == 0 && player_team[6] == 0 ){
			team_num = _T("FFA");
		}
		else{
			team_num = _T("3vs3");
		}
		break;
	case 7:
		if(player_team[1] == 0 && player_team[2] == 0 &&
		   player_team[3] == 0 && player_team[4] == 0 &&
		   player_team[5] == 0 && player_team[6] == 0 &&
		   player_team[7] == 0 ){
			team_num = _T("FFA");
		}
		break;
	case 8:
		if(player_team[1] == 0 && player_team[2] == 0 &&
		   player_team[3] == 0 && player_team[4] == 0 &&
		   player_team[5] == 0 && player_team[6] == 0 &&
		   player_team[7] == 0 && player_team[8] == 0 ){
			team_num = _T("FFA");
		}
		else{
			team_num = _T("4vs4");
		}
		break;
	}
	if(team_num == _T("")){
		team_num = _T("-");
	}
	*/

	/* pubb, 07-07-31, FillWinner() by default */
	FillWinner();
	return true;
}

/* pubb, 07-08-02, no need now */
/*
CString CRecgame::AnalyzeChat(CString chat)
{
	int		i;
	long	start_pos;
	long	end_pos;
	CString analyzed_chat;
	CString	str_line;

	start_pos = 0;

	while(start_pos < chat.GetLength()){
		end_pos = -1;
		for(i = start_pos; i < chat.GetLength(); i++){
			if(chat[i] == '\r'){
				end_pos = i;
				break;
			}
		}

		if(end_pos == -1){
			end_pos = chat.GetLength() - 1;
		}
		str_line = chat.Mid(start_pos, end_pos - start_pos);


		
		if(!(str_line[0] == '@' && str_line[1] == '#' &&
		     str_line[2] >= '0' && str_line[2] <= '8')){
			start_pos = end_pos + 2;
			continue;
		}
		else{
			
			str_line = str_line.Right(str_line.GetLength() - 3);
		}
		
		if(str_line.Left(2) == _T("--") && str_line.Right(2) == _T("--")){

		}
		
		else{
			
			analyzed_chat = analyzed_chat + str_line + _T("\r\n");
		}
		start_pos = end_pos + 2;

	}
	return analyzed_chat;
}
*/

/* pubb, 07-07-25 */

void	CRecgame::FillWinner(void)
{
	int winteam = GetWinnerTeam();

	if(winteam == 0)
	{
		SetResignFromChat();
		if((winteam = GetWinnerTeam()) == 0)
			return;
	}

	for(int i = 1; i <= 8; i++)
	{
		if(Players[i].Team == winteam)
			player_resign[i] = false;
		else
			player_resign[i] = true;
	}
}

int		CRecgame::GetWinnerTeam(void)
{
	//pubb, 14-12-13, set manual winner team according to DB
	if(ManualWinnerTeam >= 0)
		return ManualWinnerTeam;

	int i;
	int	team_lose[9];	//team_lose[0] is 'no group'. most of time, it's two teams occupying slots no.1 and no. 2

	if(PlayTime < TIME_4_INCOMPLETE)
		return 0;

	memset(team_lose, 0, sizeof(team_lose));

	for(i = 1; i <= 8; i++)
	{
		if(Players[i].ResignTime != CTimeSpan(0))
			team_lose[Players[i].Team]++;
	}

	//pubb, 07-10-31, non-zero Team be set already
	/*
	//pubb, 07-10-24, to deal with 'no group' situation
	//move 'no group' team to 'another' team
	//XXX, not a good way
	if(team_lose[0] > 0)
	{
		if(team_lose[1] == 0)
			team_lose[1] = team_lose[0];
		else
			team_lose[2] = team_lose[0];
	}
	*/

	int max = 0, max_index = 0;
	for(i = 1; i <= 8; i++)
		if(max < team_lose[i])
		{
			max = team_lose[i];
			max_index = i;
		}

	/* XXX
	 * no resign msg, then take viewer as a loser and the other team is winner team.
	 * it's not a good way, because 'out of sync' game will be in this catelog too.
	 */

	if(max_index == 0)
#if 0		
		max_index = Players[ViewerID].Team;
#else
		return 0;	//assume an incomplete game
#endif

	return max_index == 1 ? 2 : 1;
}

CTimeSpan	CRecgame::Timecnt2CTimeSpan(int time_cnt)
{
	return CTimeSpan(0, time_cnt/1000/3600, (time_cnt/1000/60)%60, (time_cnt/1000)%60);
}

bool	CRecgame::IsWinner(int player_id)
{
	return(!player_resign[player_id]);
}

void	CRecgame::SetResignFromChat(void)
{
	for(int i = 0; i < ChatInGame.GetCount(); i++)
	{
		CString msg = ChatInGame[i]->Content;
		if(msg.Find(_T("resign")) >= 0 || msg.Find(_T("RESIGN")) >= 0 || msg.Find(_T("gg")) >= 0 || msg.Find(_T("gg")) >= 0)
		{
			//07-10-21, pubb, store to playeringame directly, ignore Chat information when load
			//team_lose[Players[ChatInGame[i]->Player_Num].Team]++;
			Players[ChatInGame[i]->Player_Num].ResignTime = PlayTime;
		}
	}
}

bool CRecgame::LoadChatInfo(class IPersistentInterface * engine)
{
	return engine->LoadChatInfo(*this);
}

bool CRecgame::IsPlayerInGame(CString name)
{

	INT_PTR index = theApp.Players.GetFirstSamePlayer(name);
	if(index < 0)
		return false;	//no name in player database

	for(int i = 0; i <= MAX_PLAYERS_INGAME; i++)
		if(!Players[i].Name.IsEmpty() && theApp.Players.GetFirstSamePlayer(Players[i].Name) == index)	//found
			return true;
	return false;
}
