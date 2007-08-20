#pragma once

#include "recgame.h"
#include "DB/persistentinterface.h"

#if 0
class CPlayer :
	public CObject
{
public:
	CPlayer(void);
	~CPlayer(void);

	CStringArray	m_Names;
	int	m_PlayCount;
	int m_WinCount;
	int m_Rating;
};

#else

class CPayedFee
{
public:
	int	Money;					//本次缴纳金额
	CTime	PayTime;			//本次缴费时间
};

class CPlayer
{
	//huangjie, 07-08-03, hide copy constractor and operator =  
	CPlayer(CPlayer&);
	CPlayer& operator = (CPlayer&);
public:
	CPlayer(void);
	~CPlayer(void);

	int	ID;						//玩家唯一标识
	//pubb, 07-08-02, change variable name
	//CStringArray	NickName;
	CStringArray	NickNames;	//玩家ID，可多个
	int InitRating;				//玩家初始设定的积分
	int	Rating;					//玩家当前积分
	int	PlayCount;				//所打场次总计
	int	WinCount;				//所赢场次总计
	CTime	UpdateTime;			//最近更新时间
	int	Fee;					//玩家当前剩余会费

	//by mep
	//for technic statistic
	int MinFeud;
	int AvgFeud;
	int MinCstl;
	int AvgCstl;
	int MinImpl;
	int AvgImpl;
	//保存使用的文明，每一项表示该文明被使用一次
	//255表示那个文明？
	int Civs[19];

	//pubb,07-08-02, change to pointer for better construction
	//CArray<CPayedFee, CPayedFee&> PayedFee;	//玩家缴费记录
	CArray<CPayedFee , CPayedFee &>	Record_PayedFee;	//玩家缴费记录

	bool	Load(class IPersistentInterface * engine);
	bool	Save(IPersistentInterface * engine);
};			

#endif

#define	DEF_RATING	1500
#define	DEF_RATING_STR	_T("1500")