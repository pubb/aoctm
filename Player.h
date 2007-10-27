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

class CPaidFee
{
public:
	CPaidFee(void);
	int	Money;					//本次缴纳金额
	CTime	PayTime;			//本次缴费时间
};

class CPlayer
{
	//pubb, 07-08-23, no use of copy constructor
	/*
	//huangjie, 07-08-03, hide copy constractor and operator =  
	CPlayer(CPlayer&);
	CPlayer& operator = (CPlayer&);
	*/
public:
	CPlayer(void);
	~CPlayer(void);
	void Initialize(void);

	int	ID;						//玩家唯一标识
	//pubb, 07-08-02, change variable name
	//CStringArray	NickName;
	CStringArray	NickNames;	//玩家ID，可多个
	int InitRating;				//玩家初始设定的积分
	int	Rating;					//玩家当前积分
	int	PlayCount;				//所打场次总计
	int	WinCount;				//所赢场次总计
	CTime	UpdateTime;			//最近更新时间
	//pubb, 07-10-26, no Fee property now
	//int	Fee;					//玩家当前剩余会费
	bool	IsComputer;			//是电脑玩家

	//by mep
	//for technic statistic
	CTimeSpan MinFeudTime;
	CTimeSpan TotalFeudTime;
	int FeudCount;
	CTimeSpan MinCstlTime;
	CTimeSpan TotalCstlTime;
	int CstlCount;
	CTimeSpan MinImplTime;
	CTimeSpan TotalImplTime;
	int ImplCount;
	//保存使用的文明，每一项表示该文明被使用一次
	//255表示那个文明？
	int Civs[19];

	//pubb,07-08-02, change to pointer for better construction
	//CArray<CPaidFee, CPaidFee&> PaidFee;	//玩家缴费记录
	CArray<CPaidFee * , CPaidFee *>	Record_PaidFee;	//玩家缴费记录

	int	GetPaidFee(void);
//	int GetPlayCountWithFee(void);

	bool	Load(class IPersistentInterface * engine);
	bool	Save(IPersistentInterface * engine);
};			

#endif

#define	DEF_RATING	1500
#define	DEF_RATING_STR	_T("1500")
#define	MAX_TIMESPAN	CTimeSpan(99999999)

//pubb, 07-10-25, for computer players
#define COMPUTER    (_T("@#-"))