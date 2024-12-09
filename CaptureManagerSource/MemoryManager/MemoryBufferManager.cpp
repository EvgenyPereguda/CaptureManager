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

#include "MemoryBufferManager.h"
#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"

namespace CaptureManager
{
	namespace Core
	{
		using namespace MediaFoundation;

		namespace
		{

			template<typename MemmoryAllocator>
			class MemoryBufferManagerInner :
				public BaseUnknown<IMemoryBufferManager>
			{
				class ProxyMediaBuffer :
					public BaseUnknown<IMFMediaBuffer>
				{
				private:
					CComPtrCustom<IMFMediaBuffer> mIMFMediaBuffer;

					MemoryBufferManagerInner* mPtrParent;

				public:

					ProxyMediaBuffer(
						IMFMediaBuffer* aPtrMediaBuffer,
						MemoryBufferManagerInner* aPtrParent) :
						mIMFMediaBuffer(aPtrMediaBuffer),
						mPtrParent(aPtrParent)
					{}

					virtual ~ProxyMediaBuffer()
					{}

					void clearParent()
					{
						mPtrParent = nullptr;
					}

					virtual STDMETHODIMP_(ULONG) Release()
					{
						auto lCount = BaseUnknown::Release();

						if (lCount == 1 && mPtrParent != nullptr)
						{
							mPtrParent->pushMediaBuffer(this);
						}

						return lCount;
					}

					virtual HRESULT STDMETHODCALLTYPE Lock(
						BYTE **ppbBuffer,
						DWORD *pcbMaxLength,
						DWORD *pcbCurrentLength)
					{
						return mIMFMediaBuffer->Lock(ppbBuffer, pcbMaxLength, pcbCurrentLength);
					}

					virtual HRESULT STDMETHODCALLTYPE Unlock()
					{
						return mIMFMediaBuffer->Unlock();
					}

					virtual HRESULT STDMETHODCALLTYPE GetCurrentLength(
						DWORD *pcbCurrentLength)
					{
						return mIMFMediaBuffer->GetCurrentLength(pcbCurrentLength);
					}

					virtual HRESULT STDMETHODCALLTYPE SetCurrentLength(
						DWORD cbCurrentLength)
					{
						return mIMFMediaBuffer->SetCurrentLength(cbCurrentLength);
					}

					virtual HRESULT STDMETHODCALLTYPE GetMaxLength(
						DWORD *pcbMaxLength)
					{
						return mIMFMediaBuffer->GetMaxLength(pcbMaxLength);
					}
				};

			public:
				MemoryBufferManagerInner(DWORD aMaxBufferLength) :
					mMaxBufferLength(aMaxBufferLength) {}

				virtual HRESULT getMediaBuffer(
					IMFMediaBuffer** aPtrPtrMediaBuffer)
				{

					HRESULT lresult;

					do
					{
						if (mBufferStack.empty())
						{
							IMFMediaBuffer* lPtrMediaBuffer;

							LOG_INVOKE_FUNCTION(mMemmoryAllocator,
								mMaxBufferLength,
								&lPtrMediaBuffer);

							LOG_CHECK_PTR_MEMORY(lPtrMediaBuffer);

							CComPtrCustom<IMFMediaBuffer> lMediaBuffer = new (std::nothrow) ProxyMediaBuffer(
								lPtrMediaBuffer,
								this);

							mMemoryList.push_back(lMediaBuffer);

							*aPtrPtrMediaBuffer = lMediaBuffer.detach();

						}
						else
						{
							popMediaBuffer(aPtrPtrMediaBuffer);

							lresult = S_OK;
						}


					} while (false);

					return lresult;
				}

				virtual void clearMemory()
				{
					{
						std::lock_guard<std::mutex> lLock(mAccessMutex);

						for (auto& item : mMemoryList)
						{
							((ProxyMediaBuffer*)item.get())->clearParent();
						}
						
						while (!mBufferStack.empty())
						{
							mBufferStack.pop();
						}
					}

					mMemoryList.clear();
				}


				static HRESULT createSystem(IMemoryBufferManager** aPtrPtrMemoryBufferManager);
			private:
				virtual ~MemoryBufferManagerInner() {}

				MemmoryAllocator mMemmoryAllocator;

				void pushMediaBuffer(IMFMediaBuffer* aPtrMediaBuffer)
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					mBufferStack.push(aPtrMediaBuffer);
				}

				void popMediaBuffer(IMFMediaBuffer** aPtrPtrMediaBuffer)
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					*aPtrPtrMediaBuffer = mBufferStack.top();

					(*aPtrPtrMediaBuffer)->AddRef();

					mBufferStack.pop();
				}


				DWORD mMaxBufferLength;

				std::stack<IMFMediaBuffer*> mBufferStack;

				std::list<CComPtrCustom<IMFMediaBuffer>> mMemoryList;

				std::mutex mAccessMutex;
			};

		}

		HRESULT MemoryBufferManager::createSystemMemoryBufferManager(
			DWORD aMaxBufferLength,
			IMemoryBufferManager** aPtrPtrMemoryBufferManager)
		{
			struct SystemMemoryAllocator
			{
				HRESULT operator()(
					DWORD aMaxBufferLength,
					IMFMediaBuffer** aPtrPtrMediaBuffer) const {

					HRESULT lresult(E_FAIL);

					do
					{
						LOG_INVOKE_MF_FUNCTION(MFCreateMemoryBuffer,
							aMaxBufferLength,
							aPtrPtrMediaBuffer);

					} while (false);

					return lresult;
				};
			};


			HRESULT lresult = E_FAIL;

			do
			{

				CComPtrCustom<IMemoryBufferManager> lMemoryBufferManager(new (std::nothrow) MemoryBufferManagerInner<SystemMemoryAllocator>(aMaxBufferLength));

				LOG_CHECK_PTR_MEMORY(lMemoryBufferManager);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lMemoryBufferManager, aPtrPtrMemoryBufferManager);

			} while (false);

			return lresult;
		}
	}
}