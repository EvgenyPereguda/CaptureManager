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

#include "ASFOutputNodeFactory.h"
#include "../Common/InstanceMaker.h"
#include "../Common/ComPtrCustom.h"
#include "OutputNodeFactoryCollection.h"
#include "ASFSinkManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"

namespace CaptureManager
{
	namespace Sinks
	{
		namespace OutputNodeFactory
		{

			static const Core::InstanceMaker<ASFOutputNodeFactory, Singleton<OutputNodeFactoryCollection>> staticInstanceMaker(
				ASFOutputNodeFactory::getGUIDToNamePair());

			GUIDToNamePair ASFOutputNodeFactory::getGUIDToNamePair()
			{
				GUIDToNamePair lGUIDToNamePair;

				lGUIDToNamePair.mGUID =
				{ 0xa2a56da1, 0xeb84, 0x460e, { 0x9f, 0x5, 0xfe, 0xe5, 0x1d, 0x8c, 0x81, 0xe3 } };

				lGUIDToNamePair.mName = L"ASF";

				lGUIDToNamePair.mMaxPortCount = 126;

				lGUIDToNamePair.mTitle = L"ASF Media Container";

				lGUIDToNamePair.mMIME = L"video/x-ms-asf";

				lGUIDToNamePair.mSinkTypes.push_back(SinkType::File);

				lGUIDToNamePair.mSinkTypes.push_back(SinkType::ByteStream);

				return lGUIDToNamePair;
			}

			// IOutputNodeFactory interface inplementation

			STDMETHODIMP ASFOutputNodeFactory::createOutputNodes(
				VARIANT aArrayPtrCompressedMediaTypes,
				BSTR aFileName,
				VARIANT* aArrayPtrTopologyOutputNodes)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_STATE(!(aArrayPtrCompressedMediaTypes.vt & VT_UNKNOWN) ||
						!(aArrayPtrCompressedMediaTypes.vt & VT_SAFEARRAY));
					
					LOG_CHECK_PTR_MEMORY(aArrayPtrTopologyOutputNodes);

					SAFEARRAY* lPtrSAMediaTypes;

					lPtrSAMediaTypes = aArrayPtrCompressedMediaTypes.parray;

					LONG lBoundMediaTypes(0);

					LONG uBoundMediaTypes(0);

					LOG_INVOKE_FUNCTION(SafeArrayGetUBound, lPtrSAMediaTypes, 1, &uBoundMediaTypes);

					LOG_INVOKE_FUNCTION(SafeArrayGetLBound, lPtrSAMediaTypes, 1, &lBoundMediaTypes);

					std::vector<IUnknown*> lCompressedMediaTypes;

					for (LONG lIndex = lBoundMediaTypes; lIndex <= uBoundMediaTypes; lIndex++)
					{
						VARIANT lVar;
						
						auto lr = SafeArrayGetElement(lPtrSAMediaTypes, &lIndex, &lVar);
						
						if(SUCCEEDED(lr) && lVar.vt == VT_UNKNOWN && lVar.punkVal != nullptr)
						{
							lCompressedMediaTypes.push_back(lVar.punkVal);
						}

						if (SUCCEEDED(lr))
							VariantClear(&lVar);
					}

					std::vector<CComPtrCustom<IUnknown>> lTopologyASFOutputNodes;
					
					LOG_INVOKE_FUNCTION(ASF::ASFSinkManager::createASFOutputNodes,
						std::wstring(aFileName),
						lCompressedMediaTypes,
						lTopologyASFOutputNodes);
					
					SAFEARRAY* lPtrSAOutputNodes;

					SAFEARRAYBOUND bound[1];

					bound[0].lLbound = 0;

					bound[0].cElements = lTopologyASFOutputNodes.size();

					lPtrSAOutputNodes = SafeArrayCreate(VT_VARIANT, 1, bound);

					LONG lCount = lTopologyASFOutputNodes.size();

