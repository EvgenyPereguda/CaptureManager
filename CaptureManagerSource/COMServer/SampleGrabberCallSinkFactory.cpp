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

#include <string>
#include <algorithm>
#include <guiddef.h>


#include "SampleGrabberCallSinkFactory.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/MFHeaders.h"
#include "../Common/InstanceMaker.h"
#include "SinkCollection.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Singleton.h"
#include "../CaptureManagerBroker/ISampleGrabberCallInner.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"


namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			CreateOutputNode = 1
		};

		static const Core::InstanceMaker<SampleGrabberCallSinkFactory, Singleton<SinkCollection>> staticInstanceMaker(
			SampleGrabberCallSinkFactory::getGUIDToNamePair);

		SampleGrabberCallSinkFactory::SampleGrabberCallSinkFactory():
			mReadMode(GUID_NULL)
		{
			std::vector<GUIDToNamePair> lReadModes;

			getReadModes(lReadModes);

			mReadMode = lReadModes[0].mGUID;
		}

		GUIDToNamePair SampleGrabberCallSinkFactory::getGUIDToNamePair()
		{
			GUIDToNamePair lGUIDToNamePair;

			lGUIDToNamePair.mGUID = __uuidof(ISampleGrabberCallSinkFactory);

			lGUIDToNamePair.mName = L"SampleGrabberCallSinkFactory";

			lGUIDToNamePair.mTitle = L"Sample grabber call sink factory";

			getReadModes(lGUIDToNamePair.mContainers);
			
			return lGUIDToNamePair;
		}

		void SampleGrabberCallSinkFactory::getReadModes(
			std::vector<GUIDToNamePair>& aRefReadModes)
		{

			GUIDToNamePair lGUIDFormat;

			lGUIDFormat.mSinkTypes.push_back(SinkType::SampleGrabberCall);

			lGUIDFormat.mGUID =
			{ 0x3c9f1c2e, 0x23, 0x4861, { 0x8b, 0xd8, 0xc6, 0xde, 0xd2, 0x20, 0xe9, 0x4d } };

			lGUIDFormat.mName = L"ASYNC";

			lGUIDFormat.mMaxPortCount = 1;

			lGUIDFormat.mTitle = L"Grabbing without blocking of call thread";

			aRefReadModes.push_back(lGUIDFormat);


			lGUIDFormat.mGUID =
			{ 0xc1864678, 0x66c7, 0x48ea, { 0x8e, 0xd4, 0x48, 0xef, 0x37, 0x5, 0x49, 0x90 } };

			lGUIDFormat.mName = L"SYNC";

			lGUIDFormat.mMaxPortCount = 1;

			lGUIDFormat.mTitle = L"Grabbing with blocking of call thread";

			aRefReadModes.push_back(lGUIDFormat);


			lGUIDFormat.mGUID =
			{ 0xb1b7f389, 0x8d2f, 0x471a, { 0x99, 0x3a, 0x20, 0xab, 0x1c, 0xde, 0x89, 0xa7 } };

			lGUIDFormat.mName = L"PULL";

			lGUIDFormat.mMaxPortCount = 1;

			lGUIDFormat.mTitle = L"Grabbing with direct calling of sample";

			aRefReadModes.push_back(lGUIDFormat);
		}

		// ISampleGrabberCallSinkFactory interface implementation

		STDMETHODIMP SampleGrabberCallSinkFactory::createOutputNode(
			REFGUID aRefMajorType,
			REFGUID aRefSubType,
			DWORD aSampleByteSize,
			REFIID aREFIID,
			IUnknown **aPtrPtrISampleGrabberCall)
		{
			class Proxy :
				public BaseDispatch<ISampleGrabberCall>
			{				
			protected:

				virtual bool findIncapsulatedInterface(
					REFIID aRefIID,
					void** aPtrPtrVoidObject)
				{
					if (aRefIID == __uuidof(IMFTopologyNode) && mISampleGrabberCallInner)
					{
						auto lr = mISampleGrabberCallInner->QueryInterface(aRefIID, aPtrPtrVoidObject);

						return SUCCEEDED(lr);
					}

					return false;
				}
				
				virtual bool findInterface(
					REFIID aRefIID,
					void** aPtrPtrVoidObject)
				{
					if (aRefIID == __uuidof(IDispatch))
					{
						return castInterfaces(
							aRefIID,
							aPtrPtrVoidObject,
							static_cast<IDispatch*>(this));
					}
					else
					{
						return BaseUnknown::findInterface(
							aRefIID,
							aPtrPtrVoidObject);
					}
				}
				
			public:
				Proxy(ISampleGrabberCallInner* aISampleGrabberCallInner)
				{
					mISampleGrabberCallInner = aISampleGrabberCallInner;
				}

				virtual ~Proxy(){}

				virtual HRESULT STDMETHODCALLTYPE readData(
					LPVOID aPtrData,
					DWORD *aByteSize)
				{
					return mISampleGrabberCallInner->readData(
						(unsigned char*)aPtrData,
						*aByteSize);	
				}


				// IDispatch interface implementation
				
				virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
					__RPC__in REFIID riid,
					/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
					/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
					LCID lcid,
					/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)
				{
					HRESULT lresult;

					do
					{
						LOG_CHECK_STATE(cNames != 1);
						
						if (_wcsicmp(*rgszNames, OLESTR("readData")))
						{
							*rgDispId = 1;
						}
						else
						{
							lresult = DISP_E_UNKNOWNNAME;

							break;
						}

						lresult = S_OK;

					} while (false);

					return lresult;
				}

				virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
					/* [annotation][in] */
					_In_  DISPID dispIdMember,
					/* [annotation][in] */
					_In_  REFIID riid,
					/* [annotation][in] */
					_In_  LCID lcid,
					/* [annotation][in] */
					_In_  WORD wFlags,
					/* [annotation][out][in] */
					_In_  DISPPARAMS *pDispParams,
					/* [annotation][out] */
					VARIANT *pVarResult,
					/* [annotation][out] */
					EXCEPINFO *pExcepInfo,
					/* [annotation][out] */
					UINT *puArgErr)
				{
					HRESULT lresult;

					do
					{

						LOG_CHECK_STATE_DESCR(dispIdMember != 1, DISP_E_UNKNOWNINTERFACE);
					
						LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

						LOG_CHECK_PTR_MEMORY(pVarResult);

						LOG_CHECK_STATE(pDispParams->cArgs != 2);
						
						LOG_CHECK_PTR_MEMORY(pDispParams->rgvarg);
						
						VARIANT lFirstArg = pDispParams->rgvarg[0];

						VARIANT lSecondArg = pDispParams->rgvarg[1];

						LOG_CHECK_STATE(lFirstArg.vt != VT_VOID || lSecondArg.vt != VT_UINT_PTR);
						
						LOG_INVOKE_FUNCTION(readData, lFirstArg.byref, lSecondArg.pulVal);
						
					} while (false);
					
					return lresult;
				}


			private:

				CComPtrCustom<ISampleGrabberCallInner> mISampleGrabberCallInner;

			};

			HRESULT lresult;

			do
			{
				LOG_CHECK_STATE(aPtrPtrISampleGrabberCall == nullptr);
				
				LOG_CHECK_STATE_DESCR(aREFIID != __uuidof(ISampleGrabberCall), E_NOINTERFACE);
				
				CComPtrCustom<ISampleGrabberCallInner> lISampleGrabberCallInner;
				
				std::vector<GUIDToNamePair> lReadModes;

				getReadModes(lReadModes);

				auto lfind = std::find_if(lReadModes.begin(),
					lReadModes.end(),
					[this](const GUIDToNamePair& s) { return memcmp(&s.mGUID, &mReadMode, sizeof(mReadMode)) == 0; });

				LOG_CHECK_STATE(lfind == lReadModes.end());
								
				LOG_INVOKE_FUNCTION(Singleton<CaptureManagerBroker>::getInstance().createSampleGrabberOutputNode,
					aRefMajorType,
					aRefSubType,
					(*lfind).mName,
					aSampleByteSize,
					&lISampleGrabberCallInner);
				
				LOG_CHECK_PTR_MEMORY(lISampleGrabberCallInner);

				CComPtrCustom<Proxy> lISampleGrabber(new (std::nothrow) Proxy(lISampleGrabberCallInner));

				LOG_CHECK_PTR_MEMORY(lISampleGrabber);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lISampleGrabber, aPtrPtrISampleGrabberCall);

			} while (false);

			return lresult;
		}



		// IDispatch interface stub

		STDMETHODIMP SampleGrabberCallSinkFactory::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);
				
				if (_wcsicmp(*rgszNames, OLESTR("createOutputNode")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateOutputNode;

					lresult = S_OK;
				}

			} while (false);

			return lresult;
		}

		HRESULT SampleGrabberCallSinkFactory::invokeMethod(
			/* [annotation][in] */
			_In_  DISPID dispIdMember,
			/* [annotation][out][in] */
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			_Out_opt_  VARIANT *pVarResult) {

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

				LOG_CHECK_PTR_MEMORY(pVarResult);

				switch (dispIdMember)
				{
				case (int)MethodsEnum::CreateOutputNode:
				{

					LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 4, DISP_E_BADPARAMCOUNT);
					
					GUID lMajorType;
					
					GUID lSubType;
					
					DWORD lSampleByteSize;
					
					IID lIID;


					if (pDispParams->rgvarg[3].vt == VT_BSTR && pDispParams->rgvarg[3].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[3].bstrVal, &lMajorType);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[2].vt == VT_BSTR && pDispParams->rgvarg[2].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[2].bstrVal, &lSubType);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}


					if (pDispParams->rgvarg[1].vt == VT_UI4 || 
						pDispParams->rgvarg[1].vt == VT_I4)
					{
						lSampleByteSize = pDispParams->rgvarg[1].ulVal;
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}

					if (pDispParams->rgvarg[0].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != nullptr)
					{
						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[0].bstrVal, &lIID);
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}
					
					CComPtrCustom<IUnknown> lOutputType;

					LOG_INVOKE_FUNCTION(createOutputNode,
						lMajorType,
						lSubType,
						lSampleByteSize,
						lIID,
						&lOutputType);
					
					pVarResult->vt = VT_UNKNOWN;

					pVarResult->punkVal = lOutputType.detach();

				}
				break;
				default:
					break;
				}


			} while (false);


			return lresult;
		}


		// IContainer interface implementation

		STDMETHODIMP SampleGrabberCallSinkFactory::setContainerFormat(
			REFGUID aRefContainerTypeGUID)
		{
			HRESULT lresult;

			do
			{
				mReadMode = aRefContainerTypeGUID;

				lresult = S_OK;

			} while (false);

			return lresult;
		}
	}
}
