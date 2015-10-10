
#ifndef __Renamer_h__
#define __Renamer_h__

//07-08-10, pubb, a lot of changes
class Renamer
{
public:
	//Renamer(CString path);
	Renamer();
	virtual ~Renamer();

	CStringArray m_Prefixes;
	//CArray<RecFile*> m_Files;

	//CString Rename();
	CString	GetNormalName(CString file);

	//Pattern×Ö·û´®ËµÃ÷£º
	void SetPattern(CString pattern);
	//void Parse();
	//07-10-11, pubb, to be a static function for better usage
	//14-06-29, pubb, to merge for all format, and rename it to a more easy-understood function name.
	static CTime Parse2Playtime(CString file);

	//07-10-11, pubb, to be a static function for better usage
	static int ToMonth(CString& s);
};


#endif