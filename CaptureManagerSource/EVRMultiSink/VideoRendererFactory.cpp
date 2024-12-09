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

#include "VideoRendererFactory.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include "../Common/GUIDs.h"
#include "EVRActivate.h"
#include "VideoRenderingClassFactory.h"
#include "PresenterFactory.h"
#include "CustomisedFilterTopologyNode.h"
#include <memory>


namespace EVRMultiSink
{
	namespace Sinks
	{
		using namespace CaptureManager;
		using namespace CaptureManager::Core;

		using namespace EVR;

		VideoRendererFactory::VideoRendererFactory()
		{
		}
		
		VideoRendererFactory::~VideoRendererFactory()
		{
		}

		void VideoRendererFactory::getReadModes(
			std::vector<GUIDToNamePair>& aRefReadModes)
		{
			DWORD lMaxInputStreamCount(0);

			PresenterFactory::getMaxInputStreamCount(&lMaxInputStreamCount);

			GUIDToNamePair lGUIDFormat;

			lGUIDFormat.mSinkTypes.push_back(SinkType::SampleGrabberCall);

			lGUIDFormat.mGUID =
			{ 0xE926E7A7, 0x7DD0, 0x4B15, { 0x88, 0xD7, 0x41, 0x37, 0x04, 0xAF, 0x86, 0x5F } };

			lGUIDFormat.mName = L"Default";

			lGUIDFormat.mMaxPortCount = lMaxInputStreamCount;

			lGUIDFormat.mTitle = L"Default EVR implementation";

			aRefReadModes.push_back(lGUIDFormat);
		}
		
		HRESULT VideoRendererFactory::createRendererOutputNodes(
			HANDLE aHandle,
			IUnknown *aPtrUnkTarget,
			DWORD aOutputNodeAmount,
			std::vector<CComPtrCustom<IUnknown>>& aRefOutputNodes)
		{
			HRESULT lresult;

			do
			{			

				CComPtrCustom<IMFTransform> lMixer;

				CComPtrCustom<IPresenter> lPresenter;

				PresenterFactory::createPresenter(
					aHandle,
					aPtrUnkTarget,
					aOutputNodeAmount,
					&lPresenter,
					&lMixer);

				LOG_CHECK_PTR_MEMORY(lPresenter);

				LOG_CHECK_PTR_MEMORY(lMixer);
				
				DWORD lInputMinimum;
				DWORD lInputMaximum;
				DWORD lOutputMinimum;
				DWORD lOutputMaximum;

				LOG_INVOKE_POINTER_METHOD(lMixer, GetStreamLimits,
					&lInputMinimum,
					&lInputMaximum,
					&lOutputMinimum,
					&lOutputMaximum);

				LOG_CHECK_STATE_DESCR(aOutputNodeAmount > lInputMaximum, MF_E_OUT_OF_RANGE);

				std::unique_ptr<DWORD[]> lInput(new (std::nothrow) DWORD[aOutputNodeAmount]);

				LOG_CHECK_PTR_MEMORY(lInput.get());

				DWORD lOutputID;

				LOG_INVOKE_POINTER_METHOD(lMixer, GetStreamIDs,
					aOutputNodeAmount,
					lInput.get(),
					1,
					&lOutputID);

				LOG_INVOKE_POINTER_METHOD(lMixer, AddInputStreams,
					aOutputNodeAmount,
					lInput.get());

								

				CComPtrCustom<IMFGetService> lGetService;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lPresenter, &lGetService);

				LOG_CHECK_PTR_MEMORY(lGetService);
				
				CComPtrCustom<IMFDXGIDeviceManager> lDeviceManager;

				LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
					CM_VIDEO_ACCELERATION_SERVICE,	
					IID_PPV_ARGS (& lDeviceManager));

				LOG_CHECK_PTR_MEMORY(lDeviceManager);

				lMixer->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, 0);

				lMixer->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, (ULONG_PTR)lDeviceManager.get());

				lMixer->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);

				for (UINT32 i = 0; i < aOutputNodeAmount; i++)
				{					
					CComPtrCustom<IMFActivate> lActivate;
					
					LOG_INVOKE_FUNCTION(EVR::EVRActivate<VideoRenderingClassFactory>::createInstance,
						lPresenter,
						lMixer,
						lInput.get()[i],
						&lActivate);

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


					CComPtrCustom<IUnknown> lUnknown;

					CComPtrCustom<CustomisedFilterTopologyNode> lCustomisedFilterTopologyNode(
						new (std::nothrow) CustomisedFilterTopologyNode(RendererActivateTopologyNode, lActivate));

					LOG_CHECK_PTR_MEMORY(lCustomisedFilterTopologyNode);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lCustomisedFilterTopologyNode, &lUnknown);
					
					aRefOutputNodes.push_back(lUnknown);
				}

			} while (false);

			return lresult;
		}
	}
}