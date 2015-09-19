// Recgame.h: CRecgame 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "db/persistentinterface.h"

const int	TIME_4_INCOMPLETE = (17 * 60);		// as seconds. if game time is less than 17 min, then consider is as an incomplete one.
const int	MAX_PLAYERS_INGAME	= 8;

extern CString civ_name[24];
extern COLORREF player_color[8];

#if 1
class CMapInfo
{	
public:
	//huangjie 07-08-03, default constructor
	CMapInfo(){}
	//huangjie 07-08-03, copy constructor and operator =
	CMapInfo(CMapInfo&);
	CMapInfo& operator = (CMapInfo&);

	CString	Name;									//地图类型名
	int	MapSizeLevel;								//地图大小
	int	HardLevel;									//难度
	int	Victory;									//胜利条件
	bool FullTechTree;								//完全科技树
};	

class CPlayerInGame
{	
public:
	//pubb, 07-08-03, add for assignment of CPlayerInGame
	CPlayerInGame(void);
	CPlayerInGame(CPlayerInGame & player);			//copy construction
	//huangjie, 07-08-03, operator = 
	CPlayerInGame& operator = (CPlayerInGame&);

	CString	Name;									//玩家名字
	int	Team;										//组别
	int	Civ;										//民族ID
	int	Color;										//颜色ID
	int	PositionX;									//玩家位置
	int	PositionY;									//玩家位置

	CTimeSpan	FeudTime;								//升封时间
	CTimeSpan	CstlTime;								//升城时间
	CTimeSpan	ImplTime;								//升帝时间
	CTimeSpan	ResignTime;								//投降时间

	/* pubb, 07-08-02, copy from original, not sure for their meanings */
	char		name_key[256];
	int			number_key;
};					

class CChatInfo
{	
public:
	//huangjie, 07-08-03, copy constructor and operator = 
	CChatInfo(CChatInfo&);
	CChatInfo& operator = (CChatInfo&);

	CChatInfo(unsigned char *chat, CTimeSpan time);
	CChatInfo(void);

	//07-10-20, pubb, change to store ID instead of name
	//CString Name;
	int	Player_Num;									//说话人
	CTimeSpan	Time;									//说话时间
	int	Target;										//说话对象，0-普通；1-全体（‘*’）；2-同组（‘？’）；3-对方（‘!’），暂时没用上
	CString	Content;								//说话内容
};

class CRecgame
{	
	//huangjie, 07-08-03, hide copy constractor and operator =  
	CRecgame(CRecgame&);
	CRecgame& operator = (CRecgame&);
public:
	CRecgame();
	~CRecgame();

	int	ID;											//录像唯一标识
	CString	FileName;								//录像文件名（CString基）
	CTime	RecordTime;								//录像生成时间
	//pubb, 07-08-03, an interval of time should be a CTimeSpan. the same changes to other Time Interval members
	//CTime	PlayTime;
	CTimeSpan	PlayTime;								//录像持续时间
	//pubb, 07-08-02, need a player num				
	int		PlayerNum;								//玩家数

	CArray<CPlayerInGame, CPlayerInGame &>	Players;						//本场录像玩家信息, 第1号是GAIA（自然）
	//pubb, 07-08-02
	//CString	ViewerName;								
	int	ViewerID;									//主视角玩家
	CMapInfo	Map;								//地图信息
	//pubb, 07-08-02, it has to be points newed at time
	//CArray<CChatInfo, CChatInfo&>	ChatB4Game;		//游戏开始前的聊天信息
	//CArray<CChatInfo, CChatInfo&>	ChatInGame;		//游戏中的聊天信息
	CArray<CChatInfo *, CChatInfo *> ChatB4Game;
	CArray<CChatInfo *, CChatInfo *>	ChatInGame;
	//pubb, 14-12-13, winner info in DB, which can be different from GetWinnerTeam() calculated from resign msg. it can be changed manually in such condition as actually no winner game or should-be-ignored game (a game without meaning in ratings).
	int		ManualWinnerTeam;	//0 for no winner, 1/2 for winner team, <0 for not defined

