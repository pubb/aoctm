
#include "stdafx.h"
#include "Renamer.h"

//07-10-11, pubb, m_Path is of no use now
//Renamer::Renamer(CString path)
Renamer::Renamer()
{
	m_Prefixes.Add(_T("recorded game"));
	m_Prefixes.Add(_T("已记录游戏"));
	m_Prefixes.Add(_T("游戏录象"));
}

Renamer::~Renamer()
{
	/*
	for( int i = 0;i < m_Files.GetCount();i++ )
	{
		delete m_Files.GetAt(i);
	}
	*/
}

void
Renamer::SetPattern(CString pattern)
{
}

/* pubb, 07-08-10, change the interface and function of it
 * input: rec filename with/without path
 * output: CTime object
 */

/*
void
Renamer::Parse()
{
	CString strWildcard = m_Path + _T("\\");
	strWildcard += _T("*.mgx");
	CFileFind finder;

	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDirectory())
			continue;

		CString str = finder.GetFileName();
		RecFile *file = new RecFile(str);
		file->Parse(m_Prefixes);

		m_Files.Add(file);
	}
}
*/

/* pubb, 07-08-11, Parse(), almost rewrite */
CTime	Renamer::Parse(CString file)
{
	int index;
	//trim path
	index = file.ReverseFind('\\');
	file = file.Mid(index + 1);

	//检查文件扩展名
	int len = file.GetLength();
	if( file.Mid(len - 3, 3).Compare(_T("mgx")) != 0 )
	{
		return CTime(0);
	}

	//spite file extension
	file = file.Mid(0, len - 4);
	file.Trim();

	/* pubb, 07-08-10, don't care for any prefix before digitals */
	index = file.FindOneOf(_T("0123456789"));
	file = file.Mid(index);

	CString token;
	int curPos= 0;

	//得到时间字符串
	token = file.Tokenize(_T("-` "), curPos);
	int result[6];
	int j = 0;

	while( token != "" )
	{
		result[j] = _ttoi((LPCTSTR)token);

		if( result[j] == 0 )
		{
			//转换失败，可能是中文或者英文的月份
			result[j] = ToMonth(token);
		}

		token = file.Tokenize(_T("-` "), curPos);

		j++;
	}

	int year, day;
	if(result[0] > 31)	// //altered NormalName : 'recorded game YYYY-MM-DD hh`mm`ss'
	{
		year = result[0];
		day = result[2];
	}
	else	//orignial name : 'recorded game MM-DD-YYYY
	{
		year = result[2];
		day = result[0];
	}
	//pubb, 07-10-23, don't do that any more.
	/* XXX, SET year to be 2007 in case that the machine time is not correct in many places */
	//year = 2007;

	if(result[1] < 1 || result[1] > 12 || day < 1 || day > 31 || result[3] < 0 || result[3] > 24 || result[4] < 0 || result[4] > 60 || result[5] < 0 || result[5] > 60)
		return CTime(0);

	return CTime(year /* YEAR */, result[1] /* MONTH */, day /* DAY */, result[3] /* HOUR */, result[4] /* MINUTE */, result[5] /* SECOND */);
}

int
Renamer::ToMonth(CString& s)
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