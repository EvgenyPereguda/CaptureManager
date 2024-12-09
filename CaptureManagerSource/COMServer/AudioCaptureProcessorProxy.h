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
#include "CaptureManagerTypeInfo.h"
#include "InitilaizeCaptureSource.h"

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
	namespace COMServer
	{

		class AudioCaptureProcessorProxy :
			public BaseUnknown<IInnerCaptureProcessor, ISourceRequestResult>, public Core::CaptureInvoker
		{
		public:
			AudioCaptureProcessorProxy();

			// ISourceRequestResult
	
			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setData(
				/* [in] */ LPVOID aPtrData,
				/* [in] */ DWORD aByteSize,
				/* [in] */ BOOL aIsKeyFrame)override;

			virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getStreamIndex(
				/* [out] */ DWORD *aPtrStreamIndex)override;

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
			virtual HRESULT registerMediaEvent(IUnknown* pUnkIMediaEvent)override;

			HRESULT init(ICaptureProcessor* aPtrICaptureProcessor, InitilaizeCaptureSource* aPtrInitilaizeCaptureSource);

		private:


			struct BufferContainer
			{
				IMFMediaBuffer* mPtrMediaBuffer;
			};


			virtual ~AudioCaptureProcessorProxy();

			HRESULT checkShutdown() const;

			bool isUninitialized();

			void clearMediaBuffer();

			HRESULT allocateBuffer();

			void writeAudioBuffer(
				BYTE* aPtrData,
				UINT32 aBufferByteLength);

			void pushMediaBuffer(
				IMFMediaBuffer* aPtrMediaBuffer);

			bool popMediaBuffer(
				IMFMediaBuffer** aPtrPtrMediaBuffer);


			CComPtrCustom<ICaptureProcessor> mICaptureProcessor;

			CComPtrCustom<InitilaizeCaptureSource> mInitilaizeCaptureSource;

			CComPtrCustom<ISourceRequestResult> mSourceRequestResult;


			Sources::SourceState mState;

			bool mReleaseAudioClientLock;

			MFTIME mPrevTime;

			INT64 mCycleOfCapture;

			MFTIME mDeltaTimeDuration;

			MFTIME mCurrentSampleTime;
						
			UINT32 mBlockAlign;

			UINT32 mSamplesPerSec;

			UINT32 mExpectedBufferSize;

			DWORD mSleepDuration;

			DWORD mMillTickTime;

			LONGLONG mSampleDuration;
			
			UINT32 mBufferOffset;

			MFTIME mPrevSampleTime;

			std::mutex mAccessMutex;

			std::mutex mNewSampleMutex;

			std::condition_variable mNewSampleCondition;

			std::mutex mReleaseAudioClientMutex;

			std::condition_variable mReleaseAudioClientCondition;

			std::wstring mFriendlyName;

			std::wstring mSymbolicLink;

			DWORD mStreamIndex;

			std::queue<BufferContainer> mBufferQueue;

			std::mutex mAccessBufferQueueMutex;

			CComPtrCustom<IMFMediaBuffer> mCurrentMediaBuffer;

			BOOL mIsCallBack;
		};
	}
}