#ifndef _DESKBAND_H_
#define _DESKBAND_H_
#include "Libs.h"
#include "IDeskBandWindow.h"

class CDeskBand : public IDeskBand2, 
	              public IPersistStream,
                  public IInputObject, 
                  public IContextMenu
{
public:	
   CDeskBand();
   ~CDeskBand();

   // IUnknown 
   STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
   STDMETHODIMP_(ULONG) AddRef();
   STDMETHODIMP_(ULONG) Release();
   // IOleWindow  
   STDMETHODIMP GetWindow(HWND *phwnd);
   STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);
   // IDockingWindow  
   STDMETHODIMP ShowDW(BOOL fShow);
   STDMETHODIMP CloseDW(DWORD dwReserved);
   STDMETHODIMP ResizeBorderDW(LPCRECT prcBorder,
	   IUnknown *punkToolbarSite, BOOL fReserved);
   // IDeskBand (needed for all desk bands)  
   STDMETHODIMP GetBandInfo(DWORD dwBandID,
	   DWORD dwViewMode, DESKBANDINFO *pdbi);
   // IDeskBand2 (needed for glass desk band)  
   STDMETHODIMP CanRenderComposited(BOOL *pfCanRenderComposited);
   STDMETHODIMP SetCompositionState(BOOL fCompositionEnabled);
   STDMETHODIMP GetCompositionState(BOOL *pfCompositionEnabled);
   // IPersist  
   STDMETHODIMP GetClassID(CLSID *pclsid);
   // IPersistStream  
   STDMETHODIMP IsDirty();
   STDMETHODIMP Load(IStream *pStm);
   STDMETHODIMP Save(IStream *pStm, BOOL fClearDirty);
   STDMETHODIMP GetSizeMax(ULARGE_INTEGER *pcbSize);
   // IObjectWithSite  
   STDMETHODIMP SetSite(IUnknown *pUnkSite);
   STDMETHODIMP GetSite(REFIID riid, void **ppv);
   // IInputObject  
   STDMETHODIMP UIActivateIO(BOOL fActivate, MSG *pMsg);
   STDMETHODIMP HasFocusIO();
   STDMETHODIMP TranslateAcceleratorIO(MSG *pMsg);
   // IContextMenu methods	   
   STDMETHODIMP QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
   STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO *pici);
   STDMETHODIMP GetCommandString(UINT idCommand, UINT uFlags, LPUINT lpReserved, LPSTR lpszName, UINT uMaxNameLen);

   BOOL OnSetFocus(BOOL fSetFocus);
   BOOL IsVerticalViewMode();

private:
	// Whether desk band has focus  
	BOOL m_fHasFocus;
	// Whether desk band setting has changed            
	BOOL m_fIsDirty;
	// Whether glass is currently enabled in desk band            
	BOOL m_fCompositionEnabled;
	// The view mode of the band object  
	DWORD m_dwViewMode;
	// The ID of desk band          
	DWORD  m_dwBandID;
	// The reference count of CDeskBandObject             
	volatile LONG m_lRefCount;
	// The desk band window object          
	IDeskBandWindow    *m_pDeskBandWin;
	// Parent site that contains desk band         
	IInputObjectSite   *m_pInputObjSite;
};

#endif   //DESKBAND_H

