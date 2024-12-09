#pragma once

#include <vector>
#include <mutex>

#include "../Common/BaseUnknown.h"
#include "ITeeNode.h"

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{			
			class CustomisedTeeNode :
				public BaseUnknown<ITeeNode>
			{
			public:
				CustomisedTeeNode(
					DWORD aOutputNodeCount);

				int index;

				// ITeeNode interface

				virtual HRESULT registerOutputNodeRequest(
					DWORD aOutputNodeIndex) override;

				virtual HRESULT processInput(
					IMFSample* aPtrSample,
					std::vector<DWORD>& aRefVectorOfOutputNodesIndexes) override;

				virtual HRESULT reset() override;

			private:

				std::mutex mLockMutex;
				
				bool mIsReady;

				virtual ~CustomisedTeeNode();
			};
		}
	}
}