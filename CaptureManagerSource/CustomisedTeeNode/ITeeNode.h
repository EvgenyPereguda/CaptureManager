#pragma once
#include "Unknwnbase.h"

struct IMFSample;

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			MIDL_INTERFACE("C9FAA677-5742-4A04-8FB8-67006E0C1669")
			ITeeNode : public IUnknown
			{
			public:

				virtual HRESULT registerOutputNodeRequest(
					DWORD aOutputNodeIndex) = 0;

				virtual HRESULT processInput(
					IMFSample* aPtrSample,
					std::vector<DWORD>& aRefVectorOfOutputNodesIndexes) = 0;

				virtual HRESULT reset() = 0;
			};
		}
	}
}