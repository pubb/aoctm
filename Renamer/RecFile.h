
#ifndef __RecFile_h__
#define __RecFile_h__

#include "stdafx.h"

class RecFile
{
public:
	RecFile(CString filename);
	virtual ~RecFile();

	//pubb, 07-08-10, change to bool retval
	//void Parse(CStringArray& prefix);
	bool Parse(CStringArray& prefix);

	int ToMonth(CString& s);

	CString m_Name;
	int d1;
	int d2;
	int d3;
	int h;
	int m;
	int s;

	//一个录像文件由名字字符串和时间组成
};

#endif