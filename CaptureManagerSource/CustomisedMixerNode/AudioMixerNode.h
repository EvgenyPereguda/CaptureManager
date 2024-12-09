#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <vector>
#include <unordered_map>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "IAudioMixerControl.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			namespace Mixer
			{
				class AudioMixerNode :
					public BaseUnknown<
					IMFTransform
					, CustomisedMediaSession::IAudioMixerStreamControl
					>
				{
				public:
					AudioMixerNode();
					virtual ~AudioMixerNode();

					static HRESULT create(
						DWORD aInputNodeAmount,
						IMFTransform** aPtrPtrTransform);

					// IMFTransform implements

					STDMETHODIMP GetStreamLimits(
						DWORD* aPtrInputMinimum,
						DWORD* aPtrInputMaximum,
						DWORD* aPtrOutputMinimum,
						DWORD* aPtrOutputMaximum);

					STDMETHODIMP GetStreamIDs(
						DWORD aInputIDArraySize,
						DWORD* aPtrInputIDs,
						DWORD aOutputIDArraySize,
						DWORD* aPtrOutputIDs);

					STDMETHODIMP GetStreamCount(
						DWORD* aPtrInputStreams,
						DWORD* aPtrOutputStreams);

					STDMETHODIMP GetInputStreamInfo(
						DWORD aInputStreamID,
						MFT_INPUT_STREAM_INFO* aPtrStreamInfo);

					STDMETHODIMP GetOutputStreamInfo(
						DWORD aOutputStreamID,
						MFT_OUTPUT_STREAM_INFO* aPtrStreamInfo);

					STDMETHODIMP GetInputStreamAttributes(
						DWORD aInputStreamID,
						IMFAttributes** aPtrPtrAttributes);

					STDMETHODIMP GetOutputStreamAttributes(
						DWORD aOutputStreamID,
						IMFAttributes** aPtrPtrAttributes);

					STDMETHODIMP DeleteInputStream(
						DWORD aStreamID);

					STDMETHODIMP AddInputStreams(
						DWORD aStreams,
						DWORD* aPtrStreamIDs);

					STDMETHODIMP GetInputAvailableType(
						DWORD aInputStreamID,
						DWORD aTypeIndex,
						IMFMediaType** aPtrPtrType);

					STDMETHODIMP GetOutputAvailableType(
						DWORD aOutputStreamID,
						DWORD aTypeIndex,
						IMFMediaType** aPtrPtrType);

					STDMETHODIMP SetInputType(
						DWORD aInputStreamID,
						IMFMediaType* aPtrType,
						DWORD aFlags);

					STDMETHODIMP SetOutputType(
						DWORD aOutputStreamID,
						IMFMediaType* aPtrType,
						DWORD aFlags);

					STDMETHODIMP GetInputCurrentType(
						DWORD aInputStreamID,
						IMFMediaType** aPtrPtrType);

					STDMETHODIMP GetOutputCurrentType(
						DWORD aOutputStreamID,
						IMFMediaType** aPtrPtrType);

					STDMETHODIMP GetInputStatus(
						DWORD aInputStreamID,
						DWORD* aPtrFlags);

					STDMETHODIMP GetOutputStatus(
						DWORD* aPtrFlags);

					STDMETHODIMP SetOutputBounds(
						LONGLONG aLowerBound,
						LONGLONG aUpperBound);

					STDMETHODIMP ProcessEvent(
						DWORD aInputStreamID,
						IMFMediaEvent* aPtrEvent);

					STDMETHODIMP GetAttributes(
						IMFAttributes** aPtrPtrAttributes);

					STDMETHODIMP ProcessMessage(
						MFT_MESSAGE_TYPE aMessage,
						ULONG_PTR aParam);

					STDMETHODIMP ProcessInput(
						DWORD aInputStreamID,
						IMFSample* aPtrSample,
						DWORD aFlags);

					STDMETHODIMP ProcessOutput(
						DWORD aFlags,
						DWORD aOutputBufferCount,
						MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples,
						DWORD* aPtrStatus);



					// IAudioMixerNode implements
					
					STDMETHODIMP setRelativeVolume(
						/* [in] */ DWORD aStreamID,
						/* [in] */ FLOAT aRelativeVolume);

				private:

					std::condition_variable mConditionVariable;

					std::mutex mMutex;

					CComPtrCustom<IMFMediaType> mOutputMediaType;

					struct StreamSample
					{
						CComPtrCustom<IMFSample> mSample;

						DWORD mLength;

						DWORD mPosition;

						StreamSample()
						{}

						StreamSample(IMFSample* aPtrSample)
						{						
							mSample = aPtrSample;

							aPtrSample->GetTotalLength(&mLength);

							mPosition = 0;
						}
					};
													
					struct StreamInfo
					{
						CComPtrCustom<IMFMediaType> mInputMediaType;
						
						std::vector<StreamSample> mStreamSamples;

						bool mAllowRead = false;

						int mIndex;

						StreamInfo()
						{}
					};

					struct RelativeVolumes
					{
						float* mRelativeVolumes;

						RelativeVolumes(): mRelativeVolumes(nullptr)
						{}

						~RelativeVolumes() {
							if (mRelativeVolumes != nullptr)
								delete[] mRelativeVolumes;
						}

					};

					std::unique_ptr<RelativeVolumes> mFirstRelativeVolumes;

					std::unique_ptr<RelativeVolumes> mSecondRelativeVolumes;


					RelativeVolumes* m_PtrCurrentRelativeVolumes;

					RelativeVolumes* m_PtrPrevRelativeVolumes;

					std::unordered_map<DWORD, StreamInfo > m_InputStreams;


					CComPtrCustom<IMFSample> mOutputSample;

					void process();
				};
			}
		}
	}
}
