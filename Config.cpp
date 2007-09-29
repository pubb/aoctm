#include "stdafx.h"
#include "config.h"

CConfig::CConfig()
: Dirty(false)
{
}

CConfig::~CConfig()
{
	for(int i = 0; i < Charge.GetCount(); i++)
	{
		delete Charge.GetAt(i);
	}
}

bool CConfig::Load(IPersistentInterface * engine)
{
	if(!engine)
		return false;

	return engine->LoadCharge(&Charge, 0);
}

bool CConfig::Save(IPersistentInterface * engine)
{
	if(!engine || !Dirty)
		return false;

	engine->DeleteCharge(0);
	return engine->SaveCharge(&Charge, 0);
}

void CConfig::SetDirty(bool dirty)
{
	Dirty = dirty;
}

int CConfig::GetAllCostFee(void)
{
	int count = 0;
	for(int i = 0; i < Charge.GetCount(); i++)
	{
		count += Charge[i]->Money;
	}
	return count;
}