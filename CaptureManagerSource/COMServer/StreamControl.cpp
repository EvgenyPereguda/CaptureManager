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


#include "StreamControl.h"
#include "../PugiXML/pugixml.hpp"
#include "../DataParser/DataParser.h"
#include "SpreaderNodeFactory.h"
#include "SampleAccumulatorNodeFactory.h"
#include "SwitcherNodeFactory.h"
#include "MixerNodeFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"



namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			GetCollectionOfStreamControlNodeFactories = 1,
			CreateStreamControlNodeFactory = GetCollectionOfStreamControlNodeFactories + 1
		};
		// IStreamControl interface implementation

		STDMETHODIMP StreamControl::getCollectionOfStreamControlNodeFactories(
			/* [out][in] */ BSTR *aPtrPtrXMLstring)
		{
			using namespace pugi;

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrXMLstring);
				
				if (*aPtrPtrXMLstring != nullptr)
				{
					SysFreeString(*aPtrPtrXMLstring);
				}

				xml_document lxmlDoc;

				auto ldeclNode = lxmlDoc.append_child(node_declaration);

				ldeclNode.append_attribute(L"version") = L"1.0";

				xml_node lcommentNode = lxmlDoc.append_child(node_comment);

				lcommentNode.set_value(L"XML Document of stream control node factories");

				auto lRootXMLElement = lxmlDoc.append_child(L"StreamControlNodeFactories");

				// Spreader node factory

				{

					auto lSpreaderNodeFactoryXMLElement = lRootXMLElement.append_child(L"StreamControlNodeFactory");

					auto linfo = SpreaderNodeFactory::getStreamControlInfo();

					lSpreaderNodeFactoryXMLElement.append_attribute(L"Name") = linfo.mName.c_str();

					lSpreaderNodeFactoryXMLElement.append_attribute(L"Title") = linfo.mTitle.c_str();

					std::wstring lname;

					DataParser::GetGUIDName(linfo.mGUID, lname);

					lSpreaderNodeFactoryXMLElement.append_attribute(L"GUID") = lname.c_str();

					lSpreaderNodeFactoryXMLElement.append_attribute(L"InputPortCountConstant") = linfo.mIsInputPortCountConstant;

					lSpreaderNodeFactoryXMLElement.append_attribute(L"OutputPortCountConstant") = linfo.mIsOutputPortCountConstant;




					auto lTypesOutputNode = lSpreaderNodeFactoryXMLElement.append_child(L"Value.ValueParts");


					auto lTypeOutputNode = lTypesOutputNode.append_child(L"ValuePart");

					lTypeOutputNode.append_attribute(L"Title") = L"Count of input ports";

					lTypeOutputNode.append_attribute(L"Value") = linfo.mInputPortCount;

					lTypeOutputNode.append_attribute(L"IsPortCountConstant") = linfo.mIsInputPortCountConstant;


					lTypeOutputNode = lTypesOutputNode.append_child(L"ValuePart");

					lTypeOutputNode.append_attribute(L"Title") = L"Count of output ports";

					lTypeOutputNode.append_attribute(L"Value") = linfo.mOutputPortCount;

					lTypeOutputNode.append_attribute(L"IsPortCountConstant") = linfo.mIsOutputPortCountConstant;

				}

				// SampleAccumulator node factory

				std::vector<StreamControlInfo> lCollection;

				auto linfoSampleAccumulatorNodeFactory = SampleAccumulatorNodeFactory::getStreamControlInfo(
					lRootXMLElement,
					lCollection);


				// Mixer node factory

				std::vector<StreamControlInfo> lMixerNodeFactoryCollection;

				auto linfoMixerNodeFactory = MixerNodeFactory::getStreamControlInfo(
					lRootXMLElement,
					lMixerNodeFactoryCollection);
				//
				
				std::wstringstream lwstringstream;

				lxmlDoc.print(lwstringstream);

				*aPtrPtrXMLstring = SysAllocString(lwstringstream.str().c_str());

				lresult = S_OK;

			} while (false);
			
			return lresult;
		}

		STDMETHODIMP StreamControl::createStreamControlNodeFactory(
			REFIID aREFIID,
			IUnknown **aPtrPtrStreamControlNodeFactory)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrStreamControlNodeFactory);

				auto linfoSpreader = SpreaderNodeFactory::getStreamControlInfo();

				std::vector<StreamControlInfo> lSampleAccumulatorNodeFactoryCollection;

				using namespace pugi;

				xml_document lxmlDoc;

				auto ldeclNode = lxmlDoc.append_child(node_declaration);

				ldeclNode.append_attribute(L"version") = L"1.0";

				xml_node lcommentNode = lxmlDoc.append_child(node_comment);

				lcommentNode.set_value(L"XML Document of stream control node factories");

				auto lRootXMLElement = lxmlDoc.append_child(L"StreamControlNodeFactories");

				auto linfoSampleAccumulator = SampleAccumulatorNodeFactory::getStreamControlInfo(
					lRootXMLElement,
					lSampleAccumulatorNodeFactoryCollection);
				
				if (aREFIID == linfoSpreader.mGUID)
				{
					CComPtrCustom<SpreaderNodeFactory> lSpreaderNodeFactory = new (std::nothrow) SpreaderNodeFactory();

					LOG_INVOKE_POINTER_METHOD(lSpreaderNodeFactory, QueryInterface,
						aREFIID,
						(void**)aPtrPtrStreamControlNodeFactory);
				}
				else if (aREFIID == __uuidof(ISwitcherNodeFactory))
				{
					CComPtrCustom<SwitcherNodeFactory> lSwitcherNodeFactory = new (std::nothrow) SwitcherNodeFactory();

					LOG_INVOKE_POINTER_METHOD(lSwitcherNodeFactory, QueryInterface,
						aREFIID,
						(void**)aPtrPtrStreamControlNodeFactory);
				}
				else if (aREFIID == __uuidof(IMixerNodeFactory))
				{
					CComPtrCustom<MixerNodeFactory> lMixerNodeFactory = new (std::nothrow) MixerNodeFactory();

					LOG_INVOKE_POINTER_METHOD(lMixerNodeFactory, QueryInterface,
						aREFIID,
						(void**)aPtrPtrStreamControlNodeFactory);
				}
				else
				{
					lresult = E_NOINTERFACE;

					for (auto& linfo : lSampleAccumulatorNodeFactoryCollection)
					{
						if (aREFIID == linfo.mGUID)
						{

							CComPtrCustom<SampleAccumulatorNodeFactory> lSampleAccumulatorNodeFactory = 
								new (std::nothrow) SampleAccumulatorNodeFactory(aREFIID);

							LOG_INVOKE_POINTER_METHOD(lSampleAccumulatorNodeFactory, QueryInterface,
								__uuidof(ISpreaderNodeFactory),
								(void**)aPtrPtrStreamControlNodeFactory);

							break;
						}
					}

				}
				
			} while (false);

			return lresult;
		}



		// IDispatch interface stub

		STDMETHODIMP StreamControl::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);

				if (_wcsicmp(*rgszNames, OLESTR("getCollectionOfStreamControlNodeFactories")) == 0)
				{
					*rgDispId = (int)MethodsEnum::GetCollectionOfStreamControlNodeFactories;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createStreamControlNodeFactory")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateStreamControlNodeFactory;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT StreamControl::invokeMethod(
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
				case (int)MethodsEnum::GetCollectionOfStreamControlNodeFactories:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 0, DISP_E_BADPARAMCOUNT);
					
					CComPtrCustom<IUnknown> lCompressedMediaType;

					LOG_INVOKE_FUNCTION(getCollectionOfStreamControlNodeFactories,
						&pVarResult->bstrVal);
					
					pVarResult->vt = VT_BSTR;

				}
				break;
				case (int)MethodsEnum::CreateStreamControlNodeFactory:
				{
					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);
										
					IID lIID;
					
					if (pDispParams->rgvarg[0].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString,pDispParams->rgvarg[0].bstrVal, &lIID);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}
					
					CComPtrCustom<IUnknown> lStreamControlNodeFactory;

					LOG_INVOKE_FUNCTION(createStreamControlNodeFactory,
						lIID,
						&lStreamControlNodeFactory);
					
					pVarResult->vt = VT_UNKNOWN;

					pVarResult->punkVal = lStreamControlNodeFactory.detach();
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