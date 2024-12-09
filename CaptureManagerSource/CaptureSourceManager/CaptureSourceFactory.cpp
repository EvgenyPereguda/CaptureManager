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

#include "CaptureSourceFactory.h"
#include "../Common/MFHeaders.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/ComMassivPtr.h"
#include "../Common/Common.h"
#include "../DataParser/DataParser.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../AudioEndpointCapture/AudioEndpointCaptureProcessorFactory.h"
#include "../ScreenCaptureProcessor/ScreenCaptureProcessorFactory.h"
#include "../DirectShowCaptureProcessor/DirectShowCaptureProcessorFactory.h"
#include "../CaptureSource/CaptureSource.h"

namespace CaptureManager
{
	namespace Sources
	{
		CaptureSourceFactory::CaptureSourceFactory()
		{
		}
		
		HRESULT CaptureSourceFactory::getCaptureSources(
			std::vector<std::wstring>& aUsedSymbolicLinks, 
			IMFMediaSource ***aPtrPtrPtrSources, 
			UINT32 &aCount)
		{
			using namespace CaptureManager::Sources::AudioEndpointCapture;
						
			HRESULT lresult;

			try
			{
				do
				{


					std::vector<CComPtrCustom<IInnerCaptureProcessor>> lVectorDirectShowCaptureProcessors;

					lresult = DirectShowCaptureProcessorFactory::createDirectShowVideoInputCaptureProcessors(
						aUsedSymbolicLinks,
						lVectorDirectShowCaptureProcessors);

					if (FAILED(lresult))
					{
						lresult = S_OK;

						lVectorDirectShowCaptureProcessors.clear();
					}

					std::vector<CComPtrCustom<IInnerCaptureProcessor>> lVectorAudioEndpointCaptureProcessors;

					lresult = AudioEndpointCaptureProcessorFactory::createAudioEndpointCaptureProcessors(
						lVectorAudioEndpointCaptureProcessors);

					if (FAILED(lresult))
					{
						lresult = S_OK;

						lVectorAudioEndpointCaptureProcessors.clear();
					}

					std::vector<CComPtrCustom<IInnerCaptureProcessor>> lVectorScreenCaptureProcessors;

					lresult = ScreenCaptureProcessorFactory::getInstance().createScreenCaptureProcessors(
						lVectorScreenCaptureProcessors);

					if (FAILED(lresult))
					{
						lresult = S_OK;

						lVectorScreenCaptureProcessors.clear();
					}									

					auto lCount = lVectorDirectShowCaptureProcessors.size();

					lCount += lVectorScreenCaptureProcessors.size();

					lCount += lVectorAudioEndpointCaptureProcessors.size();

					UINT32 lCaptureSourceIndex = 0;

					auto lPtrVoid = CoTaskMemAlloc(sizeof(IMFMediaSource)* lCount);

					LOG_CHECK_PTR_MEMORY(lPtrVoid);
					
					IMFMediaSource ** lSources = (IMFMediaSource **)(lPtrVoid);

					

					for (size_t lCaptureProcessorIndex = 0; lCaptureProcessorIndex < lVectorDirectShowCaptureProcessors.size(); lCaptureProcessorIndex++)
					{
						CComPtrCustom<CaptureSource> lSource = new(std::nothrow) CaptureSource();

						LOG_CHECK_PTR_MEMORY(lSource);

						LOG_INVOKE_POINTER_METHOD(lSource, init, 
							lVectorDirectShowCaptureProcessors[lCaptureProcessorIndex],
							TRUE);

						lSources[lCaptureSourceIndex++] = lSource.detach();

					}

					for (size_t lScreenCaptureProcessorIndex = 0; lScreenCaptureProcessorIndex < lVectorScreenCaptureProcessors.size(); lScreenCaptureProcessorIndex++)
					{
						CComPtrCustom<CaptureSource> lSource = new(std::nothrow) CaptureSource();

						LOG_CHECK_PTR_MEMORY(lSource);
						
						LOG_INVOKE_POINTER_METHOD(lSource, init, lVectorScreenCaptureProcessors[lScreenCaptureProcessorIndex]);

						lSources[lCaptureSourceIndex++] = lSource.detach();

					}
					
					for (size_t lAudioEndpointIndex = 0; lAudioEndpointIndex < lVectorAudioEndpointCaptureProcessors.size(); lAudioEndpointIndex++)
					{
						CComPtrCustom<CaptureSource> lSource = new(std::nothrow) CaptureSource();

						LOG_CHECK_PTR_MEMORY(lSource);
						
						LOG_INVOKE_POINTER_METHOD(lSource, init, lVectorAudioEndpointCaptureProcessors[lAudioEndpointIndex]);
						
						lSources[lCaptureSourceIndex++] = lSource.detach();
					}

					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);
					
					*aPtrPtrPtrSources = lSources;

					aCount = lCount;

					lresult = S_OK;

				} while (false);

			}
			catch (...)
			{
				*aPtrPtrPtrSources = nullptr;

				aCount = 0;
			}

