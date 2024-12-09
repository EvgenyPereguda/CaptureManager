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

#include <string>
#include <algorithm>


#include "SampleGrabberCallbackSinkFactory.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/MFHeaders.h"
#include "../Common/InstanceMaker.h"
#include "SinkCollection.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Singleton.h"
#include "../CaptureManagerBroker/ISampleGrabberCallbackInner.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"

namespace CaptureManager
{
	namespace COMServer
	{		
		static const Core::InstanceMaker<SampleGrabberCallbackSinkFactory, Singleton<SinkCollection>> staticInstanceMaker(
			SampleGrabberCallbackSinkFactory::getGUIDToNamePair);

		GUIDToNamePair SampleGrabberCallbackSinkFactory::getGUIDToNamePair()
		{
			GUIDToNamePair lGUIDToNamePair;

			lGUIDToNamePair.mGUID = __uuidof(ISampleGrabberCallbackSinkFactory);

			lGUIDToNamePair.mName = L"SampleGrabberCallbackSinkFactory";

			lGUIDToNamePair.mTitle = L"Sample grabber callback sink factory";

			getReadModes(lGUIDToNamePair.mContainers);

			return lGUIDToNamePair;
		}

		void SampleGrabberCallbackSinkFactory::getReadModes(
			std::vector<GUIDToNamePair>& aRefReadModes)
		{


			GUIDToNamePair lGUIDFormat;

			lGUIDFormat.mSinkTypes.push_back(SinkType::SampleGrabberCall);

			lGUIDFormat.mGUID =
			{ 0x3c9f1c2e, 0x23, 0x4861, { 0x8b, 0xd8, 0xc6, 0xde, 0xd2, 0x20, 0xe9, 0x4d } };

			lGUIDFormat.mName = L"ASYNC";

			lGUIDFormat.mMaxPortCount = 1;

			lGUIDFormat.mTitle = L"Grabbing without blocking of call thread";

			aRefReadModes.push_back(lGUIDFormat);
		}

		// ISampleGrabberCallbackSinkFactory interface implementation

		STDMETHODIMP SampleGrabberCallbackSinkFactory::createOutputNode(
			REFGUID aRefMajorType,
			REFGUID aRefSubType,
			IUnknown *aPtrISampleGrabberCallback,
			IUnknown **aPtrPtrTopologyNode)
		{
			class Proxy :
				public BaseUnknown<ISampleGrabberCallbackInner>
			{
			public:
				Proxy(ISampleGrabberCallback* aISampleGrabberCallback)
				{
					mISampleGrabberCallback = aISampleGrabberCallback;
				}
				virtual ~Proxy(){}

				virtual void operator()(
					REFGUID aGUIDMajorMediaType,
					DWORD aSampleFlags,
					LONGLONG aSampleTime,
					LONGLONG aSampleDuration,
					const BYTE* aPtrSampleBuffer,
					DWORD aSampleSize)
				{
					mISampleGrabberCallback->invoke(
						aGUIDMajorMediaType,
						aSampleFlags,
						aSampleTime,
						aSampleDuration,
						(LPVOID)aPtrSampleBuffer,
						aSampleSize);
				}

			private:

				CComPtrCustom<ISampleGrabberCallback> mISampleGrabberCallback;
			};

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrISampleGrabberCallback);

				LOG_CHECK_PTR_MEMORY(aPtrPtrTopologyNode);
				
				CComQIPtrCustom<ISampleGrabberCallback> lISampleGrabberCallback = aPtrISampleGrabberCallback;

				LOG_CHECK_PTR_MEMORY(lISampleGrabberCallback);
				
				CComQIPtrCustom<ISampleGrabberCallbackInner> lISampleGrabberCallbackInner

					= new (std::nothrow)Proxy(lISampleGrabberCallback);

				LOG_CHECK_PTR_MEMORY(lISampleGrabberCallbackInner);
				
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createSampleGrabberOutputNode,
					aRefMajorType,
					aRefSubType,
					lISampleGrabberCallbackInner,
					aPtrPtrTopologyNode);

			} while (false);

			return lresult;
		}


		// IContainer interface implementation

		STDMETHODIMP SampleGrabberCallbackSinkFactory::setContainerFormat(
			REFGUID aRefContainerTypeGUID)
		{
			HRESULT lresult;

			do
			{
				lresult = S_OK;

			} while (false);

			return lresult;
		}
	}
}