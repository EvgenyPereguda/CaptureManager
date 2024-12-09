#include "pch.h"
#include "SampleGrabberCallbackSinkFactory.h"
#include "TopologyNode.h"
#include "BaseUnknown.h"

#include <memory>


using namespace Windows::Storage::Streams;

namespace CaptureManagerToWinRTProxy
{

	class WriteableBitmapCallbackProxy :
		public BaseUnknown <::ISampleGrabberCallback>
	{

		int mWidth = 0;
		int mHeight = 0;

		CaptureManagerToWinRTProxy::IWriteableBitmapCallback^ mWriteableBitmapCallback;

	public:

		WriteableBitmapCallbackProxy(
			CaptureManagerToWinRTProxy::IWriteableBitmapCallback^ aPtrIWriteableBitmapCallback,
			int aWidth,
			int aHeight)
		{
			mWidth = aWidth;
			mHeight = aHeight;

			do
			{				
				mWriteableBitmapCallback = aPtrIWriteableBitmapCallback;

			} while (false);
		}

		virtual /* [local][helpstring] */ HRESULT STDMETHODCALLTYPE invoke(
			/* [in] */ REFGUID aGUIDMajorMediaType,
			/* [in] */ DWORD aSampleFlags,
			/* [in] */ LONGLONG aSampleTime,
			/* [in] */ LONGLONG aSampleDuration,
			/* [in] */ LPVOID aPtrSampleBuffer,
			/* [in] */ DWORD aSampleSize)
		{


			do
			{
				if (mWriteableBitmapCallback == nullptr)
					break;

				std::shared_ptr<byte> lbuffer(new byte[aSampleSize], [](byte* ptr){delete[] ptr; });

				memcpy(lbuffer.get(), aPtrSampleBuffer, aSampleSize);

				auto lDispatcher = mWriteableBitmapCallback->getDispatcher();

				if (lDispatcher == nullptr)
					break;

				lDispatcher->RunAsync(
					Windows::UI::Core::CoreDispatcherPriority::Normal,
					ref new Windows::UI::Core::DispatchedHandler([this,
					lbuffer,
					aGUIDMajorMediaType,
					aSampleFlags,
					aSampleTime,
					aSampleDuration]() {

					auto lWriteableBitmap = fromArray(
						lbuffer.get(),
						mWidth,
						mHeight,
						4);
					
					mWriteableBitmapCallback->invoke(
						aGUIDMajorMediaType,
						aSampleFlags,
						aSampleTime,
						aSampleDuration,
						lWriteableBitmap);
				}));

			} while (false);

			return S_OK;
		}

		private:

			Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ fromArray(byte* data, int w, int h, int ch)
			{
				Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ wbm = ref new 
					Windows::UI::Xaml::Media::Imaging::WriteableBitmap(w, h);
				
				IUnknown* pUnk = reinterpret_cast<IUnknown*>(wbm->PixelBuffer);

				CComPtrCustom<IBufferByteAccess> lIBufferByteAccess;
				HRESULT hr = pUnk->QueryInterface(IID_PPV_ARGS(&lIBufferByteAccess));
				byte *pbytes = nullptr;
				hr = lIBufferByteAccess->Buffer(&pbytes);

				auto ll = wbm->PixelBuffer->Length;

				memcpy(pbytes, data, wbm->PixelBuffer->Length);
				
				return wbm;
			}

	};

	
	SampleGrabberCallbackSinkFactory::SampleGrabberCallbackSinkFactory(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mISampleGrabberCallbackSinkFactory));

		} while (false);
	}

	ITopologyNode^ SampleGrabberCallbackSinkFactory::createOutputNode(
		Guid aRefMajorType,
		Guid aRefSubType,
		ISampleGrabberCallback^ aPtrISampleGrabberCallback)
	{
		class SampleGrabberCallbackProxy :
			public BaseUnknown <::ISampleGrabberCallback>
		{
			CaptureManagerToWinRTProxy::ISampleGrabberCallback^ mPtrISampleGrabberCallback;

		public:

			SampleGrabberCallbackProxy(
				CaptureManagerToWinRTProxy::ISampleGrabberCallback^ aPtrISampleGrabberCallback)
			{
				do
				{					
					mPtrISampleGrabberCallback = aPtrISampleGrabberCallback;
										
				} while (false);
			}

			virtual /* [local][helpstring] */ HRESULT STDMETHODCALLTYPE invoke(
				/* [in] */ REFGUID aGUIDMajorMediaType,
				/* [in] */ DWORD aSampleFlags,
				/* [in] */ LONGLONG aSampleTime,
				/* [in] */ LONGLONG aSampleDuration,
				/* [in] */ LPVOID aPtrSampleBuffer,
				/* [in] */ DWORD aSampleSize)
			{
				do
				{
					if (mPtrISampleGrabberCallback == nullptr)
						break;

					mPtrISampleGrabberCallback->invoke(
						aGUIDMajorMediaType,
						aSampleFlags,
						aSampleTime,
						aSampleDuration,
						(uint8*)aPtrSampleBuffer,
						aSampleSize);
					
				} while (false);

				return S_OK;
			}

		};



		ITopologyNode^ lresult;

		do
		{
			if (!mISampleGrabberCallbackSinkFactory)
				break;

			if (aPtrISampleGrabberCallback == nullptr)
				break;
			
			CComPtrCustom<IUnknown> lPtrUnkISampleGrabberCallback = new (std::nothrow) SampleGrabberCallbackProxy(
				aPtrISampleGrabberCallback);

			if (lPtrUnkISampleGrabberCallback == nullptr)
				break;
			
			CComPtrCustom<IUnknown> lOutputNode;

			auto lhr = mISampleGrabberCallbackSinkFactory->createOutputNode(
				aRefMajorType,
				aRefSubType, 
				lPtrUnkISampleGrabberCallback,
				&lOutputNode);

			if (FAILED(lhr))
				break;

			if (!lOutputNode)
				break;

			lresult = ref new TopologyNode(IntPtr(lOutputNode.get()));
			
		} while (false);

		return lresult;
	}

	ITopologyNode^ SampleGrabberCallbackSinkFactory::createOutputNode(
		Guid aRefMajorType,
		Guid aRefSubType,
		int32 aWidth,
		int32 aHeight,
		IWriteableBitmapCallback^ aPtrIWriteableBitmapCallback)
	{

		ITopologyNode^ lresult;

		do
		{
			if (!mISampleGrabberCallbackSinkFactory)
				break;

			if (aPtrIWriteableBitmapCallback == nullptr)
				break;
			
			CComPtrCustom<IUnknown> lUnkISampleGrabberCallback = new (std::nothrow) WriteableBitmapCallbackProxy(
				aPtrIWriteableBitmapCallback,
				aWidth,
				aHeight);

			if (lUnkISampleGrabberCallback == nullptr)
				break;

			CComPtrCustom<IUnknown> lOutputNode;

			auto lhr = mISampleGrabberCallbackSinkFactory->createOutputNode(
				aRefMajorType,
				aRefSubType,
				lUnkISampleGrabberCallback,
				&lOutputNode);

			if (FAILED(lhr))
				break;

			if (!lOutputNode)
				break;

			lresult = ref new TopologyNode(IntPtr(lOutputNode.get()));

		} while (false);

		return lresult;
	}
}