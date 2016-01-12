#pragma once
#include "afx.h"
#include "stdiofileex.h"
#include "xlistctrl\xlistctrl.h"

class CCsvFile :
	public CStdioFileEx
{
public:
	CCsvFile(void);
	BOOL Open(CString name, UINT nOpenFlags = CFile::modeWrite | CFile::modeCreate);
	bool Read(CStringArray & a);
	void WriteLine(const CStringArray & a);
	bool WriteList(CXListCtrl & list);

private:
	bool GetItem(CString &item, const CString & line, WORD &index);
};
