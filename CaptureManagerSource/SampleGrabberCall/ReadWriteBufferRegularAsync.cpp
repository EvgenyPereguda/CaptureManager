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

#include "ReadWriteBufferRegularAsync.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../MemoryManager/MemoryManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberCall
		{
			namespace RegularSampleGrabberCall
			{
				ReadWriteBufferRegularAsync::ReadWriteBufferRegularAsync(DWORD aImageByteSize)
				{
					init(aImageByteSize);
				}


				ReadWriteBufferRegularAsync::~ReadWriteBufferRegularAsync()
				{
				}

				HRESULT ReadWriteBufferRegularAsync::readData(
					unsigned char* aPtrData,
					DWORD* aPtrSampleSize)
				{
					HRESULT lresult(S_FALSE);
					
					do
					{
						LOG_CHECK_PTR_MEMORY(aPtrSampleSize);
						
						if (mReadyToRead)
						{
							std::lock_guard<std::mutex> llock(mMutex);

							*aPtrSampleSize = mCurrentImageByteSize;

							copy(mData.get(), 
								aPtrData, 
								mCurrentImageByteSize,
								false);

							lresult = S_OK;
						}
						else
						{
							*aPtrSampleSize = 0;
						}

					} while (false);

					return lresult;
				}

				void ReadWriteBufferRegularAsync::copy(
					const unsigned char* aPtrSource,
					unsigned char *aPtrDestination,
					DWORD aSampleSize,
					bool aState)
				{
					Core::MemoryManager::memcpy(aPtrDestination, aPtrSource, aSampleSize);

					mCurrentImageByteSize = aSampleSize;

					mReadyToRead = aState;
				}
			}
		}
	}
}