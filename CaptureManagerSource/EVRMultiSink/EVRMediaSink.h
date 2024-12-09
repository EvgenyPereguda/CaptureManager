#pragma once

#include <mutex>
#include <atomic>
#include <unordered_map>


#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "IStreamSink.h"



namespace EVRMultiSink
{
	struct IPresenter;

	namespace Sinks
	{
		namespace EVR
		{
			using namespace CaptureManager;
			
			class EVRMediaSink:
				public BaseUnknown<
				IMFMediaSink,
				IMFGetService,
				IMFMediaSinkPreroll
				>
			{
			public:

				virtual bool findInterface(
					REFIID aRefIID,
					void** aPtrPtrVoidObject) override
				{
					return BaseUnknown::findInterface(
						aRefIID,
						aPtrPtrVoidObject);
				}


				static HRESULT createEVRMediaSink(
					IPresenter* aPtrPresenter,
					IMFTransform* aPtrMixer,
					DWORD aMixerStreamID,
					IMFMediaSink** aPtrPtrMediaSink);
				

				// IMFMediaSink methods
				STDMETHODIMP AddStreamSink(
					DWORD aStreamSinkIdentifier, 
					IMFMediaType* aPtrMediaType, 
					IMFStreamSink** aPtrPtrStreamSink);
				STDMETHODIMP GetCharacteristics(
					DWORD* aPtrCharacteristics);
				STDMETHODIMP GetPresentationClock(
					IMFPresentationClock** aPtrPtrPresentationClock);
				STDMETHODIMP GetStreamSinkById(
					DWORD aIdentifier, 
					IMFStreamSink** aPtrPtrStreamSink);
				STDMETHODIMP GetStreamSinkByIndex(
					DWORD aIndex, 
					IMFStreamSink** aPtrPtrStreamSink);
				STDMETHODIMP GetStreamSinkCount(
					DWORD* aPtrStreamSinkCount);
				STDMETHODIMP RemoveStreamSink(
					DWORD aStreamSinkIdentifier);
				STDMETHODIMP SetPresentationClock(
					IMFPresentationClock* aPtrPresentationClock);
				STDMETHODIMP Shutdown();
				

				// IMFGetService
				STDMETHODIMP GetService(
					REFGUID aRefGUIDService,
					REFIID aRefIID, 
					LPVOID* aPtrPtrObject);


				// IMFMediaSinkPreroll
				STDMETHODIMP NotifyPreroll(MFTIME hnsUpcomingStartTime);

			private:

				std::mutex mAccessMutex;
				bool mIsShutdown;
				CComPtrCustom<IMFPresentationClock>  mClock;
				const DWORD mStreamID;
				CComPtrCustom<IStreamSink>  mStream;
				CComPtrCustom<IMFTransform> mMixer;

				EVRMediaSink();
				virtual ~EVRMediaSink();
				HRESULT checkShutdown() const;

			};
		}
	}
}