/*
MIT License

Copyright(c) 2021 Evgeny Pereguda

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

#include "WebCamKernelStreamingControl.h"
#include "../LogPrintOut/LogPrintOut.h"
#include <ks.h>
#include <ksmedia.h>

namespace CaptureManager
{
	namespace Controls
	{
		namespace WebCamControls
		{
			namespace CustomisedWebCamControl
			{
				using namespace pugi;



				typedef struct {
					KSPROPERTY_MEMBERSHEADER    MembersHeader;
					const VOID*                 Members;
				} KSPROPERTY_MEMBERSLIST, *PKSPROPERTY_MEMBERSLIST;


				typedef struct {
					KSIDENTIFIER                    PropTypeSet;
					ULONG                           MembersListCount;
					_Field_size_(MembersListCount)
						const KSPROPERTY_MEMBERSLIST*   MembersList;
				} KSPROPERTY_VALUES, *PKSPROPERTY_VALUES;

				struct Property
				{
					LONG mCurrentValue;
					LONG mMin;
					LONG mMax;
					LONG mStep;
					LONG mDefault;
					LONG mFlag;
				};

				struct WebCapProperty
				{

					virtual HRESULT getProperty(xml_node& aRefPropertyNode, IProcessWebCapProperty* aProcessWebCapProperty) const = 0;

					virtual HRESULT setProperty(LONG   aValue,                      // Value to set or get
						ULONG  aFlags,
						IProcessWebCapProperty* aProcessWebCapProperty) const = 0;

					virtual GUID getGUIDOfGroup() const = 0;

					virtual const wchar_t* getNameOfGroup() const = 0;

					virtual const wchar_t* getNameOfProperty() const { return mPropertyName.c_str(); }

					virtual ~WebCapProperty() {}

				protected:

					std::wstring mPropertyName;					
				};

				struct WebCapVideoProcessorProperty : public WebCapProperty
				{

					KSPROPERTY_VIDEOPROCAMP_S mBuffer;

					WebCapVideoProcessorProperty(ULONG aRefID, LPCWSTR aPtrNameOfProperty)
					{
						mBuffer.Property.Id = aRefID;

						mBuffer.Property.Set = PROPSETID_VIDCAP_VIDEOPROCAMP;

						mPropertyName = aPtrNameOfProperty;

						// {F21AA1E8-8646-4D2A-AAC5-B2DA3C128461}						
					}

					virtual GUID getGUIDOfGroup() const { return{ 0xf21aa1e8, 0x8646, 0x4d2a, { 0xaa, 0xc5, 0xb2, 0xda, 0x3c, 0x12, 0x84, 0x61 } }; }

					virtual const wchar_t* getNameOfGroup() const { return L"Video processor"; };

					virtual HRESULT getProperty(xml_node& aRefPropertyNode, IProcessWebCapProperty* aProcessWebCapProperty) const
					{
						HRESULT lresult(E_NOTIMPL);

						do
						{
							LOG_CHECK_PTR_MEMORY(aProcessWebCapProperty);

							KSPROPERTY_VIDEOPROCAMP_S lInputBuffer;

							lInputBuffer.Property = mBuffer.Property;

							lInputBuffer.Property.Flags = KSPROPERTY_TYPE_BASICSUPPORT;

							KSPROPERTY_VIDEOPROCAMP_S lOutputBuffer;

							lOutputBuffer.Property = mBuffer.Property;

							lOutputBuffer.Property.Flags = KSPROPERTY_TYPE_BASICSUPPORT;

							auto lInputPair = std::make_pair(&lInputBuffer, sizeof(lInputBuffer));


							auto ldemandSize = sizeof(KSPROPERTY_DESCRIPTION);



							std::unique_ptr<BYTE> lb(new BYTE[ldemandSize]);

							lOutputBuffer.Property = mBuffer.Property;

							lOutputBuffer.Property.Flags = KSPROPERTY_TYPE_BASICSUPPORT;


							memcpy(lb.get(), &lOutputBuffer, sizeof(lOutputBuffer));

							auto lOutputPair = std::make_pair(lb.get(), ldemandSize);

							lresult = aProcessWebCapProperty->processProperty(
								lInputPair.first, lInputPair.second,
								lOutputPair.first, lOutputPair.second);

							if ((HRESULT)lresult != S_OK)
							{
								break;
							}

							KSPROPERTY_DESCRIPTION* lPtrDecr = ((KSPROPERTY_DESCRIPTION*)lOutputPair.first);

							auto lMembersListCount = lPtrDecr->MembersListCount;

							KSPROPERTY_MEMBERSLIST* k = (KSPROPERTY_MEMBERSLIST*)(lPtrDecr + 1);

							lOutputBuffer.Property = mBuffer.Property;

							lOutputBuffer.Property.Flags = KSPROPERTY_TYPE_BASICSUPPORT;

							ldemandSize = lPtrDecr->DescriptionSize;

							lb.reset(new BYTE[ldemandSize]);

							memcpy(lb.get(), &lOutputBuffer, sizeof(lOutputBuffer));


							lOutputPair = std::make_pair(lb.get(), ldemandSize);

							lresult = aProcessWebCapProperty->processProperty(
								lInputPair.first, lInputPair.second,
								lOutputPair.first, lOutputPair.second);

							if ((HRESULT)lresult != S_OK)
							{
								break;
							}

							lPtrDecr = ((KSPROPERTY_DESCRIPTION*)lOutputPair.first);

							auto lp = lOutputPair.first + sizeof(KSPROPERTY_DESCRIPTION);

							while (lMembersListCount > 0)
							{
								auto lms = (KSPROPERTY_MEMBERSHEADER*)(lp);

								auto lTypeID = lPtrDecr->PropTypeSet.Id;

								auto lMembersCount = lms->MembersCount;

								auto lMembersSize = lms->MembersSize;

								switch (lms->MembersFlags)
								{
								case KSPROPERTY_MEMBER_RANGES:
									if (lMembersSize == 16)
										processStepping(aRefPropertyNode,
											lMembersCount,
											lMembersSize,
											lTypeID,
											lp + sizeof(KSPROPERTY_MEMBERSHEADER));
									break;
								case KSPROPERTY_MEMBER_STEPPEDRANGES:
									processStepping(aRefPropertyNode,
										lMembersCount,
										lMembersSize,
										lTypeID,
										lp + sizeof(KSPROPERTY_MEMBERSHEADER));
									break;
								case KSPROPERTY_MEMBER_VALUES:
									if (lms->Flags == KSPROPERTY_MEMBER_FLAG_DEFAULT)
									{
										processDefault(aRefPropertyNode,
											lMembersCount,
											lMembersSize,
											lTypeID,
											lp + sizeof(KSPROPERTY_MEMBERSHEADER));
									}
									break;
								default:
									break;
								}

								auto lMemoryShift = sizeof(KSPROPERTY_MEMBERSHEADER) + lMembersSize * lMembersCount;

								lp += lMemoryShift;

								--lMembersListCount;
							}

							lInputBuffer.Property = mBuffer.Property;

							lInputBuffer.Property.Flags = KSPROPERTY_TYPE_GET;

							lOutputBuffer.Property = mBuffer.Property;

							lOutputBuffer.Property.Flags = KSPROPERTY_TYPE_GET;

							lInputPair = std::make_pair(&lInputBuffer, sizeof(lInputBuffer));

							lOutputPair = std::make_pair((unsigned char*)&lOutputBuffer, sizeof(lOutputBuffer));

							lresult = aProcessWebCapProperty->processProperty(
								lInputPair.first, lInputPair.second,
								lOutputPair.first, lOutputPair.second);

							if ((HRESULT)lresult != S_OK)
							{
								break;
							}

							makeXMLDoc(aRefPropertyNode, lOutputBuffer);

							//lInputBuffer = mBuffer;
							//
							//lInputBuffer.Property.Flags = KSPROPERTY_TYPE_DEFAULTVALUES;

							//lOutputBuffer = mBuffer;

							//lOutputBuffer.Property.Flags = KSPROPERTY_TYPE_DEFAULTVALUES;
							//							
							//lInputPair = std::make_pair(&lInputBuffer, sizeof(lInputBuffer));


							//ldemandSize = sizeof(KSPROPERTY_VALUES);



							//lb.reset(new BYTE[ldemandSize]);
							//
							//memcpy(lb.get(), &lOutputBuffer, sizeof(lOutputBuffer));

							//lOutputPair = std::make_pair(lb.get(), ldemandSize);

							//lresult = processProperty(
							//	lInputPair,
							//	lOutputPair,
							//	aDevice);

							//if ((HRESULT)lresult != S_OK)
							//{
							//	break;
							//}




						} while (false);

						return lresult;
					}

					virtual HRESULT setProperty(LONG   aValue,                      // Value to set or get
						ULONG  aFlags,
						IProcessWebCapProperty* aProcessWebCapProperty) const
					{
						HRESULT lresult(E_NOTIMPL);

						do
						{
							LOG_CHECK_PTR_MEMORY(aProcessWebCapProperty);

							KSPROPERTY_VIDEOPROCAMP_S lInputBuffer;

							lInputBuffer.Property = mBuffer.Property;

							lInputBuffer.Property.Flags = KSPROPERTY_TYPE_SET;

							KSPROPERTY_VIDEOPROCAMP_S lOutputBuffer;

							lOutputBuffer.Property = mBuffer.Property;

							lOutputBuffer.Property.Flags = KSPROPERTY_TYPE_SET;

							lOutputBuffer.Capabilities = aFlags;

							lOutputBuffer.Flags = aFlags;

							lInputBuffer.Capabilities = aFlags;

							lInputBuffer.Flags = aFlags;

							if (mBuffer.Property.Id == KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY)
							{
								if (aValue == 50)
									aValue = 1;
								else if (aValue == 60)
									aValue = 2;
								else
									aValue = 0;

							}

							lInputBuffer.Value = aValue;

							lOutputBuffer.Value = aValue;

							auto lInputPair = std::make_pair(&lInputBuffer, sizeof(lInputBuffer));

							auto lOutputPair = std::make_pair(&lOutputBuffer, sizeof(lOutputBuffer));

							lresult = aProcessWebCapProperty->processProperty(
								lInputPair.first, lInputPair.second,
								lOutputPair.first, lOutputPair.second);
							
							if ((HRESULT)lresult != S_OK)
							{
								break;
							}

						} while (false);

						return lresult;
					}

				private:

					void processDefault(
						xml_node& aRefPropertyNode,
						ULONG aMembersCount,
						ULONG aMembersSize,
						ULONG aTypeID,
						const VOID* aMembers) const
					{

						if (!aRefPropertyNode.attribute(L"Default").empty())
						{
							aRefPropertyNode.remove_attribute(L"Default");
						}

						switch (aTypeID)
						{
						case VT_I4:
						{
							auto lDefault = (INT*)aMembers;

							aRefPropertyNode.append_attribute(L"Default").set_value(*lDefault);
						}

						break;

						case VT_UI4:
						{
							auto lDefault = (ULONG*)aMembers;

							aRefPropertyNode.append_attribute(L"Default").set_value((ULONGLONG)lDefault);
						}

						break;

						default:
							break;
						}
					}

					void processStepping(
						xml_node& aRefPropertyNode,
						ULONG aMembersCount,
						ULONG aMembersSize,
						ULONG aTypeID,
						const VOID* aMembers) const
					{
						switch (aTypeID)
						{
						case VT_I4:
						{

							auto lStepping = (KSPROPERTY_STEPPING_LONG*)aMembers;

							auto lStep = lStepping->SteppingDelta;

							auto lBounds = lStepping->Bounds;

							auto lMax = lBounds.SignedMaximum;

							auto lMin = lBounds.SignedMinimum;

							if (mBuffer.Property.Id == KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY)
							{
								aRefPropertyNode.append_attribute(L"Step").set_value(10);

								aRefPropertyNode.append_attribute(L"Max").set_value(60);

								aRefPropertyNode.append_attribute(L"Min").set_value(50);

								if (aRefPropertyNode.attribute(L"Default").empty())
								{
									aRefPropertyNode.append_attribute(L"Default").set_value(50);
								}
							}
							else
							{

								aRefPropertyNode.append_attribute(L"Step").set_value((ULONGLONG)lStep);

								aRefPropertyNode.append_attribute(L"Max").set_value(lMax);

								aRefPropertyNode.append_attribute(L"Min").set_value(lMin);

								if (aRefPropertyNode.attribute(L"Default").empty())
								{
									aRefPropertyNode.append_attribute(L"Default").set_value(lMax - lMin);
								}
							}
						}

						break;

						case VT_UI4:
						{
							auto lStepping = (KSPROPERTY_STEPPING_LONG*)aMembers;

							auto lStep = lStepping->SteppingDelta;

							auto lBounds = lStepping->Bounds;

							aRefPropertyNode.append_attribute(L"Step").set_value((ULONGLONG)lStep);

							auto lMax = lBounds.UnsignedMaximum;

							aRefPropertyNode.append_attribute(L"Max").set_value((ULONGLONG)lMax);

							auto lMin = lBounds.UnsignedMinimum;

							aRefPropertyNode.append_attribute(L"Min").set_value((ULONGLONG)lMin);

							if (aRefPropertyNode.attribute(L"Default").empty())
							{
								aRefPropertyNode.append_attribute(L"Default").set_value((ULONGLONG)(lMax - lMin));
							}
						}

						break;

						default:
							break;
						}
					}

					void makeXMLDoc(xml_node& aRefPropertyNode, KSPROPERTY_VIDEOPROCAMP_S aOutputBuffer) const
					{
						aRefPropertyNode.append_attribute(L"Title").set_value(getNameOfProperty());

						if (mBuffer.Property.Id == KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY)
						{
							int lvalue = 50;

							if (aOutputBuffer.Value == 1 || aOutputBuffer.Value == 0)
								lvalue = 50;
							else if (aOutputBuffer.Value == 2)
								lvalue = 60;

							aRefPropertyNode.append_attribute(L"CurrentValue").set_value(lvalue);
						}
						else
							aRefPropertyNode.append_attribute(L"CurrentValue").set_value(aOutputBuffer.Value);

						aRefPropertyNode.append_attribute(L"Flag").set_value((unsigned long long)aOutputBuffer.Flags);
					}

				};


				struct WebCapCameraControlProperty : public WebCapVideoProcessorProperty
				{

					//	KSPROPERTY_VIDEOPROCAMP_S mBuffer;

					WebCapCameraControlProperty(ULONG aRefID, LPCWSTR aPtrNameOfProperty) :
						WebCapVideoProcessorProperty(aRefID, aPtrNameOfProperty)
					{
						//mBuffer.Property.Id = aRefID;

						mBuffer.Property.Set = PROPSETID_VIDCAP_CAMERACONTROL;

						//mPropertyName = aPtrNameOfProperty;

						// {6419B693-A2DB-4D2F-8D62-9CDC91FF9600}

					}

					virtual GUID getGUIDOfGroup() const { return{ 0x6419b693, 0xa2db, 0x4d2f, { 0x8d, 0x62, 0x9c, 0xdc, 0x91, 0xff, 0x96, 0x0 } }; }

					virtual const wchar_t* getNameOfGroup() const { return L"Camera control"; };

				};


				const WebCapProperty* gWebCapProperty[] = {

					// property for working wih video processor of web camera
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_BRIGHTNESS, L"Brightness"),
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_CONTRAST, L"Contrast"),
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_HUE, L"Hue"),
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_SATURATION, L"Saturation"),
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_SHARPNESS, L"Sharpness"),
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_GAMMA, L"Gamma"),
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_COLORENABLE, L"ColorEnable"),
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_WHITEBALANCE, L"WhiteBalance"),
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_BACKLIGHT_COMPENSATION, L"BacklightCompensation"),
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_GAIN, L"Gain"),

					// property for working with control of camera
					new WebCapCameraControlProperty(KSPROPERTY_CAMERACONTROL_PAN, L"Pan"),
					new WebCapCameraControlProperty(KSPROPERTY_CAMERACONTROL_TILT, L"Tilt"),
					new WebCapCameraControlProperty(KSPROPERTY_CAMERACONTROL_ROLL, L"Roll"),
					new WebCapCameraControlProperty(KSPROPERTY_CAMERACONTROL_ZOOM, L"Zoom"),
					new WebCapCameraControlProperty(KSPROPERTY_CAMERACONTROL_EXPOSURE, L"Exposure"),
					new WebCapCameraControlProperty(KSPROPERTY_CAMERACONTROL_IRIS, L"Iris"),
					new WebCapCameraControlProperty(KSPROPERTY_CAMERACONTROL_FOCUS, L"Focus"),







					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_DIGITAL_MULTIPLIER, L"Amount of digital zoom"),
					new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_DIGITAL_MULTIPLIER_LIMIT, L"Upper limit for the amount of digital zoom"),
					//	new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_WHITEBALANCE_COMPONENT, L""),
						new WebCapVideoProcessorProperty(KSPROPERTY_VIDEOPROCAMP_POWERLINE_FREQUENCY, L"Power line frequency")
				};


				WebCamKernelStreamingControl::~WebCamKernelStreamingControl() {}
							   
				HRESULT STDMETHODCALLTYPE WebCamKernelStreamingControl::getCamParametrs(
					BSTR *aXMLstring)
				{
					HRESULT lresult(E_NOTIMPL);

					xml_document lxmlDoc;

					auto ldeclNode = lxmlDoc.append_child(node_declaration);

					ldeclNode.append_attribute(L"version") = L"1.0";

					xml_node lcommentNode = lxmlDoc.append_child(node_comment);

					lcommentNode.set_value(L"XML Document of web cam parametrs");

					auto lRootXMLElement = lxmlDoc.append_child(L"Parametrs");


					struct GUIDComparer
					{
						bool operator()(const GUID & Left, const GUID & Right) const
						{
							return memcmp(&Left, &Right, sizeof(Right)) < 0;
						}
					};

					std::map<GUID, xml_node, GUIDComparer> lGroupNodeMap;

					DWORD lParametrIndex = 0;

					for (auto lItemProperty : gWebCapProperty)
					{
						HRESULT lresult;

						GUID lGUIDGroup = lItemProperty->getGUIDOfGroup();

						auto lfintIter = lGroupNodeMap.find(lGUIDGroup);

						if (lfintIter == lGroupNodeMap.end())
						{
							auto lGroupNode = lRootXMLElement.append_child(L"Group");

							WCHAR *lptrName = nullptr;

							lresult = StringFromCLSID(lGUIDGroup, &lptrName);

							if (SUCCEEDED(lresult))
							{
								lGroupNode.append_attribute(L"GUID") = lptrName;

								CoTaskMemFree(lptrName);
							}

							lGroupNode.append_attribute(L"Title") = lItemProperty->getNameOfGroup();

							lGroupNodeMap[lGUIDGroup] = std::move(lGroupNode);

							lfintIter = lGroupNodeMap.find(lGUIDGroup);
						}

						if (lfintIter != lGroupNodeMap.end())
						{
							auto lpropertyNode = (*lfintIter).second.append_child(L"Parametr");

							lpropertyNode.append_attribute(L"Index").set_value((unsigned int)lParametrIndex);

							lresult = lItemProperty->getProperty(lpropertyNode, this);

							if ((HRESULT)lresult != S_OK)
							{
								(*lfintIter).second.remove_child(lpropertyNode);
							}
						}

						++lParametrIndex;
					}

					for (auto& lPair : lGroupNodeMap)
					{
						if (lPair.second.first_child().empty())
						{
							lRootXMLElement.remove_child(lPair.second);
						}

					}


					std::wstringstream aRefXMLDocumentString;

					lxmlDoc.print(aRefXMLDocumentString);

					*aXMLstring = SysAllocString(aRefXMLDocumentString.str().c_str());

					lresult = S_OK;

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE WebCamKernelStreamingControl::getCamParametr(
					DWORD aParametrIndex,
					LONG *aCurrentValue,
					LONG *aMin,
					LONG *aMax,
					LONG *aStep,
					LONG *aDefault,
					LONG *aFlag)
				{
					HRESULT lresult;

					auto lPropertyCount = sizeof(gWebCapProperty) / sizeof(WebCapProperty*);

					do
					{
						if (aParametrIndex >= lPropertyCount)
						{
							lresult = E_INVALIDARG;

							break;
						}

						xml_document lxmlDoc;

						xml_node lcommentNode = lxmlDoc.append_child(L"Property");

						lresult = gWebCapProperty[aParametrIndex]->getProperty(lcommentNode, this);

						if ((HRESULT)lresult != S_OK)
						{
							*aFlag = 0;

							break;
						}

						*aCurrentValue = lcommentNode.attribute(L"CurrentValue").as_int();
						*aMin = lcommentNode.attribute(L"Min").as_int();
						*aMax = lcommentNode.attribute(L"Max").as_int();
						*aStep = lcommentNode.attribute(L"Step").as_int();
						*aDefault = lcommentNode.attribute(L"Default").as_int();
						*aFlag = lcommentNode.attribute(L"Flag").as_int();

					} while (false);


					return lresult;
				}

				HRESULT STDMETHODCALLTYPE WebCamKernelStreamingControl::setCamParametr(
					DWORD aParametrIndex,
					LONG aNewValue,
					LONG aFlag)
				{
					HRESULT lresult;

					auto lPropertyCount = sizeof(gWebCapProperty) / sizeof(WebCapProperty*);

					do
					{
						if (aParametrIndex >= lPropertyCount)
						{
							lresult = E_INVALIDARG;

							break;
						}

						xml_document lxmlDoc;

						xml_node lcommentNode = lxmlDoc.append_child(L"Property");

						lresult = gWebCapProperty[aParametrIndex]->setProperty(
							aNewValue,
							aFlag, this);

					} while (false);


					return lresult;
				}
			}
		}
	}
}