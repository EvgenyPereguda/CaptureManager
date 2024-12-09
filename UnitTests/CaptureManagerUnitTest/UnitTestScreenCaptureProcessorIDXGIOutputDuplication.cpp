#include "stdafx.h"
#include "CppUnitTest.h"
#include <windows.h>
#include <shlobj.h>
#include "../../CaptureManagerSource/MediaFoundationManager/MediaFoundationManager.h"
#include "../../CaptureManagerSource/CaptureDeviceManager/CaptureDeviceManager.h"
#include "../../CaptureManagerSource/Common/ComPtrCustom.h"
#include "../../CaptureManagerSource/Common/ComMassivPtr.h"
#include "../../CaptureManagerSource/Common/MFHeaders.h"
#include "../../CaptureManagerSource/Common/Singleton.h"
#include "../../CaptureManagerSource/Common/ICaptureProcessor.h"
#include "../../CaptureManagerSource/PugiXML/pugixml.hpp"
#include "../../CaptureManagerSource/LogPrintOut/LogPrintOut.h"
#include "../../CaptureManagerSource/CaptureManager/Libraries.h"
#include "../../CaptureManagerSource/ScreenCaptureProcessor/ScreenCaptureProcessorIDXGIOutputDuplication.h"
#include "../../CaptureManagerSource/DataParser/DataParser.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CaptureManager;
using namespace CaptureManager::Core;
using namespace CaptureManager::Sources::ScreenCapture;
using namespace pugi;

namespace CaptureManagerUnitTest
{
	TEST_CLASS(UnitTestScreenCaptureProcessorIDXGIOutputDuplication)
	{
	public:

		TEST_METHOD(getFrendlyName)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<ICaptureProcessor> lScreenCaptureProcessor = new ScreenCaptureProcessorIDXGIOutputDuplication;

			BSTR lFriendlyName = nullptr;

			auto lresult = lScreenCaptureProcessor->getFrendlyName(&lFriendlyName);

			if (lFriendlyName != nullptr)
				SysFreeString(lFriendlyName);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: getFrendlyName cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);


		}

		TEST_METHOD(getSymbolicLink)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<ICaptureProcessor> lScreenCaptureProcessor = new ScreenCaptureProcessorIDXGIOutputDuplication;

			BSTR lSymbolicLink = nullptr;

			auto lresult = lScreenCaptureProcessor->getSymbolicLink(&lSymbolicLink);

			if (lSymbolicLink != nullptr)
				SysFreeString(lSymbolicLink);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: getSymbolicLink cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

		}

		TEST_METHOD(getMediaTypes)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<ScreenCaptureProcessorIDXGIOutputDuplication> lScreenCaptureProcessor = new ScreenCaptureProcessorIDXGIOutputDuplication;

			CComMassivPtr<IUnknown> lMediaTypes;

			auto lresult = lScreenCaptureProcessor->getMediaTypes(
				0,
				lMediaTypes.getPtrMassivPtr(),
				lMediaTypes.getPtrSizeMassiv());

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: getMediaTypes cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}

		TEST_METHOD(readImage)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<ScreenCaptureProcessorIDXGIOutputDuplication> lScreenCaptureProcessor = new ScreenCaptureProcessorIDXGIOutputDuplication;


			CComMassivPtr<IUnknown> lMediaTypes;

			auto lresult = lScreenCaptureProcessor->getMediaTypes(
				0,
				lMediaTypes.getPtrMassivPtr(),
				lMediaTypes.getPtrSizeMassiv());

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: getMediaTypes cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			std::vector<ScreenCaptureProcessorIDXGIOutputDuplication::ScreenCaptureConfig> lVectorScreenCaptureConfigs;

			lScreenCaptureProcessor->fillVectorScreenCaptureConfigs(lVectorScreenCaptureConfigs);

			CHAR lMyDocPath[MAX_PATH];

			HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, lMyDocPath);

			std::string lFilePath = std::string(lMyDocPath) + "\\ScreenShot.bmp";

			auto lBitMapInfo = lVectorScreenCaptureConfigs[0].mBitMapInfo;

			std::unique_ptr<BYTE> pBuf(new BYTE[lBitMapInfo.bmiHeader.biSizeImage]);

			lScreenCaptureProcessor->setCurrentMediaType(0, lMediaTypes[0]);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: grabImage cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);


			lresult = lScreenCaptureProcessor->grabImage(pBuf.get());


			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: grabImage cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			FILE*				fp = NULL;

			if ((fp = fopen(lFilePath.c_str(), "wb")) == NULL)
			{


				Assert::IsFalse(true,
					std::wstring(L"Unable to Create Bitmap File!!! ").c_str()
					);

				return;
			}


			BITMAPFILEHEADER	bmpFileHeader;

			bmpFileHeader.bfReserved1 = 0;
			bmpFileHeader.bfReserved2 = 0;
			bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + lBitMapInfo.bmiHeader.biSizeImage;
			bmpFileHeader.bfType = 'MB';
			bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

			fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
			fwrite(&lBitMapInfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);
			fwrite(pBuf.get(), lBitMapInfo.bmiHeader.biSizeImage, 1, fp);

			fclose(fp);
		}

		TEST_METHOD(getNewSample)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<ScreenCaptureProcessorIDXGIOutputDuplication> lScreenCaptureProcessor = new ScreenCaptureProcessorIDXGIOutputDuplication;


			CComMassivPtr<IUnknown> lMediaTypes;

			auto lresult = lScreenCaptureProcessor->getMediaTypes(
				0,
				lMediaTypes.getPtrMassivPtr(),
				lMediaTypes.getPtrSizeMassiv());

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: getMediaTypes() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			lresult = lScreenCaptureProcessor->setCurrentMediaType(0, lMediaTypes[0]);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: setCurrentMediaType() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			lresult = lScreenCaptureProcessor->start();

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: getMediaTypes() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			CComPtrCustom<IUnknown> lSample;

			lresult = lScreenCaptureProcessor->getNewSample(
				0,
				&lSample);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: getNewSample() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			lScreenCaptureProcessor->stop();

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"ScreenCaptureProcessorIDXGIOutputDuplication: stop() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}

	};
}