#define _DEFINE_GLOBAL_VARIABLES  
#include "DeskBand.h"
#include "DeskBandDef.h"

CDeskBand::CDeskBand()
{
	m_lRefCount = 1;
	m_dwBandID = 0;
	m_fHasFocus = FALSE;
	m_fIsDirty = FALSE;
	m_dwViewMode = DBIF_VIEWMODE_NORMAL;
	m_pInputObjSite = NULL;
	m_pDeskBandWin = NULL;
	m_fCompositionEnabled = FALSE;
	InterlockedIncrement(&g_lDllRefCount);
}

CDeskBand::~CDeskBand()
{
	InterlockedDecrement(&g_lDllRefCount);
	SAFE_DELETE(m_pDeskBandWin);
	SAFE_RELEASE(m_pInputObjSite);
}

// IUnknown

STDMETHODIMP CDeskBand::QueryInterface(REFIID riid, LPVOID *ppv)
{
	static const QITAB qit[] =
	{
		QITABENT(CDeskBand, IDeskBand),
		QITABENT(CDeskBand, IDeskBand2),
		QITABENT(CDeskBand, IDockingWindow),
		QITABENT(CDeskBand, IPersist),
		QITABENT(CDeskBand, IPersistStream),
		QITABENT(CDeskBand, IInputObject),
		QITABENT(CDeskBand, IOleWindow),
		{ 0 },
	};
	return QISearch(this, qit, riid, ppv);
}                                             

STDMETHODIMP_(ULONG) CDeskBand::AddRef()
{
	return InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) CDeskBand::Release()
{
	ULONG lRef = InterlockedDecrement(&m_lRefCount);
	if (0 == lRef)
	{
		delete this;
	}
	return lRef;
}

// IOleWindow

STDMETHODIMP CDeskBand::GetWindow(HWND *phWnd)
{
	if (NULL == phWnd)
	{
		return E_INVALIDARG;
	}
	if (NULL != m_pDeskBandWin)
	{
		*phWnd = m_pDeskBandWin->GetDeskBandHandle();
	}
	return S_OK;
}

STDMETHODIMP CDeskBand::ContextSensitiveHelp(BOOL fEnterMode)
{
	UNREFERENCED_PARAMETER(fEnterMode);
	return E_NOTIMPL;
}

// IDockingWindow

STDMETHODIMP CDeskBand::ShowDW(BOOL fShow)
{
	if (NULL != m_pDeskBandWin)
	{
		m_pDeskBandWin->ShowDeskBand(fShow);
	}
	return S_OK;
}

STDMETHODIMP CDeskBand::CloseDW(DWORD dwReserved)
{
	UNREFERENCED_PARAMETER(dwReserved);
	if (NULL != m_pDeskBandWin)
	{
		m_pDeskBandWin->CloseDeskBand();
	}
	return S_OK;
}

STDMETHODIMP CDeskBand::ResizeBorderDW(LPCRECT prcBorder,
	IUnknown *punkToolbarSite, BOOL fReserved)
{
	UNREFERENCED_PARAMETER(prcBorder);
	UNREFERENCED_PARAMETER(punkToolbarSite);
	UNREFERENCED_PARAMETER(fReserved);
	return E_NOTIMPL;
}

// IDeskBand

STDMETHODIMP CDeskBand::GetBandInfo(DWORD dwBandID,
	DWORD dwViewMode, DESKBANDINFO *pdbi)
{
	if (NULL == pdbi)
	{
		return E_INVALIDARG;
	}
	m_dwBandID = dwBandID;
	m_dwViewMode = dwViewMode;
	SIZE deskBandSize = m_pDeskBandWin->GetDeskBandMinSize(
		(DBIF_VIEWMODE_VERTICAL == dwViewMode) ? FALSE : TRUE);
	POINTL ptMinSize = { deskBandSize.cx, deskBandSize.cy };
	POINTL ptActual = { deskBandSize.cx, deskBandSize.cy };
	if (pdbi->dwMask & DBIM_MINSIZE)
	{
		pdbi->ptMinSize = ptMinSize;
	}
	if (pdbi->dwMask & DBIM_ACTUAL)
	{
		pdbi->ptActual = ptActual;
	}
	if (pdbi->dwMask & DBIM_MAXSIZE)
	{
		// There is no limit for the maximum height.  
		pdbi->ptMaxSize.y = -1;
	}
	if (pdbi->dwMask & DBIM_INTEGRAL)
	{
		// The sizing step value of the band object.  
		pdbi->ptIntegral.y = 1;
	}
	if (pdbi->dwMask & DBIM_TITLE)
	{
		// Do NOT show the title by removing the flag.  
		pdbi->dwMask &= ~DBIM_TITLE;
	}
	if (pdbi->dwMask & DBIM_MODEFLAGS)
	{
		pdbi->dwModeFlags = DBIMF_NORMAL | DBIMF_VARIABLEHEIGHT;
	}
	if (pdbi->dwMask & DBIM_BKCOLOR)
	{
		// Use the default background color by removing this flag.  
		pdbi->dwMask &= ~DBIM_BKCOLOR;
	}
	return S_OK;
}

// IDeskBand2

