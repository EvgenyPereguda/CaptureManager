/*
MIT License

Copyright(c) 2020 Evgeny Pereguda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "AudioMixerControl.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Singleton.h"
#include "../Common/Common.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../LogPrintOut/LogPrintOut.h"


namespace CaptureManager
{
	namespace COMServer
	{

		AudioMixerControl::AudioMixerControl()
		{
		}


		AudioMixerControl::~AudioMixerControl()
		{
		}

		//	IAudioMixerControl implements

		HRESULT STDMETHODCALLTYPE AudioMixerControl::setRelativeVolume(
			/* [in] */ IUnknown *aPtrAudioMixerNode,
			/* [in] */ FLOAT aRelativeVolume)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrAudioMixerNode);

				if (aRelativeVolume > 1.0f)
					aRelativeVolume = 1.0f;

				if (aRelativeVolume <= 0.0f)
					aRelativeVolume = 0.0f;


				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().setAudioMixerRelativeVolume,
					aPtrAudioMixerNode,
					aRelativeVolume);

			} while (false);

			return lresult;
		}
	}
}