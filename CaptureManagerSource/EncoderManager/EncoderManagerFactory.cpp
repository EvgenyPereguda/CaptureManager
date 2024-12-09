/*
MIT License

Copyright(c) 2020 Evgeny Pereguda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "EncoderManagerFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/IMaker.h"
#include "../Common/MFHeaders.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "IEncoderManager.h"


namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{
			EncoderManagerFactory::EncoderManagerFactory()
			{
			}

			EncoderManagerFactory::~EncoderManagerFactory()
			{
			}
					
			HRESULT EncoderManagerFactory::registerInstanceMaker(
				EncoderInfoData aEncoderInfoData,
				const CaptureManager::Core::IMaker* aPtrMaker)
			{
				HRESULT lresult;

				do
				{
					auto lendIter = mEncoderManagerMakers.end();

					auto lfindIter = mEncoderManagerMakers.find(aEncoderInfoData);

					LOG_CHECK_STATE_DESCR(lfindIter != lendIter, E_INVALIDARG);

					mEncoderManagerMakers[aEncoderInfoData] = aPtrMaker;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT EncoderManagerFactory::createEncoderManager(
				REFGUID aRefEncoderCLSID,
				IEncoderManager** aPtrPtrIEncoderManager)
			{
				using namespace CaptureManager::Core;

				HRESULT lresult;

				MFT_REGISTER_TYPE_INFO* lPtrOutputTypes = nullptr;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrIEncoderManager);

					UINT32 lCountInputTypes;

					UINT32 lCountOutputTypes;

					do
					{

						LOG_INVOKE_MF_FUNCTION(MFTGetInfo,
							aRefEncoderCLSID,
							nullptr,
							nullptr,
							&lCountInputTypes,
							&lPtrOutputTypes,
							&lCountOutputTypes,
							nullptr);

					} while (false);

					if (FAILED(lresult))
					{
						lPtrOutputTypes = (MFT_REGISTER_TYPE_INFO*)CoTaskMemAlloc(sizeof(MFT_REGISTER_TYPE_INFO));

						if (lPtrOutputTypes == nullptr)
						{
							break;
						}

						lPtrOutputTypes[0].guidSubtype = aRefEncoderCLSID;

						lCountOutputTypes = 1;

						lresult = S_OK;
					}

					LOG_CHECK_STATE_DESCR(lCountOutputTypes == 0, MF_E_INVALID_CODEC_MERIT);
			
					auto lendIter = mEncoderManagerMakers.end();

					auto lfindIter = mEncoderManagerMakers.end();
					
					for (UINT32 lIndex = 0; lIndex < lCountOutputTypes; lIndex++)
					{
						lresult = MF_E_INVALID_CODEC_MERIT;

						for (auto& item : mEncoderManagerMakers)
						{
							if (item.first.mGUID == lPtrOutputTypes[lIndex].guidSubtype || item.first.mMediaSubType == lPtrOutputTypes[lIndex].guidSubtype)
							{
								lfindIter = mEncoderManagerMakers.find(item.first);

								if (lfindIter != lendIter)
								{
									lresult = S_OK;

									break;
								}
							}
						}
						
						if (SUCCEEDED(lresult))
							break;
					}

					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

					CComPtrCustom<IUnknown> lUnknown((*lfindIter).second->make());

					CComPtrCustom<IEncoderManager> lIEncoderManager;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnknown, &lIEncoderManager);

					LOG_CHECK_PTR_MEMORY(lIEncoderManager);

					*aPtrPtrIEncoderManager = lIEncoderManager.detach();
					
				} while (false);
				
				if (lPtrOutputTypes != nullptr)
				{
					CoTaskMemFree(lPtrOutputTypes);
				}
				
				return lresult;
			}



			HRESULT EncoderManagerFactory::getMaxMinBitRate(
				REFGUID aRefEncoderCLSID,
				UINT32& aRefMaxBitRate,
				UINT32& aRefMinBitRate)
			{
				HRESULT lresult(E_FAIL);

				do
				{

					auto lendIter = mEncoderManagerMakers.end();

					auto lfindIter = mEncoderManagerMakers.end();

					for (auto& item : mEncoderManagerMakers)
					{
						if (item.first.mGUID == aRefEncoderCLSID || item.first.mMediaSubType == aRefEncoderCLSID)
						{
							lfindIter = mEncoderManagerMakers.find(item.first);

							if (lfindIter != lendIter)
							{
								lresult = S_OK;

								break;
							}
						}
					}

					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

					aRefMaxBitRate = (*lfindIter).first.mMaxBitRate;

					aRefMinBitRate = (*lfindIter).first.mMinBitRate;

				} while (false);

				return lresult;
			}
		
		}
	}
}