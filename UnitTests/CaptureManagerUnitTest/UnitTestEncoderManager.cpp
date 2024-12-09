#include "stdafx.h"
#include "CppUnitTest.h"
#include <Unknwn.h>
#include "../../CaptureManagerSource/Common/ComPtrCustom.h"
#include "../../CaptureManagerSource/Common/Common.h"
#include "../../CaptureManagerSource/EncoderManager/EncoderManager.h"
#include "../../CaptureManagerSource/MediaFoundationManager/MediaFoundationManager.h"
#include "../../CaptureManagerSource/Common/MFHeaders.h"
#include "../../CaptureManagerSource/LogPrintOut/LogPrintOut.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CaptureManager;
using namespace CaptureManager::Core;
using namespace CaptureManager::Transform::Encoder;

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

	TEST_CLASS(UnitTestEncoderManager)
	{
	public:
		
		TEST_METHOD(getDictionaryOfVideoEncoders)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			std::map<std::wstring, GUID> lVideoEncoderDic;

			auto lresult = EncoderManager::getDictionaryOfVideoEncoders(
				lVideoEncoderDic);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"EncoderManager: getDictionaryOfVideoEncoders() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
				
		}

		TEST_METHOD(getEncoderOutputMediaType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");


			std::map<std::wstring, GUID> lVideoEncoderDic;

			auto lresult = EncoderManager::getDictionaryOfVideoEncoders(
				lVideoEncoderDic);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"EncoderManager: getDictionaryOfVideoEncoders() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lVideoEncoderDic.empty(),
				std::wstring(std::wstring(L"lVideoEncoderDic is empty!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			CComPtrCustom<IMFMediaType> lInputType;
			
			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, &lInputType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);


			std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

			EncodingSettings lEncodingSettings;

			lEncodingSettings.mEncodingMode = EncodingSettings::VBR;

			lEncodingSettings.mEncodingModeValue = 75;

			lresult = EncoderManager::enumEncoderMediaTypes(
				lInputType,
				lVideoEncoderDic[L"WMVideo8 Encoder MFT"],
				lListOfMediaTypes,
				lEncodingSettings);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"EncoderManager: enumEncoderMediaTypes() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lListOfMediaTypes.empty(),
				std::wstring(std::wstring(L"lListOfMediaTypes is empty!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			CComPtrCustom<IMFMediaType> lOutputMediaType;
			
			lresult = EncoderManager::getCompressedMediaType(
				lInputType,
				EncodingSettings(),
				MFT_CATEGORY_VIDEO_ENCODER,
				lVideoEncoderDic[L"WMVideo8 Encoder MFT"],
				0,
				&lOutputMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"EncoderManager: enumEncoderMediaTypes() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}

		TEST_METHOD(createEncoderTopologyNode)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			

			std::map<std::wstring, GUID> lVideoEncoderDic;

			auto lresult = EncoderManager::getDictionaryOfVideoEncoders(
				lVideoEncoderDic);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"EncoderManager: getDictionaryOfVideoEncoders() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lVideoEncoderDic.empty(),
				std::wstring(std::wstring(L"lVideoEncoderDic is empty!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			CComPtrCustom<IMFMediaType> lInputType;

			gVideoFPS = 30;

			lresult = createStubUncompressedMediaType(640, 480, &lInputType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);


			std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

			EncodingSettings lEncodingSettings;

			lEncodingSettings.mEncodingMode = EncodingSettings::VBR;

			lEncodingSettings.mEncodingModeValue = 75;

			lresult = EncoderManager::enumEncoderMediaTypes(
				lInputType,
				lVideoEncoderDic[L"WMVideo8 Encoder MFT"],
				lListOfMediaTypes,
				lEncodingSettings);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"EncoderManager: enumEncoderMediaTypes() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(lListOfMediaTypes.empty(),
				std::wstring(std::wstring(L"lListOfMediaTypes is empty!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			CComPtrCustom<IMFMediaType> lOutputMediaType;

			lresult = EncoderManager::getCompressedMediaType(
				lInputType,
				EncodingSettings(),
				MFT_CATEGORY_VIDEO_ENCODER,
				lVideoEncoderDic[L"WMVideo8 Encoder MFT"],
				0,
				&lOutputMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"EncoderManager: enumEncoderMediaTypes() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			CComPtrCustom<IMFTopologyNode> lEncoderTopologyNode;

			lresult = EncoderManager::createEncoderTopologyNode(
				lInputType,
				EncodingSettings(),
				lVideoEncoderDic[L"WMVideo8 Encoder MFT"],
				0,
				&lEncoderTopologyNode);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"EncoderManager: createEncoderTopologyNode() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}
	};
}