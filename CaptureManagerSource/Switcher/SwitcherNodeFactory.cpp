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

#include "SwitcherNodeFactory.h"
#include "Switcher.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"


namespace CaptureManager
{
	namespace Transform
	{
		SwitcherNodeFactory::SwitcherNodeFactory()
		{
		}


		SwitcherNodeFactory::~SwitcherNodeFactory()
		{
		}

		HRESULT STDMETHODCALLTYPE SwitcherNodeFactory::createSwitcherNode(
			IUnknown *aPtrDownStreamTopologyNode,
			IUnknown **aPtrPtrTopologySwitcherNode)
		{
			using namespace Core;
			
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrTopologySwitcherNode);
				
				CComPtrCustom<IMFTransform> lIMFTransform = new (std::nothrow) Switcher();
				
				LOG_CHECK_PTR_MEMORY(lIMFTransform);
				
				CComPtrCustom<IMFTopologyNode> lSwitcherNode;

				LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode, MF_TOPOLOGY_TRANSFORM_NODE, &lSwitcherNode);

				LOG_INVOKE_MF_METHOD(SetObject, lSwitcherNode, lIMFTransform);

				LOG_INVOKE_MF_METHOD(SetUINT32, lSwitcherNode, CM_SwitcherNode, TRUE);		

				if (aPtrDownStreamTopologyNode != nullptr)
				{
					CComQIPtrCustom<IMFTopologyNode> lDownStreamTopologyNode;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrDownStreamTopologyNode, &lDownStreamTopologyNode);

					LOG_CHECK_PTR_MEMORY(lDownStreamTopologyNode);

					LOG_INVOKE_MF_METHOD(ConnectOutput,
						lSwitcherNode,
						0,
						lDownStreamTopologyNode,
						0);
				}						

				*aPtrPtrTopologySwitcherNode = lSwitcherNode.Detach();

			} while (false);

			return lresult;

		}
	}
}