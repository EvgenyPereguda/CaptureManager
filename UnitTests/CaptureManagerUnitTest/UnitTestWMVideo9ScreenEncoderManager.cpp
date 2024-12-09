#include "stdafx.h"
#include <atomic>
#include <basetsd.h>
#include <Unknwnbase.h>
#include "CppUnitTest.h"
#include "../../CaptureManagerSource/Common/ComPtrCustom.h"
#include "../../CaptureManagerSource/Common/Common.h"
#include "../../CaptureManagerSource/EncoderManager/WMVideo9ScreenEncoderManager.h"
#include "../../CaptureManagerSource/MediaFoundationManager/MediaFoundationManager.h"
#include "../../CaptureManagerSource/Common/MFHeaders.h"
#include "../../CaptureManagerSource/LogPrintOut/LogPrintOut.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CaptureManager;
using namespace CaptureManager::Core;
using namespace CaptureManager::Transform::Encoder;

namespace CaptureManagerUnitTest
{
	static CComPtrCustom<IMFMediaType> lInputType;

	static UINT32 gVideoFPS = 30;

	static HRESULT createStubUncompressedMediaType(
		IMFMediaType** aPtrPtrStubUncompressedMediaType)
	{
		HRESULT lresult;

		do
		{
			int lWidth = GetSystemMetrics(SM_CXSCREEN);

			int lHeight = GetSystemMetrics(SM_CYSCREEN);

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

	TEST_CLASS(UnitTestWMVideo9ScreenEncoderManager)
	{
	public:

		TEST_CLASS_INITIALIZE(lMediaTypeUncompressedAudioInitialize)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			auto lresult = createStubUncompressedMediaType(&lInputType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

		}

		TEST_METHOD(enumEncoderMediaTypes)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");


			CComPtrCustom<IEncoderManager> lWMVideo9ScreenEncoderManager = new (std::nothrow)WMVideo9ScreenEncoderManager();

			Assert::IsFalse(!lWMVideo9ScreenEncoderManager, L"lWMVideo9ScreenEncoderManager is nullptr");

			EncodingSettings lEncodingSettings;

			lEncodingSettings.mEncodingMode = EncodingSettings::VBR;

			lEncodingSettings.mEncodingModeValue = 75;

			std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

			auto lresult = lWMVideo9ScreenEncoderManager->enumEncoderMediaTypes(
				lInputType,
				lEncodingSettings,
				__uuidof(CMSSCEncMediaObject2),
				lListOfMediaTypes);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"WMVideo9ScreenEncoderManager: enumEncoderMediaTypes() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

		}

		TEST_METHOD(getCompressedMediaType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");


			CComPtrCustom<IEncoderManager> lWMVideo9ScreenEncoderManager = new (std::nothrow)WMVideo9ScreenEncoderManager();

			Assert::IsFalse(!lWMVideo9ScreenEncoderManager, L"lWMVideo9ScreenEncoderManager is nullptr");

			EncodingSettings lEncodingSettings;

			CComPtrCustom<IMFMediaType> lMediaType;

			auto lresult = lWMVideo9ScreenEncoderManager->getCompressedMediaType(
				lInputType,
				lEncodingSettings,
				__uuidof(CMSSCEncMediaObject2),
				0,
				&lMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"WMVideo9ScreenEncoderManager: enumEncoderMediaTypes() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

		}

		TEST_METHOD(getEncoder)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");


			CComPtrCustom<IEncoderManager> lWMVideo9ScreenEncoderManager = new (std::nothrow)WMVideo9ScreenEncoderManager();

			Assert::IsFalse(!lWMVideo9ScreenEncoderManager, L"lWMVideo9ScreenEncoderManager is nullptr");

			EncodingSettings lEncodingSettings;

			lEncodingSettings.mEncodingMode = lEncodingSettings.VBR;

			lEncodingSettings.mEncodingModeValue = 50;

			CComPtrCustom<IMFTransform> lEncoderTransform;

			auto lresult = lWMVideo9ScreenEncoderManager->getEncoder(
				lInputType,
				lEncodingSettings,
				__uuidof(CMSSCEncMediaObject2),
				0,
				&lEncoderTransform);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"WMVideo9ScreenEncoderManager: getEncoder() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

		}


	};
}