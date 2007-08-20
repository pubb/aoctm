#pragma once
#include "afx.h"
#include "stdiofileex.h"

class CCsvFile :
	public CStdioFileEx
{
public:
	CCsvFile(LPCTSTR lpszFileName, UINT nOpenFlags);
	bool Read(CStringArray & a);
	void WriteLine(const CStringArray & a);

private:
	bool GetItem(CString &item, const CString & line, WORD &index);
};
