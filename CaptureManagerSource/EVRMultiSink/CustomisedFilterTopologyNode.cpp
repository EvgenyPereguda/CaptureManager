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

#include "CustomisedFilterTopologyNode.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include <inspectable.h>

namespace EVRMultiSink
{
	using namespace CaptureManager::Core;

	CustomisedFilterTopologyNode::CustomisedFilterTopologyNode(
		IMFTopologyNode* aPtrIMFTopologyNode,
		IMFActivate* aPtrIMFActivate)
	{
		mTopologyNode = aPtrIMFTopologyNode;

		mActivate = aPtrIMFActivate;
	}


	CustomisedFilterTopologyNode::~CustomisedFilterTopologyNode()
	{
	}

	
	bool CustomisedFilterTopologyNode::findIncapsulatedInterface(
		REFIID aRefIID,
		void** aPtrPtrVoidObject)
	{
		HRESULT lresult(E_FAIL);

		bool lState = false;
			   
		if (aRefIID == __uuidof(IUnknown))
		{

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrVoidObject);
				
				*aPtrPtrVoidObject = (IUnknown *)this;

				this->AddRef();

				lState = true;

			} while (false);

		}


		if (aRefIID == __uuidof(ICustomisedFilterTopologyNode))
		{

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrVoidObject);

				*aPtrPtrVoidObject = new CustomisedFilterTopologyNode(mTopologyNode, mActivate);

				this->AddRef();

				lState = true;

			} while (false);

		}
		
			   
		if (!lState && (aRefIID == __uuidof(IMFTopologyNode)))
		{

			do
			{
				LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(mTopologyNode, aRefIID, aPtrPtrVoidObject);

				lState = true;

			} while (false);

		}

		if (!lState && (aRefIID == __uuidof(IBaseFilter)))
		{

			do
			{
				LOG_CHECK_STATE(FAILED(createBaseFilter()));

				LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(mBaseFilter, aRefIID, aPtrPtrVoidObject);

				lState = true;

			} while (false);
			
		}

		//if (!lState && (aRefIID == __uuidof(IInspectable)))
		//{

		//	do
		//	{
		//		LOG_CHECK_STATE(FAILED(createBaseFilter()));

		//		LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(mBaseFilter, aRefIID, aPtrPtrVoidObject);

		//		lState = true;

		//	} while (false);

		//}		

		return lState;
	}
	
	HRESULT CustomisedFilterTopologyNode::createBaseFilter()
	{
		HRESULT lresult(E_FAIL);

		do
		{
			if (mBaseFilter)
			{
				lresult = S_OK;

				break;
			}

			LOG_CHECK_PTR_MEMORY(mActivate);

			LOG_INVOKE_MF_METHOD(ActivateObject, mActivate,
				IID_PPV_ARGS(&mBaseFilter));

			LOG_CHECK_PTR_MEMORY(mBaseFilter);

		} while (false);

		return lresult;
	}
}