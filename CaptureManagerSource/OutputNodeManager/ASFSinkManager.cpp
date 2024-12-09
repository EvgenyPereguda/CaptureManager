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

#include "ASFSinkManager.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"
#include <propvarutil.h>

namespace CaptureManager
{
	namespace Sinks
	{
		namespace ASF
		{
			using namespace Core;
			
			HRESULT ASFSinkManager::setEncodingProperties(
				REFGUID aRefGUIDMediaType,
				IPropertyStore* aPtrPropertyStore,
				EncodingSettings aEncodingSettings)
			{
				HRESULT lresult;

				PROPVARIANT var;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPropertyStore);
					
					LOG_CHECK_STATE_DESCR(aEncodingSettings.mEncodingMode == EncodingSettings::NONE, MF_E_NOT_INITIALIZED);
					
					switch (aEncodingSettings.mEncodingMode)
					{
					case EncodingSettings::CBR:

						LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean,
							FALSE,
							&var);
						
						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,
							MFPKEY_VBRENABLED, var);

						if (aRefGUIDMediaType == MFMediaType_Audio)
						{
							LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean,
								VARIANT_TRUE, &var);
							
							LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,
								MFPKEY_CONSTRAINENCLATENCY, 
								var);
							
							LOG_INVOKE_FUNCTION(InitPropVariantFromUInt32,
								100, &var);
							
							LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,MFPKEY_MAXENCLATENCYMS, var);
							
						}
						else if (aRefGUIDMediaType == MFMediaType_Video)
						{
							LOG_INVOKE_FUNCTION(InitPropVariantFromInt32,
								aEncodingSettings.mEncodingModeValue, 
								&var);

							LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, 
								SetValue,MFPKEY_VIDEOWINDOW, 
								var);
						}

						PropVariantClear(&var);
						break;

					case EncodingSettings::VBR:

						LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean,VARIANT_TRUE, &var);

						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,MFPKEY_VBRENABLED, var);

						LOG_INVOKE_FUNCTION(InitPropVariantFromInt32,1, &var);

						LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,MFPKEY_PASSESUSED, var);
						
						if (aRefGUIDMediaType == MFMediaType_Audio)
						{
							LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean,VARIANT_TRUE, &var);

							LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,MFPKEY_CONSTRAINENCLATENCY, var);

							LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,MFPKEY_CONSTRAINDECLATENCY, var);

							LOG_INVOKE_FUNCTION(InitPropVariantFromUInt32,100, &var);

							LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,MFPKEY_MAXENCLATENCYMS, var);

							LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,MFPKEY_MAXDECLATENCYMS, var);
							
							LOG_INVOKE_FUNCTION(InitPropVariantFromUInt32,
								aEncodingSettings.mEncodingModeValue, 
								&var);

							LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,MFPKEY_DESIRED_VBRQUALITY, var);
						}
						else if (aRefGUIDMediaType == MFMediaType_Video)
						{
							LOG_INVOKE_FUNCTION(InitPropVariantFromInt32,1000, &var);

							LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,MFPKEY_VIDEOWINDOW, var);

							LOG_INVOKE_FUNCTION(InitPropVariantFromInt32,aEncodingSettings.mEncodingModeValue, &var);

							LOG_INVOKE_POINTER_METHOD(aPtrPropertyStore, SetValue,MFPKEY_VBRQUALITY, var);

						}
						PropVariantClear(&var);
						break;

					default:
						lresult = E_UNEXPECTED;
						break;
					}

				} while (false);

				return lresult;
			}

			HRESULT ASFSinkManager::addASFStreamConfig(
				IMFASFProfile* aPtrASFProfile,
				IMFMediaType* aPtrMediaType,
				WORD aStreamNumber)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrASFProfile);

					LOG_CHECK_PTR_MEMORY(aPtrMediaType);
					
					CComPtrCustom<IMFASFStreamConfig> lASFStreamConfig;

					LOG_INVOKE_MF_METHOD(CreateStream,
						aPtrASFProfile,
						aPtrMediaType,
						&lASFStreamConfig);
					
					LOG_INVOKE_MF_METHOD(SetStreamNumber,
						lASFStreamConfig,
						aStreamNumber);

					LOG_INVOKE_MF_METHOD(SetStream,
						aPtrASFProfile,
						lASFStreamConfig);
					
				} while (false);
				
				return lresult;
			}

			HRESULT ASFSinkManager::createOutputNode(
				IMFActivate* aPtrActivate,
				DWORD aStreamNumber,
				IMFTopologyNode** aPtrPtrTopologyNode)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrActivate);

					LOG_CHECK_PTR_MEMORY(aPtrPtrTopologyNode);
					
					LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
						MF_TOPOLOGY_OUTPUT_NODE,
						aPtrPtrTopologyNode);

					LOG_CHECK_PTR_MEMORY(aPtrPtrTopologyNode);
					
					LOG_INVOKE_MF_METHOD(SetObject,
						(*aPtrPtrTopologyNode),
						aPtrActivate);
					
					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrTopologyNode),
						MF_TOPONODE_STREAMID,
						aStreamNumber);

					LOG_INVOKE_MF_METHOD(SetUINT32,
						(*aPtrPtrTopologyNode),
						MF_TOPONODE_NOSHUTDOWN_ON_REMOVE,
						FALSE);
					
				} while (false);
				
				return lresult;
			}

			
			HRESULT ASFSinkManager::createASFOutputNodes(
				std::wstring& aRefFileName,
				std::vector<IUnknown*> aCompressedMediaTypes,
				std::vector<CComPtrCustom<IUnknown>>& aRefTopologyASFOutputNodes)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_STATE(aRefFileName.empty());

					LOG_CHECK_STATE(aCompressedMediaTypes.empty());

					LOG_CHECK_STATE_DESCR(aCompressedMediaTypes.size() > 126, MF_E_OUT_OF_RANGE);
										
					CComPtrCustom<IMFASFContentInfo> lContentInfo;

					LOG_INVOKE_MF_FUNCTION(MFCreateASFContentInfo,
						&lContentInfo);
					
					CComPtrCustom<IMFASFProfile> lProfile;

					LOG_INVOKE_MF_FUNCTION(MFCreateASFProfile,
						&lProfile);

					auto lCompressedMediaTypeCount = aCompressedMediaTypes.size() + 1;

					for (WORD lStreamNumber = 1; lStreamNumber < lCompressedMediaTypeCount; lStreamNumber++)
					{
						CComPtrCustom<IMFMediaType> lPtrMediaType;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(aCompressedMediaTypes[lStreamNumber - 1],
							&lPtrMediaType);
						
						LOG_INVOKE_FUNCTION(addASFStreamConfig,
							lProfile,
							lPtrMediaType,
							lStreamNumber);
						
						CComPtrCustom<IPropertyStore> lContentInfoProps;

						LOG_INVOKE_MF_METHOD(GetEncodingConfigurationPropertyStore,
							lContentInfo,
							lStreamNumber,
							&lContentInfoProps);
						
						GUID lGUIDMajorType;

						LOG_INVOKE_MF_METHOD(GetGUID,
							lPtrMediaType,
							MF_MT_MAJOR_TYPE,
							&lGUIDMajorType);

						EncodingSettings lEncodingSettings;

						LOG_INVOKE_FUNCTION(setEncodingProperties,
							lGUIDMajorType,
							lContentInfoProps,
							lEncodingSettings);
						
					}

					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);					

					CComPtrCustom<IPropertyStore> lContentInfoProps;

					LOG_INVOKE_MF_METHOD(GetEncodingConfigurationPropertyStore,
						lContentInfo,
						0,
						&lContentInfoProps);
					
					PROPVARIANT lVariable;
					
					LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean, VARIANT_TRUE, &lVariable);
					
					LOG_INVOKE_MF_METHOD(SetValue,
						lContentInfoProps,
						MFPKEY_ASFMEDIASINK_AUTOADJUST_BITRATE,
						lVariable);
					
					LOG_INVOKE_MF_METHOD(SetProfile,
						lContentInfo,
						lProfile);
					
					CComPtrCustom<IMFActivate> lASFSinkActivate;

					LOG_INVOKE_MF_FUNCTION(MFCreateASFMediaSinkActivate,
						aRefFileName.c_str(),
						lContentInfo,
						&lASFSinkActivate);
					
					for (DWORD lStreamNumber = 1; lStreamNumber < lCompressedMediaTypeCount; lStreamNumber++)
					{
						CComPtrCustom<IMFTopologyNode> lASFOutputNode;

						LOG_INVOKE_FUNCTION(createOutputNode,
							lASFSinkActivate,
							lStreamNumber,
							&lASFOutputNode);
						
						CComPtrCustom<IUnknown> lNode;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lASFOutputNode, &lNode);

						aRefTopologyASFOutputNodes.push_back(lNode);
					}
										
				} while (false);
				
				return lresult;
			}


			HRESULT ASFSinkManager::createASFOutputNodes(
				IUnknown* aPtrByteStreamActivate,
				std::vector<IUnknown*> aCompressedMediaTypes,
				std::vector<CComPtrCustom<IUnknown>>& aRefTopologyASFOutputNodes)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrByteStreamActivate);

					LOG_CHECK_STATE(aCompressedMediaTypes.empty());

					LOG_CHECK_STATE_DESCR(aCompressedMediaTypes.size() > 126, MF_E_OUT_OF_RANGE);

					CComPtrCustom<IMFActivate> lActivate;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrByteStreamActivate, &lActivate);
					
					CComPtrCustom<IMFASFContentInfo> lContentInfo;

					LOG_INVOKE_MF_FUNCTION(MFCreateASFContentInfo,
						&lContentInfo);

					CComPtrCustom<IMFASFProfile> lProfile;

					LOG_INVOKE_MF_FUNCTION(MFCreateASFProfile,
						&lProfile);

					auto lCompressedMediaTypeCount = aCompressedMediaTypes.size() + 1;

					for (WORD lStreamNumber = 1; lStreamNumber < lCompressedMediaTypeCount; lStreamNumber++)
					{
						CComPtrCustom<IMFMediaType> lPtrMediaType;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(aCompressedMediaTypes[lStreamNumber - 1],
							&lPtrMediaType);

						LOG_INVOKE_FUNCTION(addASFStreamConfig,
							lProfile,
							lPtrMediaType,
							lStreamNumber);

						CComPtrCustom<IPropertyStore> lContentInfoProps;

						LOG_INVOKE_MF_METHOD(GetEncodingConfigurationPropertyStore,
							lContentInfo,
							lStreamNumber,
							&lContentInfoProps);

						GUID lGUIDMajorType;

						LOG_INVOKE_MF_METHOD(GetGUID,
							lPtrMediaType,
							MF_MT_MAJOR_TYPE,
							&lGUIDMajorType);

						EncodingSettings lEncodingSettings;

						LOG_INVOKE_FUNCTION(setEncodingProperties,
							lGUIDMajorType,
							lContentInfoProps,
							lEncodingSettings);

					}

					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

					CComPtrCustom<IPropertyStore> lContentInfoProps;

					LOG_INVOKE_MF_METHOD(GetEncodingConfigurationPropertyStore,
						lContentInfo,
						0,
						&lContentInfoProps);

					//PROPVARIANT lVariable;
					//
					//LOG_INVOKE_FUNCTION(InitPropVariantFromBoolean, TRUE, &lVariable);

					//LOG_INVOKE_MF_METHOD(SetValue,
					//	lContentInfoProps,
					//	MFPKEY_ASFMEDIASINK_AUTOADJUST_BITRATE,
					//	lVariable);

					LOG_INVOKE_MF_METHOD(SetProfile,
						lContentInfo,
						lProfile);

					CComPtrCustom<IMFActivate> lASFSinkActivate;

					LOG_INVOKE_MF_FUNCTION(MFCreateASFStreamingMediaSinkActivate,
						lActivate,
						lContentInfo,
						&lASFSinkActivate);

					for (DWORD lStreamNumber = 1; lStreamNumber < lCompressedMediaTypeCount; lStreamNumber++)
					{
						CComPtrCustom<IMFTopologyNode> lASFOutputNode;

						LOG_INVOKE_FUNCTION(createOutputNode,
							lASFSinkActivate,
							lStreamNumber,
							&lASFOutputNode);

						CComPtrCustom<IUnknown> lNode;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lASFOutputNode, &lNode);

						aRefTopologyASFOutputNodes.push_back(lNode);
					}

				} while (false);

				return lresult;
			}			
		}
	}
}