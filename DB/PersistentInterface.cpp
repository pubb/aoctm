/********************************************************
*
*	Author : Huang Jie
*   2007.08.01
*
********************************************************/
#include "stdafx.h"

#include "PersistentInterface.h"

EngineManager* instance = 0;

IPersistentInterface::~IPersistentInterface()
{
}

SelfRegister::SelfRegister(const TCHAR* enginName, PersistentEngineFactory* factory)
{
	if(!instance)
		//XXX, pubb, 07-08-03, see nowhere to delete. added in ~SelfRegister()
		instance = new EngineManager;

	instance -> RegisterEngine(enginName, factory);
}

SelfRegister::~SelfRegister(void)
{
	if(instance)
		delete instance;
	instance = 0;
}

EngineManager::EngineManager()
{
}

void
EngineManager::RegisterEngine(const TCHAR* enginName, PersistentEngineFactory* factory)
{
	m_EngineName.Add(CString(enginName));
	m_EngineFactory.Add(factory);
}

PersistentEngineFactory*
EngineManager::FindEngineFactory(const TCHAR* enginName)
{
	bool find = false;

	int i;
	for(i = 0; i < m_EngineName.GetSize(); i ++)
	{
		if(m_EngineName[i] == enginName)
		{
			find = true;
			break;
		}
	}

	if(find)
		return m_EngineFactory[i];
	else
		return 0;
}


IPersistentInterface*
PersistentEngineFactory::CreatePersistentEngine(const TCHAR* enginName)
{
	PersistentEngineFactory* factory = instance -> FindEngineFactory(enginName);
	
	if(!factory)
		return 0;
	else
		return factory -> Create();
}