#pragma once

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"


namespace CaptureManager
{
	namespace COMServer
	{
		class SwitcherControl :
			public BaseDispatch<ISwitcherControl>
		{
		public:

		// ISwitcherControl interface
	
			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE pauseSwitchers(
				/* [in] */ DWORD aSessionDescriptor) override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE resumeSwitchers(
				/* [in] */ DWORD aSessionDescriptor) override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE pauseSwitcher(
				/* [in] */ IUnknown *aPtrSwitcherNode) override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE resumeSwitcher(
				/* [in] */ IUnknown *aPtrSwitcherNode) override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE detachSwitchers(
				/* [in] */ DWORD aSessionDescriptor) override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE attachSwitcher(
				/* [in] */ IUnknown *aPtrSwitcherNode,
				/* [in] */ IUnknown *aPtrDownStreamNode) override;



			// IDispatch interface stub

			STDMETHOD(GetIDsOfNames)(
				__RPC__in REFIID riid,
				/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
				/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
				LCID lcid,
				/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);

			virtual HRESULT invokeMethod(
				/* [annotation][in] */
				_In_  DISPID dispIdMember,
				/* [annotation][out][in] */
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult);
		};
	}
}