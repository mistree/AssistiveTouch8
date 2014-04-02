#ifndef _LIBS_H_
#define _LIBS_H_

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <stdio.h>
#include <uxtheme.h>
#include <strsafe.h>

#include "resource.h"

#define SAFE_DELETE(x) if( (x) != NULL ) delete (x); (x) = NULL;
#define SAFE_RELEASE(x) if(x) { x->Release(); x = NULL; } 
#define RECTWIDTH(x)   ((x).right - (x).left)
#define RECTHEIGHT(x)  ((x).bottom - (x).top)
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#define IDM_COMMAND  0
#define DB_CLASS_NAME (L"DeskBandTouchClass")

#endif