					for (LONG lindex = 0;
						lindex < lCount;
						lindex++)
					{
						IUnknown *lPtrIUnknown;

						lTopologyASFOutputNodes[lindex]->QueryInterface(IID_PPV_ARGS(&lPtrIUnknown));


						VARIANT *lVar = new VARIANT;

						VariantInit(lVar);

						lVar->vt = VT_UNKNOWN;

						lVar->punkVal = lPtrIUnknown;

						do
						{
							LOG_INVOKE_FUNCTION(SafeArrayPutElement, lPtrSAOutputNodes, &lindex, lVar);

						} while (false);

						VariantClear(lVar);

						delete lVar;
					}

					aArrayPtrTopologyOutputNodes->vt = aArrayPtrCompressedMediaTypes.vt;

					aArrayPtrTopologyOutputNodes->parray = lPtrSAOutputNodes;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			STDMETHODIMP ASFOutputNodeFactory::createOutputNodes(
				VARIANT aArrayPtrCompressedMediaTypes,
				IUnknown *aPtrByteStreamActivate,
				VARIANT *aArrayPtrTopologyOutputNodes)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_STATE(!(aArrayPtrCompressedMediaTypes.vt & VT_UNKNOWN) ||
						!(aArrayPtrCompressedMediaTypes.vt & VT_SAFEARRAY));

					LOG_CHECK_PTR_MEMORY(aPtrByteStreamActivate);

					LOG_CHECK_PTR_MEMORY(aArrayPtrTopologyOutputNodes);

					SAFEARRAY* lPtrSAMediaTypes;

					lPtrSAMediaTypes = aArrayPtrCompressedMediaTypes.parray;

					LONG lBoundMediaTypes;

					LONG uBoundMediaTypes;

					LOG_INVOKE_FUNCTION(SafeArrayGetUBound, lPtrSAMediaTypes, 1, &uBoundMediaTypes);

					LOG_INVOKE_FUNCTION(SafeArrayGetLBound, lPtrSAMediaTypes, 1, &lBoundMediaTypes);

					std::vector<IUnknown*> lCompressedMediaTypes;

					for (LONG lIndex = lBoundMediaTypes; lIndex <= uBoundMediaTypes; lIndex++)
					{
						VARIANT lVar;

						auto lr = SafeArrayGetElement(lPtrSAMediaTypes, &lIndex, &lVar);

						if (SUCCEEDED(lr) && lVar.vt == VT_UNKNOWN)
						{
							lCompressedMediaTypes.push_back(lVar.punkVal);
						}

						if (SUCCEEDED(lr))
							VariantClear(&lVar);
					}
					
					std::vector<CComPtrCustom<IUnknown>> lTopologyASFOutputNodes;

					LOG_INVOKE_FUNCTION(ASF::ASFSinkManager::createASFOutputNodes,
						aPtrByteStreamActivate,
						lCompressedMediaTypes,
						lTopologyASFOutputNodes);
					
					SAFEARRAY* lPtrSAOutputNodes;

					SAFEARRAYBOUND bound[1];

					bound[0].lLbound = 0;

					bound[0].cElements = lTopologyASFOutputNodes.size();

					lPtrSAOutputNodes = SafeArrayCreate(VT_VARIANT, 1, bound);

					LONG lCount = lTopologyASFOutputNodes.size();
					
					for (LONG lindex = 0;
						lindex < lCount; 
						lindex++)
					{
						IUnknown *lPtrIUnknown;

						lTopologyASFOutputNodes[lindex]->QueryInterface(IID_PPV_ARGS(&lPtrIUnknown));


						VARIANT *lVar = new VARIANT;

						VariantInit(lVar);

						lVar->vt = VT_UNKNOWN;

						lVar->punkVal = lPtrIUnknown;

						do
						{
							LOG_INVOKE_FUNCTION(SafeArrayPutElement, lPtrSAOutputNodes, &lindex, lVar);
						} while (false);
						
						VariantClear(lVar);

						delete lVar;
					}

					aArrayPtrTopologyOutputNodes->vt = aArrayPtrCompressedMediaTypes.vt;

					aArrayPtrTopologyOutputNodes->parray = lPtrSAOutputNodes;

					lresult = S_OK;

				} while (false);

				return lresult;
			}
		}
	}
}