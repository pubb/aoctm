// Language.h: CLanguage
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LANGUAGE_H__B28F7785_5F02_4956_9E2D_F8DB2BCFEE24__INCLUDED_)
#define AFX_LANGUAGE_H__B28F7785_5F02_4956_9E2D_F8DB2BCFEE24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLanguage  
{
public:
	CString getString(int str_id);
	//pubb, 07-08-02, change to bool return val
	//int Open(CString path);
	bool Open(CString path);
	CLanguage();
	virtual ~CLanguage();

private:
	HMODULE m_libs[4];//14-02-15, pubb, two language_x1_p1.dll
	int m_dll_count;
};

#endif // !defined(AFX_LANGUAGE_H__B28F7785_5F02_4956_9E2D_F8DB2BCFEE24__INCLUDED_)
