#pragma once

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"

namespace CaptureManager
{
	namespace COMServer
	{
		class EVRStreamControl :
			public BaseDispatch<IEVRStreamControl>
		{
		public:
			EVRStreamControl();
			virtual ~EVRStreamControl();


		// IEVRStreamControl methods
		
		public:
			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setPosition(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [in] */ FLOAT aLeft,
				/* [in] */ FLOAT aRight,
				/* [in] */ FLOAT aTop,
				/* [in] */ FLOAT aBottom)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setZOrder(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [in] */ DWORD aZOrder)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getPosition(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [out] */ FLOAT *aPtrLeft,
				/* [out] */ FLOAT *aPtrRight,
				/* [out] */ FLOAT *aPtrTop,
				/* [out] */ FLOAT *aPtrBottom)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getZOrder(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [out] */ DWORD *aPtrZOrder)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE flush(
				/* [in] */ IUnknown *aPtrEVROutputNode)override;
			
			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setSrcPosition(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [in] */ FLOAT aLeft,
				/* [in] */ FLOAT aRight,
				/* [in] */ FLOAT aTop,
				/* [in] */ FLOAT aBottom)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getSrcPosition(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [in] */ FLOAT *aPtrLeft,
				/* [in] */ FLOAT *aPtrRight,
				/* [in] */ FLOAT *aPtrTop,
				/* [in] */ FLOAT *aPtrBottom)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getCollectionOfFilters(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [out] */ BSTR *aPtrPtrXMLstring)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setFilterParametr(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [in] */ DWORD aParametrIndex,
				/* [in] */ LONG aNewValue,
				/* [in] */ BOOL aIsEnabled)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getCollectionOfOutputFeatures(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [out] */ BSTR *aPtrPtrXMLstring)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setOutputFeatureParametr(
				/* [in] */ IUnknown *aPtrEVROutputNode,
				/* [in] */ DWORD aParametrIndex,
				/* [in] */ LONG aNewValue)override;



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


			HRESULT STDMETHODCALLTYPE invokeSetPosition(
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult);
			
			HRESULT STDMETHODCALLTYPE invokeSetZOrder(
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult);

			HRESULT STDMETHODCALLTYPE invokeGetPosition(
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult);

			HRESULT STDMETHODCALLTYPE invokeGetZOrder(
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult);

			HRESULT STDMETHODCALLTYPE invokeFlush(
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult);
		
		};
	}
}