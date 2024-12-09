#pragma once

#include <Unknwnbase.h>

#include "../PugiXML/pugixml.hpp"

struct IMFMediaSource;


namespace CaptureManager
{
	namespace Core
	{
		class MediaSourceParser
		{
		public:
			MediaSourceParser() = delete;
			~MediaSourceParser() = delete;
			MediaSourceParser(const MediaSourceParser&) = delete;
			MediaSourceParser& operator=(const MediaSourceParser&) = delete;

		protected:
			static HRESULT parse(
				IMFMediaSource* aPtrMediaSource,
				pugi::xml_node& aRefRoolXML_Node);
		};
	}
}