#if !defined(XGRAPH10_H_INCLUDED)
#define XGRAPH10_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "fredheader.h"

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif
#ifdef NoDllForXGraph//fred
//do nothing , not link with lib and dll
#else
#if defined (_DEBUG) 
  #pragma comment(lib,"XGraph10d.lib") 
  #pragma message("Automatically linking with XGraph10d.dll") 
#else 
  #pragma comment(lib,"XGraph10.lib") 
  #pragma message("Automatically linking with XGraph10.dll") 
#endif 
#endif //fred
#include "xgraph.h" 

#endif  