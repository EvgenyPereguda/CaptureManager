#pragma once
#include <thread>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "../Common/BaseUnknown.h"
#include "../Common/IInnerCaptureProcessor.h"
#include "../Common/SourceState.h"
#include "../Common/ComPtrCustom.h"
#include "../CaptureInvoker/CaptureInvoker.h"

struct IMFMediaType;
struct IMFSample;
struct IMMDevice;
struct IAudioClient;
struct IAudioCaptureClient;
struct IMFMediaBuffer;


typedef LONGLONG REFERENCE_TIME;
typedef LONGLONG MFTIME;

namespace CaptureManager
{
	namespace Sources
	{
		namespace AudioEndpointCapture
		{
			class AudioEndpointCaptureProcessor :
				public BaseUnknown<IInnerCaptureProcessor>, public Core::CaptureInvoker
			{
			public:
				AudioEndpointCaptureProcessor();


				// CaptureInvoker implementation

				virtual HRESULT STDMETHODCALLTYPE starting() override;

				virtual HRESULT STDMETHODCALLTYPE invoke() override;


				//IInnerCaptureProcessor interface

				// get friendly name of Processor
				virtual HRESULT getFrendlyName(
					BSTR* aPtrString);

				// get symbolicLink of Processor
				virtual HRESULT getSymbolicLink(
					BSTR* aPtrString);

				// get supported amount of streams
				virtual HRESULT getStreamAmount(
					UINT32* aPtrStreamAmount);

				// get supported media types of stream
				virtual HRESULT getMediaTypes(
					UINT32 aStreamIndex,
					BSTR* aPtrStreamNameString,
					IUnknown*** aPtrPtrPtrMediaType,
					UINT32* aPtrMediaTypeCount);

				// set supported media type to stream
				virtual HRESULT setCurrentMediaType(
					UINT32 aStreamIndex,
					IUnknown* aPtrMediaType);

				// get new sample of stream
				virtual HRESULT getNewSample(
					DWORD aStreamIdentifier,
					IUnknown** aPtrPtrSample);

				// start processor
				virtual HRESULT start();

				// stop processor
				virtual HRESULT stop();

				// pause processor
				virtual HRESULT pause();

				// restart processor
				virtual HRESULT restart();

				// shutdown processor
				virtual HRESULT shutdown();

				// register Media Event interface
				virtual HRESULT registerMediaEvent(IUnknown* pUnkIMediaEvent);

				// AudioEndpointCaptureProcessor interface

				HRESULT init(
					IMMDevice* aPtrMMDevice,
					std::wstring aSymbolicLink);

				//CAPTUREMANAGER_BOX

				struct AudioCaptureConfig
				{

					PWAVEFORMATEX mPWAVEFORMATEX = nullptr;

					WAVEFORMATEX mWAVEFORMATEX;

					REFERENCE_TIME mDefaultDevicePeriod;

					BOOL mFloatToShort;

					AudioCaptureConfig(){
						mPWAVEFORMATEX = nullptr;
					}
				};

				enum INVOKEMODE
				{
					NONE,
					SLEEP,
					EVENT
				};

				struct BufferContainer
				{
					IMFMediaBuffer* mPtrMediaBuffer;
				};

				IAudioClient* mAudioClient;

				IMMDevice* mPtrMMDevice;

				IAudioCaptureClient* mPtrAudioCaptureClient;

				MFTIME mPrevTime;

				MFTIME mDeltaTimeDuration;
				
				MFTIME mPrevSampleTime;
								
				std::wstring mFriendlyName;

				std::wstring mSymbolicLink;

				std::vector<AudioCaptureConfig> mVectorAudioCaptureConfigs;

				AudioCaptureConfig mCurrentAudioCaptureConfig;

				SourceState mState;

				std::mutex mAccessMutex;
				
				std::mutex mNewSampleMutex;

				std::condition_variable mNewSampleCondition;

				std::mutex mReleaseAudioClientMutex;

				std::condition_variable mReleaseAudioClientCondition;

				bool mReleaseAudioClientLock;

				LONGLONG mSampleDuration;

				DWORD mSilenceDuration;
															
				UINT32 mExpectedBufferSize;

				UINT32 mWrittenBufferSize;

				UINT32 mBlockAlign;
				
				DWORD mMillTickTime;
				
				std::queue<BufferContainer> mBufferQueue;

				std::mutex mAccessBufferQueueMutex;

				CComPtrCustom<IMFMediaBuffer> mCurrentMediaBuffer;

				UINT32 mBufferOffset;

				BOOL mSilenceBlock;
				BOOL mIsSilenceBlock;

				HANDLE mShutdownEvent;
				HANDLE mAudioSamplesReadyEvent;
				HANDLE mWaitArray[2];

				INVOKEMODE mInvokeMode;


				HRESULT allocateBuffer();

								
				HRESULT createAudioMediaType(
					AudioCaptureConfig aAudioCaptureConfig,
					IMFMediaType** aPtrPtrMediaType);

				HRESULT fillVectorAudioCaptureConfigs(
					IMMDevice* aPtrMMDevice,
					std::vector<AudioCaptureConfig>& aVectorAudioCaptureConfigs);

				HRESULT initializeAudioClient();

				HRESULT releaseAudioClient();

				void writeAudioBuffer(
					BYTE* aPtrData,
					UINT32 aBufferByteLength);

				bool isUninitialized();

				void pushMediaBuffer(
					IMFMediaBuffer* aPtrMediaBuffer);

				bool popMediaBuffer(
					IMFMediaBuffer** aPtrPtrMediaBuffer);

				void clearMediaBuffer();
				
				HRESULT STDMETHODCALLTYPE checkShutdown() const;

				virtual ~AudioEndpointCaptureProcessor();

			};
		}
	}
}