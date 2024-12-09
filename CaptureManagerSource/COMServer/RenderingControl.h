#pragma once

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"

namespace CaptureManager
{
	namespace COMServer
	{
		class RenderingControl :
			public BaseDispatch<IRenderingControl>
		{
		public:
			RenderingControl();
			virtual ~RenderingControl();

			// IRenderingControl implements

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE enableInnerRendering(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [in] */ BOOL aIsInnerRendering)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE renderToTarget(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [in] */ IUnknown *aPtrRenderTarget,
				/* [in] */ BOOL aCopyMode)override;


			// IDispatch interface stub

			STDMETHOD(GetIDsOfNames)(
				__RPC__in REFIID riid,
				/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
				/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
				LCID lcid,
				/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)override;

			virtual HRESULT invokeMethod(
				/* [annotation][in] */
				_In_  DISPID dispIdMember,
				/* [annotation][out][in] */
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult) override;

		private:

			HRESULT STDMETHODCALLTYPE invokeRenderToTarget(
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult);

			HRESULT STDMETHODCALLTYPE invokeEnableInnerRendering(
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult);

		};
	}
}