#pragma once


#include "../Common/IInnerCaptureProcessor.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/IMaker.h"

#include <map>
#include <memory>
#include <vector>
#include <string>

namespace CaptureManager
{
	namespace Sources
	{
		class ScreenCaptureProcessorFactory
		{
		public:
			
			static ScreenCaptureProcessorFactory& getInstance();

			HRESULT registerInstanceMaker(
				const int aPriorety, 
				const CaptureManager::Core::IMaker* aPtrMaker);

			HRESULT createScreenCaptureProcessors(
				std::vector<CComPtrCustom<IInnerCaptureProcessor>>& aVectorAudioEndpointCaptureProcessors);

			HRESULT createScreenCaptureProcessor(
				std::wstring aSymbolicLink,
				IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor);

		private:

			std::map<int, const CaptureManager::Core::IMaker* > mScreenCaptureProcessorMakers;

			int mMinPriorety;

			int mMaxPriorety;



			ScreenCaptureProcessorFactory();
			~ScreenCaptureProcessorFactory();
			ScreenCaptureProcessorFactory(
				const ScreenCaptureProcessorFactory&) = delete;
			ScreenCaptureProcessorFactory& operator=(
				const ScreenCaptureProcessorFactory&) = delete;
		};
	}
}