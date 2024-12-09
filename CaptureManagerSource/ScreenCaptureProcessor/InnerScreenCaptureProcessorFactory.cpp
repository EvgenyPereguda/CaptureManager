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

#include "InnerScreenCaptureProcessorFactory.h"
#include "InnerScreenCaptureProcessor.h"
#include "InnerScreenCaptureProcessorLandscape.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"
#include <memory>


namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			InnerScreenCaptureProcessorFactory::InnerScreenCaptureProcessorFactory()
			{
			}
			
			InnerScreenCaptureProcessorFactory::~InnerScreenCaptureProcessorFactory()
			{
			}

			long InnerScreenCaptureProcessorFactory::create(
				Type aType,
				IDXGIOutput1* aPtrOutput, 
				IInnerScreenCaptureProcessor** aPtrPtrIInnerScreenCaptureProcessor)
			{
				HRESULT lresult(E_FAIL);

				CComPtrCustom<IInnerScreenCaptureProcessor> lIInnerScreenCaptureProcessor;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrOutput);

					switch (aType)
					{
					case CaptureManager::Sources::ScreenCapture::InnerScreenCaptureProcessorFactory::Landscape:
					{
						CComPtrCustom<InnerScreenCaptureProcessorLandscape> lInnerScreenCaptureProcessorLandscape(new (std::nothrow) InnerScreenCaptureProcessorLandscape);

						LOG_CHECK_PTR_MEMORY(lInnerScreenCaptureProcessorLandscape);

						LOG_INVOKE_POINTER_METHOD(lInnerScreenCaptureProcessorLandscape, init, aPtrOutput);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lInnerScreenCaptureProcessorLandscape, &lIInnerScreenCaptureProcessor);
					}
						break;
					case CaptureManager::Sources::ScreenCapture::InnerScreenCaptureProcessorFactory::Regular:
					default:

						CComPtrCustom<InnerScreenCaptureProcessor> lInnerScreenCaptureProcessor(new (std::nothrow) InnerScreenCaptureProcessor);

						LOG_CHECK_PTR_MEMORY(lInnerScreenCaptureProcessor);

						LOG_INVOKE_POINTER_METHOD(lInnerScreenCaptureProcessor, init, aPtrOutput);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lInnerScreenCaptureProcessor, &lIInnerScreenCaptureProcessor);

						break;
					}

					LOG_CHECK_PTR_MEMORY(lIInnerScreenCaptureProcessor);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lIInnerScreenCaptureProcessor, aPtrPtrIInnerScreenCaptureProcessor);

				} while (false);


				return lresult;
			}
		}
	}
}
