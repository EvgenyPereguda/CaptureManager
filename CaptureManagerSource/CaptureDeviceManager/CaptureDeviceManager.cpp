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

#include "CaptureDeviceManager.h"
#include "../Common/MFHeaders.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/ComMassivPtr.h"
#include "../Common/Common.h"
#include "../DataParser/DataParser.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Singleton.h"
#include "../ConfigManager/ConfigManager.h"
#include "../Common/GUIDs.h"

#include <algorithm>

namespace CaptureManager
{
	namespace Core
	{

		HRESULT CaptureDeviceManager::getXMLDocOfSources(
			pugi::xml_node& aRefRoolXML_Node)
		{
			using namespace pugi;

			HRESULT lresult;

			do
			{
				CComPtrCustom<IMFAttributes> lAttributes;

				CComMassivPtr<IMFActivate> lCaptureDeviceActivates;

				LOG_INVOKE_MF_FUNCTION(MFCreateAttributes,
					&lAttributes,
					1);

				LOG_INVOKE_MF_METHOD(SetGUID,
					lAttributes,
					MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
					MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

				LOG_INVOKE_FUNCTION(enumSources, lAttributes,
					aRefRoolXML_Node);

				LOG_INVOKE_MF_METHOD(SetGUID,
					lAttributes,
					MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
					MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);

				LOG_INVOKE_FUNCTION(enumSources, lAttributes,
					aRefRoolXML_Node);

			} while (false);

			return lresult;
		}

		HRESULT CaptureDeviceManager::getSource(
			std::wstring& aRefSymbolicLink,
			IMFMediaSource** aPtrPtrMediaSource)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_STATE_DESCR(aRefSymbolicLink.empty(), E_POINTER);

				CComPtrCustom<IMFAttributes> lSymbolicLinkAttributes;

				LOG_INVOKE_MF_FUNCTION(MFCreateAttributes,
					&lSymbolicLinkAttributes,
					2);

				LOG_INVOKE_MF_METHOD(SetGUID,
					lSymbolicLinkAttributes,
					MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
					MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

				LOG_INVOKE_MF_METHOD(SetString,
					lSymbolicLinkAttributes,
					MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
					aRefSymbolicLink.c_str());

				CComPtrCustom<IMFActivate> lVideoSourceActivate;

				LOG_INVOKE_MF_FUNCTION(MFCreateDeviceSourceActivate,
					lSymbolicLinkAttributes,
					&lVideoSourceActivate);

				LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::GetSource,
					lVideoSourceActivate,
					aPtrPtrMediaSource);

			} while (false);

			do
			{
				if (SUCCEEDED(lresult))
					break;

				CComPtrCustom<IMFAttributes> lSymbolicLinkAttributes;

				LOG_INVOKE_MF_FUNCTION(MFCreateAttributes,
					&lSymbolicLinkAttributes,
					2);

				LOG_INVOKE_MF_METHOD(SetGUID,
					lSymbolicLinkAttributes,
					MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
					MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID);

				//LOG_INVOKE_MF_METHOD(SetString,
				//	lSymbolicLinkAttributes,
				//	MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK,
				//	aRefSymbolicLink.c_str());

				CComPtrCustom<IMFActivate> lAudioSourceActivate;



				{

					CComMassivPtr<IMFActivate> lCaptureDeviceActivates;

					LOG_INVOKE_MF_FUNCTION(MFEnumDeviceSources,
					lSymbolicLinkAttributes,
						lCaptureDeviceActivates.getPtrMassivPtr(),
						lCaptureDeviceActivates.getPtrSizeMassiv());


					for (UINT32 lsourceIndex = 0; lsourceIndex < lCaptureDeviceActivates.getSizeMassiv(); lsourceIndex++)
					{
						CComPtrCustom<IMFMediaSource> lMediaSource;

						GUID lVIDCAP_CATEGORY = GUID_NULL;

						CComPtrCustom<IMFActivate> lMediaType;

						lMediaType = lCaptureDeviceActivates[lsourceIndex];

						if (!lMediaType)
						{
							lresult = E_INVALIDARG;

							break;
						}

						UINT32 lLength = 0;

						lMediaType->GetStringLength(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK, &lLength);

						std::wstring lSymbolicLink;

						if (lLength != 0)
						{
							std::unique_ptr<WCHAR[]> lLink;
							
							lLink.reset(new WCHAR[lLength + 10]);

							UINT32 lwriteLength = 0;

							lMediaType->GetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK, lLink.get(), lLength + 10, &lwriteLength);

							lSymbolicLink = lLink.get();
						}

						if(aRefSymbolicLink == lSymbolicLink)
						{
							lAudioSourceActivate = lMediaType;

							break;
						}

						//lresult = lMediaType->GetString(
						//	MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_CATEGORY,
						//	&lVIDCAP_CATEGORY);

						//if (SUCCEEDED(lresult))
						//{
						//	using namespace Core;

						//	//if(!Singleton<ConfigManager>::getInstance().isWindows10())
						//	{
						//		if (lVIDCAP_CATEGORY == CLSID_VideoInputDeviceCategory)
						//		{
						//			aRefRoolXML_Node.remove_child(lSource);

						//			continue;
						//		}
						//	}
						//}


						//addDeviceInstanceLink(lSource);

						//LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::GetSource,
						//	lCaptureDeviceActivates[lsourceIndex],
						//	&lMediaSource);

						//LOG_INVOKE_FUNCTION(parse, lMediaSource,
						//	lSource);

						//if (lMediaSource)
						//	LOG_INVOKE_MF_METHOD(Shutdown,
						//		lMediaSource);
					}
				}

