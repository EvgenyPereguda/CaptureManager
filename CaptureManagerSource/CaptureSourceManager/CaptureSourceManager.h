#pragma once

#include <Unknwnbase.h>
#include <vector>

#include "../PugiXML/pugixml.hpp"
#include "../CaptureDeviceManager/MediaSourceParser.h"

struct IMFMediaSource;

struct IInnerCaptureProcessor;

namespace CaptureManager
{
	namespace Core
	{
		class CaptureSourceManager:
			protected MediaSourceParser
		{
		public:
			
			static HRESULT getXMLDocOfSources(
				std::vector<std::wstring>& aUsedSymbolicLinks,
				pugi::xml_node& aRefRoolXML_Node);

			static HRESULT getSource(
				std::wstring& aSymbolicLink, 
				IMFMediaSource** aPtrPtrMediaSource);

			static HRESULT createSource(
				IInnerCaptureProcessor* aPtrCaptureProcessor,
				IMFMediaSource** aPtrPtrMediaSource);

		private:
			CaptureSourceManager() = delete;
			~CaptureSourceManager() = delete;
			CaptureSourceManager(const CaptureSourceManager&) = delete;
			CaptureSourceManager& operator=(const CaptureSourceManager&) = delete;
		};
	}
}