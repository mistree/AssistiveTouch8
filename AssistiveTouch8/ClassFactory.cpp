#include "ClassFactory.h"

CClassFactory::CClassFactory(void)
{
	m_lRefCount = 1;
	// Add the reference count of dll. This is a global   
	// variable defined in SdkDeskBandDef.h file  
	InterlockedIncrement(&g_lDllRefCount);
}

CClassFactory::~CClassFactory(void)
{
	// Subtract the reference count of dll. This is a global   
	// variable defined in SdkDeskBandDef.h file  
	InterlockedDecrement(&g_lDllRefCount);
}

STDMETHODIMP CClassFactory::QueryInterface(REFIID riid, LPVOID* ppv)
{
	*ppv = NULL;

	if (IsEqualIID(riid, IID_IUnknown))
	{
		*ppv = this;
	}

	else if (IsEqualIID(riid, IID_IClassFactory))
	{
		*ppv = (IClassFactory*)this;
	}

	if (*ppv)
	{
		(*(LPUNKNOWN*)ppv)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}
//STDMETHODIMP CClassFactory::QueryInterface(REFIID riid, void **ppv)
//{
//	static const QITAB qit[] =
//	{
//		QITABENT(CClassFactory, IUnknown),
//		QITABENT(CClassFactory, IClassFactory),
//		{ 0 },
//	};
//	return QISearch(this, qit, riid, ppv);
//}

STDMETHODIMP_(ULONG) CClassFactory::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) CClassFactory::Release()
{
	ULONG lRef = InterlockedDecrement(&m_lRefCount);
	if (0 == lRef)
	{
		delete this;
	}
	return m_lRefCount;
}

STDMETHODIMP CClassFactory::CreateInstance(
	IUnknown *pUnkOuter, REFIID riid, void **ppv)
{
	HRESULT hr = CLASS_E_NOAGGREGATION;
	if (NULL == pUnkOuter)
	{
		hr = E_OUTOFMEMORY;
		CDeskBand *pDeskBand = new CDeskBand();
		if (NULL != pDeskBand)
		{
			hr = pDeskBand->QueryInterface(riid, ppv);
		}
		SAFE_RELEASE(pDeskBand);
	}
	return hr;
}

STDMETHODIMP CClassFactory::LockServer(BOOL fLock)
{
	if (TRUE == fLock)
	{
		// Add the reference count of dll. This is a   
		// global variable defined in SdkDeskBandDef.h file.  
		InterlockedIncrement(&g_lDllRefCount);
	}
	else
	{
		// Subtract the reference count of dll. This is   
		// a global variable defined in SdkDeskBandDef.h file.  
		InterlockedDecrement(&g_lDllRefCount);
	}
	return S_OK;
}