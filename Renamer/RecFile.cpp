
#include "stdafx.h"
#include "RecFile.h"



RecFile::RecFile(CString filename)
: m_Name(filename)
{
}

RecFile::~RecFile()
{
}

//pattern主要是年月日的排列，比如mdy或ymd
//pubb, 07-08-10, change to bool retval and no prefixes
//void RecFile::Parse(CStringArray& prefixes)
bool
RecFile::Parse(void)
{
	//检查文件扩展名
	int len = m_Name.GetLength();
	CString ext = m_Name.Mid(len - 3, 3);
	if( ext.Compare(_T("mgx")) != 0 )
	{
		return false;
	}

	//spite file extension
	CString name = m_Name.Mid(0, len - 4);
	name.Trim();

	//首先解析录像名字字符串，可能的情况有：游戏录像/已记录游戏/recorded game
	for(int i = 0;i < prefixes.GetCount();i++)
	{
		if( (name.Find( prefixes.GetAt(i) ) == 0) )
		{
			len = name.GetLength();
			int k = prefixes.GetAt(i).GetLength();
			CString rest = name.Mid(k, len - k);
			rest.Trim();

			CString token;
			int curPos= 0;

			//得到时间字符串
			token = rest.Tokenize(_T("-` "), curPos);
			int result[6];
			int j = 0;

			while( token != "" )
			{
				result[j] = _wtoi((LPCTSTR)token);

				if( result[j] == 0 )
				{
					//转换失败，可能是中文或者英文的月份
					result[j] = ToMonth(token);
				}

				token = rest.Tokenize(_T("-` "), curPos);

				j++;
			}

			d1 = result[0];
			d2 = result[1];
			d3 = result[2];
			
			h = result[3];
			m = result[4];
			s = result[5];
		}
	}
	return true;
}

int
RecFile::ToMonth(CString& s)
{
	if( (s.CompareNoCase(_T("Jan")) == 0) ||
		(s.CompareNoCase(_T("一月")) == 0) )
	{
		return 1;
	}
	else if( (s.CompareNoCase(_T("Feb")) == 0) ||
		(s.CompareNoCase(_T("二月")) == 0) )
	{
		return 2;
	}
	else if( (s.CompareNoCase(_T("Mar")) == 0) ||
		(s.CompareNoCase(_T("三月")) == 0) )
	{
		return 3;
	}
	else if( (s.CompareNoCase(_T("Apr")) == 0) ||
		(s.CompareNoCase(_T("四月")) == 0) )
	{
		return 4;
	}
	else if( (s.CompareNoCase(_T("May")) == 0) ||
		(s.CompareNoCase(_T("五月")) == 0) )
	{
		return 5;
	}
	else if( (s.CompareNoCase(_T("Jun")) == 0) ||
		(s.CompareNoCase(_T("六月")) == 0) )
	{
		return 6;
	}
	else if( (s.CompareNoCase(_T("Jul")) == 0) ||
		(s.CompareNoCase(_T("七月")) == 0) )
	{
		return 7;
	}
	else if( (s.CompareNoCase(_T("Aug")) == 0) ||
		(s.CompareNoCase(_T("八月")) == 0) )
	{
		return 8;
	}
	else if( (s.CompareNoCase(_T("Sep")) == 0) ||
		(s.CompareNoCase(_T("九月")) == 0) )
	{
		return 9;
	}
	else if( (s.CompareNoCase(_T("Oct")) == 0) ||
		(s.CompareNoCase(_T("十月")) == 0) )
	{
		return 10;
	}
	else if( (s.CompareNoCase(_T("Nov")) == 0) ||
		(s.CompareNoCase(_T("十一月")) == 0) )
	{
		return 11;
	}
	else if( (s.CompareNoCase(_T("Dec")) == 0) ||
		(s.CompareNoCase(_T("十二月")) == 0) )
	{
		return 12;
	}

	return 0;
}