			return lresult;
		}

		HRESULT CaptureSourceFactory::getSource(std::wstring& aRefSymbolicLink, IMFMediaSource** aPtrPtrMediaSource)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				auto lSourcePos = aRefSymbolicLink.find(L"DirectShowCapture///");

				if (lSourcePos != std::wstring::npos)
				{
					auto lmodifyStart = aRefSymbolicLink.find(L" --");

					if (lmodifyStart != std::wstring::npos)
					{
						aRefSymbolicLink = aRefSymbolicLink.substr(0, lmodifyStart);
					}

					auto lDirectShowCaptureSymbolicLink = aRefSymbolicLink.substr(lSourcePos);

					CComPtrCustom<IInnerCaptureProcessor> lCaptureProcessor;

					LOG_INVOKE_FUNCTION(DirectShowCaptureProcessorFactory::createDirectShowVideoInputCaptureProcessor,
						lDirectShowCaptureSymbolicLink,
						&lCaptureProcessor);

					CComPtrCustom<CaptureSource> lCaptureSource = new(std::nothrow) CaptureSource();

					LOG_CHECK_PTR_MEMORY(lCaptureSource);

					LOG_INVOKE_POINTER_METHOD(lCaptureSource, init, lCaptureProcessor.detach());

					*aPtrPtrMediaSource = lCaptureSource.detach();
										
					break;
				}



				lSourcePos = aRefSymbolicLink.find(L"AudioEndpointCapture///");

				if (lSourcePos != std::wstring::npos)
				{
					auto lmodifyStart = aRefSymbolicLink.find(L" --");

					if (lmodifyStart != std::wstring::npos)
					{
						aRefSymbolicLink = aRefSymbolicLink.substr(0, lmodifyStart);
					}

					auto lAudioEndpointCaptureSymbolicLink = aRefSymbolicLink.substr(lSourcePos);

					CComPtrCustom<IInnerCaptureProcessor> lCaptureProcessor;
					
					LOG_INVOKE_FUNCTION(AudioEndpointCapture::AudioEndpointCaptureProcessorFactory::createAudioEndpointCaptureProcessor,
						lAudioEndpointCaptureSymbolicLink,
						&lCaptureProcessor);
					
					CComPtrCustom<CaptureSource> lCaptureSource = new(std::nothrow) CaptureSource();

					LOG_CHECK_PTR_MEMORY(lCaptureSource);
					
					LOG_INVOKE_POINTER_METHOD(lCaptureSource, init, lCaptureProcessor.detach());

					*aPtrPtrMediaSource = lCaptureSource.detach();
					
					break;
				}

				lSourcePos = aRefSymbolicLink.find(L"ScreenCapture///");

				if (lSourcePos != std::wstring::npos)
				{
					CComPtrCustom<IInnerCaptureProcessor> lCaptureProcessor;

					LOG_INVOKE_FUNCTION(ScreenCaptureProcessorFactory::getInstance().createScreenCaptureProcessor,
						aRefSymbolicLink,
						&lCaptureProcessor);
					
					LOG_CHECK_PTR_MEMORY(lCaptureProcessor);
					
					CComPtrCustom<CaptureSource> lCaptureSource = new(std::nothrow) CaptureSource();

					LOG_CHECK_PTR_MEMORY(lCaptureSource);
					
					LOG_INVOKE_POINTER_METHOD(lCaptureSource, init, lCaptureProcessor);
					
					*aPtrPtrMediaSource = lCaptureSource.detach();

					lresult = S_OK;

					break;
				}
				
			} while (false);
			
			return lresult;
		}

		HRESULT CaptureSourceFactory::createSource(
			IInnerCaptureProcessor* aPtrCaptureProcessor,
			IMFMediaSource** aPtrPtrMediaSource)
		{
			HRESULT lresult;

			do
			{

				LOG_CHECK_PTR_MEMORY(aPtrPtrMediaSource);
				
				LOG_CHECK_PTR_MEMORY(aPtrCaptureProcessor);				
				
				CComPtrCustom<IMFMediaSource> lMediaSource;

				CComPtrCustom<IInnerCaptureProcessor> lIInnerCaptureProcessor;

				lIInnerCaptureProcessor = aPtrCaptureProcessor;

				CComPtrCustom<CaptureSource> lCaptureSource = new(std::nothrow) CaptureSource();

				LOG_CHECK_PTR_MEMORY(lCaptureSource);
				
				LOG_INVOKE_POINTER_METHOD(lCaptureSource, init, lIInnerCaptureProcessor);
				
				LOG_INVOKE_QUERY_INTERFACE_METHOD(lCaptureSource, &lMediaSource);
				
				(*aPtrPtrMediaSource) = lMediaSource.detach();

			} while (false);

			return lresult;
		}

		CaptureSourceFactory::~CaptureSourceFactory()
		{
		}
	}
}