#pragma once

#include "ReadWriteBuffer.h"


namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberCall
		{
			namespace RegularSampleGrabberCall
			{
				class ReadWriteBufferRegularAsync :
					public ReadWriteBuffer
				{
				public:
					ReadWriteBufferRegularAsync(
						DWORD aImageByteSize);

					virtual ~ReadWriteBufferRegularAsync();

					virtual HRESULT readData(
						unsigned char* aPtrData,
						DWORD* aPtrSampleSize);

					virtual void copy(
						const unsigned char* aPtrSource,
						unsigned char *aPtrDestination,
						DWORD aSampleSize,
						bool aState);
				};
			}
		}
	}
}