	bool	Loaded;

public:
	bool	Read(CString file);
	bool	Load(class IPersistentInterface * engine);
	bool	Save(class IPersistentInterface * engine);
	bool	LoadChatInfo(class IPersistentInterface * engine);
	/* pubb, 07-07-26 */
	int		GetWinnerTeam(void);
	bool	IsWinner(int player_id);
	bool	IsPlayerInGame(CString name);

private:
	/* pubb, 07-08,02, copy from original recgame structure */
	/*
	CString m_feudal_msg;
	CString m_castle_msg;
	CString m_imperial_msg;
	CString m_resign_msg;
	
	CString m_str_table[100];
	*/

	//pubb, 07-10-24, to specify 'RecordTime from file modified time', that'll be subbed by PlayTime
	bool	bSpecialRecordTime;

	SIZE		map_size;
	int			map_load_flg;
	char		map_data[57600];
	int			player_index[MAX_PLAYERS_INGAME + 1];
	bool		player_resign[MAX_PLAYERS_INGAME + 1];
	unsigned char * m_pt_header;
	unsigned char * m_pt_body;
	unsigned long m_body_len;
	unsigned long m_header_len;

	bool getGameData(void);
	//pubb, 07-08-02, no use
	/*
	void getBody(unsigned char ** pt_body, unsigned int* length);
	void getHeader(unsigned char ** pt_header, unsigned int* length);
	*/

	//pubb, 07-08-11, clear memory of data not stored into DB
	void ClearMem(void);

	int readFile(CString filename);
	
	/* pubb, 07-08-02, no need now. we have our own method to deal with chat msg */
	//CString AnalyzeChat(CString chat);
	
	/* pubb, 07-07-25, add */
	/* pubb, 07-07-31, move to 'private' */
	void	FillWinner(void);
	//07-10-20, pubb, add for more accurate winner msg
	//07-10-21, pubb, return void instead of int
	void	SetResignFromChat(void);
	/* pubb, 07-08-02, construct CTime object */
	CTimeSpan	Timecnt2CTimeSpan(int timecnt);
	
	//15-09-18, pubb, change the following two functions' return value type to 'bool' to handle error.
	bool	SetPlayersName(int pos);
	bool	SetPlayersCivColor(int pos, int end_pos);
	int SetPlayersTeam(int pos);
	void	CopyCooperatingPlayerInfo(void);
	CString	GetMapName(void);
	bool	GetPlayerDataPos(int & trigger_pos, int & player_data_pos, unsigned long tail);
};			

#else

typedef struct player_info_s {
	char		name_key[256];
	int			number_key;
	int			player_civ;
	int			player_color;
	int			feudal_time;
	int			castle_time;
	int			imperial_time;
} player_info;

class CRecgame  
{
public:
	/* pubb, 07-07-26, merged from rec_info */
    CString		file_name;
    CString		file_ext;
    ULONG		file_attr;
    SYSTEMTIME	write_time;
    long		file_size;
	int			icon_idx;
	int			map_id;
    CString		map_name;
	SIZE		map_size;
	int			map_load_flg;
	char		map_data[57600];
	int			play_time;
	int			rec_player;
	int			player_num;
    CString		team_num;
	int			player_index[9];
	player_info player_data[9];
    int			player_team[9];
    CString		player_name[9];

	/* pubb, 07-07-25
	 * for winner/loser information
	 */
	bool		player_resign[9];

	CString		chat_b4_game;
	CString		chat_in_game;
	/* end of rec_info */

	CString m_feudal_msg;
	CString m_castle_msg;
	CString m_imperial_msg;
	CString m_resign_msg;
	CString m_str_table[100];
	void getGameData(void);
	void getBody(unsigned char ** pt_body, unsigned int* length);
	void getHeader(unsigned char ** pt_header, unsigned int* length);
	int readFile(CString filename);
	
	/* pubb, 07-07-24 */
	CString GetAOCRegistry(void);
	/* pubb, 07-07-26 */
	int		GetWinnerTeam(void);

	CRecgame();
	virtual ~CRecgame();

private:
	CString AnalyzeChat(CString chat);
	unsigned char * m_pt_header;
	unsigned char * m_pt_body;
	unsigned long m_body_len;
	unsigned long m_header_len;
	/* pubb, 07-07-25, add */
	/* pubb, 07-07-31, move to 'private' */
	void	FillWinner(void);

};
#endif