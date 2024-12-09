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

#include "../LogPrintOut/LogPrintOut.h"
#include "SinkCollection.h"
#include "../Common/IMaker.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/InstanceMaker.h"


namespace CaptureManager
{
	namespace COMServer
	{

		SinkCollection::SinkCollection(){}

		HRESULT SinkCollection::registerInstanceMaker(
			GetGUIDToNamePair aGetGUIDToNamePair,
			const CaptureManager::Core::IMaker* aPtrMaker)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMaker);

				auto lendIter = mSinkMakers.end();

				auto lfindIter = mSinkMakers.find(aGetGUIDToNamePair);

				LOG_CHECK_STATE_DESCR((lfindIter != lendIter), E_INVALIDARG);
	
				mSinkMakers[aGetGUIDToNamePair] = aPtrMaker;

				lresult = S_OK;

			} while (false);
			
			return lresult;
		}

		HRESULT SinkCollection::createSink(
			REFGUID aRefSinkTypeGUID,
			IUnknown** aPtrPtrIUnknown)
		{
			using namespace CaptureManager::Core;

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrIUnknown);
				
				auto lendIter = mSinkMakers.end();

				auto lbeginIter = mSinkMakers.begin();

				CComPtrCustom<IUnknown> lIUnknown;

				lresult = E_BOUNDS;

				for (auto& lPair: mSinkMakers)
				{
					if (lPair.first().mGUID == aRefSinkTypeGUID)
					{
						CComPtrCustom<IUnknown> lTempIUnknown = lPair.second->make();

						lIUnknown.Swap(lTempIUnknown);

						lresult = S_OK;

						break;
					}
				}

				LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

				LOG_CHECK_PTR_MEMORY(lIUnknown);
				
				*aPtrPtrIUnknown = lIUnknown.detach();
				
				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT SinkCollection::getGUIDToNamePairs(
			std::vector<GUIDToNamePair> & aRefGUIDToNamePairs)
		{

			HRESULT lresult;

			do
			{
				aRefGUIDToNamePairs.clear();

				auto lIter = mSinkMakers.begin();

				auto lendIter = mSinkMakers.end();

				for (; lIter != lendIter; lIter++)
				{
					aRefGUIDToNamePairs.push_back(lIter->first());
				}

				lresult = S_OK;

			} while (false);
			
			return lresult;
		}

		HRESULT SinkCollection::getXMLDocumentStringListOfSinkFactories(
			std::wstringstream &aRefwstringstream)
		{
			using namespace pugi;

			HRESULT lresult;

			do
			{
				xml_document lxmlDoc;

				auto ldeclNode = lxmlDoc.append_child(node_declaration);

				ldeclNode.append_attribute(L"version") = L"1.0";

				xml_node lcommentNode = lxmlDoc.append_child(node_comment);

				lcommentNode.set_value(L"XML Document of sink factories");

				auto lRootXMLElement = lxmlDoc.append_child(L"SinkFactories");

				std::vector<GUIDToNamePair> lGUIDToNamePairs;

				LOG_INVOKE_FUNCTION(getGUIDToNamePairs,
					lGUIDToNamePairs);
				
				for (auto &lItem : lGUIDToNamePairs)
				{

					auto lOutputNode = lRootXMLElement.append_child(L"SinkFactory");

					lOutputNode.append_attribute(L"Name") = lItem.mName.c_str();

					WCHAR *lptrName = nullptr;

					LOG_INVOKE_FUNCTION(StringFromCLSID, lItem.mGUID, &lptrName);
					
					lOutputNode.append_attribute(L"GUID") = lptrName;

					CoTaskMemFree(lptrName);

					lOutputNode.append_attribute(L"Title") = lItem.mTitle.c_str();

					auto lTypesOutputNode = lOutputNode.append_child(L"Value.ValueParts");

					for (auto &lTypeItem : lItem.mContainers)
					{
						auto lTypeOutputNode = lTypesOutputNode.append_child(L"ValuePart");

						lTypeOutputNode.append_attribute(L"Title") = L"Container format";

						lTypeOutputNode.append_attribute(L"Value") = lTypeItem.mName.c_str();

						lTypeOutputNode.append_attribute(L"MIME") = lTypeItem.mMIME.c_str();

						lTypeOutputNode.append_attribute(L"Description") = lTypeItem.mTitle.c_str();

						lTypeOutputNode.append_attribute(L"MaxPortCount") = lTypeItem.mMaxPortCount;
						
						WCHAR *lptrName = nullptr;

						lresult = StringFromCLSID(lTypeItem.mGUID, &lptrName);

						if (FAILED(lresult))
							continue;

						lTypeOutputNode.append_attribute(L"GUID") = lptrName;

						CoTaskMemFree(lptrName);
					}

				}
				
				lxmlDoc.print(aRefwstringstream);

				lresult = S_OK;

			} while (false);
			
			return lresult;
		}
	}
}