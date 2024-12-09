#pragma once
#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			MIDL_INTERFACE("64BF40E6-8361-4E85-BC77-2196053FB99D")
			IAudioMixerControl : public IUnknown
			{
			public:
				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setRelativeVolume(
					/* [in] */ FLOAT aRelativeVolume)PURE;
			};


			MIDL_INTERFACE("1DD55EB7-D460-4C68-8402-4225D9AA3F1F")
			IAudioMixerStreamControl : public IUnknown
			{
			public:
				virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setRelativeVolume(
					/* [in] */ DWORD aStreamID,
					/* [in] */ FLOAT aRelativeVolume)PURE;
			};			
		}
	}
}
