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

#include "SampleGrabberCallFactory.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/MFHeaders.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../CaptureManagerBroker/CaptureManagerConstants.h"
#include "ReadWriteBufferRegularSync.h"
#include "ReadWriteBufferRegularAsync.h"
#include "SampleGrabberSink.h"
#include "SampleGrabberGemstone.h"
#include "SampleGrabberPullSink.h"
#include "SampleGrabberPullStream.h"
#include "../Common/CustomisedActivate.h"

namespace CaptureManager
{
	namespace Sinks
	{

		using namespace Core;
		using namespace SampleGrabberCall;

		SampleGrabberCallFactory::SampleGrabberCallFactory()
		{
		}

		SampleGrabberCallFactory::~SampleGrabberCallFactory()
		{
		}

		HRESULT SampleGrabberCallFactory::createSampleGrabberOutputNode(
			REFGUID aRefMajorType,
			REFGUID aRefSubType,
			std::wstring& aReadMode,
			LONG aSampleByteSize,
			ISampleGrabberCallInner** aPtrPtrISampleGrabber)
		{
			using namespace RegularSampleGrabberCall;
			
			HRESULT lresult;

			do
			{
				LOG_CHECK_STATE(aSampleByteSize <= 0);
			 
				LOG_CHECK_PTR_MEMORY(aPtrPtrISampleGrabber);
		

				CComPtrCustom<IMFActivate> lSampleGrabberSinkActivate;

				CComPtrCustom<IMFMediaType> lMediaType;

				CComPtrCustom<IMFTopologyNode> lSinkNode;

				LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
					&lMediaType);

				LOG_CHECK_PTR_MEMORY(lMediaType);

				LOG_INVOKE_MF_METHOD(SetGUID, lMediaType, MF_MT_MAJOR_TYPE, aRefMajorType);
				
				LOG_INVOKE_MF_METHOD(SetGUID, lMediaType, MF_MT_SUBTYPE, aRefSubType);
				
				CComPtrCustom<IWrite> lWriteBuffer;
				
				lresult = S_OK;


				if ((aReadMode == L"SYNC") || (aReadMode == L"ASYNC"))
				{
					if (aReadMode == L"ASYNC")
					{
						CComPtrCustom<IWrite> lBuffer = new(std::nothrow) ReadWriteBufferRegularAsync(aSampleByteSize);

						lWriteBuffer = lBuffer;
					}
					else if (aReadMode == L"SYNC")
					{
						std::unique_ptr<ReadWriteBufferRegularSync> lSync(new(std::nothrow) ReadWriteBufferRegularSync());

						LOG_CHECK_PTR_MEMORY(lSync.get());

						LOG_INVOKE_POINTER_METHOD(lSync, init, aSampleByteSize);
						
						LOG_INVOKE_QUERY_INTERFACE_METHOD(lSync, &lWriteBuffer);

						lSync->Release();

						lSync.release();
					}
					else
					{
						lresult = E_BOUNDS;
					}
					
					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

					LOG_CHECK_STATE_DESCR(lWriteBuffer == nullptr, E_OUTOFMEMORY);

					CComPtrCustom<SampleGrabberSink<IWrite>> lSinkCallback(new(std::nothrow) SampleGrabberSink<IWrite>(lWriteBuffer));
					
					LOG_CHECK_STATE_DESCR(lSinkCallback == nullptr, E_OUTOFMEMORY);
					
					LOG_INVOKE_MF_FUNCTION(MFCreateSampleGrabberSinkActivate,
						lMediaType,
						lSinkCallback,
						&lSampleGrabberSinkActivate);
				}
				else if (aReadMode == L"PULL")
				{
					CComPtrCustom<Sinks::SampleGrabberPull::SampleGrabberPullSink> lSampleGrabberPullSink(
						new (std::nothrow) Sinks::SampleGrabberPull::SampleGrabberPullSink());

					LOG_CHECK_PTR_MEMORY(lSampleGrabberPullSink);

					CComPtrCustom<SampleGrabberPull::SampleGrabberPullStream> lSampleGrabberPullStream(
						new(std::nothrow) SampleGrabberPull::SampleGrabberPullStream());

					LOG_CHECK_PTR_MEMORY(lSampleGrabberPullStream);

					LOG_INVOKE_POINTER_METHOD(lSampleGrabberPullStream, init, lSampleGrabberPullSink, lMediaType);

					CComPtrCustom<ReadWriteBufferRegularSync> lSync;

					lSync = lSampleGrabberPullStream;

					LOG_CHECK_PTR_MEMORY(lSync);

					LOG_INVOKE_POINTER_METHOD(lSync, init, aSampleByteSize);

					lWriteBuffer = lSync;

					LOG_INVOKE_POINTER_METHOD(lSampleGrabberPullSink, setStream,
						lWriteBuffer);
					
					lSampleGrabberSinkActivate = new (std::nothrow)
						Activates::CustomisedActivate::CustomisedActivate(lSampleGrabberPullSink);

					if (!lSampleGrabberSinkActivate)
					{
						lresult = E_OUTOFMEMORY;

						break;
					}

					lSampleGrabberSinkActivate->Release();
				}
				else
				{				
					lresult = E_BOUNDS;
				}

				LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);
					
				LOG_CHECK_STATE_DESCR(lSampleGrabberSinkActivate == nullptr, E_OUTOFMEMORY);
				
				LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
					MF_TOPOLOGY_OUTPUT_NODE,
					&lSinkNode);
				
				LOG_INVOKE_MF_METHOD(SetObject,
					lSinkNode,
					lSampleGrabberSinkActivate);
				
				LOG_INVOKE_MF_METHOD(SetUINT32,
					lSinkNode,
					MF_TOPONODE_STREAMID,
					0);

				LOG_INVOKE_MF_METHOD(SetUINT32,
					lSinkNode,
					MF_TOPONODE_NOSHUTDOWN_ON_REMOVE,
					FALSE);

				LOG_INVOKE_MF_METHOD(SetUINT32,
					lSinkNode,
					MF_MT_SAMPLEGRABBER,
					TRUE);

				CComPtrCustom<IRead> lReadBuffer;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lWriteBuffer, &lReadBuffer);
				
				CComPtrCustom<SampleGrabberGemstone> lSampleGrabberGemstone(new(std::nothrow) SampleGrabberGemstone(
					lSinkNode,
					lReadBuffer));

				LOG_CHECK_PTR_MEMORY(lSampleGrabberGemstone);
				
				*aPtrPtrISampleGrabber = lSampleGrabberGemstone.Detach();

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					L"CaptureManager: createSampleGrabberOutputNode is not executable!!! ",
					L" Error code: ",
					(HRESULT)lresult);
			}

			return lresult;
		}

	}
}