#ifndef _SDKDESKBANDDEF_H_  
#define _SDKDESKBANDDEF_H_  
#include "Libs.h"

#ifdef _DEFINE_GLOBAL_VARIABLES  
  // A handle to the DLL module. The value is the base address of the Dll.  
  HINSTANCE g_hDllInst = NULL;
  // The reference count of the Dll.  
  LONG      g_lDllRefCount = 1;
#else  
  // A handle to the DLL module. The value is the base address of the Dll.  
  extern HINSTANCE g_hDllInst;
  // The reference count of the Dll.  
  extern LONG      g_lDllRefCount;
#endif // _DEFINE_GLOBAL_VARIABLES  


// {DAF82547-D986-4f30-AF80-24CC3577D6EB}  
const CLSID CLSID_TouchBand = {
	0xdaf82547, 0xd986, 0x4f30,
	{ 0xaf, 0x80, 0x24, 0xcc, 0x35, 0x77, 0xd6, 0xeb } };

#endif