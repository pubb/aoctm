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

CChatInfo::CChatInfo(unsigned char * chat)
{
	//format : "pubb:1"
}

CChatInfo::CChatInfo(void)
{

}

//huangjie, 07-08-03, copy constructor
CChatInfo::CChatInfo(CChatInfo& rh)
{
	Name = rh.Name;
	Time = rh.Time;
	Target = rh.Target;
	Content = rh.Content;
}

//huangjie, 07-08-03, operator =
CChatInfo&
CChatInfo::operator = (CChatInfo& rh)
{
	Name = rh.Name;
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
	for(int i = 0; i < 256; i++)
		name_key[i] = 0;
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
	for(int i = 0; i < 256; i++)
		name_key[i] = player.name_key[i];
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
	for(int i = 0; i < 256; i++)
		name_key[i] = rh.name_key[i];
	number_key = number_key;

	return *this;
}

CRecgame::CRecgame()
: ID(0), Loaded(false)
{
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

	getGameData();

	//Analyze the filename and retrieve corrected Game Time from it.
	int index = file.ReverseFind('\\');
	Renamer ren(file.Mid(0, index + 1));	//that's the PATH
	file = file.Mid(index + 1);	//only FILENAME
	
	FileName = file;
	RecordTime = ren.Parse(file);

	Loaded = true;

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

void CRecgame::getGameData(void)
{
	TCHAR maptype_jp[15] = _T("ƒ}ƒbƒv‚ÌŽí—Þ");	
	TCHAR maptype_en[15] = _T("Map Type");		
	TCHAR maptype_sp[15] = _T("Tipo de mapa");	
	TCHAR maptype_de[15] = _T("Kartentyp");		
	TCHAR maptype_cn[15] = _T("µØÍ¼ÀàÐÍ");		
	TCHAR maptype_c2[15] = _T("¦a¹ÏÃþ§O");		
	TCHAR maptype_kr[15] = _T("Áöµµ Á¾·\xf9");	
	TCHAR gaia[15]       = _T("GAIA");

	long i,j;
	int ret;
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
	short str_len;
	short player_id;
	short research_id;
	short unit_type_id;
	int object_id;
	short unit_num;
	int hour;
	int minute;
	int second;
	CStringArray  str_chat;

	unsigned char map_buff[100];
	unsigned char age_flag[9] ={0,0,0,0,0,0,0,0,0};
	int chat_len;
	int player_name_len;
	unsigned char player_number;
	unsigned char chat_msg[256];
	unsigned char t_player_name[256];
	CString str_edit;
	CString t_map_name;
	CString alt_map_name;
	CString str_work;
	CString str_advance;

	//pubb, 07-08-02, map_id not public now.
	int map_id = -1;

	ViewerID = 0;

	map_load_flg = 0;
	for(int j = 0; j <= 8 ; j++){
		data_ref[j] = 0;
		player_resign[j] = false;
	}

	int maptype_len_jp  = (int)_tcslen(maptype_jp);
	int maptype_len_en  = (int)_tcslen(maptype_en);
	int maptype_len_sp  = (int)_tcslen(maptype_sp);
	int maptype_len_de  = (int)_tcslen(maptype_de);
	int maptype_len_cn  = (int)_tcslen(maptype_cn);
	int maptype_len_c2  = (int)_tcslen(maptype_c2);
	int maptype_len_kr  = (int)_tcslen(maptype_kr);

	
	map_size.cx = 0;
	map_size.cy = 0;

	if(m_header_len == 0){
		return ;
	}

	
	for(i = m_header_len; i > 18000; i--){
		
		if(m_pt_header[i-7] == 0x9A && m_pt_header[i-6] == 0x99 &&
		   m_pt_header[i-5] == 0x99 && m_pt_header[i-4] == 0x99 &&
		   m_pt_header[i-3] == 0x99 && m_pt_header[i-2] == 0x99 &&
		   m_pt_header[i-1] == 0xF9 && m_pt_header[i]   == 0x3F){
			if(trigger_pos == 0){
				trigger_pos = i + 1;
			}
		}
		
		if(m_pt_header[i-3] == 0x9D && m_pt_header[i-2] == 0xFF &&
		   m_pt_header[i-1] == 0xFF && m_pt_header[i]   == 0xFF){
			if(trigger_pos != 0){
				player_data_pos = i - 3;
				break;
			}
		}
	}
	
	pos = player_data_pos;
	
	pos += 12;
	
	map_id = *((int*)&m_pt_header[pos]);
	pos += 4;
	pos += 8;
	
	for(i = 0; i < 9; i++){
		
		data_ref[i] = *((int*)&m_pt_header[pos]);
		
		pos += 8;
		
		player_name_len = *(int*)(&m_pt_header[pos]);
		pos += 4;
		
		if(data_ref[i] != 0){
			
			memset(t_player_name, 0, 256);
			memcpy(t_player_name, &m_pt_header[pos], player_name_len);
			Players[i].Name = t_player_name;

			if(strlen(Players[data_ref[i]].name_key) == 0){
				
				memcpy(Players[data_ref[i]].name_key, t_player_name, player_name_len);
				
				Players[data_ref[i]].number_key = i;
			}
		}
		
		Players[i].Color = 0;
		Players[i].Civ = 0;

		pos += player_name_len;
	}

	
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
				Players[j].Color = m_pt_header[pos];
				j++;
				
				if(j > PlayerNum){
					break;
				}
			}
		}
	}

	
	for(i = m_header_len - 18000; i > 100000; i--){

		if((m_pt_header[i-1] == ':' && m_pt_header[i] == ' ') || (m_pt_header[i-1] == 0xa1 && m_pt_header[i] == 'G')){ 

			
			ret = memcmp(&m_pt_header[i - 1 - maptype_len_jp], maptype_jp, maptype_len_jp);
			
			if(ret == 0){
				i = i + 1;
				memset(map_buff,'\0',100);
				for(j = 0; j < 100; j++){
					if(m_pt_header[i+j] == 0x0A){
						break;
					}
					map_buff[j] = m_pt_header[i+j];
					t_map_name = map_buff;
				}
				break;
			}
			ret = memcmp(&m_pt_header[i - 1 - maptype_len_en], maptype_en, maptype_len_en);
			
			if(ret == 0){
				i = i + 1;
				memset(map_buff,'\0',100);
				for(j = 0; j < 100; j++){
					if(m_pt_header[i+j] == 0x0A){
						break;
					}
					map_buff[j] = m_pt_header[i+j];
					t_map_name = map_buff;
				}
				break;
			}
			ret = memcmp(&m_pt_header[i - 1 - maptype_len_sp], maptype_sp, maptype_len_sp);
			
			if(ret == 0){
				i = i + 1;
				memset(map_buff,'\0',100);
				for(j = 0; j < 100; j++){
					if(m_pt_header[i+j] == 0x0A){
						break;
					}
					map_buff[j] = m_pt_header[i+j];
					t_map_name = map_buff;
				}
				break;
			}
			ret = memcmp(&m_pt_header[i - 1 - maptype_len_de], maptype_de, maptype_len_de);
			
			if(ret == 0){
				i = i + 1;
				memset(map_buff,'\0',100);
				for(j = 0; j < 100; j++){
					if(m_pt_header[i+j] == 0x0A){
						break;
					}
					map_buff[j] = m_pt_header[i+j];
					t_map_name = map_buff;
				}
				break;
			}
			ret = memcmp(&m_pt_header[i - 1 - maptype_len_cn], maptype_cn, maptype_len_cn);
			
			if(ret == 0){
				i = i + 1;
				memset(map_buff,'\0',100);
				for(j = 0; j < 100; j++){
					if(m_pt_header[i+j] == 0x0A){
						break;
					}
					map_buff[j] = m_pt_header[i+j];
					t_map_name = map_buff;
				}
				break;
			}
			ret = memcmp(&m_pt_header[i - 1 - maptype_len_c2], maptype_c2, maptype_len_c2);
			
			if(ret == 0){
				i = i + 1;
				memset(map_buff,'\0',100);
				for(j = 0; j < 100; j++){
					if(m_pt_header[i+j] == 0x0A){
						break;
					}
					map_buff[j] = m_pt_header[i+j];
					t_map_name = map_buff;
				}
				break;
			}
			ret = memcmp(&m_pt_header[i - 1 - maptype_len_kr], maptype_kr, maptype_len_kr);
			
			if(ret == 0){
				i = i + 1;
				memset(map_buff,'\0',100);
				for(j = 0; j < 100; j++){
					if(m_pt_header[i+j] == 0x0A){
						break;
					}
					map_buff[j] = m_pt_header[i+j];
					t_map_name = map_buff;
				}
				break;
			}
		}
		else if(m_pt_header[i] == 'A'){
			
			if(m_pt_header[i-3] == 'G' && m_pt_header[i-2] == 'A' && m_pt_header[i-1] == 'I'){
				t_map_name = _T("");
				break;		
			}
		}
	}

	Map.Name = theApp.Recgames.GetMapName(map_id);
	if(Map.Name.IsEmpty())
		Map.Name = t_map_name;
	
	pos = trigger_pos;

	
	pos += 1;
	
	trigger_num = *((int*)&m_pt_header[pos]);
	
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


	/* pubb, 07-07-26, player_team as integer */
	for(j = 0; j < 8; j++)
	{
		//pubb, 07-08-04, set Players.Team
//		player_team[j+1] = m_pt_header[pos+j] -1;
		Players[j+1].Team = m_pt_header[pos+j] -1;
	/*
		if(m_pt_header[pos+j] - 1 == 0){
			player_team[j+1] = _T("-");
		}
		else{
			player_team[j+1].Format(_T("%d"), m_pt_header[pos+j] - 1);
		}
	*/
	}
	
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
			str_edit += CString(chat_msg) + _T("\r\n");

			/* pubb, 07-08-02, copy chAocTMsg. it's stored in format like "pubb:1" */
			CChatInfo * chat = new CChatInfo(chat_msg);
			ChatB4Game.Add(chat);
		}
	}
	//pubb, 07-08-02, no need now */
	//AnalyzeChat(str_edit, ChatB4Game);

	if(trigger_num > 0){
		
		Map.Name = _T("scenario");
	}

	
	str_edit = _T("");

	pos = 0;
	time_cnt = 0;

	for(; pos < (long)m_body_len - 3;){

		type = *((int*)&m_pt_body[pos]);
		pos += 4;

		switch(type){
		case 4: 
			command = *((int*)&m_pt_body[pos]);
			pos += 4;
			if(command == 500){ 
				pos += 20;
			}
			else if(command == -1){ 
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
				
				hour   = (int)(time_cnt/1000/3600);
				minute = ((int)(time_cnt/1000/60))%60;
				second = ((int)(time_cnt/1000))%60;
				str_work.Format(_T("(%02d:%02d:%02d) %s"), hour, minute, second, chat_msg);
				str_chat.Add(str_work);

				str_edit = str_edit + CString(chat_msg) + _T("\r\n");

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
			pos += 12;
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
			case 0x080B:
				pos += 2;
				player_number = *((unsigned char*)&m_pt_body[pos]);
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
	int i;
	int	team_lose[9];	//team_lose[0] is nonsense. most of time, it's two teams occupying slots no.1 and no. 2

	if(PlayTime < TIME_4_INCOMPLETE)
		return 0;

	memset(team_lose, 0, sizeof(team_lose));

	for(i = 1; i <= 8; i++)
	{
		if(Players[i].ResignTime != CTimeSpan(0))
			team_lose[Players[i].Team]++;
	}

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
		max_index = Players[ViewerID].Team;

	return max_index == 1 ? 2 : 1;
}

CTimeSpan	CRecgame::Timecnt2CTimeSpan(int time_cnt)
{
	return CTimeSpan(0, time_cnt/1000/3600, (time_cnt/1000/60)%60, (time_cnt/1000)%60);
}

bool	CRecgame::IsLoser(int player_id)
{
	return(player_resign[player_id]);
}