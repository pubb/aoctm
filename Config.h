#pragma once

#include "player.h"

class CConfig
{
public:
	CConfig();
	~CConfig();

	bool Load(IPersistentInterface * engine);
	bool Save(IPersistentInterface * engine);
	int GetAllCostFee(void);

	void SetDirty(bool dirty);

	CArray<CPaidFee *, CPaidFee *> Charge;

private:
	bool Dirty;
};