#pragma once
#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			MIDL_INTERFACE("64AC68F1-EAD4-4B8C-9836-B22087C8F1C6")
			IMixerStreamPositionControl : public IUnknown
			{
			public:
				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setPosition(
					/* [in] */ DWORD aStreamID,
					/* [in] */ FLOAT aLeft,
					/* [in] */ FLOAT aRight,
					/* [in] */ FLOAT aTop,
					/* [in] */ FLOAT aBottom) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setSrcPosition(
					/* [in] */ DWORD aStreamID,
					/* [in] */ FLOAT aLeft,
					/* [in] */ FLOAT aRight,
					/* [in] */ FLOAT aTop,
					/* [in] */ FLOAT aBottom) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setOpacity(
					/* [in] */ DWORD aStreamID,
					/* [in] */ FLOAT aOpacity) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setZOrder(
					/* [in] */ DWORD aStreamID,
					/* [in] */ DWORD aZOrder) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getPosition(
					/* [in] */ DWORD aStreamID,
					/* [out] */ FLOAT *aPtrLeft,
					/* [out] */ FLOAT *aPtrRight,
					/* [out] */ FLOAT *aPtrTop,
					/* [out] */ FLOAT *aPtrBottom) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getSrcPosition(
					/* [in] */ DWORD aStreamID,
					/* [out] */ FLOAT *aPtrLeft,
					/* [out] */ FLOAT *aPtrRight,
					/* [out] */ FLOAT *aPtrTop,
					/* [out] */ FLOAT *aPtrBottom) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getOpacity(
					/* [in] */ DWORD aStreamID,
					/* [out] */ FLOAT *aPtrOpacity) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getZOrder(
					/* [in] */ DWORD aStreamID,
					/* [out] */ DWORD *aPtrZOrder) = 0;

				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE flush(
					/* [in] */ DWORD aStreamID) = 0;
			};
		}
	}
}