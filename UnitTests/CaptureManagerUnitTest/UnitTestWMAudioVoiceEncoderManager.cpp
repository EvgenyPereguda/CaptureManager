#include "stdafx.h"
#include <atomic>
#include <basetsd.h>
#include <Unknwnbase.h>
#include "CppUnitTest.h"
#include "../../CaptureManagerSource/Common/ComPtrCustom.h"
#include "../../CaptureManagerSource/Common/Common.h"
#include "../../CaptureManagerSource/EncoderManager/WMAudioVoiceEncoderManager.h"
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

	static HRESULT createStubUncompressedMediaType(
		UINT32 aSamplePerSecond,
		UINT32 aNumChannels,
		UINT32 aBitsPerSample,
		IMFMediaType** aPtrPtrStubUncompressedMediaType)
	{
		HRESULT lresult;

		do
		{
			LOG_CHECK_PTR_MEMORY(aPtrPtrStubUncompressedMediaType);

			UINT32 lbytePerSample = aBitsPerSample / 8;

			CComPtrCustom<IMFMediaType> lMediaType;

			LOG_INVOKE_MF_FUNCTION(MFCreateMediaType, &lMediaType);

			LOG_INVOKE_MF_METHOD(SetGUID,
				lMediaType,
				MF_MT_MAJOR_TYPE,
				MFMediaType_Audio);

			LOG_INVOKE_MF_METHOD(SetGUID,
				lMediaType,
				MF_MT_SUBTYPE,
				MFAudioFormat_PCM);

			LOG_INVOKE_MF_METHOD(SetUINT32,
				lMediaType,
				MF_MT_ALL_SAMPLES_INDEPENDENT,
				1);

			LOG_INVOKE_MF_METHOD(SetUINT32,
				lMediaType,
				MF_MT_AUDIO_AVG_BYTES_PER_SECOND,
				aSamplePerSecond * aNumChannels * lbytePerSample);

			LOG_INVOKE_MF_METHOD(SetUINT32,
				lMediaType,
				MF_MT_AUDIO_BITS_PER_SAMPLE,
				aBitsPerSample);

			LOG_INVOKE_MF_METHOD(SetUINT32,
				lMediaType,
				MF_MT_AUDIO_BLOCK_ALIGNMENT,
				aNumChannels * lbytePerSample);

			LOG_INVOKE_MF_METHOD(SetUINT32,
				lMediaType,
				MF_MT_AUDIO_SAMPLES_PER_SECOND,
				aSamplePerSecond);

			LOG_INVOKE_MF_METHOD(SetUINT32,
				lMediaType,
				MF_MT_AUDIO_NUM_CHANNELS,
				aNumChannels);

			*aPtrPtrStubUncompressedMediaType = lMediaType.detach();

		} while (false);

		return lresult;
	}

	TEST_CLASS(UnitTestWMAudioVoiceEncoderManager)
	{
	public:

		TEST_CLASS_INITIALIZE(lMediaTypeUncompressedAudioInitialize)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			UINT32 lFrequency = 48000;

			UINT32 lBytePerSample = 16;

			UINT32 lChannels = 2;

			auto lresult = createStubUncompressedMediaType(
				lFrequency,
				lChannels,
				lBytePerSample,
				&lInputType);

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


			CComPtrCustom<IEncoderManager> lWMAudioVoiceEncoderManager = new (std::nothrow)WMAudioVoiceEncoderManager();

			Assert::IsFalse(!lWMAudioVoiceEncoderManager, L"lWMAudioVoiceEncoderManager is nullptr");

			EncodingSettings lEncodingSettings;

			std::vector<CComPtrCustom<IUnknown>> lListOfMediaTypes;

			auto lresult = lWMAudioVoiceEncoderManager->enumEncoderMediaTypes(
				lInputType,
				lEncodingSettings,
				__uuidof(CWMSPEncMediaObject2),
				lListOfMediaTypes);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"WMAudioVoiceEncoderManager: enumEncoderMediaTypes() cannot be executed!!! ") +
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


			CComPtrCustom<IEncoderManager> lWMAudioVoiceEncoderManager = new (std::nothrow)WMAudioVoiceEncoderManager();

			Assert::IsFalse(!lWMAudioVoiceEncoderManager, L"lWMAudioVoiceEncoderManager is nullptr");

			EncodingSettings lEncodingSettings;

			CComPtrCustom<IMFMediaType> lMediaType;

			auto lresult = lWMAudioVoiceEncoderManager->getCompressedMediaType(
				lInputType,
				lEncodingSettings,
				__uuidof(CWMSPEncMediaObject2),
				0,
				&lMediaType);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"WMAudioVoiceEncoderManager: enumEncoderMediaTypes() cannot be executed!!! ") +
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


			CComPtrCustom<IEncoderManager> lWMAudioVoiceEncoderManager = new (std::nothrow)WMAudioVoiceEncoderManager();

			Assert::IsFalse(!lWMAudioVoiceEncoderManager, L"lWMAudioVoiceEncoderManager is nullptr");

			EncodingSettings lEncodingSettings;

			CComPtrCustom<IMFTransform> lEncoderTransform;

			auto lresult = lWMAudioVoiceEncoderManager->getEncoder(
				lInputType,
				lEncodingSettings,
				__uuidof(CWMSPEncMediaObject2),
				0,
				&lEncoderTransform);

			Assert::IsFalse(lresult,
				std::wstring(std::wstring(L"WMAudioVoiceEncoderManager: getEncoder() cannot be executed!!! ") +
				std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

		}


	};
}