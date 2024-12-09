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

#include <algorithm>
#include "CustomisedTeeNode.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{


			CustomisedTeeNode::CustomisedTeeNode(
				DWORD aOutputNodeCount):
				mIsReady(false)
			{
			}
			
			CustomisedTeeNode::~CustomisedTeeNode()
			{
			}

			// ITeeNode interface

			HRESULT CustomisedTeeNode::registerOutputNodeRequest(
				DWORD aOutputNodeIndex)
			{				
				HRESULT lresult;
				
				std::lock_guard<std::mutex> lLock(mLockMutex);

				do
				{
					if (!mIsReady)
					{
						mIsReady = true;

						lresult = S_OK;

						break;
					}
					
					lresult = S_FALSE;									

				} while (false);

				return lresult;
			}

			HRESULT CustomisedTeeNode::processInput(
				IMFSample* aPtrSample,
				std::vector<DWORD>& aRefVectorOfOutputNodesIndexes)
			{
				HRESULT lresult;

				std::lock_guard<std::mutex> lLock(mLockMutex);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrSample);
					
					if (mIsReady)
					{
						mIsReady = false;
					}
					
					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT CustomisedTeeNode::reset()
			{
				std::lock_guard<std::mutex> lLock(mLockMutex);

				mIsReady = false;

				return S_OK;
			}
		}
	}
}