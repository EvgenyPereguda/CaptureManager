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

#include <sstream>
#include <unordered_map>
#include <memory>


#include "CaptureManagerBroker.h"
#include "../PugiXML/pugixml.hpp"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Singleton.h"
#include "../Common/BaseUnknown.h"
#include "../Common/Common.h"
#include "../Common/Macros.h"
#include "../Common/GUIDs.h"
#include "../Common/Constants.h"
#include "../Common/IInnerCaptureProcessor.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../CaptureDeviceManager/CaptureDeviceManager.h"
#include "../CaptureSourceManager/CaptureSourceManager.h"
#include "../VideoRendererManager/VideoRendererFactory.h"
#include "../WebCamControls/WebCamKernelStreamingControlManager.h"
#include "../EncoderManager/EncoderManager.h"
#include "../DataParser/DataParser.h"
#include "../SampleGrabberCall/SampleGrabberCallFactory.h"
#include "ISampleGrabberCallbackInner.h"
#include "CaptureManagerConstants.h"
#include "../OutputNodeManager/OutputNodeFactoryCollection.h"
#include "../SessionProcessorManager/SessionProcessorManager.h"
#include "../MemoryManager/MemoryManager.h"
#include "../SampleAccumulatorNode/SampleAccumulatorNodeFactory.h"
#include "../VideoRendererManager/IEVRStreamControl.h"
#include "../VideoRendererManager/IRenderingControl.h"
#include "../DMOManager/DMOManager.h"
#include "../Scheduler/IScheduler.h"
#include "../Scheduler/SchedulerFactory.h"
#include "../MemoryManager/IMemoryBufferManager.h"
#include "../MemoryManager/MemoryBufferManager.h"
#include "../EVRMultiSink/VideoRendererFactory.h"
#include "../VideoRendererManager/IMixerStreamPositionControl.h"
#include "../MCSSManager/MCSSManager.h"
#include "../CaptureInvoker/CaptureInvoker.h"
#include "../Switcher/SwitcherNodeFactory.h"
#include "../CustomisedMixerNode/CustomisedMixerNode.h"
#include "../CustomisedMixerNode/CustomisedMixerNodeFactory.h"
#include "../CustomisedMixerNode/MixerWrapper.h"
#include "../CustomisedMixerNode/IVideoMixerControl.h"
#include "../CustomisedMixerNode/IAudioMixerControl.h"
#include "../AudioRendererManager/AudioRendererFactory.h"

#include <ks.h>
#include <ksproxy.h>


extern void InitLogOut();
extern void UnInitLogOut();

namespace CaptureManager
{
	using namespace Core;
	
	CaptureManagerBroker::CaptureManagerBroker():
		mDestroyedFlag(true)
	{
		InitLogOut();
		
		if (SUCCEEDED(Singleton<Core::MediaFoundation::MediaFoundationManager>::getInstance().initialize()))
			mDestroyedFlag = false;

		Singleton<Core::MemoryManager>::getInstance().initialize();		
		
		Singleton<DMO::DMOManager>::getInstance().initialize();

		Singleton<Core::MCSSManager>::getInstance().initialize();
	}

	CaptureManagerBroker::~CaptureManagerBroker()
	{
		Singleton<Core::MediaFoundation::MediaFoundationManager>::getInstance().unInitialize();

		UnInitLogOut();

		mDestroyedFlag = true;
	}
	
	HRESULT CaptureManagerBroker::getXMLDocumentStringListOfSources(
		std::wstring& aRefXMLDocumentString)
	{
		using namespace pugi;

		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			xml_document lxmlDoc;

			auto ldeclNode = lxmlDoc.append_child(node_declaration);

			ldeclNode.append_attribute(L"version") = L"1.0";

			xml_node lcommentNode = lxmlDoc.append_child(node_comment);

			lcommentNode.set_value(L"XML Document of sources");

			auto lRootXMLElement = lxmlDoc.append_child(L"Sources");

			LOG_INVOKE_FUNCTION(CaptureDeviceManager::getXMLDocOfSources, lRootXMLElement);

			auto lVideoSymbolicLinkNodes = lRootXMLElement.select_nodes(L"//Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK']/SingleValue/@Value");

			std::vector<std::wstring> lUsedSymbolicLinks;

			const wchar_t* lMediaFoundationGuid = L"e5323777-f976-4f5b-9b55-b94699c46e44";

			for (auto& litem : lVideoSymbolicLinkNodes)
			{
				std::wstring lsymbolicLink(litem.attribute().value());

				auto lpos = lsymbolicLink.find(lMediaFoundationGuid);
				
				if (lpos != std::wstring::npos)
				{
					lUsedSymbolicLinks.push_back(lsymbolicLink.substr(0, lpos));
				}
			}

			LOG_INVOKE_FUNCTION(CaptureSourceManager::getXMLDocOfSources,
				lUsedSymbolicLinks,
				lRootXMLElement);
			
			std::wstringstream lwstringstream;

			lxmlDoc.print(lwstringstream);

			aRefXMLDocumentString = lwstringstream.str();

			lresult = S_OK;
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getXMLDocumentStringListOfSources is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}
		
