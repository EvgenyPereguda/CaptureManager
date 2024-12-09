#include "pch.h"
#include "SourceControl.h"
#include "MediaType.h"
#include "TopologyNode.h"
#include "MediaSource.h"
#include "WebCamControl.h"
#include "BaseUnknown.h"

namespace CaptureManagerToWinRTProxy
{
	
	SourceControl::SourceControl(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mISourceControl));

		} while (false);
	}

	String^ SourceControl::getCollectionOfSources()
	{
		Platform::String^ lresult = nullptr;

		do
		{
			if (!mISourceControl)
				break;

			BSTR lXMLstring = nullptr;
			
			CHK(mISourceControl->getCollectionOfSources(&lXMLstring));

			if (lXMLstring == nullptr)
				break;

			lresult = ref new String(lXMLstring);

			SysFreeString(lXMLstring);

		} while (false);

		return lresult;
	}

	IMediaType^ SourceControl::getSourceOutputMediaType(
		String^ aSymbolicLink,
		uint32 aIndexStream,
		uint32 aIndexMediaType)
	{
		IMediaType^ lresult = nullptr;

		do
		{
			if (!mISourceControl)
				break;

			if (aSymbolicLink == nullptr)
				break;

			CComPtrCustom<IUnknown> lOutputMediaType;

			std::unique_ptr<OLECHAR> lStrSymbolicLink(new (std::nothrow)OLECHAR[aSymbolicLink->Length() + 1]);

			memcpy(lStrSymbolicLink.get(),
				aSymbolicLink->Data(),
				(aSymbolicLink->Length() + 1)*sizeof(OLECHAR));

			lStrSymbolicLink.get()[aSymbolicLink->Length()] = '\0';
			
			BSTR lSymbolicLink = lStrSymbolicLink.get();

			CHK(mISourceControl->getSourceOutputMediaType(
				lSymbolicLink,
				aIndexStream,
				aIndexMediaType,
				&lOutputMediaType));

			if (!lOutputMediaType)
				break;

			lresult = ref new MediaType(IntPtr(lOutputMediaType.get()));

		} while (false);

		return lresult;
	}

	ITopologyNode^ SourceControl::createSourceNode(
		String^ aSymbolicLink,
		uint32 aIndexStream,
		uint32 aIndexMediaType)
	{
		ITopologyNode^ lresult = nullptr;

		do
		{
			if (!mISourceControl)
				break;

			if (aSymbolicLink == nullptr)
				break;

			CComPtrCustom<IUnknown> lSourceNode;


			auto lSize = aSymbolicLink->Length();

			std::unique_ptr<OLECHAR[]> lStrSymbolicLink(new OLECHAR[lSize + 1]);

			memcpy(lStrSymbolicLink.get(),
				aSymbolicLink->Data(),
				(lSize + 1)*sizeof(OLECHAR));

			lStrSymbolicLink.get()[lSize] = '\0';

			CHK(mISourceControl->createSourceNode(
				lStrSymbolicLink.get(),
				aIndexStream,
				aIndexMediaType,
				&lSourceNode));

			if (!lSourceNode)
				break;

			lresult = ref new TopologyNode(IntPtr(lSourceNode.get()));

		} while (false);

		return lresult;
	}

	ITopologyNode^ SourceControl::createSourceNodeWithDownStreamConnection(
		String^ aSymbolicLink,
		uint32 aIndexStream,
		uint32 aIndexMediaType,
		ITopologyNode^ aPtrDownStreamTopologyNode)
	{
		ITopologyNode^ lresult = nullptr;

		do
		{
			if (!mISourceControl)
				break;

			if (aSymbolicLink == nullptr)
				break;

			if (aPtrDownStreamTopologyNode == nullptr)
				break;

			CComPtrCustom<IUnknown> lSourceNode;


			auto lSize = aSymbolicLink->Length();

			std::unique_ptr<OLECHAR[]> lStrSymbolicLink(new OLECHAR[lSize + 1]);

			memcpy(lStrSymbolicLink.get(),
				aSymbolicLink->Data(),
				(lSize + 1)*sizeof(OLECHAR));

			lStrSymbolicLink.get()[lSize] = '\0';

			
			TopologyNode^ lTopologyNode = (TopologyNode^)aPtrDownStreamTopologyNode;

			if (lTopologyNode == nullptr)
				break;


			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)lTopologyNode->getTopologyNode());

			if (lPtrUnk == nullptr)
				break;
			
			CHK(mISourceControl->createSourceNodeWithDownStreamConnection(
				lStrSymbolicLink.get(),
				aIndexStream,
				aIndexMediaType,
				lPtrUnk,
				&lSourceNode));

			if (!lSourceNode)
				break;

			lresult = ref new TopologyNode(IntPtr(lSourceNode.get()));

		} while (false);

		return lresult;
	}

	ref class InitilaizeCaptureSource sealed: 
		public IInitilaizeCaptureSource
	{
	internal:
		String^ mPresentationDescriptor;

	public:
		InitilaizeCaptureSource(){}
		virtual ~InitilaizeCaptureSource(){}

		virtual void setPresentationDescriptor(String^ aPresentationDescriptor)override
		{
			mPresentationDescriptor = aPresentationDescriptor;
		}


	private:

	};

	ref class CurrentMediaType sealed :
		public ICurrentMediaType
	{
	internal:

		uint32 mStreamIndex;

		uint32 mMediaTypeIndex;

		IMediaType^ mIMediaType;
	public:
		CurrentMediaType(){}
		virtual ~CurrentMediaType(){}
		

		virtual IMediaType^ getMediaType()override
		{
			return mIMediaType;
		}

		virtual uint32 getMediaTypeIndex()override
		{
			return mMediaTypeIndex;
		}

		virtual uint32 getStreamIndex()override
		{
			return mStreamIndex;
		}

	private:
	};

	ref class SourceRequestResult sealed :
		public ISourceRequestResult
	{
		CComPtrCustom<::ISourceRequestResult> mISourceRequestResult;

	internal:

		uint32 mStreamIndex;

	public:
		SourceRequestResult(IntPtr aIntPtr)
		{
			do
			{
				auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

				if (lPtrUnk == nullptr)
					break;

				lPtrUnk->QueryInterface(IID_PPV_ARGS(&mISourceRequestResult));

			} while (false);
		}
		virtual ~SourceRequestResult(){}


		virtual uint32 getStreamIndex()override
		{
			return mStreamIndex;
		}

		virtual void setData(
			const Platform::Array<uint8>^ aData,
			bool aIsKeyFrame)override
		{

			if (aData == nullptr)
				return;
						
			if (!mISourceRequestResult)
				return;

			mISourceRequestResult->setData(
				aData->begin(),
				aData->Length,
				aIsKeyFrame);
		}
		
	};

	IMediaSource^ SourceControl::createMediaSource(ICaptureProcessor^ aICaptureProcessor)
	{
		class CaptureProcessorProxy :
			public BaseUnknown<::ICaptureProcessor>
		{

			CaptureManagerToWinRTProxy::ICaptureProcessor^ mICaptureProcessor;

		public:
			CaptureProcessorProxy(
				CaptureManagerToWinRTProxy::ICaptureProcessor^ aICaptureProcessor)
			{
				mICaptureProcessor = aICaptureProcessor;
			}

			virtual ~CaptureProcessorProxy(){}


		// ICaptureProcessor methods
	
		public:
			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE start(
				/* [in] */ LONGLONG aStartPositionInHundredNanosecondUnits,
				/* [in] */ REFGUID aGUIDTimeFormat)override
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					if (mICaptureProcessor == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					mICaptureProcessor->start(
						aStartPositionInHundredNanosecondUnits,
						aGUIDTimeFormat);

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE stop()override
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					if (mICaptureProcessor == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					mICaptureProcessor->stop();

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE pause()override
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					if (mICaptureProcessor == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					mICaptureProcessor->pause();

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE shutdown()override
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					if (mICaptureProcessor == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					mICaptureProcessor->shutdown();

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE initilaize(
				/* [in] */ IUnknown *aPtrIInitilaizeCaptureSource)override
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					if (mICaptureProcessor == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrIInitilaizeCaptureSource == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					CComQIPtrCustom<::IInitilaizeCaptureSource> lIInitilaizeCaptureSource = aPtrIInitilaizeCaptureSource;
					
					if (lIInitilaizeCaptureSource == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					InitilaizeCaptureSource^ lInitilaizeCaptureSource = ref new InitilaizeCaptureSource();
					
					mICaptureProcessor->initilaize(lInitilaizeCaptureSource);


					auto lSize = lInitilaizeCaptureSource->mPresentationDescriptor ->Length();

					std::unique_ptr<OLECHAR[]> lPresentationDescriptor(new OLECHAR[lSize + 1]);

					memcpy(lPresentationDescriptor.get(),
						lInitilaizeCaptureSource->mPresentationDescriptor->Data(),
						(lSize + 1)*sizeof(OLECHAR));

					lPresentationDescriptor.get()[lSize] = '\0';

					lresult = lIInitilaizeCaptureSource->setPresentationDescriptor(lPresentationDescriptor.get());

				} while (false);

				return lresult;
			}

			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE setCurrentMediaType(
				/* [in] */ IUnknown *aPtrICurrentMediaType)override
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					if (mICaptureProcessor == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrICurrentMediaType == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					CComQIPtrCustom<::ICurrentMediaType> lICurrentMediaType = aPtrICurrentMediaType;

					if (lICurrentMediaType == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					CurrentMediaType^ lCurrentMediaType = ref new CurrentMediaType();

					DWORD lStreamIndex = 0;

					lICurrentMediaType->getStreamIndex(&lStreamIndex);

					lCurrentMediaType->mStreamIndex = lStreamIndex;

					DWORD lMediaTypeIndex = 0;

					lICurrentMediaType->getMediaTypeIndex(&lMediaTypeIndex);

					lCurrentMediaType->mMediaTypeIndex = lMediaTypeIndex;

					CComPtrCustom<IUnknown> lUnkMediaType;

					lICurrentMediaType->getMediaType(&lUnkMediaType);


					IMediaType^ lMediaType = ref new MediaType(IntPtr(lUnkMediaType.get()));

					lCurrentMediaType->mIMediaType = lMediaType;

					mICaptureProcessor->setCurrentMediaType(lCurrentMediaType);

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE sourceRequest(
				/* [in] */ IUnknown *aPtrISourceRequestResult)override
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					if (mICaptureProcessor == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					if (aPtrISourceRequestResult == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					CComQIPtrCustom<::ISourceRequestResult> lISourceRequestResult = aPtrISourceRequestResult;

					if (lISourceRequestResult == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					DWORD lStreamIndex = 0;

					lISourceRequestResult->getStreamIndex(&lStreamIndex);

					SourceRequestResult^ lSourceRequestResult = ref new SourceRequestResult(IntPtr(lISourceRequestResult.get()));

					lSourceRequestResult->mStreamIndex = lStreamIndex;

					mICaptureProcessor->sourceRequest(lSourceRequestResult);

					lresult = S_OK;

				} while (false);

				return lresult;
			}

		private:

		};

		
		

		IMediaSource^ lresult = nullptr;

		do
		{
			if (!mISourceControl)
				break;

			if (aICaptureProcessor == nullptr)
				break;

			CComPtrCustom<::ICaptureProcessor> lCaptureProcessorProxy(new (std::nothrow) CaptureProcessorProxy(aICaptureProcessor));

			if (!lCaptureProcessorProxy)
				break;

			CComPtrCustom<IUnknown> lMediaSource;

			CHK(mISourceControl->createSourceFromCaptureProcessor(
				lCaptureProcessorProxy,
				&lMediaSource));

			if (!lMediaSource)
				break;

			lresult = ref new MediaSource(IntPtr(lMediaSource.get()));

		} while (false);

		return lresult;
	}

	IMediaSource^ SourceControl::createMediaSource(String^ aSymbolicLink)
	{
		IMediaSource^ lresult = nullptr;

		do
		{
			if (!mISourceControl)
				break;

			if (aSymbolicLink == nullptr)
				break;

			CComPtrCustom<IUnknown> lMediaSource;


			auto lSize = aSymbolicLink->Length();

			std::unique_ptr<OLECHAR[]> lStrSymbolicLink(new OLECHAR[lSize + 1]);

			memcpy(lStrSymbolicLink.get(),
				aSymbolicLink->Data(),
				(lSize + 1)*sizeof(OLECHAR));

			lStrSymbolicLink.get()[lSize] = '\0';
			
			CHK(mISourceControl->createSource(
				lStrSymbolicLink.get(),
				&lMediaSource));

			if (!lMediaSource)
				break;

			lresult = ref new MediaSource(IntPtr(lMediaSource.get()));

		} while (false);

		return lresult;
	}

	IMediaType^ SourceControl::getSourceOutputMediaTypeFromMediaSource(
		IMediaSource^ aPtrMediaSource,
		uint32 aIndexStream,
		uint32 aIndexMediaType)
	{
		IMediaType^ lresult = nullptr;

		do
		{
			if (!mISourceControl)
				break;

			if (aPtrMediaSource == nullptr)
				break;

			MediaSource^ lMediaSource = (MediaSource^)aPtrMediaSource;

			if (lMediaSource == nullptr)
				break;

			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)lMediaSource->getMediaSource());

			if (lPtrUnk == nullptr)
				break;

			CComPtrCustom<IUnknown> lOutputMediaType;



			CHK(mISourceControl->getSourceOutputMediaTypeFromMediaSource(
				lPtrUnk,
				aIndexStream,
				aIndexMediaType,
				&lOutputMediaType));

			if (!lOutputMediaType)
				break;

			lresult = ref new MediaType(IntPtr(lOutputMediaType.get()));

		} while (false);

		return lresult;
	}

	ITopologyNode^ SourceControl::createSourceNodeFromExternalSource(
		IMediaSource^ aPtrMediaSource,
		uint32 aIndexStream,
		uint32 aIndexMediaType)
	{
		ITopologyNode^ lresult = nullptr;

		do
		{
			if (!mISourceControl)
				break;

			if (aPtrMediaSource == nullptr)
				break;

			MediaSource^ lMediaSource = (MediaSource^)aPtrMediaSource;

			if (lMediaSource == nullptr)
				break;

			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)lMediaSource->getMediaSource());

			if (lPtrUnk == nullptr)
				break;

			CComPtrCustom<IUnknown> lSourceNode;

			CHK(mISourceControl->createSourceNodeFromExternalSource(
				lPtrUnk,
				aIndexStream,
				aIndexMediaType,
				&lSourceNode));

			if (!lSourceNode)
				break;
			
			lresult = ref new TopologyNode(IntPtr(lSourceNode.get()));

		} while (false);

		return lresult;
	}

	ITopologyNode^ SourceControl::createSourceNodeFromExternalSourceWithDownStreamConnection(
		IMediaSource^ aPtrMediaSource,
		uint32 aIndexStream,
		uint32 aIndexMediaType,
		ITopologyNode^ aPtrDownStreamTopologyNode)
	{
		ITopologyNode^ lresult = nullptr;

		do
		{
			if (!mISourceControl)
				break;

			if (aPtrMediaSource == nullptr)
				break;

			if (aPtrDownStreamTopologyNode == nullptr)
				break;

			MediaSource^ lMediaSource = (MediaSource^)aPtrMediaSource;

			if (lMediaSource == nullptr)
				break;

			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)lMediaSource->getMediaSource());

			if (lPtrUnk == nullptr)
				break;





			TopologyNode^ lDownStreamTopologyNode = (TopologyNode^)aPtrDownStreamTopologyNode;

			if (lDownStreamTopologyNode == nullptr)
				break;
			
			auto lPtrDownStreamNodeUnk = reinterpret_cast<IUnknown*>((void*)lDownStreamTopologyNode->getTopologyNode());

			if (lPtrDownStreamNodeUnk == nullptr)
				break;



			CComPtrCustom<IUnknown> lSourceNode;

			CHK(mISourceControl->createSourceNodeFromExternalSourceWithDownStreamConnection(
				lPtrUnk,
				aIndexStream,
				aIndexMediaType,
				lPtrDownStreamNodeUnk,
				&lSourceNode));

			if (!lSourceNode)
				break;

			lresult = ref new TopologyNode(IntPtr(lSourceNode.get()));

		} while (false);

		return lresult;
	}

	IWebCamControl^ SourceControl::createWebCamControl(String^ aSymbolicLink)
	{
		IWebCamControl^ lresult = nullptr;

		do
		{
			if (!mISourceControl)
				break;

			if (aSymbolicLink == nullptr)
				break;

			CComPtrCustom<IUnknown> lOutputMediaType;

			std::unique_ptr<OLECHAR> lStrSymbolicLink(new (std::nothrow)OLECHAR[aSymbolicLink->Length()]);

			memcpy(lStrSymbolicLink.get(),
				aSymbolicLink->Data(),
				(aSymbolicLink->Length() + 1)*sizeof(OLECHAR));

			lStrSymbolicLink.get()[aSymbolicLink->Length()] = '\0';

			BSTR lSymbolicLink = lStrSymbolicLink.get();

			
			CComPtrCustom<::IWebCamControl> lIWebCamControl;
			
			CHK(mISourceControl->createSourceControl(
				lSymbolicLink,
				IID_PPV_IUNKNOWN_ARGS(&lIWebCamControl)
				));

			if (!lIWebCamControl)
				break;

			lresult = ref new WebCamControl(IntPtr(lIWebCamControl.get()));

		} while (false);

		return lresult;
	}
}