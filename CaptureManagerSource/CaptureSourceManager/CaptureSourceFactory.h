#pragma once

#include <Unknwnbase.h>
#include <string>
#include <vector>

struct IMFMediaSource;

struct IInnerCaptureProcessor;

namespace CaptureManager
{
	namespace Sources
	{
		class CaptureSourceFactory
		{
		public:
			
			HRESULT getCaptureSources(
				std::vector<std::wstring>& aUsedSymbolicLinks,
				IMFMediaSource ***aPtrPtrPtrSources, 
				UINT32 &aCount);

			HRESULT getSource(
				std::wstring& aSymbolicLink, 
				IMFMediaSource** aPtrPtrMediaSource);

			HRESULT createSource(
				IInnerCaptureProcessor* aPtrCaptureProcessor,
				IMFMediaSource** aPtrPtrMediaSource);

		protected:
			CaptureSourceFactory();
			~CaptureSourceFactory();

		private:
			CaptureSourceFactory(const CaptureSourceFactory&) = delete;
			CaptureSourceFactory& operator=(const CaptureSourceFactory&) = delete;
		};

	}
}