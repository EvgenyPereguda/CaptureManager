#pragma once
#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			MIDL_INTERFACE("C2818A0A-B852-4C56-AF60-6D9F164BF452")
			IVideoMixerControl : public IUnknown
			{
				public:
					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setPosition(
						/* [in] */ FLOAT aLeft,
						/* [in] */ FLOAT aRight,
						/* [in] */ FLOAT aTop,
						/* [in] */ FLOAT aBottom)PURE;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setSrcPosition(
						/* [in] */ FLOAT aLeft,
						/* [in] */ FLOAT aRight,
						/* [in] */ FLOAT aTop,
						/* [in] */ FLOAT aBottom)PURE;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setZOrder(
						/* [in] */ DWORD aZOrder)PURE;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setOpacity(
						/* [in] */ FLOAT aOpacity)PURE;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE flush()PURE;
			};
		}
	}
}