#pragma once
#include "../Common/MFHeaders.h"
#include "../CustomisedMediaPipelineProcessor/IMediaPipelineProcessorControl.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			MIDL_INTERFACE("D9AAE580-0D95-4061-B721-B91959295D6E")
			IPresentationClock : public IUnknown
			{
				virtual HRESULT addIMediaPipelineProcessorControl(IMediaPipelineProcessorControl* aPtrIMediaPipelineProcessorControl) = 0;

				virtual HRESULT removeIMediaPipelineProcessorControl(IMediaPipelineProcessorControl* aPtrIMediaPipelineProcessorControl) = 0;
				
				virtual HRESULT getPresentationClock(IMFPresentationClock** aPtrPtrIMFPresentationClock) = 0;

				virtual HRESULT shutdown() = 0;
				
			};
		}
	}
}