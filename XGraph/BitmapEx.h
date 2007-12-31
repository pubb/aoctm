// BitmapEx.h: Schnittstelle für die Klasse CBitmapEx.
//
//////////////////////////////////////////////////////////////////////
#include "fredheader.h"

#if !defined(AFX_BITMAPEX_H__10F9E5D7_F111_430A_BD87_4EC33416F455__INCLUDED_)
#define AFX_BITMAPEX_H__10F9E5D7_F111_430A_BD87_4EC33416F455__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef NoDllForXGraph//fred
class CBitmapEx : public CBitmap
#else
#ifdef _AFXDLL
class __declspec(dllexport)  CBitmapEx : public CBitmap
#else
class __declspec(dllimport)  CBitmapEx : public CBitmap
#endif
#endif //fred 
{
	DECLARE_SERIAL( CBitmapEx )

public:

	CBitmapEx() {};
	virtual ~CBitmapEx() {};

	virtual void Serialize( CArchive& archive );

};

#endif // !defined(AFX_BITMAPEX_H__10F9E5D7_F111_430A_BD87_4EC33416F455__INCLUDED_)
