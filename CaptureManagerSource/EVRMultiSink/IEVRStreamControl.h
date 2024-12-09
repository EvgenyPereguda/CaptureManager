#pragma once
#include <Unknwnbase.h>

namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			MIDL_INTERFACE("57523B2A-D3DF-4BA3-B7A1-91BD1E0E7DE8")
			IEVRStreamControl : public IDispatch
			{
			public:
				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setPosition(
					/* [in] */ FLOAT aLeft,
					/* [in] */ FLOAT aRight,
					/* [in] */ FLOAT aTop,
					/* [in] */ FLOAT aBottom) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setOpacity(
					/* [in] */ FLOAT aOpacity) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setZOrder(
					/* [in] */ DWORD aZOrder) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setSrcPosition(
					/* [in] */ FLOAT aLeft,
					/* [in] */ FLOAT aRight,
					/* [in] */ FLOAT aTop,
					/* [in] */ FLOAT aBottom) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getPosition(
					/* [out] */ FLOAT *aPtrLeft,
					/* [out] */ FLOAT *aPtrRight,
					/* [out] */ FLOAT *aPtrTop,
					/* [out] */ FLOAT *aPtrBottom) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getSrcPosition(
					/* [out] */ FLOAT *aPtrLeft,
					/* [out] */ FLOAT *aPtrRight,
					/* [out] */ FLOAT *aPtrTop,
					/* [out] */ FLOAT *aPtrBottom) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getOpacity(
					/* [out] */ FLOAT *aPtrOpacity) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getZOrder(
					/* [out] */ DWORD *aPtrZOrder) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE flush() = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getCollectionOfFilters(
					/* [out] */ BSTR *aPtrPtrXMLstring) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setFilterParametr(
					/* [in] */ DWORD aParametrIndex,
					/* [in] */ LONG aNewValue,
					/* [in] */ BOOL aIsEnabled) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getCollectionOfOutputFeatures(
					/* [out] */ BSTR *aPtrPtrXMLstring) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setOutputFeatureParametr(
					/* [in] */ DWORD aParametrIndex,
					/* [in] */ LONG aNewValue) = 0;
			};
		}
	}
}