		return lresult;
	}
	
	HRESULT CaptureManagerBroker::getSourceOutputMediaType(
		std::wstring aSymbolicLink,
		DWORD aIndexStream,
		DWORD aIndexMediaType,
		IUnknown** aPtrPtrOutputMediaType)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrPtrOutputMediaType);

			CComPtrCustom<IUnknown> lMediaSource;

			LOG_INVOKE_FUNCTION(createSource, aSymbolicLink, &lMediaSource);

			LOG_INVOKE_FUNCTION(getSourceOutputMediaType, 
				lMediaSource,
				aIndexStream,
				aIndexMediaType,
				aPtrPtrOutputMediaType);

			lresult = S_OK;
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getSourceOutputMediaType is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::getSourceOutputMediaType(
		IUnknown* aPtrMediaSource,
		DWORD aIndexStream,
		DWORD aIndexMediaType,
		IUnknown** aPtrPtrOutputMediaType)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrMediaSource);

			LOG_CHECK_PTR_MEMORY(aPtrPtrOutputMediaType);
						
			CComPtrCustom<IMFMediaSource> lMediaSource;
			
			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrMediaSource, &lMediaSource);

			LOG_CHECK_PTR_MEMORY(lMediaSource);

			CComPtrCustom<IMFPresentationDescriptor> lPD;

			LOG_INVOKE_MF_METHOD(CreatePresentationDescriptor,
				lMediaSource,
				&lPD);

			LOG_CHECK_PTR_MEMORY(lPD);

			CComPtrCustom<IMFStreamDescriptor> lSD;

			BOOL aIsSelected = FALSE;

			LOG_INVOKE_MF_METHOD(GetStreamDescriptorByIndex,
				lPD,
				aIndexStream,
				&aIsSelected,
				&lSD);

			CComPtrCustom<IMFMediaTypeHandler> lHandler;

			LOG_INVOKE_MF_METHOD(GetMediaTypeHandler,
				lSD, 
				&lHandler);

			if (aIndexMediaType < 0)
			{
				lresult = E_INVALIDARG;

				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					L" - MediaType with index: ",
					aIndexMediaType,
					L" is less zero.");

				break;
			}

			DWORD lMediaTypeCount = 0;

			LOG_INVOKE_MF_METHOD(GetMediaTypeCount,
				lHandler, 
				&lMediaTypeCount);
						
			if (aIndexMediaType >= lMediaTypeCount)
			{
				lresult = E_INVALIDARG;

				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					L" - MediaType with index: ",
					aIndexMediaType,
					L" is out of range.");

				break;
			}

			CComPtrCustom<IMFMediaType> lMediaType;

			LOG_INVOKE_MF_METHOD(GetMediaTypeByIndex,
				lHandler,
				aIndexMediaType,
				&lMediaType);

			*aPtrPtrOutputMediaType = lMediaType.Detach();

			lresult = S_OK;

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getSourceOutputMediaType is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}


	HRESULT CaptureManagerBroker::createSource(
		std::wstring& aRefSymbolicLink,
		IUnknown** aPtrPtrMediaSource)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE_DESCR(aRefSymbolicLink.empty(), E_POINTER);
			
			LOG_CHECK_PTR_MEMORY(aPtrPtrMediaSource);

			CComPtrCustom<IMFMediaSource> lMediaSource;

			auto lfindResult = aRefSymbolicLink.find(L"CaptureManager///Software///Sources///");

			if (lfindResult == 0)
			{
				LOG_INVOKE_FUNCTION(CaptureSourceManager::getSource, 
					aRefSymbolicLink,
					&lMediaSource);
			}
			else
			{
				auto lmodifyStart = aRefSymbolicLink.find(L" --");

				if (lmodifyStart != std::wstring::npos)
				{
					aRefSymbolicLink = aRefSymbolicLink.substr(0, lmodifyStart);
				}

				LOG_INVOKE_FUNCTION(CaptureDeviceManager::getSource, 
					aRefSymbolicLink,
					&lMediaSource);
			}

			LOG_CHECK_PTR_MEMORY(lMediaSource);

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lMediaSource, aPtrPtrMediaSource);

			lresult = S_OK;

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createSource is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createSourceNode(
		std::wstring& aSymbolicLink,
		DWORD aIndexStream,
		DWORD aIndexMediaType,
		IUnknown** aPtrPtrTopologyNode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrPtrTopologyNode);
			
			CComPtrCustom<IUnknown> lMediaSource;

			LOG_INVOKE_FUNCTION(createSource, 
				aSymbolicLink, 
				&lMediaSource);

			LOG_CHECK_PTR_MEMORY(lMediaSource);

			LOG_INVOKE_FUNCTION(createSourceNode,
				lMediaSource,
				aIndexStream,
				aIndexMediaType,
				aPtrPtrTopologyNode);

			lresult = S_OK;

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createSourceNode is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createSourceNode(
		IUnknown* aPtrMediaSource,
		DWORD aIndexStream,
		DWORD aIndexMediaType,
		IUnknown** aPtrPtrTopologyNode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrMediaSource);

			LOG_CHECK_PTR_MEMORY(aPtrPtrTopologyNode);

			CComPtrCustom<IMFMediaSource> lMediaSource;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrMediaSource, &lMediaSource);

			LOG_CHECK_PTR_MEMORY(lMediaSource);

			CComPtrCustom<IMFTopologyNode> lNode;

			LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
				MF_TOPOLOGY_SOURCESTREAM_NODE,
				&lNode);

			LOG_CHECK_PTR_MEMORY(lNode);

			LOG_INVOKE_MF_METHOD(SetUnknown,
				lNode,
				MF_TOPONODE_SOURCE,
				aPtrMediaSource);

			CComPtrCustom<IMFPresentationDescriptor> lPD;

			LOG_INVOKE_MF_METHOD(CreatePresentationDescriptor,
				lMediaSource,
				&lPD);

			LOG_CHECK_PTR_MEMORY(lPD);

			LOG_INVOKE_MF_METHOD(SetUnknown,
				lNode,
				MF_TOPONODE_PRESENTATION_DESCRIPTOR,
				lPD);

			CComPtrCustom<IMFStreamDescriptor> lSD;

			BOOL aIsSelected = FALSE;

			LOG_INVOKE_MF_METHOD(GetStreamDescriptorByIndex, 
				lPD,
				aIndexStream,
				&aIsSelected,
				&lSD);
			
			LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::SelectStream,
				lPD,
				aIndexStream);

			LOG_INVOKE_FUNCTION(MediaFoundation::MediaFoundationManager::SetCurrentMediaType,
				lSD,
				aIndexMediaType);

			LOG_INVOKE_MF_METHOD(SetUnknown,
				lNode,
				MF_TOPONODE_STREAM_DESCRIPTOR,
				lSD);

			*aPtrPtrTopologyNode = lNode.Detach();

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createSourceNode is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::connectOutputNode(
		IUnknown *aPtrUpStreamTopologyNode,
		DWORD aUpStreamOutputIndex,
		IUnknown *aPtrDownStreamTopologyNode,
		DWORD aDownStreamInputIndex)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrUpStreamTopologyNode);

			LOG_CHECK_PTR_MEMORY(aPtrDownStreamTopologyNode);

			CComQIPtrCustom<IMFTopologyNode> lUpStreamTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrUpStreamTopologyNode, &lUpStreamTopologyNode);

			LOG_CHECK_PTR_MEMORY(lUpStreamTopologyNode);
			
			CComQIPtrCustom<IMFTopologyNode> lDownStreamTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrDownStreamTopologyNode, &lDownStreamTopologyNode);


			LOG_CHECK_PTR_MEMORY(lDownStreamTopologyNode);

			LOG_INVOKE_MF_METHOD(ConnectOutput,
				lUpStreamTopologyNode,
				aUpStreamOutputIndex,
				lDownStreamTopologyNode,
				aDownStreamInputIndex);
			
		} while (false);

		return lresult;
	}
	
	HRESULT CaptureManagerBroker::createSource(
		IUnknown* aPtrCaptureProcessor,
		IUnknown** aPtrPtrMediaSource)
	{
		using namespace pugi;
		
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrCaptureProcessor);

			LOG_CHECK_PTR_MEMORY(aPtrPtrMediaSource);

			CComPtrCustom<IInnerCaptureProcessor> lIInnerCaptureProcessor;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrCaptureProcessor, &lIInnerCaptureProcessor);
			
			CComPtrCustom<IMFMediaSource> lMediaSource;

			LOG_INVOKE_FUNCTION(CaptureSourceManager::createSource, lIInnerCaptureProcessor, &lMediaSource);

			LOG_CHECK_PTR_MEMORY(lMediaSource);
			
			CComPtrCustom<IUnknown> lUnknow;
			
			LOG_INVOKE_QUERY_INTERFACE_METHOD(lMediaSource, &lUnknow);

			LOG_CHECK_PTR_MEMORY(lUnknow);

			*aPtrPtrMediaSource = lUnknow.detach();

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createSource is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createWebCamControl(
		std::wstring& aSymbolicLink,
		Controls::WebCamControls::IWebCamKernelStreamingControl** aPtrPtrWebCamControl)
	{
		using namespace Controls::WebCamControls;

		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE_DESCR(aSymbolicLink.empty(), E_INVALIDARG);

			BOOL lIsCreatedKsControl = FALSE;

			do
			{
				CComPtrCustom<IMFMediaSource> lMediaSource;

				CComPtrCustom<IUnknown> lUnkMediaSource;

				LOG_INVOKE_FUNCTION(createSource,
					aSymbolicLink,
					&lUnkMediaSource);

				LOG_CHECK_PTR_MEMORY(lUnkMediaSource);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkMediaSource, &lMediaSource);

				LOG_CHECK_PTR_MEMORY(lMediaSource);
							   
				CComPtrCustom<IKsControl> lIKsControl;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lMediaSource, &lIKsControl);

				LOG_CHECK_PTR_MEMORY(lIKsControl);

				LOG_INVOKE_FUNCTION(WebCamKernelStreamingControlManager::createIWebCamKernelStreamingControlFromKsControl,
					lIKsControl,
					aPtrPtrWebCamControl);

				lIsCreatedKsControl = TRUE;

			} while (false);


			if(lIsCreatedKsControl == FALSE)
				LOG_INVOKE_FUNCTION(WebCamKernelStreamingControlManager::createIWebCamKernelStreamingControlFromFile,
					aSymbolicLink,
					aPtrPtrWebCamControl);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getWebCamControl is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
	
	HRESULT CaptureManagerBroker::getXMLDocumentStringOfEncoders(
		std::wstring& aRefXMLstring)
	{

		using namespace pugi;

		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			xml_document lxmlDoc;

			auto ldeclNode = lxmlDoc.append_child(node_declaration);

			ldeclNode.append_attribute(L"version") = L"1.0";

			xml_node lcommentNode = lxmlDoc.append_child(node_comment);

			lcommentNode.set_value(L"XML Document of encoders");

			auto lRootXMLElement = lxmlDoc.append_child(L"EncoderFactories");

			auto lGroup = lRootXMLElement.append_child(L"Group");

			lGroup.append_attribute(L"Title") = L"Video";

			WCHAR *lptrName = nullptr;

			lresult = StringFromCLSID(MFMediaType_Video, &lptrName);

			if (SUCCEEDED(lresult))
			{
				lGroup.append_attribute(L"GUID") = lptrName;

				CoTaskMemFree(lptrName);
			}

			std::list<Transform::Encoder::EncoderInfo> lEncoderDic;

			lEncoderDic.clear();

			LOG_INVOKE_FUNCTION(Transform::Encoder::EncoderManager::getDictionaryOfVideoEncoders,
				lEncoderDic);

			LogPrintOut::getInstance().printOutln(LogPrintOut::INFO_LEVEL, L"Amount of Video Encoders: ", lEncoderDic.size());

			for (auto lItem : lEncoderDic)
			{
				LogPrintOut::getInstance().printOutln(LogPrintOut::INFO_LEVEL, L"Name of Video Encoder: ", lItem.mName);
			}

			
			for (auto lIter : lEncoderDic)
			{
				auto lEncoder = lGroup.append_child(L"EncoderFactory");

				lEncoder.append_attribute(L"Name") = lIter.mName.c_str();

				lEncoder.append_attribute(L"Title") = lIter.mName.c_str();

				WCHAR *lptrName = nullptr;

				lresult = StringFromCLSID(lIter.mGUID, &lptrName);

				if (FAILED(lresult))
					continue;

				lEncoder.append_attribute(L"CLSID") = lptrName;

				CoTaskMemFree(lptrName);

				if (lIter.mIsStreaming == FALSE)
					lEncoder.append_attribute(L"IsStreaming") = L"FALSE";
				else
					lEncoder.append_attribute(L"IsStreaming") = L"TRUE";
			}

			lGroup = lRootXMLElement.append_child(L"Group");

			lGroup.append_attribute(L"Title") = L"Audio";

			lresult = StringFromCLSID(MFMediaType_Audio, &lptrName);

			if (SUCCEEDED(lresult))
			{
				lGroup.append_attribute(L"GUID") = lptrName;

				CoTaskMemFree(lptrName);
			}

			lEncoderDic.clear();

			LOG_INVOKE_FUNCTION(Transform::Encoder::EncoderManager::getDictionaryOfAudioEncoders,
				lEncoderDic);

			LogPrintOut::getInstance().printOutln(LogPrintOut::INFO_LEVEL, L"Amount of Audio Encoders: ", lEncoderDic.size());

			for (auto lItem : lEncoderDic)
			{
				LogPrintOut::getInstance().printOutln(LogPrintOut::INFO_LEVEL, L"Name of Audio Encoder: ", lItem.mName);
			}
						
			for (auto lIter : lEncoderDic)
			{
				auto lEncoder = lGroup.append_child(L"EncoderFactory");

				lEncoder.append_attribute(L"Name") = lIter.mName.c_str();

				lEncoder.append_attribute(L"Title") = lIter.mName.c_str();

				WCHAR *lptrName = nullptr;

				lresult = StringFromCLSID(lIter.mGUID, &lptrName);

				if (FAILED(lresult))
					continue;

				lEncoder.append_attribute(L"CLSID") = lptrName;

				CoTaskMemFree(lptrName);

				if (lIter.mIsStreaming == FALSE)
					lEncoder.append_attribute(L"IsStreaming") = L"FALSE";
				else
					lEncoder.append_attribute(L"IsStreaming") = L"TRUE";
			}

			std::wstringstream lwstringstream;

			lxmlDoc.print(lwstringstream);

			aRefXMLstring = lwstringstream.str();

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getXMLDocumentStringOfVideoEncoders is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}
		
		return lresult;
	}

	HRESULT CaptureManagerBroker::getXMLDocumentStringEncoderMediaTypes(
		IUnknown* aPtrInputMediaType,
		REFGUID aEncoderCLSID,
		std::wstring& aRefXMLstring)
	{
		using namespace pugi;

		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrInputMediaType);
	
			CComQIPtrCustom<IMFMediaType> lInputType = aPtrInputMediaType;

			LOG_CHECK_PTR_MEMORY(lInputType);
			
			xml_document lxmlDoc;

			auto ldeclNode = lxmlDoc.append_child(node_declaration);

			ldeclNode.append_attribute(L"version") = L"1.0";

			xml_node lcommentNode = lxmlDoc.append_child(node_comment);

			lcommentNode.set_value(L"XML Document of Encoder MediaTypes");

			auto lRootXMLElement = lxmlDoc.append_child(L"EncoderMediaTypes");

			do
			{
				auto lmode = EncodingSettings::EncodingMode::VBR;

				EncodingSettings lEncodingSettings;

				lEncodingSettings.mEncodingMode = lmode;

				lEncodingSettings.mEncodingModeValue = 75;

				std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

				LOG_INVOKE_FUNCTION(Transform::Encoder::EncoderManager::enumEncoderMediaTypes,
					lInputType,
					aEncoderCLSID,
					lListOfMediaTypes,
					lEncodingSettings);
				
				auto lGroupXMLElement = lRootXMLElement.append_child(L"Group");

				auto lModeGUID = Transform::Encoder::EncoderManager::getModeGUID(lmode);

				lGroupXMLElement.append_attribute(L"Title") = Transform::Encoder::EncoderManager::getModeTitle(lmode).c_str();

				std::wstring lGUIDName;

				LOG_INVOKE_FUNCTION(DataParser::GetGUIDName,
					lModeGUID, 
					lGUIDName);
				
				lGroupXMLElement.append_attribute(L"GUID") = lGUIDName.c_str();


				auto lMediaTypesXMLElement = lGroupXMLElement.append_child(L"MediaTypes");

				lMediaTypesXMLElement.append_attribute(L"TypeCount") = lListOfMediaTypes.size();

				for (decltype(lListOfMediaTypes.size()) lIndex = 0;
					lIndex < lListOfMediaTypes.size();
					lIndex++)
				{
					auto lNode = lMediaTypesXMLElement.append_child(L"MediaType");

					lNode.append_attribute(L"Index") = lIndex;

					lresult = DataParser::readMediaType(
						lListOfMediaTypes[lIndex],
						lNode);

					if (FAILED(lresult))
					{
						lMediaTypesXMLElement.remove_child(lNode);
					}
				}

			} while (false);

			do
			{
				auto lmode = EncodingSettings::EncodingMode::CBR;

				EncodingSettings lEncodingSettings;

				lEncodingSettings.mEncodingMode = lmode;

				lEncodingSettings.mEncodingModeValue = lEncodingSettings.mAverageBitrateValue;

				std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

				LOG_INVOKE_FUNCTION(Transform::Encoder::EncoderManager::enumEncoderMediaTypes,
					lInputType,
					aEncoderCLSID,
					lListOfMediaTypes,
					lEncodingSettings);
				
				auto lGroupXMLElement = lRootXMLElement.append_child(L"Group");

				auto lModeGUID = Transform::Encoder::EncoderManager::getModeGUID(lmode);

				lGroupXMLElement.append_attribute(L"Title") = Transform::Encoder::EncoderManager::getModeTitle(lmode).c_str();

				std::wstring lGUIDName;

				LOG_INVOKE_FUNCTION(DataParser::GetGUIDName,
					lModeGUID,
					lGUIDName);

				lGroupXMLElement.append_attribute(L"GUID") = lGUIDName.c_str();


				auto lMediaTypesXMLElement = lGroupXMLElement.append_child(L"MediaTypes");

				lMediaTypesXMLElement.append_attribute(L"TypeCount") = lListOfMediaTypes.size();

				for (decltype(lListOfMediaTypes.size()) lIndex = 0;
					lIndex < lListOfMediaTypes.size();
					lIndex++)
				{
					auto lNode = lMediaTypesXMLElement.append_child(L"MediaType");

					lNode.append_attribute(L"Index") = lIndex;

					lresult = DataParser::readMediaType(
						lListOfMediaTypes[lIndex],
						lNode);

					if (FAILED(lresult))
					{
						lMediaTypesXMLElement.remove_child(lNode);
					}
				}
			} while (false);

			do
			{
				auto lmode = EncodingSettings::EncodingMode::StreamingCBR;

				EncodingSettings lEncodingSettings;

				lEncodingSettings.mEncodingMode = lmode;

				lEncodingSettings.mEncodingModeValue = lEncodingSettings.mAverageBitrateValue;

				std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

				LOG_INVOKE_FUNCTION(Transform::Encoder::EncoderManager::enumEncoderMediaTypes,
					lInputType,
					aEncoderCLSID,
					lListOfMediaTypes,
					lEncodingSettings);

				auto lGroupXMLElement = lRootXMLElement.append_child(L"Group");

				auto lModeGUID = Transform::Encoder::EncoderManager::getModeGUID(lmode);

				lGroupXMLElement.append_attribute(L"Title") = Transform::Encoder::EncoderManager::getModeTitle(lmode).c_str();

				std::wstring lGUIDName;

				LOG_INVOKE_FUNCTION(DataParser::GetGUIDName,
					lModeGUID,
					lGUIDName);

				lGroupXMLElement.append_attribute(L"GUID") = lGUIDName.c_str();

				UINT32 lMaxBitRate;
				UINT32 lMinBitRate;

				LOG_INVOKE_FUNCTION(Transform::Encoder::EncoderManager::getMaxMinBitRate,
					aEncoderCLSID,
					lMaxBitRate,
					lMinBitRate);

				wchar_t l_buffer[MAXBYTE];

				ZeroMemory(l_buffer, MAXBYTE);

				_itow_s(lMaxBitRate, l_buffer, 10);

				lGroupXMLElement.append_attribute(L"MaxBitRate") = l_buffer;

				ZeroMemory(l_buffer, MAXBYTE);

				_itow_s(lMinBitRate, l_buffer, 10);

				lGroupXMLElement.append_attribute(L"MinBitRate") = l_buffer;
								
				auto lMediaTypesXMLElement = lGroupXMLElement.append_child(L"MediaTypes");

				lMediaTypesXMLElement.append_attribute(L"TypeCount") = lListOfMediaTypes.size();

				for (decltype(lListOfMediaTypes.size()) lIndex = 0;
					lIndex < lListOfMediaTypes.size();
					lIndex++)
				{
					auto lNode = lMediaTypesXMLElement.append_child(L"MediaType");

					lNode.append_attribute(L"Index") = lIndex;

					lresult = DataParser::readMediaType(
						lListOfMediaTypes[lIndex],
						lNode);

					if (FAILED(lresult))
					{
						lMediaTypesXMLElement.remove_child(lNode);
					}
				}
			} while (false);

			if (FAILED(lresult))
			{
				lresult = S_OK;
			}

			std::wstringstream lwstringstream;

			lxmlDoc.print(lwstringstream);

			aRefXMLstring = lwstringstream.str();

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: enumEncoderMediaTypes is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
	
	HRESULT CaptureManagerBroker::getEncodingSettingsMode(
		REFGUID aRefEncodingModeGUID,
		EncodingSettings::EncodingMode& aRefEncodingMode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;
			
			auto lEncodingMode = Transform::Encoder::EncoderManager::getEncodingMode(aRefEncodingModeGUID);

			LOG_CHECK_STATE_DESCR((lEncodingMode == EncodingSettings::EncodingMode::NONE), E_INVALIDARG);
	
			aRefEncodingMode = lEncodingMode;

			lresult = S_OK;

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getEncodingSettingsMode is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createCompressedMediaType(
		IUnknown* aPtrUncompressedMediaType,
		EncodingSettings aEncodingSettings,
		REFGUID aEncoderCLSID,
		DWORD lIndexCompressedMediaType,
		IUnknown** aPtrPtrCompressedMediaType)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);

			LOG_CHECK_PTR_MEMORY(aPtrPtrCompressedMediaType);

			CComQIPtrCustom<IMFMediaType> lInputType = aPtrUncompressedMediaType;

			LOG_CHECK_PTR_MEMORY(lInputType);

			GUID lMajorType;

			LOG_INVOKE_MF_METHOD(GetGUID,
				lInputType,
				MF_MT_MAJOR_TYPE,
				&lMajorType);
			
			GUID lCategory;

			if (lMajorType == MFMediaType_Video)
			{
				lCategory = MFT_CATEGORY_VIDEO_ENCODER;
			}
			else if (lMajorType == MFMediaType_Audio)
			{
				lCategory = MFT_CATEGORY_AUDIO_ENCODER;
			}
			else
			{
				lresult = E_INVALIDARG;
			}

			LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

			CComPtrCustom<IMFMediaType> lCompressedMediaType;

			LOG_INVOKE_FUNCTION(Transform::Encoder::EncoderManager::getCompressedMediaType,
				lInputType,
				aEncodingSettings,
				lCategory,
				aEncoderCLSID,
				lIndexCompressedMediaType,
				&lCompressedMediaType);
			
			*aPtrPtrCompressedMediaType = lCompressedMediaType.Detach();

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createCompressedMediaType is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createEncoderNode(
		IUnknown *aPtrUncompressedMediaType,
		EncodingSettings aEncodingSettings,
		REFGUID aEncoderCLSID,
		DWORD lIndexOutputMediaType,
		IUnknown* aPtrOutputNode,
		IUnknown** aPtrPtrEncoderNode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrUncompressedMediaType);

			LOG_CHECK_PTR_MEMORY(aPtrOutputNode);

			LOG_CHECK_PTR_MEMORY(aPtrPtrEncoderNode);

			CComQIPtrCustom<IMFTopologyNode> lOutputNode = aPtrOutputNode;

			LOG_CHECK_PTR_MEMORY(lOutputNode);
			
			CComQIPtrCustom<IMFMediaType> lUncompressedInputType = aPtrUncompressedMediaType;

			LOG_CHECK_PTR_MEMORY(lUncompressedInputType);
			
			CComPtrCustom<IMFTopologyNode> lEncoderTopologyNode;

			LOG_INVOKE_FUNCTION(Transform::Encoder::EncoderManager::createEncoderTopologyNode,
				lUncompressedInputType,
				aEncodingSettings,
				aEncoderCLSID,
				lIndexOutputMediaType,
				&lEncoderTopologyNode);
			
			LOG_INVOKE_FUNCTION(connectOutputNode,
				lEncoderTopologyNode, 
				0,
				lOutputNode,
				0);
			
			*aPtrPtrEncoderNode = lEncoderTopologyNode.Detach();

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createEncoderNode is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createSampleGrabberOutputNode(
		REFGUID aRefMajorType,
		REFGUID aRefSubType,
		std::wstring& aReadMode,
		LONG aSampleByteSize,
		ISampleGrabberCallInner** aPtrPtrISampleGrabber)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrPtrISampleGrabber);

			LOG_INVOKE_FUNCTION(Sinks::SampleGrabberCallFactory::createSampleGrabberOutputNode, 
				aRefMajorType,
				aRefSubType,
				aReadMode,
				aSampleByteSize,
				aPtrPtrISampleGrabber);
						
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createEncoderNode is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;		
	}

	HRESULT CaptureManagerBroker::createSampleGrabberOutputNode(
		REFGUID aRefMajorType,
		REFGUID aRefSubType,
		IUnknown* aPtrISampleGrabberCallback,
		IUnknown** aPtrPtrTopologyNode)
	{

		class SampleGrabberSink :
			public BaseUnknown < IMFSampleGrabberSinkCallback2 >
		{
		protected:

			virtual bool findInterface(
				REFIID aRefIID,
				void** aPtrPtrVoidObject)
			{
				if (aRefIID == __uuidof(IMFSampleGrabberSinkCallback2))
				{
					return castInterfaces(
						aRefIID,
						aPtrPtrVoidObject,
						static_cast<IMFSampleGrabberSinkCallback2*>(this));
				}
				else if (aRefIID == __uuidof(IMFClockStateSink))
				{
					return castInterfaces(
						aRefIID,
						aPtrPtrVoidObject,
						static_cast<IMFClockStateSink*>(this));
				}
				else
				{
					return BaseUnknown::findInterface(
						aRefIID,
						aPtrPtrVoidObject);
				}
			}

		public:

			SampleGrabberSink(ISampleGrabberCallbackInner* aSampleGrabberCallback = nullptr)
			{
				mSampleGrabberCallback = aSampleGrabberCallback;
			}

			virtual ~SampleGrabberSink(){}

			STDMETHODIMP OnClockStart(
				MFTIME aHnsSystemTime,
				LONGLONG aClockStartOffset)
			{
				return S_OK;
			}

			STDMETHODIMP OnClockStop(
				MFTIME aHnsSystemTime)
			{
				return S_OK;
			}

			STDMETHODIMP OnClockPause(
				MFTIME aHnsSystemTime)
			{
				return S_OK;
			}

			STDMETHODIMP OnClockRestart(
				MFTIME aHnsSystemTime)
			{
				return S_OK;
			}

			STDMETHODIMP OnClockSetRate(
				MFTIME aHnsSystemTime,
				float aRate)
			{
				return S_OK;
			}

			STDMETHODIMP OnSetPresentationClock(
				IMFPresentationClock* aPtrClock)
			{
				return S_OK;
			}

			STDMETHODIMP OnProcessSample(
				REFGUID aGUIDMajorMediaType,
				DWORD aSampleFlags,
				LONGLONG aSampleTime,
				LONGLONG aSampleDuration,
				const BYTE* aPtrSampleBuffer,
				DWORD aSampleSize)
			{
				mSampleGrabberCallback->operator()(
					aGUIDMajorMediaType,
					aSampleFlags,
					aSampleTime,
					aSampleDuration,
					aPtrSampleBuffer,
					aSampleSize);

				return S_OK;
			}


			STDMETHODIMP OnProcessSampleEx(
				/* [in] */ REFGUID aGUIDMajorMediaType,
				/* [in] */ DWORD aSampleFlags,
				/* [in] */ LONGLONG aSampleTime,
				/* [in] */ LONGLONG aSampleDuration,
				/* [annotation][in] */
				_In_reads_bytes_(dwSampleSize)  const BYTE *aPtrSampleBuffer,
				/* [in] */ DWORD aSampleSize,
				/* [annotation][in] */
				_In_  IMFAttributes *pAttributes)
			{
				if (pAttributes != nullptr)
				{
					UINT32 lBlobSize = 0;

					auto lresult = pAttributes->GetBlobSize(CM_HEADER, &lBlobSize);					

					if (SUCCEEDED(lresult) && lBlobSize > 0)
					{
						UINT8 * lPtrBlob = nullptr;

						auto lresult = pAttributes->GetAllocatedBlob(CM_HEADER, &lPtrBlob, &lBlobSize);

						mSampleGrabberCallback->operator()(
							aGUIDMajorMediaType,
							HeaderFlag,
							aSampleTime,
							aSampleDuration,
							lPtrBlob,
							lBlobSize);

						++aSampleTime;

						CoTaskMemFree(lPtrBlob);
					}
				}


				mSampleGrabberCallback->operator()(
					aGUIDMajorMediaType,
					aSampleFlags,
					aSampleTime,
					aSampleDuration,
					aPtrSampleBuffer,
					aSampleSize);

				return S_OK;
			}

			STDMETHODIMP OnShutdown()
			{
				return S_OK;
			}


		private:

			CComPtrCustom<ISampleGrabberCallbackInner> mSampleGrabberCallback;
		};

		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrISampleGrabberCallback);

			LOG_CHECK_PTR_MEMORY(aPtrPtrTopologyNode);
			
			CComPtrCustom<IMFActivate> lSampleGrabberSinkActivate;

			CComPtrCustom<IMFMediaType> lMediaType;

			CComPtrCustom<IMFTopologyNode> lSinkNode;

			LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
				&lMediaType);

			LOG_INVOKE_MF_METHOD(SetGUID,
				lMediaType, 
				MF_MT_MAJOR_TYPE, 
				aRefMajorType);

			LOG_INVOKE_MF_METHOD(SetGUID,
				lMediaType, 
				MF_MT_SUBTYPE, 
				aRefSubType);

			CComQIPtrCustom<ISampleGrabberCallbackInner> lPtrISampleGrabberCallback;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrISampleGrabberCallback, &lPtrISampleGrabberCallback);

			CComPtrCustom<SampleGrabberSink> lSinkCallback(new(std::nothrow) SampleGrabberSink(lPtrISampleGrabberCallback));

			LOG_CHECK_PTR_MEMORY(lSinkCallback);

			LOG_INVOKE_MF_FUNCTION(MFCreateSampleGrabberSinkActivate,
				lMediaType,
				lSinkCallback,
				&lSampleGrabberSinkActivate);

			LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
				MF_TOPOLOGY_OUTPUT_NODE,
				&lSinkNode);

			LOG_INVOKE_MF_METHOD(SetObject,
				lSinkNode,
				lSampleGrabberSinkActivate);

			LOG_INVOKE_MF_METHOD(SetUINT32,
				lSinkNode,
				MF_TOPONODE_STREAMID,
				0);

			LOG_INVOKE_MF_METHOD(SetUINT32,
				lSinkNode,
				MF_TOPONODE_NOSHUTDOWN_ON_REMOVE,
				FALSE);

			LOG_INVOKE_MF_METHOD(SetUINT32,
				lSinkNode,
				MF_MT_SAMPLEGRABBER,
				TRUE);

			*aPtrPtrTopologyNode = lSinkNode.Detach();

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createSampleGrabberOutputNode is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::GetOutputFactoryCollection(
		SinkType aSinkType,
		std::vector<GUIDToNamePair>& mContainers)
	{
		
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_INVOKE_FUNCTION(Singleton<Sinks::OutputNodeFactoryCollection>::getInstance().getGUIDToNamePairs,
				aSinkType,
				mContainers);
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: GetOutputFactoryCollection is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::getSampleAccumulatorNodeFactoryCollection(
		pugi::xml_node& aRefRootXMLElement,
		std::vector<StreamControlInfo>& aCollection)

	{

		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_INVOKE_FUNCTION(Singleton<Transform::SampleAccumulatorNodeFactory>::getInstance().getSampleAccumulatorNodeFactoryCollection,
				aRefRootXMLElement,
				aCollection);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getSampleAccumulatorNodeFactoryCollection is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::getMixerNodeFactoryCollection(
		pugi::xml_node& aRefRootXMLElement,
		std::vector<StreamControlInfo>& aCollection)

	{

		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_INVOKE_FUNCTION(
				Singleton<MediaSession::CustomisedMediaSession::CustomisedMixerNodeFactory>::getInstance().getCustomisedMixerNodeFactoryCollection,
				aRefRootXMLElement,
				aCollection);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getMixerNodeFactoryCollection is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}


	



	HRESULT CaptureManagerBroker::createOutputNodeFactory(
		REFGUID aRefContainerTypeGUID,
		IUnknown** aPtrPtrOutputNodeFactory)
	{

		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_INVOKE_FUNCTION(Singleton<Sinks::OutputNodeFactoryCollection>::getInstance().createOutputNodeFactory,
				aRefContainerTypeGUID,
				aPtrPtrOutputNodeFactory);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createOutputNodeFactory is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
	
	HRESULT CaptureManagerBroker::createSessionDescriptor(
		std::vector<IUnknown*> lSourceNodes,
		IUnknown* aPtrISessionCallback,
		SessionDescriptor& aRefSessionDescriptor)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE(lSourceNodes.empty());

			LOG_CHECK_PTR_MEMORY(aPtrISessionCallback);
			
			lresult = S_OK;

			std::vector<IMFTopologyNode*> lTopologyNodeSourceList;

			for (size_t lIndex = 0; lIndex < lSourceNodes.size(); lIndex++)
			{

				CComQIPtrCustom<IMFTopologyNode> lSourceNode(lSourceNodes[lIndex]);

				if (!lSourceNode)
				{
					lresult = E_NOINTERFACE;

					break;
				}
								
				lTopologyNodeSourceList.push_back(lSourceNode);
			}

			LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

			LOG_INVOKE_FUNCTION(Singleton<SessionProcessorManager>::getInstance().createSession,
				lTopologyNodeSourceList,
				aPtrISessionCallback,
				aRefSessionDescriptor);
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createSessionDescriptor is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
	
	// get XML form of media type
	HRESULT CaptureManagerBroker::getXMLDocumentStringOfMediaType(
		IUnknown* aPtrMediaType,
		std::wstring& aRefXMLDocumentString)
	{
		using namespace pugi;

		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE_DESCR(aPtrMediaType == nullptr, E_INVALIDARG);
			
			xml_document lxmlDoc;

			auto ldeclNode = lxmlDoc.append_child(node_declaration);

			ldeclNode.append_attribute(L"version") = L"1.0";

			xml_node lcommentNode = lxmlDoc.append_child(node_comment);

			lcommentNode.set_value(L"XML Document of media type");

			auto lRootXMLElement = lxmlDoc.append_child(L"MediaType");

			LOG_INVOKE_FUNCTION(DataParser::readMediaType,
				aPtrMediaType,
				lRootXMLElement);
			
			std::wstringstream lwstringstream;

			lxmlDoc.print(lwstringstream);

			aRefXMLDocumentString = lwstringstream.str();

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getXMLDocumentStringOfMediaType is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
	
	HRESULT CaptureManagerBroker::getStrideForBitmapInfoHeader(
		const GUID aMFVideoFormat,
		DWORD aWidthInPixels,
		LONG& aRefStride)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE(aWidthInPixels == 0);

			LOG_INVOKE_MF_FUNCTION(MFGetStrideForBitmapInfoHeader,
				aMFVideoFormat.Data1,
				aWidthInPixels,
				&aRefStride);
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getStrideForBitmapInfoHeader is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createSpreaderNode(
		std::vector<IUnknown*> aArrayDownStreamTopologyNodes,
		IUnknown** aPtrPtrTopologySpreaderNode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrPtrTopologySpreaderNode);
			
			LOG_CHECK_STATE(aArrayDownStreamTopologyNodes.empty());
			
			CComPtrCustom<IMFTopologyNode> lTeeNode;

			LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
				MF_TOPOLOGY_TEE_NODE,
				&lTeeNode);
			
			UINT32 lDownStreamTopologyNodeCount = 0;

			for (auto& litem : aArrayDownStreamTopologyNodes)
			{
				CComQIPtrCustom<IMFTopologyNode> lDownStreamTopologyNode = litem;
				
				LOG_CHECK_PTR_MEMORY(lDownStreamTopologyNode);
				
				LOG_INVOKE_MF_METHOD(ConnectOutput,
					lTeeNode,
					lDownStreamTopologyNodeCount++,
					lDownStreamTopologyNode,
					0);
			}

			if (FAILED(lresult))
				break;

			*aPtrPtrTopologySpreaderNode = lTeeNode.detach();

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createSpreaderNode is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createSampleAccumulatorNode(
		REFIID aREFIID,
		std::vector<IUnknown*> aArrayDownStreamTopologyNodes,
		IUnknown** aPtrPtrTopologySampleAccumulatorNode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrPtrTopologySampleAccumulatorNode);

			LOG_CHECK_STATE(aArrayDownStreamTopologyNodes.empty());
						
			LOG_INVOKE_FUNCTION(Singleton<Transform::SampleAccumulatorNodeFactory>::getInstance().createSampleAccumulatorNode,
				aREFIID, 
				aArrayDownStreamTopologyNodes,
				aPtrPtrTopologySampleAccumulatorNode);
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createSampleAccumulatorNode is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
	
	HRESULT CaptureManagerBroker::startSession(
		SessionDescriptor& aRefSessionDescriptor,
		LONGLONG aStartPositionInHundredNanosecondUnits,
		const GUID aGUIDTimeFormat)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE(aRefSessionDescriptor < 0);
		
			LOG_INVOKE_FUNCTION(Singleton<SessionProcessorManager>::getInstance().startSession,
				aRefSessionDescriptor,
				aStartPositionInHundredNanosecondUnits,
				aGUIDTimeFormat);
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: startSession is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::stopSession(
		SessionDescriptor& aRefSessionDescriptor)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE(aRefSessionDescriptor < 0);

			LOG_INVOKE_FUNCTION(Singleton<SessionProcessorManager>::getInstance().stopSession,
				aRefSessionDescriptor);
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: stopSession is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::pauseSession(
		SessionDescriptor& aRefSessionDescriptor)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE(aRefSessionDescriptor < 0);

			LOG_INVOKE_FUNCTION(Singleton<SessionProcessorManager>::getInstance().pauseSession,
				aRefSessionDescriptor);
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: stopSession is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::closeSession(
		SessionDescriptor& aRefSessionDescriptor)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE(aRefSessionDescriptor < 0);

			LOG_INVOKE_FUNCTION(Singleton<SessionProcessorManager>::getInstance().closeSession,
				aRefSessionDescriptor);
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: stopSession is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
	
	void CaptureManagerBroker::getVideoRendererModes(
		BOOL aMultiOutputNodeVideoRenderer,
		std::vector<GUIDToNamePair>& aRefReadModes)
	{
		Singleton<Sinks::VideoRendererFactory>::getInstance().getReadModes(
			aMultiOutputNodeVideoRenderer,
			aRefReadModes);
	}
		
	HRESULT CaptureManagerBroker::createVideoRendererOutputNodes(
		HANDLE aHandle,
		IUnknown *aPtrUnkTarget,
		DWORD aOutputNodeAmount,
		std::vector<CComPtrCustom<IUnknown>>& aRefOutputNodes)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;
			
			LOG_CHECK_STATE_DESCR(aOutputNodeAmount == 0, E_INVALIDARG);

			LOG_INVOKE_FUNCTION(Singleton<Sinks::VideoRendererFactory>::getInstance().createRendererOutputNodes,
				aHandle,
				aPtrUnkTarget,
				aOutputNodeAmount,
				aRefOutputNodes);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createVideoRendererOutputNodes is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createMixerNodes(
		IUnknown *aPtrDownStreamTopologyNode,
		DWORD aInputNodeAmount,
		std::vector<CComPtrCustom<IUnknown>>& aRefInputNodes)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;
			
			LOG_CHECK_PTR_MEMORY(aPtrDownStreamTopologyNode);

			LOG_CHECK_STATE_DESCR(aInputNodeAmount == 0, E_INVALIDARG);

			LOG_INVOKE_FUNCTION(
				Singleton<MediaSession::CustomisedMediaSession::CustomisedMixerNodeFactory>::getInstance().createCustomisedMixerNodes,
				aPtrDownStreamTopologyNode,
				aInputNodeAmount,
				aRefInputNodes);
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createMixerNodes is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createEVRMultiSinkOutputNodes(
		HANDLE aHandle,
		IUnknown *aPtrUnkTarget,
		DWORD aOutputNodeAmount,
		std::vector<CComPtrCustom<IUnknown>>& aRefOutputNodes)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE_DESCR(aOutputNodeAmount == 0, E_INVALIDARG);

			LOG_INVOKE_FUNCTION(Singleton<EVRMultiSink::Sinks::VideoRendererFactory>::getInstance().createRendererOutputNodes,
				aHandle,
				aPtrUnkTarget,
				aOutputNodeAmount,
				aRefOutputNodes);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createEVRMultiSinkOutputNodes is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::createSARSinkOutputNode(
		CComPtrCustom<IUnknown>& aRefOutputNode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;
			
			LOG_INVOKE_FUNCTION(Singleton<Sinks::AudioRendererFactory>::getInstance().createRendererOutputNode,
				aRefOutputNode);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createSARSinkOutputNode is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	void CaptureManagerBroker::getEVRMultiSinkModes(
		std::vector<GUIDToNamePair>& aRefReadModes)
	{
		Singleton<EVRMultiSink::Sinks::VideoRendererFactory>::getInstance().getReadModes(
			aRefReadModes);
	}

	HRESULT CaptureManagerBroker::setEVRStreamPosition(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ FLOAT aLeft,
		/* [in] */ FLOAT aRight,
		/* [in] */ FLOAT aTop,
		/* [in] */ FLOAT aBottom)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			if (aLeft > 1.0f)
				aLeft = 1.0f;

			if (aLeft < 0.0f)
				aLeft = 0.0f;

			if (aRight > 1.0f)
				aRight = 1.0f;

			if (aRight < 0.0f)
				aRight = 0.0f;

			if (aTop > 1.0f)
				aTop = 1.0f;

			if (aTop < 0.0f)
				aTop = 0.0f;

			if (aBottom > 1.0f)
				aBottom = 1.0f;

			if (aBottom < 0.0f)
				aBottom = 0.0f;
									
			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;
						
			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);	

			do
			{
				CComPtrCustom<IDispatch> lIDispatch;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lIDispatch);

				UINT lCount = 0;

				auto name = SysAllocString(OLESTR("setPosition"));
			
				DISPID lDISPID = 0;

				lresult = lIDispatch->GetIDsOfNames(GUID_NULL, &name, 1, 0, &lDISPID);

				DISPPARAMS param;

				VARIANTARG args[4];

				param.rgvarg = &args[0];

				VARIANT result;

				VariantInit(&result);

				

				VariantInit(&args[3]);
				V_VT(&args[3]) = VT_R4;
				V_R4(&args[3]) = aLeft;

				VariantInit(&args[2]);
				V_VT(&args[2]) = VT_R4;
				V_R4(&args[2]) = aRight;

				VariantInit(&args[1]);
				V_VT(&args[1]) = VT_R4;
				V_R4(&args[1]) = aTop;

				VariantInit(&args[0]);
				V_VT(&args[0]) = VT_R4;
				V_R4(&args[0]) = aBottom;

				param.cArgs = 4;

				param.cNamedArgs = 0;

				param.rgdispidNamedArgs = nullptr;

				lresult = lIDispatch->Invoke(lDISPID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &param, &result, NULL, NULL);

				SysFreeString(name);				
			} while (false);

			if (FAILED(lresult))
			{

				CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

				LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

				LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, setPosition,
					aLeft,
					aRight,
					aTop,
					aBottom);

			}
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setEVRStreamPosition is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::setEVRStreamSrcPosition(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ FLOAT aLeft,
		/* [in] */ FLOAT aRight,
		/* [in] */ FLOAT aTop,
		/* [in] */ FLOAT aBottom)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			if (aLeft > 1.0f)
				aLeft = 1.0f;

			if (aLeft < 0.0f)
				aLeft = 0.0f;

			if (aRight > 1.0f)
				aRight = 1.0f;

			if (aRight < 0.0f)
				aRight = 0.0f;

			if (aTop > 1.0f)
				aTop = 1.0f;

			if (aTop < 0.0f)
				aTop = 0.0f;

			if (aBottom > 1.0f)
				aBottom = 1.0f;

			if (aBottom < 0.0f)
				aBottom = 0.0f;

			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			do
			{
				CComPtrCustom<IDispatch> lIDispatch;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lIDispatch);

				UINT lCount = 0;

				auto name = SysAllocString(OLESTR("setSrcPosition"));

				DISPID lDISPID = 0;

				lresult = lIDispatch->GetIDsOfNames(GUID_NULL, &name, 1, 0, &lDISPID);

				DISPPARAMS param;

				VARIANTARG args[4];

				param.rgvarg = &args[0];

				VARIANT result;

				VariantInit(&result);



				VariantInit(&args[3]);
				V_VT(&args[3]) = VT_R4;
				V_R4(&args[3]) = aLeft;

				VariantInit(&args[2]);
				V_VT(&args[2]) = VT_R4;
				V_R4(&args[2]) = aRight;

				VariantInit(&args[1]);
				V_VT(&args[1]) = VT_R4;
				V_R4(&args[1]) = aTop;

				VariantInit(&args[0]);
				V_VT(&args[0]) = VT_R4;
				V_R4(&args[0]) = aBottom;

				param.cArgs = 4;

				param.cNamedArgs = 0;

				param.rgdispidNamedArgs = nullptr;

				lresult = lIDispatch->Invoke(lDISPID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &param, &result, NULL, NULL);

				SysFreeString(name);

			} while (false);
			
			if (FAILED(lresult))
			{

				CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

				LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

				LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, setSrcPosition,
					aLeft,
					aRight,
					aTop,
					aBottom);
			}
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setEVRStreamPosition is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::getEVRStreamSrcPosition(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ FLOAT* aPtrLeft,
		/* [in] */ FLOAT* aPtrRight,
		/* [in] */ FLOAT* aPtrTop,
		/* [in] */ FLOAT* aPtrBottom)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			LOG_CHECK_PTR_MEMORY(aPtrLeft);

			LOG_CHECK_PTR_MEMORY(aPtrRight);

			LOG_CHECK_PTR_MEMORY(aPtrTop);

			LOG_CHECK_PTR_MEMORY(aPtrBottom);

			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

			LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

			LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, getSrcPosition,
				aPtrLeft,
				aPtrRight,
				aPtrTop,
				aPtrBottom);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setEVRStreamPosition is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::setEVRStreamOpacity(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ FLOAT aOpacity)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			if (aOpacity > 1.0f)
				aOpacity = 1.0f;

			if (aOpacity < 0.0f)
				aOpacity = 0.0f;
			
			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			do
			{
				CComPtrCustom<IDispatch> lIDispatch;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lIDispatch);

				UINT lCount = 0;

				auto name = SysAllocString(OLESTR("setOpacity"));

				DISPID lDISPID = 0;

				lresult = lIDispatch->GetIDsOfNames(GUID_NULL, &name, 1, 0, &lDISPID);

				DISPPARAMS param;

				VARIANTARG args[1];

				param.rgvarg = &args[0];

				VARIANT result;

				VariantInit(&result);

				VariantInit(&args[0]);
				V_VT(&args[0]) = VT_R4;
				V_R4(&args[0]) = aOpacity;

				param.cArgs = 1;

				param.cNamedArgs = 0;

				param.rgdispidNamedArgs = nullptr;

				lresult = lIDispatch->Invoke(lDISPID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &param, &result, NULL, NULL);

				SysFreeString(name);

			} while (false);

			if (FAILED(lresult))
			{
				CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

				LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

				LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, setOpacity,
					aOpacity);
			}
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setEVRStreamOpacity is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::setEVRStreamFlush(
		/* [in] */ IUnknown *aPtrEVROutputNode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			do
			{
				CComPtrCustom<IDispatch> lIDispatch;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lIDispatch);

				UINT lCount = 0;

				auto name = SysAllocString(OLESTR("flush"));

				DISPID lDISPID = 0;

				lresult = lIDispatch->GetIDsOfNames(GUID_NULL, &name, 1, 0, &lDISPID);

				DISPPARAMS param;

				VARIANTARG args[1];

				param.rgvarg = &args[0];

				VARIANT result;

				VariantInit(&result);

				VariantInit(&args[0]);
				V_VT(&args[0]) = VT_R4;
				V_R4(&args[0]) = 0;

				param.cArgs = 0;

				param.cNamedArgs = 0;

				param.rgdispidNamedArgs = nullptr;

				lresult = lIDispatch->Invoke(lDISPID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &param, &result, NULL, NULL);

				SysFreeString(name);

			} while (false);

			if (FAILED(lresult))
			{
				CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

				LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

				LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, flush);
			}

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setEVRStreamFlush is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::getCollectionOfFilters(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [out] */ BSTR *aPtrPtrXMLstring)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			LOG_CHECK_PTR_MEMORY(aPtrPtrXMLstring);
			
			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

			LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

			LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, getCollectionOfFilters,
				aPtrPtrXMLstring);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getCollectionOfFilters is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::setFilterParametr(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ DWORD aParametrIndex,
		/* [in] */ LONG aNewValue,
		/* [in] */ BOOL aIsEnabled)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);
			
			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

			LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

			LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, setFilterParametr,
				aParametrIndex,
				aNewValue,
				aIsEnabled);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setFilterParametr is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::getCollectionOfOutputFeatures(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [out] */ BSTR *aPtrPtrXMLstring)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

			LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

			LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, getCollectionOfOutputFeatures,
				aPtrPtrXMLstring);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getCollectionOfOutputFeatures is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}


	HRESULT CaptureManagerBroker::setOutputFeatureParametr(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ DWORD aParametrIndex,
		/* [in] */ LONG aNewValue)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

			LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

			LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, setOutputFeatureParametr,
				aParametrIndex,
				aNewValue);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setFilterParametr is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::setEVRStreamZOrder(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ DWORD aZOrder)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;
			
			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			do
			{
				CComPtrCustom<IDispatch> lIDispatch;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lIDispatch);

				UINT lCount = 0;

				auto name = SysAllocString(OLESTR("setZOrder"));

				DISPID lDISPID = 0;

				lresult = lIDispatch->GetIDsOfNames(GUID_NULL, &name, 1, 0, &lDISPID);

				DISPPARAMS param;

				VARIANTARG args[1];

				param.rgvarg = &args[0];

				VARIANT result;

				VariantInit(&result);

				VariantInit(&args[0]);
				V_VT(&args[0]) = VT_UI4;
				V_UI4(&args[0]) = aZOrder;

				param.cArgs = 1;

				param.cNamedArgs = 0;

				param.rgdispidNamedArgs = nullptr;

				lresult = lIDispatch->Invoke(lDISPID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &param, &result, NULL, NULL);

				SysFreeString(name);

			} while (false);

			if (FAILED(lresult))
			{
				CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

				LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

				LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, setZOrder,
					aZOrder);
			}

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setEVRStreamZOrder is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}


	HRESULT CaptureManagerBroker::getEVRStreamPosition(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ FLOAT* aPtrLeft,
		/* [in] */ FLOAT* aPtrRight,
		/* [in] */ FLOAT* aPtrTop,
		/* [in] */ FLOAT* aPtrBottom)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			LOG_CHECK_PTR_MEMORY(aPtrLeft);

			LOG_CHECK_PTR_MEMORY(aPtrRight);

			LOG_CHECK_PTR_MEMORY(aPtrTop);

			LOG_CHECK_PTR_MEMORY(aPtrBottom);

			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

			LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

			LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, getPosition,
				aPtrLeft,
				aPtrRight,
				aPtrTop,
				aPtrBottom);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getEVRStreamPosition is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::getEVRStreamOpacity(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ FLOAT* aPtrOpacity)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			LOG_CHECK_PTR_MEMORY(aPtrOpacity);
			
			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

			LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

			LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, getOpacity,
				aPtrOpacity);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getEVRStreamOpacity is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::getEVRStreamZOrder(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ DWORD* aPtrZOrder)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			LOG_CHECK_PTR_MEMORY(aPtrZOrder);

			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			do
			{
				CComPtrCustom<IDispatch> lIDispatch;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lIDispatch);

				UINT lCount = 0;

				auto name = SysAllocString(OLESTR("getZOrder"));

				DISPID lDISPID = 0;

				lresult = lIDispatch->GetIDsOfNames(GUID_NULL, &name, 1, 0, &lDISPID);

				DISPPARAMS param;

				VARIANTARG args[4];

				param.rgvarg = &args[0];

				VARIANT result;

				VariantInit(&result);



				VariantInit(&args[3]);
				V_VT(&args[3]) = VT_R4;
				V_R4(&args[3]) = 0;

				VariantInit(&args[2]);
				V_VT(&args[2]) = VT_R4;
				V_R4(&args[2]) = 0;

				VariantInit(&args[1]);
				V_VT(&args[1]) = VT_R4;
				V_R4(&args[1]) = 0;

				VariantInit(&args[0]);
				V_VT(&args[0]) = VT_R4;
				V_R4(&args[0]) = 0;

				param.cArgs = 0;

				param.cNamedArgs = 0;

				param.rgdispidNamedArgs = nullptr;

				lresult = lIDispatch->Invoke(lDISPID, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &param, &result, NULL, NULL);

				if (SUCCEEDED(lresult) && result.vt == VT_UI4)
				{
					*aPtrZOrder = result.uintVal;
				}

				SysFreeString(name);


			} while (false);
					   
			if (FAILED(lresult))
			{
				CComPtrCustom<Sinks::EVR::IEVRStreamControl> lEVRStreamControl;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lEVRStreamControl);

				LOG_CHECK_PTR_MEMORY(lEVRStreamControl);

				LOG_INVOKE_POINTER_METHOD(lEVRStreamControl, getZOrder,
					aPtrZOrder);
			}

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getEVRStreamZOrder is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
		
	HRESULT STDMETHODCALLTYPE CaptureManagerBroker::enableInnerRendering(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ BOOL aIsInnerRendering)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);
			
			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<Sinks::EVR::IRenderingControl> lRenderingControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lRenderingControl);

			LOG_CHECK_PTR_MEMORY(lRenderingControl);

			LOG_INVOKE_POINTER_METHOD(lRenderingControl, enableInnerRendering,
				aIsInnerRendering);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: enableInnerRendering is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT STDMETHODCALLTYPE CaptureManagerBroker::renderToTarget(
		/* [in] */ IUnknown *aPtrEVROutputNode,
		/* [in] */ IUnknown *aPtrRenderTarget,
		/* [in] */ BOOL aCopyMode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrEVROutputNode);

			LOG_CHECK_PTR_MEMORY(aPtrRenderTarget);

			CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrEVROutputNode, &RendererActivateTopologyNode);

			LOG_CHECK_PTR_MEMORY(RendererActivateTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, RendererActivateTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				RendererActivateTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<Sinks::EVR::IRenderingControl> lRenderingControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lRenderingControl);

			LOG_CHECK_PTR_MEMORY(lRenderingControl);

			LOG_INVOKE_POINTER_METHOD(lRenderingControl, renderToTarget,
				aPtrRenderTarget,
				aCopyMode);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: renderToTarget is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
		
	HRESULT STDMETHODCALLTYPE CaptureManagerBroker::createSwitcherNode(
		IUnknown *aPtrDownStreamTopologyNode,
		IUnknown **aPtrPtrTopologySwitcherNode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;
			
			LOG_CHECK_PTR_MEMORY(aPtrPtrTopologySwitcherNode);

			LOG_INVOKE_FUNCTION(Singleton<Transform::SwitcherNodeFactory>::getInstance().createSwitcherNode,
				aPtrDownStreamTopologyNode,
				aPtrPtrTopologySwitcherNode);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: createSwitcherNode is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::pauseSwitchers(
		SessionDescriptor& aRefSessionDescriptor)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE(aRefSessionDescriptor < 0);

			LOG_INVOKE_FUNCTION(Singleton<SessionProcessorManager>::getInstance().pauseSwitchers,
				aRefSessionDescriptor);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: pauseSwitchers is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::resumeSwitchers(
		SessionDescriptor& aRefSessionDescriptor)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE(aRefSessionDescriptor < 0);

			LOG_INVOKE_FUNCTION(Singleton<SessionProcessorManager>::getInstance().resumeSwitchers,
				aRefSessionDescriptor);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: resumeSwitchers is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::detachSwitchers(
		SessionDescriptor& aRefSessionDescriptor)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_STATE(aRefSessionDescriptor < 0);

			LOG_INVOKE_FUNCTION(Singleton<SessionProcessorManager>::getInstance().detachSwitchers,
				aRefSessionDescriptor);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: detachSwitchers is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::attachSwitcher(
		/* [in] */ IUnknown *aPtrSwitcherNode,
		/* [in] */ IUnknown *aPtrDownStreamNode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrSwitcherNode);

			LOG_CHECK_PTR_MEMORY(aPtrDownStreamNode);

			LOG_INVOKE_FUNCTION(Singleton<SessionProcessorManager>::getInstance().attachSwitcher,
				aPtrSwitcherNode,
				aPtrDownStreamNode);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: detachSwitchers is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::setVideoMixerPosition(
		/* [in] */ IUnknown *aPtrVideoMixerNode,
		/* [in] */ FLOAT aLeft,
		/* [in] */ FLOAT aRight,
		/* [in] */ FLOAT aTop,
		/* [in] */ FLOAT aBottom)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrVideoMixerNode);

			if (aLeft > 1.0f)
				aLeft = 1.0f;

			if (aLeft < 0.0f)
				aLeft = 0.0f;

			if (aRight > 1.0f)
				aRight = 1.0f;

			if (aRight < 0.0f)
				aRight = 0.0f;

			if (aTop > 1.0f)
				aTop = 1.0f;

			if (aTop < 0.0f)
				aTop = 0.0f;

			if (aBottom > 1.0f)
				aBottom = 1.0f;

			if (aBottom < 0.0f)
				aBottom = 0.0f;

			CComPtrCustom<IMFTopologyNode> lVideoMixerTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrVideoMixerNode, &lVideoMixerTopologyNode);

			LOG_CHECK_PTR_MEMORY(lVideoMixerTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, lVideoMixerTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_TRANSFORM_NODE,
				E_INVALIDARG);

			UINT32 l_isMixerNode = FALSE;

			LOG_INVOKE_MF_METHOD(GetUINT32,
				lVideoMixerTopologyNode,
				CM_MixerNode,
				&l_isMixerNode);

			LOG_CHECK_STATE_DESCR(l_isMixerNode == FALSE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				lVideoMixerTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<MediaSession::CustomisedMediaSession::IVideoMixerControl> lVideoMixerControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lVideoMixerControl);

			LOG_CHECK_PTR_MEMORY(lVideoMixerControl);

			LOG_INVOKE_POINTER_METHOD(lVideoMixerControl, setPosition,
				aLeft,
				aRight,
				aTop,
				aBottom);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setVideoMixerPosition is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::setVideoMixerSrcPosition(
		/* [in] */ IUnknown *aPtrVideoMixerNode,
		/* [in] */ FLOAT aLeft,
		/* [in] */ FLOAT aRight,
		/* [in] */ FLOAT aTop,
		/* [in] */ FLOAT aBottom)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrVideoMixerNode);

			if (aLeft > 1.0f)
				aLeft = 1.0f;

			if (aLeft < 0.0f)
				aLeft = 0.0f;

			if (aRight > 1.0f)
				aRight = 1.0f;

			if (aRight < 0.0f)
				aRight = 0.0f;

			if (aTop > 1.0f)
				aTop = 1.0f;

			if (aTop < 0.0f)
				aTop = 0.0f;

			if (aBottom > 1.0f)
				aBottom = 1.0f;

			if (aBottom < 0.0f)
				aBottom = 0.0f;

			CComPtrCustom<IMFTopologyNode> lVideoMixerTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrVideoMixerNode, &lVideoMixerTopologyNode);

			LOG_CHECK_PTR_MEMORY(lVideoMixerTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, lVideoMixerTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_TRANSFORM_NODE,
				E_INVALIDARG);

			UINT32 l_isMixerNode = FALSE;

			LOG_INVOKE_MF_METHOD(GetUINT32,
				lVideoMixerTopologyNode,
				CM_MixerNode,
				&l_isMixerNode);

			LOG_CHECK_STATE_DESCR(l_isMixerNode == FALSE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				lVideoMixerTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<MediaSession::CustomisedMediaSession::IVideoMixerControl> lVideoMixerControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lVideoMixerControl);

			LOG_CHECK_PTR_MEMORY(lVideoMixerControl);

			LOG_INVOKE_POINTER_METHOD(lVideoMixerControl, setSrcPosition,
				aLeft,
				aRight,
				aTop,
				aBottom);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setVideoMixerSrcPosition is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::setVideoMixerZOrder(
		/* [in] */ IUnknown *aPtrVideoMixerNode,
		/* [in] */ DWORD aZOrder)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			CComPtrCustom<IMFTopologyNode> lVideoMixerTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrVideoMixerNode, &lVideoMixerTopologyNode);

			LOG_CHECK_PTR_MEMORY(lVideoMixerTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, lVideoMixerTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_TRANSFORM_NODE,
				E_INVALIDARG);

			UINT32 l_isMixerNode = FALSE;

			LOG_INVOKE_MF_METHOD(GetUINT32,
				lVideoMixerTopologyNode,
				CM_MixerNode,
				&l_isMixerNode);

			LOG_CHECK_STATE_DESCR(l_isMixerNode == FALSE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				lVideoMixerTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<MediaSession::CustomisedMediaSession::IVideoMixerControl> lVideoMixerControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lVideoMixerControl);

			LOG_CHECK_PTR_MEMORY(lVideoMixerControl);

			LOG_INVOKE_POINTER_METHOD(lVideoMixerControl, setZOrder,
				aZOrder);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setVideoMixerZOrder is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}


	HRESULT CaptureManagerBroker::setVideoMixerOpacity(
		/* [in] */ IUnknown *aPtrVideoMixerNode,
		/* [in] */ FLOAT aOpacity)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			CComPtrCustom<IMFTopologyNode> lVideoMixerTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrVideoMixerNode, &lVideoMixerTopologyNode);

			LOG_CHECK_PTR_MEMORY(lVideoMixerTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, lVideoMixerTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_TRANSFORM_NODE,
				E_INVALIDARG);

			UINT32 l_isMixerNode = FALSE;

			LOG_INVOKE_MF_METHOD(GetUINT32,
				lVideoMixerTopologyNode,
				CM_MixerNode,
				&l_isMixerNode);

			LOG_CHECK_STATE_DESCR(l_isMixerNode == FALSE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				lVideoMixerTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<MediaSession::CustomisedMediaSession::IVideoMixerControl> lVideoMixerControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lVideoMixerControl);

			LOG_CHECK_PTR_MEMORY(lVideoMixerControl);

			LOG_INVOKE_POINTER_METHOD(lVideoMixerControl, setOpacity,
				aOpacity);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setVideoMixerOpacity is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}


	HRESULT CaptureManagerBroker::setVideoMixerFlush(
		/* [in] */ IUnknown *aPtrVideoMixerNode)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			CComPtrCustom<IMFTopologyNode> lVideoMixerTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrVideoMixerNode, &lVideoMixerTopologyNode);

			LOG_CHECK_PTR_MEMORY(lVideoMixerTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, lVideoMixerTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_TRANSFORM_NODE,
				E_INVALIDARG);

			UINT32 l_isMixerNode = FALSE;

			LOG_INVOKE_MF_METHOD(GetUINT32,
				lVideoMixerTopologyNode,
				CM_MixerNode,
				&l_isMixerNode);

			LOG_CHECK_STATE_DESCR(l_isMixerNode == FALSE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				lVideoMixerTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<MediaSession::CustomisedMediaSession::IVideoMixerControl> lVideoMixerControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lVideoMixerControl);

			LOG_CHECK_PTR_MEMORY(lVideoMixerControl);

			LOG_INVOKE_POINTER_METHOD(lVideoMixerControl, flush);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setVideoMixerOpacity is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::setAudioMixerRelativeVolume(
		/* [in] */ IUnknown *aPtrAudioMixerNode,
		/* [in] */ FLOAT aRelativeVolume)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			CComPtrCustom<IMFTopologyNode> lAudioMixerTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrAudioMixerNode, &lAudioMixerTopologyNode);

			LOG_CHECK_PTR_MEMORY(lAudioMixerTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, lAudioMixerTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_TRANSFORM_NODE,
				E_INVALIDARG);

			UINT32 l_isMixerNode = FALSE;

			LOG_INVOKE_MF_METHOD(GetUINT32,
				lAudioMixerTopologyNode,
				CM_MixerNode,
				&l_isMixerNode);

			LOG_CHECK_STATE_DESCR(l_isMixerNode == FALSE,
				E_INVALIDARG);

			CComPtrCustom<IUnknown> lUnkActivate;

			LOG_INVOKE_MF_METHOD(GetObject,
				lAudioMixerTopologyNode,
				&lUnkActivate);

			LOG_CHECK_PTR_MEMORY(lUnkActivate);

			CComPtrCustom<MediaSession::CustomisedMediaSession::IAudioMixerControl> lAudioMixerControl;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkActivate, &lAudioMixerControl);

			LOG_CHECK_PTR_MEMORY(lAudioMixerControl);

			LOG_INVOKE_POINTER_METHOD(lAudioMixerControl, setRelativeVolume, aRelativeVolume);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setAudioMixerRelativeVolume is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}

	HRESULT CaptureManagerBroker::getSARChannelCount(
		/* [in] */ IUnknown *aPtrSARNode,
		/* [out] */ UINT32 *aPtrCount)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrSARNode);

			LOG_CHECK_PTR_MEMORY(aPtrCount);

			CComPtrCustom<IMFTopologyNode> lSARTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrSARNode, &lSARTopologyNode);

			LOG_CHECK_PTR_MEMORY(lSARTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, lSARTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);
			
			CComPtrCustom<IMFMediaSink> lMediaSink;

			LOG_INVOKE_MF_METHOD(GetUnknown,
				lSARTopologyNode,
				CM_Sink, 
				__uuidof(IMFMediaSink),
				(void**)&lMediaSink);

			LOG_CHECK_PTR_MEMORY(lMediaSink);

			CComPtrCustom<IMFGetService> lGetService;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lMediaSink, &lGetService);

			LOG_CHECK_PTR_MEMORY(lGetService);

			CComPtrCustom<IMFAudioStreamVolume> lAudioStreamVolume;
						
			LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
				MR_STREAM_VOLUME_SERVICE,
				__uuidof(IMFAudioStreamVolume),
				(void**)&lAudioStreamVolume);

			LOG_CHECK_PTR_MEMORY(lAudioStreamVolume);

			LOG_INVOKE_POINTER_METHOD(lAudioStreamVolume, GetChannelCount, aPtrCount);
			
		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getSARChannelCount is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
	
	HRESULT CaptureManagerBroker::getSARChannelVolume(
		/* [in] */ IUnknown *aPtrSARNode,
		/* [in] */ UINT32 aIndex,
		/* [out] */ float *aPtrLevel)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrSARNode);

			LOG_CHECK_PTR_MEMORY(aPtrLevel);

			CComPtrCustom<IMFTopologyNode> lSARTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrSARNode, &lSARTopologyNode);

			LOG_CHECK_PTR_MEMORY(lSARTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, lSARTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IMFMediaSink> lMediaSink;

			LOG_INVOKE_MF_METHOD(GetUnknown,
				lSARTopologyNode,
				CM_Sink,
				__uuidof(IMFMediaSink),
				(void**)&lMediaSink);

			LOG_CHECK_PTR_MEMORY(lMediaSink);

			CComPtrCustom<IMFGetService> lGetService;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lMediaSink, &lGetService);

			LOG_CHECK_PTR_MEMORY(lGetService);

			CComPtrCustom<IMFAudioStreamVolume> lAudioStreamVolume;

			LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
				MR_STREAM_VOLUME_SERVICE,
				__uuidof(IMFAudioStreamVolume),
				(void**)&lAudioStreamVolume);

			LOG_CHECK_PTR_MEMORY(lAudioStreamVolume);

			LOG_INVOKE_POINTER_METHOD(lAudioStreamVolume, GetChannelVolume, aIndex, aPtrLevel);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: getSARChannelVolume is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
	
	HRESULT CaptureManagerBroker::setSARChannelVolume(
		/* [in] */ IUnknown *aPtrSARNode,
		/* [in] */ UINT32 aIndex,
		/* [out] */ float aLevel)
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mDestroyedFlag)
				break;

			LOG_CHECK_PTR_MEMORY(aPtrSARNode);
			
			CComPtrCustom<IMFTopologyNode> lSARTopologyNode;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrSARNode, &lSARTopologyNode);

			LOG_CHECK_PTR_MEMORY(lSARTopologyNode);

			MF_TOPOLOGY_TYPE lNodeType(MF_TOPOLOGY_TYPE::MF_TOPOLOGY_MAX);

			LOG_INVOKE_MF_METHOD(GetNodeType, lSARTopologyNode,
				&lNodeType);

			LOG_CHECK_STATE_DESCR(lNodeType != MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
				E_INVALIDARG);

			CComPtrCustom<IMFMediaSink> lMediaSink;

			LOG_INVOKE_MF_METHOD(GetUnknown,
				lSARTopologyNode,
				CM_Sink,
				__uuidof(IMFMediaSink),
				(void**)&lMediaSink);

			LOG_CHECK_PTR_MEMORY(lMediaSink);

			CComPtrCustom<IMFGetService> lGetService;

			LOG_INVOKE_QUERY_INTERFACE_METHOD(lMediaSink, &lGetService);

			LOG_CHECK_PTR_MEMORY(lGetService);

			CComPtrCustom<IMFAudioStreamVolume> lAudioStreamVolume;

			LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
				MR_STREAM_VOLUME_SERVICE,
				__uuidof(IMFAudioStreamVolume),
				(void**)&lAudioStreamVolume);

			LOG_CHECK_PTR_MEMORY(lAudioStreamVolume);

			LOG_INVOKE_POINTER_METHOD(lAudioStreamVolume, SetChannelVolume, aIndex, aLevel);

		} while (false);

		if (FAILED(lresult))
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::ERROR_LEVEL,
				L"CaptureManager: setSARChannelVolume is not executable!!! ",
				L" Error code: ",
				(HRESULT)lresult);
		}

		return lresult;
	}
}