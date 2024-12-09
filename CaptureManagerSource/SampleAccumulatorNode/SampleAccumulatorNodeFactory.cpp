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

#include "SampleAccumulatorNodeFactory.h"
#include "SampleAccumulator.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"


#define DEFINE_GUID_DIRECT(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

// {EC792646-F391-4785-A071-F2F62D232CDE}
DEFINE_GUID_DIRECT(IID_SampleAccumulatorGroup,
	0xec792646, 0xf391, 0x4785, 0xa0, 0x71, 0xf2, 0xf6, 0x2d, 0x23, 0x2c, 0xde);


// {C1094B7B-7F68-421C-B818-AC15C5DC7A34}
DEFINE_GUID_DIRECT(IID_SampleAccumulator_5_Samples,
	0xc1094b7b, 0x7f68, 0x421c, 0xb8, 0x18, 0xac, 0x15, 0xc5, 0xdc, 0x7a, 0x34);

// {8CC58070-1393-421E-B79B-952A84A1A4CB}
DEFINE_GUID_DIRECT(IID_SampleAccumulator_10_Samples,
	0x8cc58070, 0x1393, 0x421e, 0xb7, 0x9b, 0x95, 0x2a, 0x84, 0xa1, 0xa4, 0xcb);

// {89C8D7CD-7E00-4A5B-A26F-EB83E41A6702}
DEFINE_GUID_DIRECT(IID_SampleAccumulator_15_Samples,
	0x89c8d7cd, 0x7e00, 0x4a5b, 0xa2, 0x6f, 0xeb, 0x83, 0xe4, 0x1a, 0x67, 0x2);



namespace CaptureManager
{
	namespace Transform
	{
		using namespace Core;

		SampleAccumulatorNodeFactory::SampleAccumulatorNodeFactory()
		{

			{
				SampleAccumulatorInfo lStreamControlInfo;

				lStreamControlInfo.mStreamControlInfo.mName = L"SampleAccumulatorNodeFactory";

				lStreamControlInfo.mStreamControlInfo.mTitle = L"Sample Accumulator node factory on 5 samples";

				lStreamControlInfo.mStreamControlInfo.mGUID = IID_SampleAccumulator_5_Samples;

				lStreamControlInfo.mStreamControlInfo.mInputPortCount = 1;

				lStreamControlInfo.mStreamControlInfo.mIsInputPortCountConstant = true;

				lStreamControlInfo.mStreamControlInfo.mOutputPortCount = 1;

				lStreamControlInfo.mStreamControlInfo.mIsOutputPortCountConstant = true;

				lStreamControlInfo.mSampleSize = 5;

				mSampleAccumulatorInfo.push_back(lStreamControlInfo);

			}


			{
				SampleAccumulatorInfo lStreamControlInfo;

				lStreamControlInfo.mStreamControlInfo.mName = L"SampleAccumulatorNodeFactory";

				lStreamControlInfo.mStreamControlInfo.mTitle = L"Sample Accumulator node factory on 10 samples";

				lStreamControlInfo.mStreamControlInfo.mGUID = IID_SampleAccumulator_10_Samples;

				lStreamControlInfo.mStreamControlInfo.mInputPortCount = 1;

				lStreamControlInfo.mStreamControlInfo.mIsInputPortCountConstant = true;

				lStreamControlInfo.mStreamControlInfo.mOutputPortCount = 1;

				lStreamControlInfo.mStreamControlInfo.mIsOutputPortCountConstant = true;

				lStreamControlInfo.mSampleSize = 10;

				mSampleAccumulatorInfo.push_back(lStreamControlInfo);

			}


			//{
			//	SampleAccumulatorInfo lStreamControlInfo;

			//	lStreamControlInfo.mStreamControlInfo.mName = L"SampleAccumulatorNodeFactory";

			//	lStreamControlInfo.mStreamControlInfo.mTitle = L"Sample Accumulator node factory on 15 samples";

			//	lStreamControlInfo.mStreamControlInfo.mGUID = IID_SampleAccumulator_15_Samples;

			//	lStreamControlInfo.mStreamControlInfo.mInputPortCount = 1;

			//	lStreamControlInfo.mStreamControlInfo.mIsInputPortCountConstant = true;

			//	lStreamControlInfo.mStreamControlInfo.mOutputPortCount = 1;

			//	lStreamControlInfo.mStreamControlInfo.mIsOutputPortCountConstant = true;

			//	lStreamControlInfo.mSampleSize = 15;

			//	mSampleAccumulatorInfo.push_back(lStreamControlInfo);

			//}

			
		}
		
