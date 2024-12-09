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

#include "MediaSourceParser.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/ComMassivPtr.h"
#include "../Common/Common.h"
#include "../Common/MFHeaders.h"
#include "../DataParser/DataParser.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"


namespace CaptureManager
{
	namespace Core
	{
		HRESULT MediaSourceParser::parse(
			IMFMediaSource* aPtrMediaSource,
			pugi::xml_node& aRefRoolXML_Node)
		{
			using namespace pugi;

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrMediaSource);
									
				CComPtrCustom<IMFPresentationDescriptor> lPD;

				LOG_INVOKE_MF_METHOD(CreatePresentationDescriptor,
					aPtrMediaSource,
					&lPD);

				auto lPresentationDescriptorNode = aRefRoolXML_Node.append_child(L"PresentationDescriptor");

				LOG_INVOKE_FUNCTION(DataParser::readPresentationDescriptor,
					lPD,
					lPresentationDescriptorNode);

				DWORD lstreamCount = 0;

				LOG_INVOKE_MF_METHOD(GetStreamDescriptorCount,
					lPD,
					&lstreamCount);

				if (lstreamCount > 1)
					lstreamCount = 1;

				for (DWORD lstreamIndex = 0; lstreamIndex < lstreamCount; lstreamIndex++)
				{
					CComPtrCustom<IMFStreamDescriptor> lSD;

					BOOL lisSelected;

					LOG_INVOKE_MF_METHOD(GetStreamDescriptorByIndex,
						lPD,
						lstreamIndex,
						&lisSelected,
						&lSD);

					if (!lSD)
						continue;
					
					PROPVARIANT lvar;

					PropVariantInit(&lvar);

					lresult = lSD->GetItem(MF_SD_STREAM_NAME, &lvar);

					PropVariantClear(&lvar);

					if (FAILED(lresult))
					{
						CComPtrCustom<IMFMediaTypeHandler> lHandler;

						LOG_INVOKE_MF_FUNCTION(GetMediaTypeHandler, lSD, &lHandler);

						GUID lMajor_type;

						LOG_INVOKE_MF_FUNCTION(GetMajorType, lHandler.get(), &lMajor_type);				
						
						std::wstring lStreamNameGUID;

						lresult = DataParser::GetGUIDName(lMajor_type, lStreamNameGUID);

						if (FAILED(lresult))
							break;

						auto l_find = lStreamNameGUID.find(L"MFMediaType");

						if (l_find != std::wstring::npos)
						{
							l_find = lStreamNameGUID.find_last_of(L"_");

							if (l_find != std::wstring::npos)
							{
								++l_find;

								lStreamNameGUID = lStreamNameGUID.substr(l_find, lStreamNameGUID.length() - l_find);

								lStreamNameGUID += L" stream";
							}
						}
						
						lSD->SetString(MF_SD_STREAM_NAME, lStreamNameGUID.c_str());
					}

					auto lStreamDescriptorNode = lPresentationDescriptorNode.append_child(L"StreamDescriptor");

					lStreamDescriptorNode.append_attribute(L"Index") = (unsigned long long)lstreamIndex;

					LOG_INVOKE_FUNCTION(DataParser::readStreamDescriptor,
						lSD,
						lStreamDescriptorNode);

					LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::enumMediaTypes,
						lSD,
						lStreamDescriptorNode);
				}

				lPresentationDescriptorNode.append_attribute(L"StreamCount") = (unsigned long long)lstreamCount;
					
			} while (false);

			return lresult;
		}
	}
}