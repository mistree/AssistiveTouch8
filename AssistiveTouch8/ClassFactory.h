#ifndef CLASSFACTORY_H
#define CLASSFACTORY_H
#include "Libs.h"
#include "DeskBand.h"
#include "DeskBandDef.h"

class CClassFactory : public IClassFactory
{
protected:
   DWORD m_ObjRefCount;

public:
   CClassFactory();
   ~CClassFactory();

   //IUnknown methods
   STDMETHODIMP QueryInterface(REFIID riid, LPVOID* ppv);
   STDMETHODIMP_(ULONG) AddRef();
   STDMETHODIMP_(ULONG) Release();

   //IClassFactory methods
   STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
   STDMETHODIMP LockServer(BOOL fLock);

private:
   volatile LONG m_lRefCount;    // The reference count of current class.  
};

#endif   //CLASSFACTORY_H
