#include "StdAfx.h"
#include ".\csvfile.h"

CCsvFile::CCsvFile(LPCTSTR lpszFileName,UINT nOpenFlags)
: CStdioFileEx(lpszFileName, nOpenFlags | CFile::typeText)
{
}

/* extract CSV items using undocumented 'AfxExtractSubString()' to speed up (maybe) */
bool	CCsvFile::GetItem(CStringW &item, const CStringW &line, WORD &index)
{
	CStringW last_item;
	bool	ret = false;

	last_item.Empty();
	if(!AfxExtractSubString(item,line,index,','))
		return false;

	if(item.Left(1) == _T("\""))
	{
		do
		{
			if(item.Right(1) == _T("\""))
			{
				if(!last_item.IsEmpty())
					item = last_item + item;
				item.Trim(_T("\""));
				ret = true;
				break;
			}
			last_item += item + _T(",");
		}
		while(AfxExtractSubString(item,line,++index,','));
	}
	else
		ret = true;
	return ret;
}

bool CCsvFile::Read(CStringArray & a)
{
	CStringW line,item;

	a.RemoveAll();

	if(!ReadString(line))
		return false;

	for(WORD i=0;!line.IsEmpty();i++)
	{
		if(!GetItem(item,line,i))
			break;
		a.Add(item);
	}
	return true;
}

void CCsvFile::WriteLine(const CStringArray & a)
{
	CStringW	line;

	for(int i = 0; i < a.GetCount(); i++)
	{
		if(a[i].Find(',') >= 0)
			line += _T("\"") + a[i] + _T("\"");
		else
			line += a[i];
		line.Append(_T(","));
	}
	line.TrimRight(',');
	WriteString(line);
	WriteString(_T("\n"));
}