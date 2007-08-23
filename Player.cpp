#include "StdAfx.h"
#include "Player.h"

CPaidFee::CPaidFee(void)
: Money(0), PayTime(CTime(0))
{
}

CPlayer::CPlayer(void)
: ID(-1), PlayCount(0), WinCount(0), InitRating(DEF_RATING), Rating(DEF_RATING), Fee(0)
{
	for(int i = 0; i < 19;i++)
	{
		Civs[i] = 0;
	}
}

CPlayer::~CPlayer(void)
{
	for(int i = 0; i < Record_PaidFee.GetCount(); i++)
		delete Record_PaidFee[i];
}

bool	CPlayer::Load(IPersistentInterface * engine)
{
	if(!engine)
		return false;

	return engine->LoadPlayer(*this);
}

bool	CPlayer::Save(IPersistentInterface * engine)
{
	if(!engine || NickNames[0].IsEmpty())
		return false;

	return engine->SavePlayer(*this);
}

int	CPlayer::GetPaidFee(void)
{
	int fee = 0;
	for(int j = 0; j < Record_PaidFee.GetCount(); j++)
		fee += Record_PaidFee[j]->Money;

	return fee;
}