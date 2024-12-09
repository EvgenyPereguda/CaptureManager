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

#include "OutputNodeFactoryCollection.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/IMaker.h"
#include "../Common/InstanceMaker.h"
#include "../Common/ComPtrCustom.h"


namespace CaptureManager
{
	namespace Sinks
	{		

		OutputNodeFactoryCollection::OutputNodeFactoryCollection(){}

		HRESULT OutputNodeFactoryCollection::registerInstanceMaker(
			GUIDToNamePair aGUIDToNamePair,
			const Core::IMaker* aPtrMaker)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMaker);				

				auto lendIter = mSinkMakers.end();

				auto lfindIter = mSinkMakers.find(aGUIDToNamePair);

				LOG_CHECK_STATE(lfindIter != lendIter);
				
				mSinkMakers[aGUIDToNamePair] = aPtrMaker;

				for (auto &litem : aGUIDToNamePair.mSinkTypes)
				{
					mSinkTypes.insert(
					{ litem, aGUIDToNamePair });
				}

				lresult = S_OK;

			} while (false);
			
			return lresult;
		}

		HRESULT OutputNodeFactoryCollection::createOutputNodeFactory(
			REFGUID aRefOutputNodeFactoryCollectionGUID,
			IUnknown** aPtrPtrIUnknown)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrIUnknown);
				
				GUIDToNamePair lGUIDToNamePair;

				lGUIDToNamePair.mGUID = aRefOutputNodeFactoryCollectionGUID;

				auto lendIter = mSinkMakers.end();

				auto lfindIter = mSinkMakers.find(lGUIDToNamePair);

				LOG_CHECK_STATE_DESCR(lfindIter == lendIter, E_BOUNDS)
				
				CComPtrCustom<IUnknown> lIUnknown = (*lfindIter).second->make();

				LOG_CHECK_PTR_MEMORY(lIUnknown);
		
				*aPtrPtrIUnknown = lIUnknown.detach();
				
				lresult = S_OK;

			} while (false);
			
			return lresult;
		}

		HRESULT OutputNodeFactoryCollection::getGUIDToNamePairs(
			SinkType aSinkType,
			std::vector<GUIDToNamePair> & aRefGUIDToNamePairs)
		{
			HRESULT lresult;

			do
			{
				aRefGUIDToNamePairs.clear();
								
				auto lIter = mSinkTypes.lower_bound(aSinkType);

				auto lendIter = mSinkTypes.upper_bound(aSinkType);

				for (; lIter != lendIter; lIter++)
				{
					aRefGUIDToNamePairs.push_back(lIter->second);
				}

				lresult = S_OK;

			} while (false);
			
			return lresult;
		}

	}
}