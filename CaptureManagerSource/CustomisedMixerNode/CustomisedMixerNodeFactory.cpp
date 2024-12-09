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

#include "CustomisedMixerNodeFactory.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/MFHeaders.h"
#include "MixerWrapper.h"
#include "CustomisedMixerNode.h"


// {A080FA3C-4870-48E2-96DB-522EEB94FD0D}
static const GUID IID_MixerNodeGroup =
{ 0xa080fa3c, 0x4870, 0x48e2, { 0x96, 0xdb, 0x52, 0x2e, 0xeb, 0x94, 0xfd, 0xd } };

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			using namespace Core;

			CustomisedMixerNodeFactory::CustomisedMixerNodeFactory()
			{
				DWORD lInputCount = 0;
				
				if(SUCCEEDED(MixerWrapper::getStreamCount(MFMediaType_Video, lInputCount)))
				{
					MixerNodeInfo lMixerNodeInfo;

					lMixerNodeInfo.mStreamControlInfo.mName = L"MixerNodeFactory";

					lMixerNodeInfo.mStreamControlInfo.mTitle = L"Video Mixer node factory";

					lMixerNodeInfo.mStreamControlInfo.mGUID = MFMediaType_Video;

					lMixerNodeInfo.mStreamControlInfo.mInputPortCount = lInputCount;

					lMixerNodeInfo.mStreamControlInfo.mIsInputPortCountConstant = false;

					lMixerNodeInfo.mStreamControlInfo.mOutputPortCount = 1;

					lMixerNodeInfo.mStreamControlInfo.mIsOutputPortCountConstant = true;
					
					mMixerNodeInfos.push_back(lMixerNodeInfo);

				}

				if (SUCCEEDED(MixerWrapper::getStreamCount(MFMediaType_Audio, lInputCount)))
				{
					MixerNodeInfo lMixerNodeInfo;

					lMixerNodeInfo.mStreamControlInfo.mName = L"MixerNodeFactory";

					lMixerNodeInfo.mStreamControlInfo.mTitle = L"Audio Mixer node factory";

					lMixerNodeInfo.mStreamControlInfo.mGUID = MFMediaType_Audio;

					lMixerNodeInfo.mStreamControlInfo.mInputPortCount = lInputCount;

					lMixerNodeInfo.mStreamControlInfo.mIsInputPortCountConstant = false;

					lMixerNodeInfo.mStreamControlInfo.mOutputPortCount = 1;

					lMixerNodeInfo.mStreamControlInfo.mIsOutputPortCountConstant = true;

					mMixerNodeInfos.push_back(lMixerNodeInfo);
				}
			}
			
			CustomisedMixerNodeFactory::~CustomisedMixerNodeFactory()
			{
			}
			


			HRESULT CustomisedMixerNodeFactory::getCustomisedMixerNodeFactoryCollection(
				pugi::xml_node& aRefRootXMLElement,
				std::vector<StreamControlInfo>& aCollection)
			{
				auto lSampleAccumulatorGroupXMLElement = aRefRootXMLElement.append_child(L"Group");

				std::wstring lGUIDtext;

				DataParser::GetGUIDName(IID_MixerNodeGroup, lGUIDtext);

				lSampleAccumulatorGroupXMLElement.append_attribute(L"GUID") = lGUIDtext.c_str();

				lSampleAccumulatorGroupXMLElement.append_attribute(L"Title") = L"Mixer group";

				for (auto& linfo : mMixerNodeInfos)
				{
					auto lSpreaderNodeFactoryXMLElement = lSampleAccumulatorGroupXMLElement.append_child(L"StreamControlNodeFactory");

					lSpreaderNodeFactoryXMLElement.append_attribute(L"Name") = linfo.mStreamControlInfo.mName.c_str();

					lSpreaderNodeFactoryXMLElement.append_attribute(L"Title") = linfo.mStreamControlInfo.mTitle.c_str();


					std::wstring lname;

					DataParser::GetGUIDName(linfo.mStreamControlInfo.mGUID, lname);
					
					WCHAR *lptrName = nullptr;

					auto lresult = StringFromCLSID(linfo.mStreamControlInfo.mGUID, &lptrName);

					if (FAILED(lresult))
						break;

					lGUIDtext = std::wstring(lptrName);

					CoTaskMemFree(lptrName);


					lSpreaderNodeFactoryXMLElement.append_attribute(L"MajorTypeGUID") = lGUIDtext.c_str();
					
					lSpreaderNodeFactoryXMLElement.append_attribute(L"MajorType") = lname.c_str();

					lSpreaderNodeFactoryXMLElement.append_attribute(L"InputPortCountConstant") = linfo.mStreamControlInfo.mIsInputPortCountConstant;

					lSpreaderNodeFactoryXMLElement.append_attribute(L"OutputPortCountConstant") = linfo.mStreamControlInfo.mIsOutputPortCountConstant;




					auto lTypesOutputNode = lSpreaderNodeFactoryXMLElement.append_child(L"Value.ValueParts");


					auto lTypeOutputNode = lTypesOutputNode.append_child(L"ValuePart");

					lTypeOutputNode.append_attribute(L"Title") = L"Count of input ports";

					lTypeOutputNode.append_attribute(L"Value") = linfo.mStreamControlInfo.mInputPortCount;

					lTypeOutputNode.append_attribute(L"IsPortCountConstant") = linfo.mStreamControlInfo.mIsInputPortCountConstant;


					lTypeOutputNode = lTypesOutputNode.append_child(L"ValuePart");

					lTypeOutputNode.append_attribute(L"Title") = L"Count of output ports";

					lTypeOutputNode.append_attribute(L"Value") = linfo.mStreamControlInfo.mOutputPortCount;

					lTypeOutputNode.append_attribute(L"IsPortCountConstant") = linfo.mStreamControlInfo.mIsOutputPortCountConstant;

				}

				for (auto& litem : mMixerNodeInfos)
				{
					aCollection.push_back(litem.mStreamControlInfo);
				}

				return S_OK;
			}

			HRESULT CustomisedMixerNodeFactory::createCustomisedMixerNodes(
				IUnknown *aPtrDownStreamTopologyNode,
				DWORD aInputNodeAmount,
				std::vector<CComPtrCustom<IUnknown>>& aRefInputNodes)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrDownStreamTopologyNode);

					LOG_CHECK_STATE_DESCR(aInputNodeAmount == 0, E_INVALIDARG);

					CComQIPtrCustom<IMFTopologyNode> lDownStreamTopologyNode = aPtrDownStreamTopologyNode;

					CComPtrCustom<IMFTransform> lTransform;

					LOG_INVOKE_FUNCTION(MediaSession::CustomisedMediaSession::CustomisedMixerNode::create,
						aInputNodeAmount,
						aRefInputNodes);

					if (aRefInputNodes.size() > 0)
					{
						CComQIPtrCustom<IMFTopologyNode> lUpStreamTopologyNode = aRefInputNodes[0].get();

						LOG_CHECK_PTR_MEMORY(lUpStreamTopologyNode);

						LOG_INVOKE_MF_METHOD(ConnectOutput,
							lUpStreamTopologyNode,
							0,
							lDownStreamTopologyNode,
							0);
					}

				} while (false);

				return lresult;
			}

		}
	}
}