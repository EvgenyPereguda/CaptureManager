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
		class RgbToNV12 :
			public BaseUnknown<ISampleConvertor>
		{

		public:
			RgbToNV12();

			virtual HRESULT Convert(IMFSample* aPtrInputSample, IMFSample** aPtrPtrOutputSample) override;
			
			HRESULT init(
				IMFMediaType* aPtrInputMediaType,
				IMFMediaType* aPtrOutputMediaType);

		private:

			CComPtrCustom<Core::IMemoryBufferManager> mMemoryBufferManager;

			int image_width;
			int image_height;
			int src_stride;
			int dst_stride;

			UINT32 mOutputSampleSize;

			virtual ~RgbToNV12();

			void convert(const unsigned char* I, unsigned char* J);
		};
	}
}