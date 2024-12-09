#include "stdafx.h"
#include "CppUnitTest.h"
#include <basetsd.h>
#include <Unknwnbase.h>
#include "../../CaptureManagerSource/Common/ComPtrCustom.h"
#include "../../CaptureManagerSource/Common/Common.h"
#include "../../CaptureManagerSource/TopologyResolver/VideoTopologyResolver.h"
#include "../../CaptureManagerSource/MediaFoundationManager/MediaFoundationManager.h"
#include "../../CaptureManagerSource/Common/MFHeaders.h"
#include "../../CaptureManagerSource/LogPrintOut/LogPrintOut.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CaptureManager;
using namespace CaptureManager::Core;
using namespace CaptureManager::MediaSession::CustomisedMediaSession;



namespace CaptureManagerUnitTest
{
	static UINT32 gVideoFPS = 30;

	static HRESULT createStubUncompressedMediaType(
		int lWidth,
		int lHeight,
		IMFMediaType** aPtrPtrStubUncompressedMediaType)
	{
		HRESULT lresult;

		do
		{
			UINT32 lSizeImage = 0;

			BITMAPINFO	lBmpInfo;

			// BMP 32 bpp

			ZeroMemory(&lBmpInfo, sizeof(BITMAPINFO));

			lBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

			lBmpInfo.bmiHeader.biBitCount = 32;

			lBmpInfo.bmiHeader.biCompression = BI_RGB;

			lBmpInfo.bmiHeader.biWidth = lWidth;

			lBmpInfo.bmiHeader.biHeight = lHeight;

			lBmpInfo.bmiHeader.biPlanes = 1;


			LOG_INVOKE_MF_FUNCTION(MFCalculateImageSize,
				MFVideoFormat_RGB32,
				lBmpInfo.bmiHeader.biWidth,
				lBmpInfo.bmiHeader.biHeight,
				&lSizeImage);

			lBmpInfo.bmiHeader.biSizeImage = lSizeImage;

			auto lBitmapHeader = lBmpInfo.bmiHeader;

			CComPtrCustom<IMFVideoMediaType> lVideoMediaType;

			DWORD lBitRate = lBitmapHeader.biSizeImage * 8 * gVideoFPS;


			LOG_INVOKE_MF_FUNCTION(MFCreateVideoMediaTypeFromBitMapInfoHeaderEx,
				&lBitmapHeader,                     // video info header to convert
				lBitmapHeader.biSize,               // size of the header structure
				1,                                  // pixel aspect ratio X
				1,                                  // pixel aspect ratio Y
				MFVideoInterlace_Progressive,       // interlace mode 
				0,                                  // video flags
				gVideoFPS,           // FPS numerator
				1,          // FPS denominator
				lBitRate,          // max bitrate
				&lVideoMediaType);                           // result - out

			LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
				lVideoMediaType,
				MF_MT_FRAME_RATE_RANGE_MAX,
				gVideoFPS,
				1);

			LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
				lVideoMediaType,
				MF_MT_FRAME_RATE_RANGE_MIN,
				gVideoFPS,
				1);

