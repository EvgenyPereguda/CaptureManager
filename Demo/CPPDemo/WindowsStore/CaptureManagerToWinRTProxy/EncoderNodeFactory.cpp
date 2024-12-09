#include "pch.h"
#include "EncoderNodeFactory.h"
#include "MediaType.h"
#include "TopologyNode.h"


namespace CaptureManagerToWinRTProxy
{
	EncoderNodeFactory::EncoderNodeFactory(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIEncoderNodeFactory));

		} while (false);
	}

	IMediaType^ EncoderNodeFactory::createCompressedMediaType(
		IMediaType^ aPtrUncompressedMediaType,
		Guid aRefEncodingModeGUID,
		uint32 aEncodingModeValue,
		uint32 aIndexCompressedMediaType)
	{
		IMediaType^ lresult = nullptr;

		do
		{
			if (!mIEncoderNodeFactory)
				break;

			if (aPtrUncompressedMediaType == nullptr)
				break;

			MediaType^ lIMediaType = (MediaType^)aPtrUncompressedMediaType;
			
			auto lPtrUnkUncompressedMediaType = reinterpret_cast<IUnknown*>((void*)lIMediaType->getMediaType());

			if (lPtrUnkUncompressedMediaType == nullptr)
				break;

			CComPtrCustom<IUnknown> lCompressedMediaType;

			auto lhr = mIEncoderNodeFactory->createCompressedMediaType(
				lPtrUnkUncompressedMediaType,
				aRefEncodingModeGUID,
				aEncodingModeValue,
				aIndexCompressedMediaType,
				&lCompressedMediaType);

			if (FAILED(lhr))
				break;

			if (!lCompressedMediaType)
				break;

			lresult = ref new MediaType(IntPtr(lCompressedMediaType.get()));

		} while (false);

		return lresult;
	}

	ITopologyNode^ EncoderNodeFactory::createEncoderNode(
		IMediaType^ aPtrUncompressedMediaType,
		Guid aRefEncodingModeGUID,
		uint32 aEncodingModeValue,
		uint32 aIndexCompressedMediaType,
		ITopologyNode^ aPtrDownStreamNode)
	{
		ITopologyNode^ lresult = nullptr;

		do
		{
			if (!mIEncoderNodeFactory)
				break;

			if (aPtrUncompressedMediaType == nullptr)
				break;

			if (aPtrDownStreamNode == nullptr)
				break;

			MediaType^ lIMediaType = (MediaType^)aPtrUncompressedMediaType;

			if (lIMediaType == nullptr)
				break;

			auto lPtrUnkUncompressedMediaType = reinterpret_cast<IUnknown*>((void*)lIMediaType->getMediaType());

			if (lPtrUnkUncompressedMediaType == nullptr)
				break;

			TopologyNode^ lTopologyNode = (TopologyNode^)aPtrDownStreamNode;

			if (lTopologyNode == nullptr)
				break;

			auto lPtrUnkDownStreamNode = reinterpret_cast<IUnknown*>((void*)lTopologyNode->getTopologyNode());

			if (lPtrUnkDownStreamNode == nullptr)
				break;

			CComPtrCustom<IUnknown> lEncoderNode;

			auto lhr = mIEncoderNodeFactory->createEncoderNode(
				lPtrUnkUncompressedMediaType,
				aRefEncodingModeGUID,
				aEncodingModeValue,
				aIndexCompressedMediaType,
				lPtrUnkDownStreamNode,
				&lEncoderNode);

			if (FAILED(lhr))
				break;

			if (!lEncoderNode)
				break;

			lresult = ref new TopologyNode(IntPtr(lEncoderNode.get()));

		} while (false);

		return lresult;
	}
}