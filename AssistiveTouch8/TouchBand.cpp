#include <ole2.h>
#include <comcat.h>
#include <olectl.h>
#include "ClassFactory.h"

HINSTANCE   g_hInst = NULL;
UINT        g_DllRefCount = 0;


extern "C" BOOL WINAPI DllMain(  HINSTANCE hInstance, 
                                 DWORD dwReason, 
                                 LPVOID lpReserved)
{
switch(dwReason)
   {
   case DLL_PROCESS_ATTACH:
      g_hInst = hInstance;
	  g_hDllInst = hInstance;
	  DisableThreadLibraryCalls(hInstance);
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

HRESULT RegisterServer()
{
	WCHAR szCLSID[MAX_PATH];
	StringFromGUID2(CLSID_TouchBand, szCLSID, ARRAYSIZE(szCLSID));

	WCHAR szSubkey[MAX_PATH];
	HKEY hKey;

	HRESULT hr = StringCchPrintfW(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
	if (SUCCEEDED(hr))
	{
		hr = E_FAIL;
		if (ERROR_SUCCESS == RegCreateKeyExW(HKEY_CLASSES_ROOT,
			szSubkey,
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_WRITE,
			NULL,
			&hKey,
			NULL))
		{
			WCHAR const szName[] = L"AssistiveTouch8";
			if (ERROR_SUCCESS == RegSetValueExW(hKey,
				NULL,
				0,
				REG_SZ,
				(LPBYTE)szName,
				sizeof(szName)))
			{
				hr = S_OK;
			}

			RegCloseKey(hKey);
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = StringCchPrintfW(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s\\InprocServer32", szCLSID);
		if (SUCCEEDED(hr))
		{
			hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CLASSES_ROOT, szSubkey,
				0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));
			if (SUCCEEDED(hr))
			{
				WCHAR szModule[MAX_PATH];
				if (GetModuleFileNameW(g_hInst, szModule, ARRAYSIZE(szModule)))
				{
					DWORD cch = lstrlen(szModule);
					hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, NULL, 0, REG_SZ, (LPBYTE)szModule, cch * sizeof(szModule[0])));
				}

				if (SUCCEEDED(hr))
				{
					WCHAR const szModel[] = L"Apartment";
					hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, L"ThreadingModel", 0, REG_SZ, (LPBYTE)szModel, sizeof(szModel)));
				}

				RegCloseKey(hKey);
			}
		}
	}

	return hr;
}

HRESULT RegisterComCat()
{
	ICatRegister *pcr;
	HRESULT hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pcr));
	if (SUCCEEDED(hr))
	{
		CATID catid = CATID_DeskBand;
		hr = pcr->RegisterClassImplCategories(CLSID_TouchBand, 1, &catid);
		pcr->Release();
	}
	return hr;
}

STDAPI DllRegisterServer()
{
	// Register the deskband object.
	HRESULT hr = RegisterServer();
	if (SUCCEEDED(hr))
	{
		// Register the component category for the deskband object.
		hr = RegisterComCat();
	}

	return SUCCEEDED(hr) ? S_OK : SELFREG_E_CLASS;
}

STDAPI DllUnregisterServer()
{
	WCHAR szCLSID[MAX_PATH];
	StringFromGUID2(CLSID_TouchBand, szCLSID, ARRAYSIZE(szCLSID));

	WCHAR szSubkey[MAX_PATH];
	HRESULT hr = StringCchPrintfW(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
	if (SUCCEEDED(hr))
	{
		if (ERROR_SUCCESS != RegDeleteTreeW(HKEY_CLASSES_ROOT, szSubkey))
		{
			hr = E_FAIL;
		}
	}

	HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", NULL);
	// If task bar receives this message, it will call CoFreeUnusedLibraries function  
	// immediately to free unused libraries.  
	PostMessage(hTaskbarWnd, WM_TIMER, 24, 0);

	return SUCCEEDED(hr) ? S_OK : SELFREG_E_CLASS;
}

//wchar_t RESULT[100];
//swprintf(RESULT, L"0x%08lx", r);
//if (r != S_OK)
//MessageBox(NULL, RESULT, NULL, MB_OK);