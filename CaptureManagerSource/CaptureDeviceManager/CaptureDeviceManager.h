#pragma once

#include <Unknwnbase.h>

#include "../PugiXML/pugixml.hpp"
#include "MediaSourceParser.h"


struct IMFMediaSource;
struct IMFAttributes;

namespace CaptureManager
{
	namespace Core
	{
		class CaptureDeviceManager : 
			protected MediaSourceParser
		{
		public:

			static HRESULT getXMLDocOfSources(
				pugi::xml_node& aRefRoolXML_Node);

			static HRESULT getSource(
				std::wstring& aSymbolicLink, 
				IMFMediaSource** aPtrPtrMediaSource);

			static HRESULT addDeviceInstanceLink(
				pugi::xml_node& aRefSourceNode);

		private:
			CaptureDeviceManager() = delete;
			~CaptureDeviceManager() = delete;
			CaptureDeviceManager(const CaptureDeviceManager&) = delete;
			CaptureDeviceManager& operator=(const CaptureDeviceManager&) = delete;


			static HRESULT enumSources(
				IMFAttributes* aAttributes,
				pugi::xml_node& aRefRoolXML_Node);

			static HRESULT getDeviceInstanceLink(
				pugi::xpath_node& aRefAttributeNode,
				std::wstring& aRefDeviceInstanceLink);

			static HRESULT getAudioHaradwareLink(
				pugi::xpath_node& aRefAttributeNode,
				std::wstring& aRefDeviceInstanceLink);
			
		};
	}
}