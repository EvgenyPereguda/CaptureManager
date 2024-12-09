#pragma once

#include <list>

typedef unsigned __int64 TOPOID;

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			struct CollectionOfIDs
			{
				std::list<TOPOID> mListOfSourceIDs;

				std::list<TOPOID> mListOfSinkIDs;
			};
		}
	}
}