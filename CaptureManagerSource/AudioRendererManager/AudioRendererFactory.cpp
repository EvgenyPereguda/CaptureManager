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

#include "AudioRendererFactory.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../Common/Singleton.h"
#include <memory>


namespace CaptureManager
{
	namespace Sinks
	{
		using namespace Core;
		
		AudioRendererFactory::AudioRendererFactory()
		{
		}

		AudioRendererFactory::~AudioRendererFactory()
		{
		}

		HRESULT AudioRendererFactory::createRendererOutputNode(
			CComPtrCustom<IUnknown>& aRefOutputNode)
		{
			HRESULT lresult;

			do
			{
				CComPtrCustom<IMFActivate> lActivate;

				LOG_INVOKE_MF_FUNCTION(MFCreateAudioRendererActivate, &lActivate);


				LOG_CHECK_PTR_MEMORY(lActivate);

				CComPtrCustom<IMFTopologyNode> RendererActivateTopologyNode;

				LOG_INVOKE_MF_FUNCTION(MFCreateTopologyNode,
					MF_TOPOLOGY_TYPE::MF_TOPOLOGY_OUTPUT_NODE,
					&RendererActivateTopologyNode);

				LOG_INVOKE_MF_METHOD(SetObject,
					RendererActivateTopologyNode,
					lActivate);

				LOG_INVOKE_MF_METHOD(SetUINT32,
					RendererActivateTopologyNode,
					MF_TOPONODE_STREAMID,
					0);

				LOG_INVOKE_MF_METHOD(SetUINT32,
					RendererActivateTopologyNode,
					CM_SAROutputNode,
					TRUE);				

				CComPtrCustom<IUnknown> lUnknown;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(RendererActivateTopologyNode, &lUnknown);

				aRefOutputNode = lUnknown;			

			} while (false);

			return lresult;
		}

		void AudioRendererFactory::getReadModes(
			BOOL aMultiOutputNodeVideoRenderer,
			std::vector<GUIDToNamePair>& aRefReadModes)
		{
		}
	}
}
