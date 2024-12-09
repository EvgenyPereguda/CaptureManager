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

#include <string>
#include <algorithm>
#include <vector>


#include "SampleAccumulatorNodeFactory.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	namespace COMServer
	{
				
		enum class MethodsEnum :DISPID
		{
			CreateSpreaderNode = 1
		};

		SampleAccumulatorNodeFactory::SampleAccumulatorNodeFactory(REFIID aREFIID):
			mIID(aREFIID)
		{}

		// ISpreaderNodeFactory interface implementation

		STDMETHODIMP SampleAccumulatorNodeFactory::createSpreaderNode(
			VARIANT aArrayPtrDownStreamTopologyNodes,
			IUnknown **aPtrPtrTopologySpreaderNode)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrTopologySpreaderNode);

				LOG_CHECK_STATE(!(aArrayPtrDownStreamTopologyNodes.vt & VT_UNKNOWN) ||
					!(aArrayPtrDownStreamTopologyNodes.vt & VT_SAFEARRAY));

				SAFEARRAY* lPtrSATopologyNodes = nullptr;

				lPtrSATopologyNodes = aArrayPtrDownStreamTopologyNodes.parray;

				LOG_CHECK_PTR_MEMORY(lPtrSATopologyNodes);

				LONG lBoundTopologyNodes;

				LONG uBoundTopologyNodes;

				LOG_INVOKE_FUNCTION(SafeArrayGetUBound, lPtrSATopologyNodes, 1, &uBoundTopologyNodes);

				LOG_INVOKE_FUNCTION(SafeArrayGetLBound, lPtrSATopologyNodes, 1, &lBoundTopologyNodes);

				std::vector<IUnknown*> lDownStreamTopologyNodes;

				for (LONG lIndex = lBoundTopologyNodes; lIndex <= uBoundTopologyNodes; lIndex++)
				{
					VARIANT lVar;

					LOG_INVOKE_FUNCTION(SafeArrayGetElement, lPtrSATopologyNodes, &lIndex, &lVar);

					if (lVar.vt == VT_UNKNOWN)
					{
						lDownStreamTopologyNodes.push_back(lVar.punkVal);
					}
					else if (lVar.vt == VT_DISPATCH)
					{
						lDownStreamTopologyNodes.push_back(lVar.pdispVal);
					}

					VariantClear(&lVar);
				}

				LOG_CHECK_STATE(FAILED(lresult));


				using namespace pugi;

				xml_document lxmlDoc;

				auto ldeclNode = lxmlDoc.append_child(node_declaration);

				ldeclNode.append_attribute(L"version") = L"1.0";

				xml_node lcommentNode = lxmlDoc.append_child(node_comment);

				lcommentNode.set_value(L"XML Document of stream control node factories");

				auto lRootXMLElement = lxmlDoc.append_child(L"StreamControlNodeFactories");



				std::vector<StreamControlInfo> lSampleAccumulatorNodeFactoryCollection;

				auto linfoSampleAccumulator = SampleAccumulatorNodeFactory::getStreamControlInfo(
					lRootXMLElement,
					lSampleAccumulatorNodeFactoryCollection);

				//lSampleAccumulatorNodeFactoryCollection

				auto lIID = mIID;
				
				auto lresFind = std::find_if(lSampleAccumulatorNodeFactoryCollection.begin(),
					lSampleAccumulatorNodeFactoryCollection.end(),
					[lIID](StreamControlInfo ainfo){
					return memcmp(&(ainfo.mGUID), &lIID, sizeof(lIID)) == 0; 
				});

				LOG_CHECK_STATE(lresFind == lSampleAccumulatorNodeFactoryCollection.end());
				

				if ((*lresFind).mIsOutputPortCountConstant)
				{
					LOG_CHECK_STATE((*lresFind).mOutputPortCount != lDownStreamTopologyNodes.size());
				}
				else
				{
					LOG_CHECK_STATE((*lresFind).mOutputPortCount < lDownStreamTopologyNodes.size());
				}

				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createSampleAccumulatorNode,
					mIID,
					lDownStreamTopologyNodes,
					aPtrPtrTopologySpreaderNode);

			} while (false);

			return lresult;
		}

		StreamControlInfo SampleAccumulatorNodeFactory::getStreamControlInfo(
			pugi::xml_node& aRefRootXMLElement,
			std::vector<StreamControlInfo>& aCollection)
		{
			StreamControlInfo lStreamControlInfo;

			lStreamControlInfo.mName = L"";

			lStreamControlInfo.mTitle = L"";

			lStreamControlInfo.mGUID = GUID_NULL;

			lStreamControlInfo.mInputPortCount = 1;

			lStreamControlInfo.mIsInputPortCountConstant = true;

			lStreamControlInfo.mOutputPortCount = 1;

			lStreamControlInfo.mIsOutputPortCountConstant = true;
			

			Singleton<CaptureManagerBroker>::getInstance().getSampleAccumulatorNodeFactoryCollection(
				aRefRootXMLElement,
				aCollection);

			return lStreamControlInfo;
		}



		// IDispatch interface stub

		STDMETHODIMP SampleAccumulatorNodeFactory::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);

				if (_wcsicmp(*rgszNames, OLESTR("createSpreaderNode")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateSpreaderNode;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT SampleAccumulatorNodeFactory::invokeMethod(
			/* [annotation][in] */
			_In_  DISPID dispIdMember,
			/* [annotation][out][in] */
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				switch (dispIdMember)
				{
				case (int)MethodsEnum::CreateSpreaderNode:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);

					VARIANT lArrayPtrDownStreamTopologyNodes;

					if (pDispParams->rgvarg[0].vt == VT_VARIANT)
					{
						lArrayPtrDownStreamTopologyNodes = *(pDispParams->rgvarg[0].pvarVal);
					}
					else if (pDispParams->rgvarg[0].vt == (VT_ARRAY | VT_VARIANT))
					{

						SAFEARRAY* lPtrSAMediaTypes = nullptr;

						lPtrSAMediaTypes = pDispParams->rgvarg[0].parray;

						LOG_CHECK_STATE_DESCR(lPtrSAMediaTypes == nullptr, DISP_E_BADVARTYPE);

						lArrayPtrDownStreamTopologyNodes.vt = VT_UNKNOWN | VT_SAFEARRAY;

						lArrayPtrDownStreamTopologyNodes.parray = lPtrSAMediaTypes;

					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}


					CComPtrCustom<IUnknown> lSpreaderNode;

					LOG_INVOKE_FUNCTION(createSpreaderNode,
						lArrayPtrDownStreamTopologyNodes,
						&lSpreaderNode);

					pVarResult->vt = VT_UNKNOWN;

					pVarResult->punkVal = lSpreaderNode.detach();

				}
					break;
				default:
					break;
				}


			} while (false);

			return lresult;
		}
	}
}