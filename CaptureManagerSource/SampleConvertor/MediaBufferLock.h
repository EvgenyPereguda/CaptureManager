#pragma once

#include "../Common/MFHeaders.h"
//#include "../LogPrintOut/LogPrintOut.h"
//#include "../Common/Common.h"
//#include "../MediaFoundationManager/MediaFoundationManager.h"


namespace CaptureManager
{
	namespace SampleConvertorInner
	{
		using namespace Core;

		class MediaBufferLock
		{
		public:
			MediaBufferLock(
				IMFMediaBuffer* aPtrInputBuffer,
				DWORD& aRefMaxLength,
				DWORD& aRefCurrentLength,
				BYTE** aPtrPtrInputBuffer,
				HRESULT& aRefResult)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrInputBuffer);

					LOG_CHECK_PTR_MEMORY(aPtrPtrInputBuffer);

					LOG_INVOKE_MF_METHOD(Lock,
						aPtrInputBuffer,
						aPtrPtrInputBuffer,
						&aRefMaxLength,
						&aRefCurrentLength);

					LOG_CHECK_PTR_MEMORY(aPtrPtrInputBuffer);

					mInputBuffer = aPtrInputBuffer;

				} while (false);

				aRefResult = lresult;
			}

			~MediaBufferLock()
			{
				HRESULT lresult;

				do
				{

					if (mInputBuffer)
					{
						LOG_INVOKE_MF_METHOD(Unlock,
							mInputBuffer);
					}

				} while (false);
			}

		private:

			CComPtrCustom<IMFMediaBuffer> mInputBuffer;

			MediaBufferLock(
				const MediaBufferLock&){}

			MediaBufferLock& operator=(
				const MediaBufferLock&){
				return *this;
			}

		};
	}
}

