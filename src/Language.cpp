// Language.cpp: CLanguage 
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Language.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CLanguage::CLanguage()
{
	m_dll_count = 0;
}

CLanguage::~CLanguage()
{
	int i;

	for (i = 0; i < m_dll_count; i++)
	{
		FreeLibrary(m_libs[i]);
	}
}

bool CLanguage::Open(CString filename)
{
	HMODULE Library = LoadLibrary(filename);
	if (! Library){
		return false;
	}
	m_libs[m_dll_count] = Library;
	m_dll_count++;

	return true;
}

CString CLanguage::getString(int str_id)
{
	const int bufferSize = 1024;
	WCHAR buffer[bufferSize];
	int result;
	int i;
	CString ret = _T("");

	for (i = 0; i < m_dll_count; i++)
	{
		result = LoadStringW(m_libs[i], str_id, buffer, bufferSize/sizeof(WCHAR));
		if(result)
		{
			ret = buffer;
			return ret;
		}
	}
	return ret;
}
