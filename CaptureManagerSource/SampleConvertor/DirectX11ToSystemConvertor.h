#pragma once

#include "../Common/BaseUnknown.h"
#include "../Common/ComPtrCustom.h"
#include "ISampleConvertor.h" 
#include "../Common/MFHeaders.h"
#include "../MemoryManager/IMemoryBufferManager.h"


namespace CaptureManager
{
	namespace SampleConvertorInner
	{
		class DirectX11ToSystemConvertor :
			public BaseUnknown<ISampleConvertor>
		{

		public:
			DirectX11ToSystemConvertor();

			virtual HRESULT Convert(IMFSample* aPtrInputSample, IMFSample** aPtrPtrOutputSample) override;


			HRESULT init(
				IUnknown* aPtrInputDeviceManager,
				IMFMediaType* aPtrInputMediaType);

		private:
			
			CComPtrCustom<Core::IMemoryBufferManager> mMemoryBufferManager;
						
			virtual ~DirectX11ToSystemConvertor();

			HRESULT videoMemoryCopy(IMFSample* aPtrInputSample, IMFMediaBuffer* aPtrDstBuffer);
		};
	}
}