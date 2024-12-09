#include "stdafx.h"
#include "CppUnitTest.h"
#include <basetsd.h>
#include <Unknwnbase.h>
#include <thread>
#include "../../CaptureManagerSource/Common/ComPtrCustom.h"
#include "../../CaptureManagerSource/Common/Common.h"
#include "../../CaptureManagerSource/SampleGrabberCall/ReadWriteBufferRegularAsync.h"
#include "../../CaptureManagerSource/SampleGrabberCall/ReadWriteBufferRegularSync.h"
#include "../../CaptureManagerSource/MediaFoundationManager/MediaFoundationManager.h"
#include "../../CaptureManagerSource/Common/MFHeaders.h"
#include "../../CaptureManagerSource/LogPrintOut/LogPrintOut.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CaptureManager;
using namespace CaptureManager::Core;
using namespace CaptureManager::Sinks::SampleGrabberCall;
using namespace CaptureManager::Sinks::SampleGrabberCall::RegularSampleGrabberCall;

namespace CaptureManagerUnitTest
{
	TEST_CLASS(UnitTestReadWriteBuffer)
	{
	public:
		

		TEST_METHOD(createRegularReadWriteBuffer)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			unsigned long lsizeImage = 1920 * 1080 * 4;

			CComPtrCustom<ReadWriteBuffer> lReadWriteBuffer(new ReadWriteBufferRegularSync());

			lReadWriteBuffer->init(lsizeImage);
			
			Assert::IsNotNull(lReadWriteBuffer.get(), L"lReadWriteBuffer is NULL");

		}

		TEST_METHOD(ASYNCreadData)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			IRead *lPtrRead;
			
			unsigned long lsizeImage = 1920 * 1080 * 4;

			CComPtrCustom<ReadWriteBuffer> lReadWriteBuffer(new ReadWriteBufferRegularAsync(lsizeImage));

			std::auto_ptr<unsigned char> lpixels;

			lpixels.reset(new unsigned char[lsizeImage]);
			
			Assert::IsNotNull(lReadWriteBuffer.get(), L"lReadWriteBuffer is NULL");

			lPtrRead = lReadWriteBuffer;

			Assert::IsNotNull(lReadWriteBuffer.get(), L"lReadWriteBuffer is NULL");

			DWORD lSampleSize;

			auto lstate = lPtrRead->readData(lpixels.get(), &lSampleSize);

			Assert::IsFalse(FAILED(lstate), L"Reading Rejected");

			lstate = lPtrRead->readData(lpixels.get(), &lSampleSize);

			Assert::IsTrue(lstate == S_FALSE, std::wstring(std::wstring(L"Reading Rejected: ") + std::wstring(DataParser::getHRESULTDescription(lstate))).c_str());

		}

		TEST_METHOD(SYNCreadData)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");
			
			IRead *lPtrRead;

			unsigned long lsizeImage = 1920 * 1080 * 4;

			CComPtrCustom<ReadWriteBuffer> lReadWriteBuffer(new ReadWriteBufferRegularSync());

			lReadWriteBuffer->init(lsizeImage);

			std::auto_ptr<unsigned char> lpixels;

			lpixels.reset(new unsigned char[lsizeImage]);

			Assert::IsNotNull(lReadWriteBuffer.get(), L"lReadWriteBuffer is NULL");

			lPtrRead = lReadWriteBuffer;

			Assert::IsNotNull(lReadWriteBuffer.get(), L"lPtrReadWriteBuffer is NULL");

			DWORD lSampleSize;

			auto lstate = lPtrRead->readData(lpixels.get(), &lSampleSize);

			Assert::IsTrue(lstate, std::wstring(std::wstring(L"Reading Rejected: ") + std::wstring(DataParser::getHRESULTDescription(lstate))).c_str());

		}

		TEST_METHOD(ASYNCwriteData_readData)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");


			IRead *lPtrRead;
			
			IWrite *lPtrIWrite;
			
			unsigned long lsizeImage = 1920 * 1080 * 4;

			std::auto_ptr<unsigned char> pixels;

			pixels.reset(new unsigned char[lsizeImage]);

			CComPtrCustom<ReadWriteBuffer> lReadWriteBuffer(new ReadWriteBufferRegularAsync(lsizeImage));
			
			Assert::IsNotNull(lReadWriteBuffer.get(), L"lReadWriteBuffer is NULL");

			lPtrRead = lReadWriteBuffer;

			lPtrIWrite = lReadWriteBuffer;

			Assert::IsNotNull(lReadWriteBuffer.get(), L"lPtrReadWriteBuffer is NULL");

			DWORD lSampleSize;

			auto lstate = lPtrRead->readData(pixels.get(), &lSampleSize);

			Assert::IsFalse(FAILED(lstate), L"Reading is rejected");

			lPtrIWrite->writeData(pixels.get(), lsizeImage);

			lstate = lPtrRead->readData(pixels.get(), &lSampleSize);

			Assert::IsFalse(lstate, L"Reading is not done");

			lstate = lPtrRead->readData(pixels.get(), &lSampleSize);

			Assert::IsTrue(lstate == S_FALSE, std::wstring(std::wstring(L"Reading is not rejected: ") + std::wstring(DataParser::getHRESULTDescription(lstate))).c_str());

		}

		TEST_METHOD(SYNCwriteData_readData)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");


			IRead *lPtrRead;
			
			IWrite *lPtrIWrite;
			
			unsigned long lsizeImage = 1920 * 1080 * 4;

			std::auto_ptr<unsigned char> pixels;

			pixels.reset(new unsigned char[lsizeImage]);

			CComPtrCustom<ReadWriteBuffer> lReadWriteBuffer(new ReadWriteBufferRegularSync());

			lReadWriteBuffer->init(lsizeImage);
			
			Assert::IsNotNull(lReadWriteBuffer.get(), L"lReadWriteBuffer is NULL");

			lPtrRead = lReadWriteBuffer;

			lPtrIWrite = lReadWriteBuffer;

			Assert::IsNotNull(lReadWriteBuffer.get(), L"lPtrReadWriteBuffer is NULL");

			DWORD lSampleSize;

			auto lstate = lPtrRead->readData(pixels.get(), &lSampleSize);

			Assert::IsTrue(lstate == CONTEXT_E_SYNCH_TIMEOUT, std::wstring(std::wstring(L"Reading Rejected: ") + std::wstring(DataParser::getHRESULTDescription(lstate))).c_str());

			lPtrIWrite->writeData(pixels.get(), lsizeImage);

			lstate = lPtrRead->readData(pixels.get(), &lSampleSize);

			Assert::IsTrue(SUCCEEDED(lstate), std::wstring(std::wstring(L"Reading Rejected: ") + std::wstring(DataParser::getHRESULTDescription(lstate))).c_str());

		}
		
	};
}