				//LOG_INVOKE_MF_FUNCTION(MFCreateDeviceSourceActivate,
				//	lSymbolicLinkAttributes,
				//	&lAudioSourceActivate);

				LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::GetSource,
					lAudioSourceActivate,
					aPtrPtrMediaSource);

			} while (false);

			return lresult;
		}

		HRESULT CaptureDeviceManager::enumSources(
			IMFAttributes* aAttributes,
			pugi::xml_node& aRefRoolXML_Node)
		{
			using namespace pugi;

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aAttributes);

				CComMassivPtr<IMFActivate> lCaptureDeviceActivates;

				LOG_INVOKE_MF_FUNCTION(MFEnumDeviceSources,
					aAttributes,
					lCaptureDeviceActivates.getPtrMassivPtr(),
					lCaptureDeviceActivates.getPtrSizeMassiv());


				for (UINT32 lsourceIndex = 0; lsourceIndex < lCaptureDeviceActivates.getSizeMassiv(); lsourceIndex++)
				{
					auto lSource = aRefRoolXML_Node.append_child(L"Source");

					do
					{

						CComPtrCustom<IMFMediaSource> lMediaSource;

						GUID lVIDCAP_CATEGORY = GUID_NULL;

						CComPtrCustom<IMFActivate> lMediaType;

						lMediaType = lCaptureDeviceActivates[lsourceIndex];

						if (!lMediaType)
						{
							lresult = E_INVALIDARG;

							break;
						}

						lresult = lMediaType->GetGUID(
							MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_CATEGORY,
							&lVIDCAP_CATEGORY);

						if (SUCCEEDED(lresult))
						{
							using namespace Core;

							//if(!Singleton<ConfigManager>::getInstance().isWindows10())
							{
								if (lVIDCAP_CATEGORY == CLSID_VideoInputDeviceCategory)
								{
									lresult = E_INVALIDARG;

									break;
								}
							}
						}

						DataParser::readSourceActivate(
							lCaptureDeviceActivates[lsourceIndex],
							lSource.append_child(L"Source.Attributes"));

						addDeviceInstanceLink(lSource);

						LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::GetSource,
							lCaptureDeviceActivates[lsourceIndex],
							&lMediaSource);

						LOG_INVOKE_FUNCTION(parse, lMediaSource,
							lSource);

						if (lMediaSource)
							LOG_INVOKE_MF_METHOD(Shutdown,
								lMediaSource);

					} while (false);

					if (FAILED(lresult))
					{
						aRefRoolXML_Node.remove_child(lSource);

						lresult = S_OK;
					}
				}

			} while (false);

			return lresult;
		}

		HRESULT CaptureDeviceManager::addDeviceInstanceLink(
			pugi::xml_node& aRefSourceNode)
		{
			using namespace pugi;

			HRESULT lresult;

			std::wstring ldeviceInstanceLink;

			do
			{

				auto lSymbolicLinkNode = aRefSourceNode.select_node(L"Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK']/SingleValue/@Value");

				lresult = getDeviceInstanceLink(lSymbolicLinkNode, ldeviceInstanceLink);

				if (SUCCEEDED(lresult))
				{
					break;
				}


				lSymbolicLinkNode = aRefSourceNode.select_node(L"Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK']/SingleValue/@Value");

				lresult = getAudioHaradwareLink(lSymbolicLinkNode, ldeviceInstanceLink);

				if (SUCCEEDED(lresult))
				{
					break;
				}

			} while (false);

			if (SUCCEEDED(lresult))
			{
				auto lAttributesNode = aRefSourceNode.select_node(L"Source.Attributes");

				if (!lAttributesNode.node().empty())
				{
					auto lAttributeNode = lAttributesNode.node().append_child(L"Attribute");

					lAttributeNode.append_attribute(L"Name").set_value(L"CM_DEVICE_LINK");

					WCHAR *lptrName = nullptr;

					lresult = StringFromCLSID(CM_DEVICE_LINK, &lptrName);

					if (SUCCEEDED(lresult))
					{
						lAttributeNode.append_attribute(L"GUID") = lptrName;
					}

					if (lptrName != nullptr)
						CoTaskMemFree(lptrName);

					lAttributeNode.append_attribute(L"Title") = L"The link for a capture device.";

					lAttributeNode.append_attribute(L"Description") = L"Contains the link for a capture device.";

					auto lSingleValue = lAttributeNode.append_child(L"SingleValue");

					lSingleValue.append_attribute(L"Value").set_value(ldeviceInstanceLink.c_str());
				}
			}

			return lresult;
		}

		HRESULT CaptureDeviceManager::getDeviceInstanceLink(
			pugi::xpath_node& aRefAttributeNode,
			std::wstring& aRefDeviceInstanceLink)
		{
			using namespace pugi;

			HRESULT lresult(E_FAIL);

			do
			{
				if (!aRefAttributeNode.attribute().empty())
				{

					lresult = S_OK;

					std::wstring lsymbolicLink(aRefAttributeNode.attribute().value());

					std::transform(lsymbolicLink.begin(), lsymbolicLink.end(), lsymbolicLink.begin(), ::tolower);

					aRefDeviceInstanceLink = lsymbolicLink;

					auto lfind = lsymbolicLink.find(L"\\\\?\\usb#");

					if (lfind != std::wstring::npos)
					{

						lsymbolicLink = lsymbolicLink.substr(lfind);

						lfind = lsymbolicLink.find(L"&");

						if (lfind != std::wstring::npos)
						{
							aRefDeviceInstanceLink = lsymbolicLink.substr(0, ++lfind);

							lsymbolicLink = lsymbolicLink.substr(lfind);

							lfind = lsymbolicLink.find(L"&");

							if (lfind != std::wstring::npos)
							{
								aRefDeviceInstanceLink += lsymbolicLink.substr(0, ++lfind);

								lsymbolicLink = lsymbolicLink.substr(lfind);

								lfind = lsymbolicLink.find(L"&");

								if (lfind != std::wstring::npos)
								{
									lsymbolicLink = lsymbolicLink.substr(++lfind);

									lfind = lsymbolicLink.find(L"&");

									if (lfind != std::wstring::npos)
									{
										aRefDeviceInstanceLink += lsymbolicLink.substr(0, lfind);
									}
								}
							}
						}
					}
					else
					{
						//std::wstring(L"CaptureManager///Software///Sources///AudioEndpointCapture///")
						lfind = lsymbolicLink.find(L"capturemanager///software///sources");

						if (lfind != std::wstring::npos)
						{
							aRefDeviceInstanceLink = L"capturemanager///software///sources";
						}
					}
				}

			} while (false);

			return lresult;
		}

		HRESULT CaptureDeviceManager::getAudioHaradwareLink(
			pugi::xpath_node& aRefAttributeNode,
			std::wstring& aRefDeviceInstanceLink)
		{
			using namespace pugi;

			HRESULT lresult;

			do
			{
				if (!aRefAttributeNode.attribute().empty())
				{

					std::wstring lsymbolicLink(aRefAttributeNode.attribute().value());

					CComPtrCustom<IMMDeviceEnumerator> lMMDeviceEnumerator;

					LOG_INVOKE_OBJECT_METHOD(lMMDeviceEnumerator,
						CoCreateInstance,
						__uuidof(MMDeviceEnumerator));

					LOG_CHECK_PTR_MEMORY(lMMDeviceEnumerator);

					CComPtrCustom<IMMDeviceCollection> lIMMDeviceCollection;

					lMMDeviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &lIMMDeviceCollection);

					if (lIMMDeviceCollection)
					{
						UINT lDeviceCount = 0;

						LOG_INVOKE_POINTER_METHOD(lIMMDeviceCollection, GetCount, &lDeviceCount);

						for (size_t i = 0; i < lDeviceCount; i++)
						{
							CComPtrCustom<IMMDevice> lMMDevice;

							lIMMDeviceCollection->Item(i, &lMMDevice);

							if (lMMDevice)
							{

								LPWSTR lPtrStrId;

								lresult = lMMDevice->GetId(&lPtrStrId);

								if (FAILED(lresult))
								{
									continue;
								}

								if (lPtrStrId == nullptr)
								{
									lresult = E_POINTER;

									continue;
								}

								std::wstring lSymbolicLink(lPtrStrId);

								CoTaskMemFree(lPtrStrId);

								auto lfind = lsymbolicLink.find(lSymbolicLink);

								if (lfind != std::wstring::npos)
								{
									CComPtrCustom<IPropertyStore> lPropertyStore;

									LOG_INVOKE_POINTER_METHOD(lMMDevice, OpenPropertyStore, STGM_READ, &lPropertyStore);

									if (lPropertyStore)
									{
										DWORD lPropsCount(0);

										lPropertyStore->GetCount(&lPropsCount);

										for (DWORD lPropsIndex = 0; lPropsIndex < lPropsCount; lPropsIndex++)
										{
											PROPVARIANT pv;

											PropVariantInit(&pv);

											PROPERTYKEY lPropKey;

											lPropertyStore->GetAt(lPropsIndex, &lPropKey);

											if (CM_MMDeviceDeviceInstanceLink == lPropKey.fmtid && lPropKey.pid == 2)
											{
												LOG_INVOKE_POINTER_METHOD(lPropertyStore, GetValue, lPropKey, &pv);

												if (VT_LPWSTR == pv.vt)
												{
													lsymbolicLink = std::wstring(pv.pwszVal);
												}

											}

											LOG_INVOKE_FUNCTION(PropVariantClear, &pv);
										}
									}
								}
							}
						}
					}

					lresult = S_OK;

					std::transform(lsymbolicLink.begin(), lsymbolicLink.end(), lsymbolicLink.begin(), ::tolower);

					aRefDeviceInstanceLink = lsymbolicLink;

					//_Ptr = 0x0810b150 "\\?\usb#vid_0c45&pid_6a04&mi_00#7&2b6a047&0&0000#{e5323777-f976-4f5b-9b55-b94699c46e44}\global"

					auto lfind = lsymbolicLink.find(L"usb");

					if (lfind != std::wstring::npos)
					{
						aRefDeviceInstanceLink = L"\\\\?\\usb#";

						lsymbolicLink = lsymbolicLink.substr(lfind + 4);

						lfind = lsymbolicLink.find(L"&");

						if (lfind != std::wstring::npos)
						{
							aRefDeviceInstanceLink += lsymbolicLink.substr(0, ++lfind);

							lsymbolicLink = lsymbolicLink.substr(lfind);

							lfind = lsymbolicLink.find(L"&");

							if (lfind != std::wstring::npos)
							{
								aRefDeviceInstanceLink += lsymbolicLink.substr(0, ++lfind);

								lsymbolicLink = lsymbolicLink.substr(lfind);

								lfind = lsymbolicLink.find(L"&");

								if (lfind != std::wstring::npos)
								{
									lsymbolicLink = lsymbolicLink.substr(++lfind);

									lfind = lsymbolicLink.find(L"&");

									if (lfind != std::wstring::npos)
									{
										aRefDeviceInstanceLink += lsymbolicLink.substr(0, lfind);
									}
								}
							}
						}
					}
					else
					{
						//std::wstring(L"CaptureManager///Software///Sources///AudioEndpointCapture///")
						lfind = lsymbolicLink.find(L"capturemanager///software///sources");

						if (lfind != std::wstring::npos)
						{
							aRefDeviceInstanceLink = L"capturemanager///software///sources";
						}
					}
				}

			} while (false);

			return lresult;
		}
	}
}