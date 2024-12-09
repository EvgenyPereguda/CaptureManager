#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../CaptureManagerSource/DirectShowCaptureProcessor/DirectShowCaptureProcessorFactory.h"
#include "../../CaptureManagerSource/Common/ComPtrCustom.h"
#include "../../CaptureManagerSource/Common/ComMassivPtr.h"
#include "../../CaptureManagerSource/Common/MFHeaders.h"
#include "../../CaptureManagerSource/Common/Singleton.h"
#include "../../CaptureManagerSource/PugiXML/pugixml.hpp"
#include "../../CaptureManagerSource/LogPrintOut/LogPrintOut.h"
#include "../../CaptureManagerSource/CaptureManager/Libraries.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CaptureManager;
using namespace CaptureManager::Core;
using namespace CaptureManager::Sources;
using namespace pugi;


//#pragma comment(lib, "strmiids")
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

namespace CaptureManagerUnitTest
{
	TEST_CLASS(UnitTestDirectShowCaptureProcessor)
	{
	public:
		
		TEST_METHOD(TestcreateDirectShowVideoInputCaptureProcessors)
		{
			std::vector<CComPtrCustom<ICaptureProcessor>> lVectorDirectShowVideoInputCaptureProcessors;

			auto lresult = Singleton<DirectShowCaptureProcessorFactory>::getInstance().createDirectShowVideoInputCaptureProcessors(
				lVectorDirectShowVideoInputCaptureProcessors);

			Assert::IsFalse(lresult, L"DirectShowCaptureProcessorFactory: createDirectShowVideoInputCaptureProcessors() cannot be executed!!!");

			Assert::IsFalse(lVectorDirectShowVideoInputCaptureProcessors.empty(), L"DirectShowCaptureProcessorFactory: createDirectShowVideoInputCaptureProcessors() cannot be executed!!!");
		}
		
		TEST_METHOD(getFrendlyName)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			std::vector<CComPtrCustom<ICaptureProcessor>> lVectorDirectShowVideoInputCaptureProcessors;

			auto lresult = Singleton<DirectShowCaptureProcessorFactory>::getInstance().createDirectShowVideoInputCaptureProcessors(
				lVectorDirectShowVideoInputCaptureProcessors);

			Assert::IsFalse(lresult, L"DirectShowCaptureProcessorFactory: createDirectShowVideoInputCaptureProcessors() cannot be executed!!!");

			Assert::IsFalse(lVectorDirectShowVideoInputCaptureProcessors.empty(), L"DirectShowCaptureProcessorFactory: createDirectShowVideoInputCaptureProcessors() cannot be executed!!!");


			CComPtrCustom<ICaptureProcessor> lScreenCaptureProcessor = lVectorDirectShowVideoInputCaptureProcessors[0];

			BSTR lFriendlyName = nullptr;

			lresult = lScreenCaptureProcessor->getFrendlyName(&lFriendlyName);

			if (lFriendlyName != nullptr)
				SysFreeString(lFriendlyName);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"DirectShowCaptureProcessorFactory: getFrendlyName cannot be executed!!! ") +
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

			std::vector<CComPtrCustom<ICaptureProcessor>> lVectorDirectShowVideoInputCaptureProcessors;

			auto lresult = Singleton<DirectShowCaptureProcessorFactory>::getInstance().createDirectShowVideoInputCaptureProcessors(
				lVectorDirectShowVideoInputCaptureProcessors);

			Assert::IsFalse(lresult, L"DirectShowCaptureProcessorFactory: createDirectShowVideoInputCaptureProcessors() cannot be executed!!!");

			Assert::IsFalse(lVectorDirectShowVideoInputCaptureProcessors.empty(), L"DirectShowCaptureProcessorFactory: createDirectShowVideoInputCaptureProcessors() cannot be executed!!!");


			CComPtrCustom<ICaptureProcessor> lScreenCaptureProcessor = lVectorDirectShowVideoInputCaptureProcessors[0];

			BSTR lSymbolicLink = nullptr;

			lresult = lScreenCaptureProcessor->getSymbolicLink(&lSymbolicLink);

			if (lSymbolicLink != nullptr)
				SysFreeString(lSymbolicLink);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"DirectShowCaptureProcessorFactory: getSymbolicLink cannot be executed!!! ") +
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

			std::vector<CComPtrCustom<ICaptureProcessor>> lVectorDirectShowVideoInputCaptureProcessors;

			auto lresult = Singleton<DirectShowCaptureProcessorFactory>::getInstance().createDirectShowVideoInputCaptureProcessors(
				lVectorDirectShowVideoInputCaptureProcessors);

			Assert::IsFalse(lresult, L"DirectShowCaptureProcessorFactory: createDirectShowVideoInputCaptureProcessors() cannot be executed!!!");

			Assert::IsFalse(lVectorDirectShowVideoInputCaptureProcessors.empty(), L"DirectShowCaptureProcessorFactory: createDirectShowVideoInputCaptureProcessors() cannot be executed!!!");


			CComPtrCustom<ICaptureProcessor> lScreenCaptureProcessor = lVectorDirectShowVideoInputCaptureProcessors[0];

			CComMassivPtr<IUnknown> lMediaTypes;

			lresult = lScreenCaptureProcessor->getMediaTypes(
				0,
				lMediaTypes.getPtrMassivPtr(),
				lMediaTypes.getPtrSizeMassiv());

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"DirectShowCaptureProcessorFactory: getMediaTypes cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}


		TEST_METHOD(getNewSample)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			std::vector<CComPtrCustom<ICaptureProcessor>> lVectorDirectShowVideoInputCaptureProcessors;

			auto lresult = Singleton<DirectShowCaptureProcessorFactory>::getInstance().createDirectShowVideoInputCaptureProcessors(
				lVectorDirectShowVideoInputCaptureProcessors);

			Assert::IsFalse(lresult, L"DirectShowCaptureProcessorFactory: createDirectShowVideoInputCaptureProcessors() cannot be executed!!!");

			Assert::IsFalse(lVectorDirectShowVideoInputCaptureProcessors.empty(), L"DirectShowCaptureProcessorFactory: createDirectShowVideoInputCaptureProcessors() cannot be executed!!!");


			CComPtrCustom<ICaptureProcessor> lScreenCaptureProcessor = lVectorDirectShowVideoInputCaptureProcessors[0];


			CComMassivPtr<IUnknown> lMediaTypes;

			lresult = lScreenCaptureProcessor->getMediaTypes(
				0,
				lMediaTypes.getPtrMassivPtr(),
				lMediaTypes.getPtrSizeMassiv());

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"DirectShowCaptureProcessorFactory: getMediaTypes() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			lresult = lScreenCaptureProcessor->setCurrentMediaType(0, lMediaTypes[0]);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"DirectShowCaptureProcessorFactory: setCurrentMediaType() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			lresult = lScreenCaptureProcessor->start();

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"DirectShowCaptureProcessorFactory: getMediaTypes() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			CComPtrCustom<IUnknown> lSample;

			lresult = lScreenCaptureProcessor->getNewSample(
				0,
				&lSample);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"ScreenCaptureProcessorDirectX9: getNewSample() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			lScreenCaptureProcessor->stop();

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"ScreenCaptureProcessorDirectX9: stop() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}
	};
}