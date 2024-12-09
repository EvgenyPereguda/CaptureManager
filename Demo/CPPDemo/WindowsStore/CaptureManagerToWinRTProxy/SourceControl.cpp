#include "pch.h"
#include "SourceControl.h"
#include "MediaType.h"
#include "TopologyNode.h"
#include "MediaSource.h"
#include "WebCamControl.h"

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

			std::unique_ptr<OLECHAR> lStrSymbolicLink(new (std::nothrow)OLECHAR[aSymbolicLink->Length()]);

			BSTR lSymbolicLink = lStrSymbolicLink.get();

			CHK(mISourceControl->createSourceNode(
				lSymbolicLink,
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

			std::unique_ptr<OLECHAR[]> lStrSymbolicLink(new (std::nothrow)OLECHAR[aSymbolicLink->Length()]);

			BSTR lSymbolicLink = lStrSymbolicLink.get();
			
			CHK(mISourceControl->createSource(
				lSymbolicLink,
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