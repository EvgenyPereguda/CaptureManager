#pragma once

#include <Unknwn.h>

MIDL_INTERFACE("F71FDD77-7FB8-445F-8B6D-DEBFE4F8D300")
IInnerGetService : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetService( 
        /* [in] */ __RPC__in REFGUID guidService,
        /* [in] */ __RPC__in REFIID riid,
        /* [iid_is][out] */ __RPC__deref_out_opt LPVOID *ppvObject) = 0;
        
};