STDMETHODIMP CDeskBand::CanRenderComposited(BOOL *pfCanRenderComposited)
{
	if (NULL == pfCanRenderComposited)
	{
		return E_INVALIDARG;
	}
	*pfCanRenderComposited = TRUE;
	return S_OK;
}

STDMETHODIMP CDeskBand::SetCompositionState(BOOL fCompositionEnabled)
{
	m_fCompositionEnabled = fCompositionEnabled;
	if (NULL != m_pDeskBandWin)
	{
		m_pDeskBandWin->RefreshDeskBand();
	}
	return S_OK;
}

STDMETHODIMP CDeskBand::GetCompositionState(BOOL *pfCompositionEnabled)
{
	if (NULL == pfCompositionEnabled)
	{
		return E_INVALIDARG;
	}
	*pfCompositionEnabled = m_fCompositionEnabled;
	return S_OK;
}

// IPersist  

STDMETHODIMP CDeskBand::GetClassID(CLSID *pclsid)
{
	if (NULL == pclsid)
	{
		return E_INVALIDARG;
	}
	*pclsid = CLSID_TouchBand;
	return S_OK;
}

// IPersistStream  

STDMETHODIMP CDeskBand::IsDirty()
{
	return (TRUE == m_fIsDirty) ? S_OK : S_FALSE;
}

STDMETHODIMP CDeskBand::Load(IStream *pStm)
{
	UNREFERENCED_PARAMETER(pStm);
	return E_NOTIMPL;
}

STDMETHODIMP CDeskBand::Save(IStream *pStm, BOOL fClearDirty)
{
	UNREFERENCED_PARAMETER(pStm);
	if (TRUE == fClearDirty)
	{
		m_fIsDirty = FALSE;
	}
	return E_NOTIMPL;
}
 
STDMETHODIMP CDeskBand::GetSizeMax(ULARGE_INTEGER *pcbSize)
{
	UNREFERENCED_PARAMETER(pcbSize);
	return E_NOTIMPL;
}

// IObjectWithSite  

STDMETHODIMP CDeskBand::SetSite(IUnknown *pUnkSite)
{
	SAFE_RELEASE(m_pInputObjSite);
	if (NULL == pUnkSite)
	{
		return S_OK;
	}
	IOleWindow *pOleWindow = NULL;
	HRESULT hr = pUnkSite->QueryInterface(IID_PPV_ARGS(&pOleWindow));
	if (SUCCEEDED(hr))
	{
		HWND hParentWnd = NULL;
		hr = pOleWindow->GetWindow(&hParentWnd);
		if (SUCCEEDED(hr))
		{
			// Create the desk band window.  
			if (NULL != m_pDeskBandWin)
			{
				// g_hDllInst is a global variable to   
				// indicate instance of current dll defined   
				// in SdkDeskBanddef.h file.  
				m_pDeskBandWin->CreateDeskBand(hParentWnd, g_hDllInst, this);
			}
		}
		hr = pUnkSite->QueryInterface(IID_PPV_ARGS(&m_pInputObjSite));
	}
	SAFE_RELEASE(pOleWindow);
	return hr;
}

STDMETHODIMP CDeskBand::GetSite(REFIID riid, void **ppv)
{
	if (NULL == ppv)
	{
		return E_INVALIDARG;
	}
	HRESULT hr = E_FAIL;
	if (NULL == m_pInputObjSite)
	{
		*ppv = NULL;
	}
	else
	{
		hr = m_pInputObjSite->QueryInterface(riid, ppv);
	}
	return hr;
}

// IInputObject

STDMETHODIMP CDeskBand::UIActivateIO(BOOL fActivate, MSG *pMsg)
{
	UNREFERENCED_PARAMETER(pMsg);
	if (TRUE == fActivate)
	{
		if (NULL != m_pDeskBandWin)
		{
			SetFocus(m_pDeskBandWin->GetDeskBandHandle());
		}
	}
	return S_OK;
}

STDMETHODIMP CDeskBand::HasFocusIO()
{
	return (TRUE == m_fHasFocus) ? S_OK : S_FALSE;
}

STDMETHODIMP CDeskBand::TranslateAcceleratorIO(MSG *pMsg)
{
	UNREFERENCED_PARAMETER(pMsg);
	return S_FALSE;
}

// IContextMenu methods

STDMETHODIMP CDeskBand::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{

}

STDMETHODIMP CDeskBand::InvokeCommand(LPCMINVOKECOMMANDINFO *pici)
{
	UNREFERENCED_PARAMETER(pici);
}

STDMETHODIMP CDeskBand::GetCommandString(UINT idCommand, UINT uFlags, LPUINT lpReserved, LPSTR lpszName, UINT uMaxNameLen)
{

}

BOOL CDeskBand::OnSetFocus(BOOL fSetFocus)
{
	if (NULL == m_pInputObjSite)
	{
		return FALSE;
	}
	m_fHasFocus = fSetFocus;
	HRESULT hr = m_pInputObjSite->OnFocusChangeIS(
		static_cast<IOleWindow*>(this), fSetFocus);
	return SUCCEEDED(hr) ? TRUE : FALSE;
}
//////////////////////////////////////////////////////////////////////////  
BOOL CDeskBand::IsVerticalViewMode()
{
	return (DBIF_VIEWMODE_VERTICAL == m_dwViewMode) ? TRUE : FALSE;
}