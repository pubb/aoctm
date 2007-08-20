
#ifndef __Renamer_h__
#define __Renamer_h__

//07-08-10, pubb, a lot of changes
class Renamer
{
public:
	//Renamer(CString path);
	Renamer(CString path = _T(""));
	virtual ~Renamer();

	CStringArray m_Prefixes;
	//CArray<RecFile*> m_Files;

	//CString Rename();
	CString	GetNormalName(CString file);

	//Pattern×Ö·û´®ËµÃ÷£º
	void SetPattern(CString pattern);
	//void Parse();
	CTime Parse(CString file);

private:
	int ToMonth(CString& s);
	CString m_Path;

};


#endif