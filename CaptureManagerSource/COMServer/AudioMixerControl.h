#pragma once

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"

namespace CaptureManager
{
	namespace COMServer
	{
		class AudioMixerControl :
			public BaseDispatch<IAudioMixerControl>
		{
		public:
			AudioMixerControl();
			virtual ~AudioMixerControl();

		//	IAudioMixerControl implements
		
			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setRelativeVolume(
				/* [in] */ IUnknown *aPtrAudioMixerNode,
				/* [in] */ FLOAT aRelativeVolume) override;
		};
	}
}