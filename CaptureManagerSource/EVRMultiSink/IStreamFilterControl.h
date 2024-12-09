#pragma once
#include <Unknwnbase.h>

namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			MIDL_INTERFACE("0C5F1CE5-488B-4B3C-B0DF-0DDCA8C0FE12")
			IStreamFilterControl : public IUnknown
			{
			public:

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getCollectionOfFilters(
					/* [in] */ DWORD aMixerStreamID,
					/* [out] */ BSTR *aPtrPtrXMLstring) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setFilterParametr(
					/* [in] */ DWORD aMixerStreamID,
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