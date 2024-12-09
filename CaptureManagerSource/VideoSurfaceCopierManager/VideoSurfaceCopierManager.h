#pragma once

#include <Unknwn.h>


namespace CaptureManager
{
	namespace Transform
	{
		class VideoSurfaceCopierManager
		{
		public:

			HRESULT createVideoSurfaceCopier(
				IUnknown* aPtrDeviceManager,
				IUnknown* aPtrMediaType,
				IUnknown** aPtrPtrTransform);

		protected:
			VideoSurfaceCopierManager();
			virtual ~VideoSurfaceCopierManager();
		};
	}
}