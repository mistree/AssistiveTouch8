#ifndef _LIBS_H_
#define _LIBS_H_

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <stdio.h>
#include <Shlwapi.h>
#include <strsafe.h>

#define SAFE_DELETE(x) if( (x) != NULL ) delete (x); (x) = NULL;
#define SAFE_RELEASE(x) if(x) { x->Release(); x = NULL; } 

#endif