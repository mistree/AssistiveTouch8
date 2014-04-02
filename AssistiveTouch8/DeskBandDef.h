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
  const CLSID CLSID_TouchBand = { 0x46b3d3ef, 0x71a, 0x4b7e, { 0x8a, 0xa2, 0xe5, 0x60, 0x81, 0xd, 0xab, 0x35 } };

  const CLSID CLSID_CAT_TouchBand = { 0x46b3d3ef, 0x71a, 0x4b7e, { 0x8a, 0xa2, 0xe5, 0x60, 0x81, 0xd, 0xab, 0x35 } };

#endif