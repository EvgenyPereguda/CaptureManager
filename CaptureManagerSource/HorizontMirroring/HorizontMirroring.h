#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"

namespace CaptureManager
{
	namespace Transform
	{
		namespace Image
		{
			class HorizontMirroring : 
				public BaseUnknown<IMFTransform>
			{
			public:
				HorizontMirroring(
					const GUID aProcessedSubType);
				
				virtual ~HorizontMirroring();


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
				
			private:

				std::condition_variable mConditionVariable;

				std::mutex mMutex;

				const GUID mProcessedSubType;

				CComPtrCustom<IMFMediaType> mInputMediaType;

				CComPtrCustom<IMFMediaType> mOutputMediaType;

				CComPtrCustom<IMFTransform> mSampleCopierMFT;

				CComPtrCustom<IMFSample>  mSample;

				bool g = true;


				HRESULT createMediaType(
					IMFMediaType** aPtrPtrMediaType);

				HRESULT checkMediaType(
					IMFMediaType* aPtrMediaType);

				HRESULT doHorizontMirroring(
					IMFSample* aPtrSample);

				HRESULT initOutputDataBuffer(
					IMFTransform* aPtrTransform,
					DWORD lCurrentLength,
					MFT_OUTPUT_DATA_BUFFER& aRefOutputBuffer);


				class Processor
				{
				public:
					Processor(){}
					virtual ~Processor(){}

					virtual HRESULT process(
						IMFSample* aPtrSample){ return S_OK; }

				};

				std::unique_ptr<Processor> mProcessor;
			};
		}
	}
}