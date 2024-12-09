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

#include "CustomisedMediaPipelineProcessorFactory.h"
#include "CustomisedMediaPipelineProcessor.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			HRESULT CustomisedMediaPipelineProcessorFactory::createCustomisedMediaPipelineProcessor(
				CustomisedMediaSession* aPtrCustomisedMediaSession,
				CollectionOfIDs& aRefCollectionOfIDs,
				IMFTopology* aPtrTopology,
				IMFPresentationClock* aPtrPresentationClock,
				IMediaPipeline** aPtrPtrIMediaPipeline)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrCustomisedMediaSession);

					LOG_CHECK_PTR_MEMORY(aPtrTopology);

					LOG_CHECK_PTR_MEMORY(aPtrPresentationClock);
					
					CComPtrCustom<CustomisedMediaPipelineProcessor> 
						lLocalMediaPipeline = new (std::nothrow)CustomisedMediaPipelineProcessor(
						aPtrCustomisedMediaSession);

					LOG_CHECK_PTR_MEMORY(lLocalMediaPipeline);
					
					LOG_INVOKE_POINTER_METHOD(lLocalMediaPipeline, init,
						aRefCollectionOfIDs,
						aPtrTopology,
						aPtrPresentationClock);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lLocalMediaPipeline, aPtrPtrIMediaPipeline);
					
				} while (false);

				return lresult;
			}
		}
	}
}