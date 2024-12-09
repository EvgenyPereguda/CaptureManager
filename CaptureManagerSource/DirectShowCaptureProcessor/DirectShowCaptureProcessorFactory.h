#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>


#include "../Common/IInnerCaptureProcessor.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/IMaker.h"


struct IBaseFilter;

namespace CaptureManager
{
	namespace Sources
	{
		class DirectShowCaptureProcessorFactory
		{
		public:
			DirectShowCaptureProcessorFactory();
			~DirectShowCaptureProcessorFactory();

			static HRESULT createDirectShowVideoInputCaptureProcessors(
				std::vector<std::wstring>& aUsedSymbolicLinks,
				std::vector<CComPtrCustom<IInnerCaptureProcessor>>& aVectorAudioEndpointCaptureProcessors);

			static HRESULT createDirectShowVideoInputCaptureProcessor(
				std::wstring aSymbolicLink,
				IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor);

		private:

			static HRESULT checkCrossbar(IBaseFilter* aPtrVideoInputFilter, std::unordered_map<int, std::vector<long>>& aRefOutputInputPinMap);

			static HRESULT checkVideoCapture(IBaseFilter* aPtrVideoInputFilter, std::unordered_map<int, std::vector<long>>& aRefOutputInputPinMap);

			static HRESULT checkStillImage(IBaseFilter* aPtrVideoInputFilter);

		};
	}
}