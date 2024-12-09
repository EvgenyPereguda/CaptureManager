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

#include "CaptureSourceManager.h"
#include "CaptureSourceFactory.h"
#include "../Common/MFHeaders.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/ComMassivPtr.h"
#include "../Common/Common.h"
#include "../DataParser/DataParser.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Singleton.h"
#include "../CaptureDeviceManager/CaptureDeviceManager.h"


namespace CaptureManager
{
	namespace Core
	{
		HRESULT CaptureSourceManager::getXMLDocOfSources(
			std::vector<std::wstring>& aUsedSymbolicLinks,
			pugi::xml_node& aRefRoolXML_Node)
		{
			using namespace pugi;

			HRESULT lresult;

			do
			{

				CComMassivPtr<IMFMediaSource> lCaptureSources;

				LOG_INVOKE_FUNCTION(Singleton<Sources::CaptureSourceFactory>::getInstance().getCaptureSources,
					aUsedSymbolicLinks,
					lCaptureSources.getPtrMassivPtr(),
					lCaptureSources.getRefSizeMassiv());
				
				for (UINT32 lsourceIndex = 0; lsourceIndex < lCaptureSources.getSizeMassiv(); lsourceIndex++)
				{
					{
						xml_document lxmlDoc;

						auto ldeclNode = lxmlDoc.append_child(node_declaration);

						ldeclNode.append_attribute(L"version") = L"1.0";

						xml_node lcommentNode = lxmlDoc.append_child(node_comment);

						lcommentNode.set_value(L"XML Document of sources");

						auto lSource = lxmlDoc.append_child(L"Source");

						CComPtrCustom<IMFMediaSource> lMediaSource;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lCaptureSources[lsourceIndex],
							&lMediaSource);

						CComPtrCustom<IMFPresentationDescriptor> lPD;

						LOG_INVOKE_MF_METHOD(CreatePresentationDescriptor,
							lMediaSource,
							&lPD);

						LOG_INVOKE_FUNCTION(DataParser::readSourceActivate,
							lPD,
							lSource.append_child(L"Source.Attributes"));

						auto lCaptureSourceVideoSymbolicLinkNode = lSource.select_node(L"//Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK']/SingleValue/@Value");

						std::wstring lCaptureSourcesymbolicLink(lCaptureSourceVideoSymbolicLinkNode.attribute().value());

						if (!lCaptureSourcesymbolicLink.empty())
							for (auto& lUsedSymbolicLink : aUsedSymbolicLinks)
							{
								if (lCaptureSourcesymbolicLink.find(lUsedSymbolicLink) != std::wstring::npos)
								{
									auto lVideoSourceNodes = aRefRoolXML_Node.select_nodes(L"//Source");

									const wchar_t* lMediaFoundationGuid = L"e5323777-f976-4f5b-9b55-b94699c46e44";

									for (auto& lVideoSourceNode : lVideoSourceNodes)
									{
										auto lVideoSymbolicLinkNodes = lVideoSourceNode.node().select_node(L"Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK']/SingleValue/@Value");
													
										std::wstring lSourceSymbolicLink(lVideoSymbolicLinkNodes.attribute().value());

										auto lpos = lSourceSymbolicLink.find(lMediaFoundationGuid);

										if (lpos != std::wstring::npos)
										{
											if (lCaptureSourcesymbolicLink.find(lSourceSymbolicLink.substr(0, lpos)) != std::wstring::npos)
											{
												aRefRoolXML_Node.remove_child(lVideoSourceNode.node());

												break;
											}											
										}

									}

									break;
								}
							}
					}

					{
						auto lSource = aRefRoolXML_Node.append_child(L"Source");

						CComPtrCustom<IMFMediaSource> lMediaSource;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lCaptureSources[lsourceIndex],
							&lMediaSource);

						CComPtrCustom<IMFPresentationDescriptor> lPD;

						LOG_INVOKE_MF_METHOD(CreatePresentationDescriptor,
							lMediaSource,
							&lPD);

						LOG_INVOKE_FUNCTION(DataParser::readSourceActivate,
							lPD,
							lSource.append_child(L"Source.Attributes"));

						CaptureDeviceManager::addDeviceInstanceLink(lSource);

						LOG_INVOKE_FUNCTION(parse, lMediaSource,
							lSource);

						if (lMediaSource)
							LOG_INVOKE_MF_METHOD(Shutdown,
								lMediaSource);
					}
				}

			} while (false);

			return lresult;
		}

		HRESULT CaptureSourceManager::getSource(std::wstring& aSymbolicLink, IMFMediaSource** aPtrPtrMediaSource)
		{
			return Singleton<Sources::CaptureSourceFactory>::getInstance().getSource(
				aSymbolicLink,
				aPtrPtrMediaSource);
		}

		HRESULT CaptureSourceManager::createSource(
			IInnerCaptureProcessor* aPtrCaptureProcessor,
			IMFMediaSource** aPtrPtrMediaSource)
		{
			return Singleton<Sources::CaptureSourceFactory>::getInstance().createSource(
				aPtrCaptureProcessor,
				aPtrPtrMediaSource);
		}
	}
}