		SampleAccumulatorNodeFactory::~SampleAccumulatorNodeFactory()
		{
		}

		HRESULT SampleAccumulatorNodeFactory::getSampleAccumulatorNodeFactoryCollection(
			pugi::xml_node& aRefRootXMLElement,
			std::vector<StreamControlInfo>& aCollection)
		{
			auto lSampleAccumulatorGroupXMLElement = aRefRootXMLElement.append_child(L"Group");

			std::wstring lGUIDtext;

			DataParser::GetGUIDName(IID_SampleAccumulatorGroup, lGUIDtext);

			lSampleAccumulatorGroupXMLElement.append_attribute(L"GUID") = lGUIDtext.c_str();

			lSampleAccumulatorGroupXMLElement.append_attribute(L"Title") = "Sample accumulator group";
			
			for (auto& linfo : mSampleAccumulatorInfo)
			{
				auto lSpreaderNodeFactoryXMLElement = lSampleAccumulatorGroupXMLElement.append_child(L"StreamControlNodeFactory");
				
				lSpreaderNodeFactoryXMLElement.append_attribute(L"Name") = linfo.mStreamControlInfo.mName.c_str();

				lSpreaderNodeFactoryXMLElement.append_attribute(L"Title") = linfo.mStreamControlInfo.mTitle.c_str();

				lSpreaderNodeFactoryXMLElement.append_attribute(L"Samples") = linfo.mSampleSize;

				std::wstring lname;

				DataParser::GetGUIDName(linfo.mStreamControlInfo.mGUID, lname);

				lSpreaderNodeFactoryXMLElement.append_attribute(L"GUID") = lname.c_str();

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

			for (auto& litem : mSampleAccumulatorInfo)
			{
				aCollection.push_back(litem.mStreamControlInfo);
			}			
			
			return S_OK;
		}


		HRESULT SampleAccumulatorNodeFactory::createSampleAccumulatorNode(
			REFIID aREFIID,
			std::vector<IUnknown*> aArrayDownStreamTopologyNodes,
			IUnknown** aPtrPtrTopologySampleAccumulatorNode)
		{
			using namespace Accumulator;

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_STATE_DESCR(aArrayDownStreamTopologyNodes.size() != 1, E_BOUNDS);
				
				CComPtrCustom<IMFTransform> lIMFTransform;

				for (auto& litem : mSampleAccumulatorInfo)
				{
					if (litem.mStreamControlInfo.mGUID == aREFIID)
					{
						CComPtrCustom<IMFTransform> lTempIMFTransform = new (std::nothrow) SampleAccumulator(litem.mSampleSize);

						LOG_CHECK_PTR_MEMORY(lTempIMFTransform);

						lIMFTransform = lTempIMFTransform;

						break;
					} 
					
				}

				LOG_CHECK_PTR_MEMORY(lIMFTransform);
				
				CComPtrCustom<IMFTopologyNode> lSampleAccumulatorNode;

				LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode, MF_TOPOLOGY_TRANSFORM_NODE, &lSampleAccumulatorNode);

				LOG_INVOKE_MF_METHOD(SetObject, lSampleAccumulatorNode, lIMFTransform);



				CComQIPtrCustom<IMFTopologyNode> lDownStreamTopologyNode;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(aArrayDownStreamTopologyNodes[0], &lDownStreamTopologyNode);


				LOG_CHECK_PTR_MEMORY(lDownStreamTopologyNode);

				
				LOG_INVOKE_MF_METHOD(ConnectOutput,
					lSampleAccumulatorNode,
					0,
					lDownStreamTopologyNode,
					0);
				
				*aPtrPtrTopologySampleAccumulatorNode = lSampleAccumulatorNode.Detach();

			} while (false);
						
			return lresult;
		}
	}
}