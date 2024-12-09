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

#include "MixerWrapper.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/GUIDs.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "Direct3D11VideoMixer.h"
#include "Direct3D9VideoMixer.h"
#include "AudioMixerNode.h"
#include "../Common/Singleton.h"
#include "../ConfigManager/ConfigManager.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			using namespace CaptureManager::Core;

			MixerWrapper::MixerWrapper(DWORD aInputNodeAmount):
				mInputNodeAmount(aInputNodeAmount)
			{
			}

			MixerWrapper::~MixerWrapper()
			{
			}

			HRESULT MixerWrapper::getStreamCount(
				REFGUID aRefMajorType,
				DWORD& aRefInputNodeAmount)
			{
				HRESULT lresult = S_OK;

				do
				{
					if (Singleton<ConfigManager>::getInstance().isWindows10_Or_Greater())
					{
						if (aRefMajorType == MFMediaType_Video)
						{
							aRefInputNodeAmount = Singleton<ConfigManager>::getInstance().getMaxVideoRenderStreamCount();
						}
						else if (aRefMajorType == MFMediaType_Audio)
						{
							aRefInputNodeAmount = Singleton<ConfigManager>::getInstance().getMaxAudioInputMixerNodeAmount();
						}
						else
							LOG_CHECK_STATE_DESCR(true, E_FAIL);
					}
					else
						LOG_CHECK_STATE_DESCR(true, E_FAIL);

				} while (false);

				return lresult;
			}


			//	IMixerWrapper implementation

			HRESULT MixerWrapper::initialize(
				IMFMediaType* aPtrMediaTypeType)
			{
				HRESULT lresult = S_OK;

				do
				{

					GUID lMajorType = GUID_NULL;

					LOG_INVOKE_MF_METHOD(GetGUID,
						aPtrMediaTypeType,
						MF_MT_MAJOR_TYPE,
						&lMajorType);

					CComPtrCustom<IMFTransform> lMixerTransform;

					if (lMajorType == MFMediaType_Video)
					{
						do
						{
							Mixer::Direct3D11VideoMixer::createProcessor(&lMixerTransform, Singleton<ConfigManager>::getInstance().getMaxVideoRenderStreamCount());

							LOG_CHECK_PTR_MEMORY(lMixerTransform);

							do
							{
								CComPtrCustom<IUnknown> lInputStreamDeviceManager;

								LOG_INVOKE_MF_METHOD(GetUnknown, aPtrMediaTypeType,
									CM_DeviceManager,
									IID_PPV_ARGS(&lInputStreamDeviceManager));

								lMixerTransform->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, 0);

								lMixerTransform->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, (ULONG_PTR)lInputStreamDeviceManager.get());

								lMixerTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);

							} while (false);

							if (FAILED(lresult))
							{
								LOG_INVOKE_MF_METHOD(ProcessMessage, lMixerTransform, MFT_MESSAGE_SET_D3D_MANAGER, 0);

								LOG_INVOKE_MF_METHOD(ProcessMessage, lMixerTransform, MFT_MESSAGE_COMMAND_MARKER,
									Mixer::Direct3D11VideoMixer::COMMAND_MARKER_VIDEO_MIXER::INNER_DIRECTX_DEVICE);

								lMixerTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);
							}

						} while (false);

						if (FAILED(lresult))
						{
							lMixerTransform.Release();

							Mixer::Direct3D9VideoMixer::createProcessor(&lMixerTransform, Singleton<ConfigManager>::getInstance().getMaxVideoRenderStreamCount());

							LOG_CHECK_PTR_MEMORY(lMixerTransform);

							do
							{
								CComPtrCustom<IUnknown> lInputStreamDeviceManager;

								LOG_INVOKE_MF_METHOD(GetUnknown, aPtrMediaTypeType,
									CM_DeviceManager,
									IID_PPV_ARGS(&lInputStreamDeviceManager));

								lMixerTransform->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, 0);

								lMixerTransform->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, (ULONG_PTR)lInputStreamDeviceManager.get());

								lMixerTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);

							} while (false);

							if (FAILED(lresult))
							{
								LOG_INVOKE_MF_METHOD(ProcessMessage, lMixerTransform, MFT_MESSAGE_SET_D3D_MANAGER, 0);

								LOG_INVOKE_MF_METHOD(ProcessMessage, lMixerTransform, MFT_MESSAGE_COMMAND_MARKER,
									Mixer::Direct3D9VideoMixer::COMMAND_MARKER_VIDEO_MIXER::INNER_DIRECTX_DEVICE);

								lMixerTransform->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);
							}
						}						
					}
					else if (lMajorType == MFMediaType_Audio)
					{
						using namespace Mixer;
												
						AudioMixerNode::create(Singleton<ConfigManager>::getInstance().getMaxAudioInputMixerNodeAmount(), &lMixerTransform);
					}
					else
						LOG_CHECK_STATE_DESCR(true, MF_E_INVALIDMEDIATYPE);

					LOG_CHECK_PTR_MEMORY(lMixerTransform);


					DWORD lInputMin = 0;

					DWORD lInputMax = 0;

					DWORD lOutputMin = 0;

					DWORD lOutputMax = 0;

					LOG_INVOKE_POINTER_METHOD(lMixerTransform, GetStreamLimits,
						&lInputMin,
						&lInputMax,
						&lOutputMin,
						&lOutputMax);

					LOG_CHECK_STATE_DESCR(mInputNodeAmount > lInputMax, E_BOUNDS);
								
					std::unique_ptr<DWORD[]> lInputIDStream;

					lInputIDStream.reset(new DWORD[mInputNodeAmount]);

					DWORD lOutputIDStream;

					LOG_INVOKE_POINTER_METHOD(lMixerTransform, GetStreamIDs,
						mInputNodeAmount,
						lInputIDStream.get(),
						1,
						&lOutputIDStream);
					

					LOG_INVOKE_POINTER_METHOD(lMixerTransform, AddInputStreams,
						mInputNodeAmount,
						lInputIDStream.get());

					mMixerUnkTransform = lMixerTransform;

				} while (false);

				return lresult;
			}


			HRESULT MixerWrapper::getMixer(
				IMFTransform** aPtrMixer)
			{
				HRESULT lresult = S_OK;

				do
				{
					LOG_CHECK_PTR_MEMORY(mMixerUnkTransform);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixerUnkTransform, aPtrMixer);

				} while (false);

				return lresult;
			}
		}
	}
}