			*aPtrPtrStubUncompressedMediaType = lVideoMediaType.detach();

		} while (false);

		return lresult;
	}

	static HRESULT createStubUncompressedMediaType(
		int lWidth,
		int lHeight,
		REFGUID aMediaType,
		IMFMediaType** aPtrPtrStubUncompressedMediaType)
	{
		HRESULT lresult;

		do
		{
			UINT32 lSizeImage = 0;

			BITMAPINFO	lBmpInfo;

			// BMP 32 bpp

			ZeroMemory(&lBmpInfo, sizeof(BITMAPINFO));

			lBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

			lBmpInfo.bmiHeader.biBitCount = 32;

			lBmpInfo.bmiHeader.biCompression = BI_RGB;

			lBmpInfo.bmiHeader.biWidth = lWidth;

			lBmpInfo.bmiHeader.biHeight = lHeight;

			lBmpInfo.bmiHeader.biPlanes = 1;


			LOG_INVOKE_MF_FUNCTION(MFCalculateImageSize,
				MFVideoFormat_RGB32,
				lBmpInfo.bmiHeader.biWidth,
				lBmpInfo.bmiHeader.biHeight,
				&lSizeImage);

			lBmpInfo.bmiHeader.biSizeImage = lSizeImage;

			auto lBitmapHeader = lBmpInfo.bmiHeader;

			CComPtrCustom<IMFVideoMediaType> lVideoMediaType;

			DWORD lBitRate = lBitmapHeader.biSizeImage * 8 * gVideoFPS;


			LOG_INVOKE_MF_FUNCTION(MFCreateVideoMediaTypeFromBitMapInfoHeaderEx,
				&lBitmapHeader,                     // video info header to convert
				lBitmapHeader.biSize,               // size of the header structure
				1,                                  // pixel aspect ratio X
				1,                                  // pixel aspect ratio Y
				MFVideoInterlace_Progressive,       // interlace mode 
				0,                                  // video flags
				gVideoFPS,           // FPS numerator
				1,          // FPS denominator
				lBitRate,          // max bitrate
				&lVideoMediaType);                           // result - out

			LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
				lVideoMediaType,
				MF_MT_FRAME_RATE_RANGE_MAX,
				gVideoFPS,
				1);

			LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
				lVideoMediaType,
				MF_MT_FRAME_RATE_RANGE_MIN,
				gVideoFPS,
				1);

			LOG_INVOKE_MF_METHOD(SetGUID,
				lVideoMediaType,
				MF_MT_SUBTYPE,
				aMediaType);

			*aPtrPtrStubUncompressedMediaType = lVideoMediaType.detach();

		} while (false);

		return lresult;
	}

	TEST_CLASS(UnitTestVideoTopologyResolver)
	{
	public:

		TEST_METHOD(resolveResizeVideoConnectionWithoutResizing)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");				
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			auto lresult = createStubUncompressedMediaType(640, 480, & lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
							
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			lresult = createStubUncompressedMediaType(640, 480, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);
			
			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveResizeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_FALSE,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveCompatibilityOfVideoMediaTypes() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lHeadTopologyNode,
				L"lHeadTopologyNode is not nullptr "
				);

			Assert::IsFalse(lTailTopologyNode,
				L"lTailTopologyNode is not nullptr "
				);
		}

		TEST_METHOD(resolveResizeVideoConnectionWithResizingAndTheSameUncompressedTypes)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			auto lresult = createStubUncompressedMediaType(640, 480, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
						
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			lresult = createStubUncompressedMediaType(600, 400, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveResizeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveCompatibilityOfVideoMediaTypes() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

			Assert::IsFalse(lHeadTopologyNode.get()!= lTailTopologyNode.get(),
				L"lHeadTopologyNode is not the same as lTailTopologyNode "
				);

		}

		TEST_METHOD(resolveResizeVideoConnectionWithResizingAndTheDifferentUncompressedTypesAndUnsupportedOutputType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			auto lresult = createStubUncompressedMediaType(640, 480, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
			

			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			lresult = createStubUncompressedMediaType(600, 400, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveResizeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveCompatibilityOfVideoMediaTypes() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

			Assert::IsFalse(lHeadTopologyNode.get() == lTailTopologyNode.get(),
				L"lHeadTopologyNode is the same as lTailTopologyNode "
				);

		}

		TEST_METHOD(resolveResizeVideoConnectionWithResizingAndTheDifferentUncompressedTypesAndUnsupportedInputType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");









			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			lresult = createStubUncompressedMediaType(600, 400, MFVideoFormat_RGB32, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveResizeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveCompatibilityOfVideoMediaTypes() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

			Assert::IsFalse(lHeadTopologyNode.get() == lTailTopologyNode.get(),
				L"lHeadTopologyNode is the same as lTailTopologyNode "
				);

		}

		TEST_METHOD(resolveResizeVideoConnectionWithResizingAndTheDifferentUncompressedTypesAndUnsupportedInputTypeAndOutputType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;
			
			lresult = createStubUncompressedMediaType(600, 400, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lDownStreamMediaType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveResizeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveCompatibilityOfVideoMediaTypes() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

			Assert::IsFalse(lHeadTopologyNode.get() == lTailTopologyNode.get(),
				L"lHeadTopologyNode is the same as lTailTopologyNode "
				);
			
		}

		TEST_METHOD(resolveFrameRateVideoConnectionWithoutFrameRateChanging)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;
			
			auto lresult = createStubUncompressedMediaType(640, 480, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");

			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			lresult = createStubUncompressedMediaType(640, 480, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveFrameRateVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_FALSE,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameRateVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lHeadTopologyNode,
				L"lHeadTopologyNode is not nullptr "
				);

			Assert::IsFalse(lTailTopologyNode,
				L"lTailTopologyNode is not nullptr "
				);
		}

		TEST_METHOD(resolveFrameRateVideoConnectionWithFrameRateChangingAndTheSameUncompressedTypes)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			gVideoFPS = 30;
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;
			
			auto lresult = createStubUncompressedMediaType(640, 480, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");




			
			gVideoFPS = 25;

			CComPtrCustom<IMFMediaType> lDownStreamMediaType;
			
			lresult = createStubUncompressedMediaType(640, 480, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);			

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveFrameRateVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_OK,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameRateVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);
		}

		TEST_METHOD(resolveFrameRateVideoConnectionWithFrameRateChangingAndAndTheDifferentUncompressedTypesAndUnsupportedOutputType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			

			gVideoFPS = 30;

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			auto lresult = createStubUncompressedMediaType(640, 480, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");





			

			CComPtrCustom<IMFMediaType> lDownStreamMediaType;
			

			gVideoFPS = 25;


			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveFrameRateVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_OK,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameRateVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);
		}

		TEST_METHOD(resolveFrameRateVideoConnectionWithFrameRateChangingAndAndTheDifferentUncompressedTypesAndUnsupportedInputType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
						
			gVideoFPS = 30;

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");

			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 25;
			
			lresult = createStubUncompressedMediaType(640, 480, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveFrameRateVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_OK,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameRateVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);
		}

		TEST_METHOD(resolveFrameRateVideoConnectionWithFrameRateChangingAndAndTheDifferentUncompressedTypesAndUnsupportedInputTypeAndOutputType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			gVideoFPS = 30;

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");

			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;
			
			gVideoFPS = 25;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveFrameRateVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_OK,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameRateVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);
		}

		TEST_METHOD(resolveFrameRateAndResizeVideoConnectionWithoutFrameRateAndSizeChanging)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			gVideoFPS = 30;

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			auto lresult = createStubUncompressedMediaType(640, 480, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");

			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 30;
			
			lresult = createStubUncompressedMediaType(640, 480, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveFrameRateAndResizeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_FALSE,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameRateAndResizeVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lHeadTopologyNode,
				L"lHeadTopologyNode is not nullptr "
				);

			Assert::IsFalse(lTailTopologyNode,
				L"lTailTopologyNode is not nullptr "
				);
		}

		TEST_METHOD(resolveFrameRateAndResizeVideoConnectionWithFrameRateAndSizeChangingAndUncompressedTypesAndUnsupportedOutputType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;


			gVideoFPS = 30;
			
			auto lresult = createStubUncompressedMediaType(640, 480, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");

						

			CComPtrCustom<IMFMediaType> lDownStreamMediaType;


			gVideoFPS = 25;
			
			lresult = createStubUncompressedMediaType(600, 400, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveFrameRateAndResizeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_OK,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameRateAndResizeVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);
		}

		TEST_METHOD(resolveFrameRateAndResizeVideoConnectionWithFrameRateAndSizeChangingAndUncompressedTypesAndUnsupportedInputType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
			

			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 25;

			lresult = createStubUncompressedMediaType(600, 400, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveFrameRateAndResizeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_OK,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameRateAndResizeVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);
		}

		TEST_METHOD(resolveFrameRateAndResizeVideoConnectionWithFrameRateAndSizeChangingAndUncompressedTypesAndUnsupportedInputTypeAndOutputType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 25;

			lresult = createStubUncompressedMediaType(600, 400, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			

			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			lresult = lVideoTopologyResolver->resolveFrameRateAndResizeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_OK,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameRateAndResizeVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);
		}

		TEST_METHOD(resolveColorConvertionVideoConnectionWithoutColorConvertor)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
					




			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");






			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = lVideoTopologyResolver->resolveColorConvertionVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_FALSE,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveColorConvertionVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lHeadTopologyNode,
				L"lHeadTopologyNode is not nullptr "
				);

			Assert::IsFalse(lTailTopologyNode,
				L"lTailTopologyNode is not nullptr "
				);

		}

		TEST_METHOD(resolveColorConvertionVideoConnectionWithColorConvertor)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;
			
			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;
			
			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");






			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = 

				lresult = lVideoTopologyResolver->resolveColorConvertionVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_OK,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveColorConvertionVideoConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

		}

		TEST_METHOD(resolveDecompressionVideoConnectionWithoutColorConvertor)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_MJPG, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");


			CComPtrCustom<IMFMediaType> lDownStreamMediaType;
			
			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lDownStreamMediaType is nullptr");





			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;
			
			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;
			
			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);
			
				lresult = lVideoTopologyResolver->resolveDecompressionVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveDecompressionVideoConnection() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

		}

		TEST_METHOD(resolveDecompressionVideoConnectionWithColorConvertor)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_MJPG, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");


			


			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);
			
			lresult = lVideoTopologyResolver->resolveDecompressionVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveDecompressionVideoConnection() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

		}

		TEST_METHOD(resolveDecompressionVideoConnectionWithTheSameCompressionOutput)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_MJPG, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");




			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_MJPG, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);
			
			lresult = lVideoTopologyResolver->resolveDecompressionVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse((HRESULT)lresult != S_FALSE,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveDecompressionVideoConnection() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lHeadTopologyNode,
				L"lHeadTopologyNode is not nullptr "
				);

			Assert::IsFalse(lTailTopologyNode,
				L"lTailTopologyNode is not nullptr "
				);

		}

		TEST_METHOD(resolveDecompressionVideoConnectionWithTheDifferCompressionOutput)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_MJPG, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");

			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_H264, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lDownStreamMediaType is nullptr");





			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);
			
			lresult = lVideoTopologyResolver->resolveDecompressionVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(!lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveDecompressionVideoConnection() compressed format to differ compressed format can be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lHeadTopologyNode,
				L"lHeadTopologyNode is not nullptr "
				);

			Assert::IsFalse(lTailTopologyNode,
				L"lTailTopologyNode is not nullptr "
				);

		}
		
		TEST_METHOD(resolveSubTypeVideoConnectionWithTheSameMediaTypes)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");


			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_AYUV, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");


			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_AYUV, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);
			
			lresult = lVideoTopologyResolver->resolveSubTypeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveSubTypeVideoConnection() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lHeadTopologyNode,
				L"lHeadTopologyNode is not nullptr "
				);

			Assert::IsFalse(lTailTopologyNode,
				L"lTailTopologyNode is not nullptr "
				);

			Assert::IsFalse(lTailTopologyNode.get() != lHeadTopologyNode.get(),
				L"lTailTopologyNode is not the same as lHeadTopologyNode "
				);
		}

		TEST_METHOD(resolveSubTypeVideoConnectionWithTheDifferentUncompressedSameMediaTypes)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_AYUV, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");


			

			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");





			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = lVideoTopologyResolver->resolveSubTypeVideoConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveSubTypeVideoConnection() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

			Assert::IsFalse(lTailTopologyNode.get() != lHeadTopologyNode.get(),
				L"lTailTopologyNode is not the same as lHeadTopologyNode "
				);
		}
				
		TEST_METHOD(checkDemandForVideoResizer)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_MJPG, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");

			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(600, 400, MFVideoFormat_H264, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");



			lresult = MediaFoundation::MediaFoundationManager::SetUINT32(
				lDownStreamMediaType,
				MF_MT_AVG_BITRATE,
				100000);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: setUINT32() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);



			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = 

			lresult = lVideoTopologyResolver->checkDemandForVideoResizer(
				lUpStreamMediaType,
				lDownStreamMediaType);

			Assert::IsFalse((HRESULT)lresult != S_OK,
				std::wstring(std::wstring(L"VideoTopologyResolver: checkDemandForVideoResizer() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);			
		}

		TEST_METHOD(checkDemandForVideoResizerMediaTypesWithTheSameSize)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_MJPG, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");

			CComPtrCustom<IMFMediaType> lDownStreamMediaType;
			
			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_H264, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");
			
			lresult = MediaFoundation::MediaFoundationManager::SetUINT32(
				lDownStreamMediaType,
				MF_MT_AVG_BITRATE,
				100000);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: setUINT32() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
			
			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = 

			lresult = lVideoTopologyResolver->checkDemandForVideoResizer(
				lUpStreamMediaType,
				lDownStreamMediaType);

			Assert::IsFalse((HRESULT)lresult != S_FALSE,
				std::wstring(std::wstring(L"VideoTopologyResolver: checkDemandForVideoResizer() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}

		TEST_METHOD(checkDemandForFrameRateConvertorMediaTypesWithTheSameFrameRate)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_MJPG, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");


			CComPtrCustom<IMFMediaType> lDownStreamMediaType;
			
			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_H264, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");



			lresult = MediaFoundation::MediaFoundationManager::SetUINT32(
				lDownStreamMediaType,
				MF_MT_AVG_BITRATE,
				100000);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: setUINT32() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);



			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = 

			lresult = lVideoTopologyResolver->checkDemandForFrameRateConvertor(
				lUpStreamMediaType,
				lDownStreamMediaType);

			Assert::IsFalse((HRESULT)lresult != S_FALSE,
				std::wstring(std::wstring(L"VideoTopologyResolver: checkDemandForFrameRateConvertor() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}
		
		TEST_METHOD(checkDemandForFrameRateConvertorMediaTypesWithTheDifferentFrameRate)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;
			
			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_MJPG, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;
			
			gVideoFPS = 25;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_H264, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");



			lresult = MediaFoundation::MediaFoundationManager::SetUINT32(
				lDownStreamMediaType,
				MF_MT_AVG_BITRATE,
				100000);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: setUINT32() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);



			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);
			
			lresult = lVideoTopologyResolver->checkDemandForFrameRateConvertor(
				lUpStreamMediaType,
				lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: checkDemandForFrameRateConvertor() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}

		TEST_METHOD(resolveFrameConvertorConnectionMediaTypesWithTheSameUncompressedSubTypesAndDifferentFrameSize)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_YUY2, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");

			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(600, 400, MFVideoFormat_YUY2, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");



			lresult = MediaFoundation::MediaFoundationManager::SetUINT32(
				lDownStreamMediaType,
				MF_MT_AVG_BITRATE,
				100000);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: setUINT32() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);



			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = lVideoTopologyResolver->resolveFrameConvertorConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameConvertorConnection() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

			Assert::IsFalse(lTailTopologyNode.get() != lHeadTopologyNode.get(),
				L"lTailTopologyNode is the same as lHeadTopologyNode "
				);
		}

		TEST_METHOD(resolveFrameConvertorConnectionMediaTypesWithTheDifferentUncompressedSubTypesAndDifferentFrameSize)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_YUY2, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(600, 400, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");



			lresult = MediaFoundation::MediaFoundationManager::SetUINT32(
				lDownStreamMediaType,
				MF_MT_AVG_BITRATE,
				100000);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: setUINT32() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);



			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = lVideoTopologyResolver->resolveFrameConvertorConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameConvertorConnection() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

			Assert::IsFalse(lTailTopologyNode.get() == lHeadTopologyNode.get(),
				L"lTailTopologyNode is the same as lHeadTopologyNode "
				);
		}

		TEST_METHOD(resolveFrameConvertorConnectionMediaTypesWithTheSameUncompressedSubTypesAndDifferentFrameRate)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_YUY2, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");


			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 25;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_YUY2, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");



			lresult = MediaFoundation::MediaFoundationManager::SetUINT32(
				lDownStreamMediaType,
				MF_MT_AVG_BITRATE,
				100000);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: setUINT32() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);



			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = lVideoTopologyResolver->resolveFrameConvertorConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameConvertorConnection() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

			Assert::IsFalse(lTailTopologyNode.get() != lHeadTopologyNode.get(),
				L"lTailTopologyNode is the same as lHeadTopologyNode "
				);
		}

		TEST_METHOD(resolveFrameConvertorConnectionMediaTypesWithTheDifferentUncompressedSubTypesAndDifferentFrameRate)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<IMFMediaType> lUpStreamMediaType;
			
			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_YUY2, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");
			
			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 25;

			lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_NV12, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");



			lresult = MediaFoundation::MediaFoundationManager::SetUINT32(
				lDownStreamMediaType,
				MF_MT_AVG_BITRATE,
				100000);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: setUINT32() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);



			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = lVideoTopologyResolver->resolveFrameConvertorConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameConvertorConnection() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

			Assert::IsFalse(lTailTopologyNode.get() == lHeadTopologyNode.get(),
				L"lTailTopologyNode is the same as lHeadTopologyNode "
				);
		}
				
		TEST_METHOD(resolveFrameConvertorConnectionMediaTypesWithTheSameUncompressedSubTypesAndDifferentFrameRateAndDifferentFrameSize)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			CComPtrCustom<IMFMediaType> lUpStreamMediaType;

			gVideoFPS = 30;

			auto lresult = createStubUncompressedMediaType(640, 480, MFVideoFormat_YUY2, &lUpStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamMediaType, L"lInputType is nullptr");

			CComPtrCustom<IMFMediaType> lDownStreamMediaType;

			gVideoFPS = 25;

			lresult = createStubUncompressedMediaType(600, 400, MFVideoFormat_YUY2, &lDownStreamMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamMediaType, L"lInputType is nullptr");



			lresult = MediaFoundation::MediaFoundationManager::SetUINT32(
				lDownStreamMediaType,
				MF_MT_AVG_BITRATE,
				100000);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: setUINT32() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);



			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<VideoTopologyResolver> lVideoTopologyResolver(new (std::nothrow)VideoTopologyResolver());

			Assert::IsFalse(!lVideoTopologyResolver, L"lVideoTopologyResolver is nullptr!!!");

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = lVideoTopologyResolver->resolveFrameConvertorConnection(
				lTopology,
				lUpStreamMediaType,
				lDownStreamMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"VideoTopologyResolver: resolveFrameConvertorConnection() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lHeadTopologyNode,
				L"lHeadTopologyNode is nullptr "
				);

			Assert::IsFalse(!lTailTopologyNode,
				L"lTailTopologyNode is nullptr "
				);

			Assert::IsFalse(lTailTopologyNode.get() == lHeadTopologyNode.get(),
				L"lTailTopologyNode is the same as lHeadTopologyNode "
				);
		}

	};
}