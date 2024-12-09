#pragma once
#include <Unknwnbase.h>

struct IMFMediaType;
struct IMFSample;
struct IMFMediaEventGenerator;
struct IMFTransform;

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			MIDL_INTERFACE("CB75F4ED-6F7E-4DB4-BD6D-E7E374032FB3")
			IMixerWrapper : public IUnknown
			{
				virtual HRESULT initialize(
				IMFMediaType* aPtrMediaTypeType) = 0;
			
				virtual HRESULT getMixer(
					IMFTransform** aPtrMixer) = 0;
			};
		}
	}
}
