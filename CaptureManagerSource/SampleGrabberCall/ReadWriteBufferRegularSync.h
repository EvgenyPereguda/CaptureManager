#pragma once

#include <condition_variable>

#include "ReadWriteBuffer.h"

namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberCall
		{
			namespace RegularSampleGrabberCall
			{
				class __declspec(uuid("ACD3F085-CBFB-464D-B8C9-99F75692B570")) ReadWriteBufferRegularSync;
				class ReadWriteBufferRegularSync :
					public ReadWriteBuffer
				{
				public:
					ReadWriteBufferRegularSync();

					HRESULT init(ULONG aImageByteSize);

					virtual ~ReadWriteBufferRegularSync();

					virtual HRESULT readData(
						unsigned char* aPtrData,
						DWORD* aPtrSampleSize);

				protected:

					virtual void copy(
						const unsigned char* aPtrSource,
						unsigned char *aPtrDestination,
						DWORD aSampleSize,
						bool aState);
					
				private:

					std::condition_variable mConditionVariable;

				};
			}
		}
	}
}