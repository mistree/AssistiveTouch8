#include <ole2.h>
#include <comcat.h>
#include <olectl.h>
#include "ClassFactory.h"

HINSTANCE   g_hInst;
UINT        g_DllRefCount;

BOOL RegisterServer(
	HINSTANCE hDlInstance, CLSID clsidCom, IN const WCHAR* pszDefValueText)
{
	WCHAR *pszTempDefValueText =
		(NULL == pszDefValueText) ? L"" : pszDefValueText;
	WCHAR szClsid[MAX_PATH] = { 0 };
	StringFromGUID2(clsidCom, szClsid, MAX_PATH);
	WCHAR szSubKey[MAX_PATH] = { 0 };
	HRESULT hr = StringCchPrintf(szSubKey, MAX_PATH, L"CLSID\\%s", szClsid);
	HKEY hKey = NULL;
	LONG lResult = (SUCCEEDED(hr)) ? ERROR_SUCCESS : ERROR_INVALID_FUNCTION;
	if (ERROR_SUCCESS == lResult)
	{
		lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT,
			szSubKey, 0,
			NULL, REG_OPTION_NON_VOLATILE,
			KEY_WRITE, NULL,
			&hKey, NULL);
		if (ERROR_SUCCESS == lResult)
		{
			// Set default value.
			lResult = RegSetValueEx(hKey, NULL, 0, REG_SZ,
				(LPBYTE)pszDefValueText,
				(DWORD)wcslen(pszTempDefValueText) * sizeof(WCHAR));
			hr = (ERROR_SUCCESS == lResult) ? S_OK : E_FAIL;
		}
		RegCloseKey(hKey);
	}
	if (ERROR_SUCCESS == lResult)
	{
		StringCchPrintf(szSubKey,
			MAX_PATH, L"CLSID\\%s\\InprocServer32", szClsid);
		lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT,
			szSubKey, 0,
			NULL, REG_OPTION_NON_VOLATILE,
			KEY_WRITE, NULL,
			&hKey, NULL);
		if (ERROR_SUCCESS == lResult)
		{
			WCHAR szModuleFile[MAX_PATH] = { 0 };
			GetModuleFileName(hDlInstance, szModuleFile, MAX_PATH);
			lResult = RegSetValueEx(hKey, NULL, 0, REG_SZ,
				(LPBYTE)szModuleFile, (DWORD)wcslen(szModuleFile) * sizeof(WCHAR));
			WCHAR const szModel[] = L"Apartment";
			lResult = RegSetValueEx(hKey, "ThreadingModel", 0,
				REG_SZ, (LPBYTE)szModel, sizeof(szModel));
		}
	}
	return (ERROR_SUCCESS == lResult) ? TRUE : FALSE;
}

BOOL RegisterComCategory(CLSID clsidCom)
{
	ICatRegister *pCatReg = NULL;
	HRESULT hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr,
		NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pCatReg));
	if (SUCCEEDED(hr))
	{
		// 注册Band对象的类型
		CATID catid[] = { CATID_DeskBand };
		hr = pCatReg->RegisterClassImplCategories(clsidCom, 1, catid);
	}
	SAFE_RELEASE(pCatReg);
	return (SUCCEEDED(hr)) ? TRUE : FALSE;
}

BOOL UnregisterServer(CLSID clsidCom)
{
	WCHAR szClsid[MAX_PATH] = { 0 };
	StringFromGUID2(clsidCom, szClsid, MAX_PATH);
	WCHAR szSubKey[MAX_PATH] = { 0 };
	StringCchPrintf(szSubKey, MAX_PATH, L"CLSID\\%s", szClsid);
	LONG lResult = RegDeleteTree(HKEY_CLASSES_ROOT, szSubKey);
	return (ERROR_SUCCESS == lResult) ? TRUE : FALSE;
}

STDAPI DllRegisterServer()
{
	BOOL isSuccess = RegisterServer(
		g_hDllInst, CLSID_SdkDeskBand, L"Sampele Band");
	if (TRUE == isSuccess)
	{
		// 注册类型
		isSuccess = RegisterComCategory(CLSID_LyraDeskBand);
	}

	// 以下代码就是让Toolbars子菜单项中立即出现我们注册的Band对象菜单项
	if (TRUE == isSuccess)
	{
		ITrayDeskBand *pTrayDeskBand = NULL;
		HRESULT hr = CoCreateInstance(CLSID_TrayDeskBand,
			NULL, CLSCTX_ALL, IID_PPV_ARGS(&pTrayDeskBand));
		if (SUCCEEDED(hr))
		{
			pTrayDeskBand->DeskBandRegistrationChanged();
		}
		SAFE_RELEASE(pTrayDeskBand);
	}

	return (TRUE == isSuccess) ? S_OK : E_FAIL;
}

STDAPI DllUnregisterServer()

{
	// Hide the desk band window.
	ShowDeskBand(FALSE);
	// Find handle to the task bar.
	HWND hTaskbarWnd = FindWindow("Shell_TrayWnd", NULL);
	// If task bar receives this message, 
	// it will call CoFreeUnusedLibraries function
	// immediately to free unused libraries.
	PostMessage(hTaskbarWnd, WM_TIMER, 24, 0);
	return (TRUE == UnregisterServer(CLSID_SdkDeskBand)) ?
	S_OK : SELFREG_E_CLASS;
}

