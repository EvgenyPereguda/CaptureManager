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
#include "EVRActivate.h"
#include "EVRMediaSinkFactory.h"
#include "PresenterFactory.h"
#include <memory>


namespace CaptureManager
{
	namespace Sinks
	{
		using namespace Core;

		using namespace EVR;

		VideoRendererFactory::VideoRendererFactory()
		{
		}
		
		VideoRendererFactory::~VideoRendererFactory()
		{
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

				HRESULT lcreateType = PresenterFactory::createPresenter(
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
				
				CComPtrCustom<IUnknown> lDeviceManager;

				bool lisAllowSingleStream = true;


				do
				{
					LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
						MR_VIDEO_ACCELERATION_SERVICE,
						__uuidof(IDirect3DDeviceManager9),
						(void**)&lDeviceManager);

				} while (false);

				if (FAILED(lresult))
				{
					LOG_INVOKE_POINTER_METHOD(lGetService, GetService,
						MR_VIDEO_ACCELERATION_SERVICE,
						__uuidof(IMFDXGIDeviceManager),
						(void**)&lDeviceManager);

					if (lcreateType == S_FALSE)
						lisAllowSingleStream = false;
				}
				

				LOG_CHECK_PTR_MEMORY(lDeviceManager);

				lMixer->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, 0);

				lMixer->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, (ULONG_PTR)lDeviceManager.get());

				lMixer->ProcessMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);

				for (UINT32 i = 0; i < aOutputNodeAmount; i++)
				{					
					CComPtrCustom<IMFActivate> lActivate;

					LOG_INVOKE_FUNCTION(EVR::EVRActivate<EVR::EVRMediaSinkFactory>::createInstance,
						lPresenter,
						lMixer,
						lInput.get()[i],
						(aOutputNodeAmount == 1) && lisAllowSingleStream,
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

					LOG_INVOKE_QUERY_INTERFACE_METHOD(RendererActivateTopologyNode, &lUnknown);

					aRefOutputNodes.push_back(lUnknown);
				}

			} while (false);

			return lresult;
		}

		void VideoRendererFactory::getReadModes(
			BOOL aMultiOutputNodeVideoRenderer,
			std::vector<GUIDToNamePair>& aRefReadModes)
		{
			if (aMultiOutputNodeVideoRenderer == FALSE)
			{
				GUIDToNamePair lGUIDFormat;

				lGUIDFormat.mSinkTypes.push_back(SinkType::SampleGrabberCall);

				lGUIDFormat.mGUID =
				{ 0x71fba544, 0x3a8e, 0x4d6c, { 0xb3, 0x22, 0x98, 0x18, 0x4b, 0xc8, 0xdc, 0xea } };

				lGUIDFormat.mName = L"Default";

				lGUIDFormat.mMaxPortCount = 1;

				lGUIDFormat.mTitle = L"Default EVR implementation";

				aRefReadModes.push_back(lGUIDFormat);
			}
			else
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
		}
	}
}