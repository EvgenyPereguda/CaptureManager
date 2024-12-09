#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../CaptureManagerSource/Common/Common.h"
#include "../../CaptureManagerSource/TopologyResolver/AudioTopologyResolver.h"
#include "../../CaptureManagerSource/MediaFoundationManager/MediaFoundationManager.h"
#include "../../CaptureManagerSource/Common/MFHeaders.h"
#include "../../CaptureManagerSource/LogPrintOut/LogPrintOut.h"
#include "../../CaptureManagerSource/Common/ComPtrCustom.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CaptureManager;
using namespace CaptureManager::Core;
using namespace CaptureManager::MediaSession::CustomisedMediaSession;

namespace CaptureManagerUnitTest
{

	static HRESULT createStubUncompressedMediaType(
		UINT32 aSamplePerSecond,
		UINT32 aNumChannels,
		UINT32 aBitsPerSample,
		IMFMediaType** aPtrPtrStubUncompressedMediaType,
		GUID aMediaType = MFAudioFormat_PCM)
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
				aMediaType);

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

	
	TEST_CLASS(UnitTestAudioTopologyResolver)
	{
	public:
		
		TEST_METHOD(checkDemandForAudioResamplerWithTheSameUncompressedMediaTypes)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<AudioTopologyResolver> lAudioTopologyResolver =
				new (std::nothrow) AudioTopologyResolver();

			HRESULT lresult = S_OK;

			if (!lAudioTopologyResolver)
			{
				lresult = E_POINTER;
			}

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"lAudioTopologyResolver: instance of AudioTopologyResolver cannot be created!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
			
			CComPtrCustom<IMFMediaType> lUpStreamUncompressedAudioMediaType;

			lresult = createStubUncompressedMediaType(44100, 1, 16, &lUpStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamUncompressedAudioMediaType, L"lInputType is nullptr");



			CComPtrCustom<IMFMediaType> lDownStreamUncompressedAudioMediaType;
			
			lresult = createStubUncompressedMediaType(44100, 1, 16, &lDownStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamUncompressedAudioMediaType, L"lIOutputType is nullptr");




			lresult = lAudioTopologyResolver->checkDemandForAudioResampler(
				lUpStreamUncompressedAudioMediaType,
				lDownStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"AudioTopologyResolver: checkDemandForAudioResampler() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse((HRESULT)lresult == S_OK,
				std::wstring(std::wstring(L"AudioTopologyResolver: Audio Mediatypes are not the same!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}

		TEST_METHOD(checkDemandForAudioResamplerWithTheSameUncompressedMediaTypesAndDirrerentFrequency)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<AudioTopologyResolver> lAudioTopologyResolver =
				new (std::nothrow) AudioTopologyResolver();

			HRESULT lresult = S_OK;

			if (!lAudioTopologyResolver)
			{
				lresult = E_POINTER;
			}

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"lAudioTopologyResolver: instance of AudioTopologyResolver cannot be created!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			
			CComPtrCustom<IMFMediaType> lUpStreamUncompressedAudioMediaType;
			
			lresult = createStubUncompressedMediaType(44100, 1, 16, &lUpStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamUncompressedAudioMediaType, L"lInputType is nullptr");



			CComPtrCustom<IMFMediaType> lDownStreamUncompressedAudioMediaType;
			
			lresult = createStubUncompressedMediaType(48000, 1, 16, &lDownStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamUncompressedAudioMediaType, L"lIOutputType is nullptr");




			lresult = lAudioTopologyResolver->checkDemandForAudioResampler(
				lUpStreamUncompressedAudioMediaType,
				lDownStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"AudioTopologyResolver: checkDemandForAudioResampler() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse((HRESULT)lresult == S_FALSE,
				std::wstring(std::wstring(L"AudioTopologyResolver: Audio Mediatypes are not the same!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}

		TEST_METHOD(checkDemandForAudioResamplerWithTheSameUncompressedMediaTypesAndDirrerentNumChannels)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<AudioTopologyResolver> lAudioTopologyResolver =
				new (std::nothrow) AudioTopologyResolver();

			HRESULT lresult = S_OK;

			if (!lAudioTopologyResolver)
			{
				lresult = E_POINTER;
			}

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"lAudioTopologyResolver: instance of AudioTopologyResolver cannot be created!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
			

			CComPtrCustom<IMFMediaType> lUpStreamUncompressedAudioMediaType;
			
			lresult = createStubUncompressedMediaType(48000, 2, 16, &lUpStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamUncompressedAudioMediaType, L"lInputType is nullptr");



			CComPtrCustom<IMFMediaType> lDownStreamUncompressedAudioMediaType;
			
			lresult = createStubUncompressedMediaType(48000, 1, 16, &lDownStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamUncompressedAudioMediaType, L"lIOutputType is nullptr");




			lresult = lAudioTopologyResolver->checkDemandForAudioResampler(
				lUpStreamUncompressedAudioMediaType,
				lDownStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"AudioTopologyResolver: checkDemandForAudioResampler() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse((HRESULT)lresult == S_FALSE,
				std::wstring(std::wstring(L"AudioTopologyResolver: Audio Mediatypes are not the same!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}

		TEST_METHOD(checkDemandForAudioResamplerWithTheSameUncompressedMediaTypesAndDirrerentNumChannelsAndDirrerentFrequency)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<AudioTopologyResolver> lAudioTopologyResolver =
				new (std::nothrow) AudioTopologyResolver();

			HRESULT lresult = S_OK;

			if (!lAudioTopologyResolver)
			{
				lresult = E_POINTER;
			}

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"lAudioTopologyResolver: instance of AudioTopologyResolver cannot be created!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
			

			CComPtrCustom<IMFMediaType> lUpStreamUncompressedAudioMediaType;
			
			lresult = createStubUncompressedMediaType(44100, 2, 16, &lUpStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamUncompressedAudioMediaType, L"lInputType is nullptr");



			CComPtrCustom<IMFMediaType> lDownStreamUncompressedAudioMediaType;
			
			lresult = createStubUncompressedMediaType(48000, 1, 16, &lDownStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamUncompressedAudioMediaType, L"lIOutputType is nullptr");




			lresult = lAudioTopologyResolver->checkDemandForAudioResampler(
				lUpStreamUncompressedAudioMediaType,
				lDownStreamUncompressedAudioMediaType);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"AudioTopologyResolver: checkDemandForAudioResampler() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse((HRESULT)lresult == S_FALSE,
				std::wstring(std::wstring(L"AudioTopologyResolver: Audio Mediatypes are not the same!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
		}
		
		TEST_METHOD(resolveDecompressionAudioConnectionWithTheCompressedMediaTypeAndTheUncompressedMediaType)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");

			CComPtrCustom<AudioTopologyResolver> lAudioTopologyResolver =
				new (std::nothrow) AudioTopologyResolver();

			HRESULT lresult = S_OK;

			if (!lAudioTopologyResolver)
			{
				lresult = E_POINTER;
			}

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"lAudioTopologyResolver: instance of AudioTopologyResolver cannot be created!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);
			
			CComPtrCustom<IMFMediaType> lUpStreamUncompressedAudioMediaType;
			
			lresult = createStubUncompressedMediaType(44100, 2, 16, &lUpStreamUncompressedAudioMediaType, MFAudioFormat_MP3);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lUpStreamUncompressedAudioMediaType, L"lInputType is nullptr");



			CComPtrCustom<IMFMediaType> lDownStreamCompressedAudioMediaType;
			
			lresult = createStubUncompressedMediaType(44100, 2, 16, &lDownStreamCompressedAudioMediaType);


			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: getMediaType() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lDownStreamCompressedAudioMediaType, L"lIOutputType is nullptr");


			CComPtrCustom<IMFTopologyNode> lHeadTopologyNode;

			CComPtrCustom<IMFTopologyNode> lTailTopologyNode;

			CComPtrCustom<IMFTopology> lTopology;

			lresult = MediaFoundation::MediaFoundationManager::MFCreateTopology(&lTopology);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"MediaFoundationManager: createTopology() compressed format to uncompressed format cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
				);

			Assert::IsFalse(!lTopology,
				L"lTopology is nullptr "
				);

			lresult = lAudioTopologyResolver->resolveDecompressionAudioConnection(
				lTopology,
				lUpStreamUncompressedAudioMediaType,
				lDownStreamCompressedAudioMediaType,
				&lHeadTopologyNode,
				&lTailTopologyNode);

			Assert::IsFalse(FAILED(lresult),
				std::wstring(std::wstring(L"AudioTopologyResolver: resolveDecompressionAudioConnection() cannot be executed!!! ") + std::wstring(DataParser::getHRESULTDescription(lresult))).c_str()
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
		
	};
}