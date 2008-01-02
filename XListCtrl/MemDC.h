// MemDC.h : header file
//

#ifndef MEMDC_H
#define MEMDC_H

//////////////////////////////////////////////////
// CMemDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// History - 10/3/97 Fixed scrolling bug.
//                   Added print support.
//           25 feb 98 - fixed minor assertion bug
//
// This class implements a memory Device Context

class CMemDC : public CDC
{
public:

    // constructor sets up the memory DC
    CMemDC(CDC* pDC, CRect rect) ;

    
    // Destructor copies the contents of the mem DC to the original DC
  //  ~CMemDC()
  //  {
		//AfxMessageBox(_T("destructor\n"));
  //      if (m_bMemDC) 
  //      {    
  //          // Copy the offscreen bitmap onto the screen.
  //          m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
  //                        this, m_rect.left, m_rect.top, SRCCOPY);

		//	CString t("");
		//	t.Format(_T("xxx~CMemDC m_rect(%d,%d,%d,%d)\n"), m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height());
		//	OutputDebugString(t);

  //          //Swap back the original bitmap.
  //          SelectObject(m_pOldBitmap);
  //      } else {
  //          // All we need to do is replace the DC with an illegal value,
  //          // this keeps us from accidently deleting the handles associated with
  //          // the CDC that was passed to the constructor.
  //          m_hDC = m_hAttribDC = NULL;
  //      }
  //  }

	virtual ~CMemDC();

    CMemDC* operator->() {return this;}
        
    // Allow usage as a pointer
    operator CMemDC*() {return this;}

private:
    CBitmap  m_bitmap;      // Offscreen bitmap
    CBitmap* m_pOldBitmap;  // bitmap originally found in CMemDC
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
    BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif //MEMDC_H