extern "C" BOOL WINAPI DllMain(  HINSTANCE hInstance, 
                                 DWORD dwReason, 
                                 LPVOID lpReserved)
{
switch(dwReason)
   {
   case DLL_PROCESS_ATTACH:
      g_hInst = hInstance;
      break;

   case DLL_PROCESS_DETACH:
      break;
   }
   
return TRUE;
}                                 

STDAPI DllCanUnloadNow(void)
{
return (g_DllRefCount ? S_FALSE : S_OK);
}

STDAPI DllGetClassObject(  REFCLSID rclsid, 
                           REFIID riid, 
                           LPVOID *ppReturn)
{
*ppReturn = NULL;

//if we don't support this classid, return the proper error code
if(   !IsEqualCLSID(rclsid, CLSID_TouchBand))
   return CLASS_E_CLASSNOTAVAILABLE;
   
//create a CClassFactory object and check it for validity
CClassFactory *pClassFactory = new CClassFactory();
if(NULL == pClassFactory)
   return E_OUTOFMEMORY;
   
//get the QueryInterface return for our return value
HRESULT hResult = pClassFactory->QueryInterface(riid, ppReturn);

//call Release to decement the ref count - creating the object set it to one 
//and QueryInterface incremented it - since its being used externally (not by 
//us), we only want the ref count to be 1
pClassFactory->Release();

//return the result from QueryInterface
return hResult;
}

typedef struct{
   HKEY  hRootKey;
   LPTSTR szSubKey;//TCHAR szSubKey[MAX_PATH];
   LPTSTR lpszValueName;
   LPTSTR szData;//TCHAR szData[MAX_PATH];
}DOREGSTRUCT, *LPDOREGSTRUCT;

BOOL RegisterServer(CLSID clsid, LPTSTR lpszTitle)
{
int      i;
HKEY     hKey;
LRESULT  lResult;
DWORD    dwDisp;
TCHAR    szSubKey[MAX_PATH];
TCHAR    szCLSID[MAX_PATH];
TCHAR    szModule[MAX_PATH];
LPWSTR   pwsz;

//get the CLSID in string form
StringFromIID(clsid, &pwsz);

if(pwsz)
   {
#ifdef UNICODE
   lstrcpy(szCLSID, pwsz);
#else
   WideCharToMultiByte( CP_ACP,
                        0,
                        pwsz,
                        -1,
                        szCLSID,
                        ARRAYSIZE(szCLSID),
                        NULL,
                        NULL);
#endif

   //free the string
   LPMALLOC pMalloc;
   CoGetMalloc(1, &pMalloc);
   pMalloc->Free(pwsz);
   pMalloc->Release();
   }

//get this app's path and file name
GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule));

DOREGSTRUCT ClsidEntries[] = {HKEY_CLASSES_ROOT,   TEXT("CLSID\\%s"),                  NULL,                   lpszTitle,
                              HKEY_CLASSES_ROOT,   TEXT("CLSID\\%s\\InprocServer32"),  NULL,                   szModule,
                              HKEY_CLASSES_ROOT,   TEXT("CLSID\\%s\\InprocServer32"),  TEXT("ThreadingModel"), TEXT("Apartment"),
                              NULL,                NULL,                               NULL,                   NULL};

//register the CLSID entries
for(i = 0; ClsidEntries[i].hRootKey; i++)
   {
   //create the sub key string - for this case, insert the file extension
   wsprintf(szSubKey, ClsidEntries[i].szSubKey, szCLSID);

   lResult = RegCreateKeyEx(  ClsidEntries[i].hRootKey,
                              szSubKey,
                              0,
                              NULL,
                              REG_OPTION_NON_VOLATILE,
                              KEY_WRITE,
                              NULL,
                              &hKey,
                              &dwDisp);
   
   if(NOERROR == lResult)
      {
      TCHAR szData[MAX_PATH];

      //if necessary, create the value string
      wsprintf(szData, ClsidEntries[i].szData, szModule);
   
      lResult = RegSetValueEx(   hKey,
                                 ClsidEntries[i].lpszValueName,
                                 0,
                                 REG_SZ,
                                 (LPBYTE)szData,
                                 lstrlen(szData) + 1);
      
      RegCloseKey(hKey);
      }
   else
      return FALSE;
   }

return TRUE;
}

/**************************************************************************

   RegisterComCat

**************************************************************************/

BOOL RegisterComCat(CLSID clsid, CATID CatID)
{
ICatRegister   *pcr;
HRESULT        hr = S_OK ;
    
CoInitialize(NULL);

hr = CoCreateInstance(  CLSID_StdComponentCategoriesMgr, 
                        NULL, 
                        CLSCTX_INPROC_SERVER, 
                        IID_ICatRegister, 
                        (LPVOID*)&pcr);

if(SUCCEEDED(hr))
   {
   hr = pcr->RegisterClassImplCategories(clsid, 1, &CatID);

   pcr->Release();
   }
        
CoUninitialize();

return SUCCEEDED(hr);
}

