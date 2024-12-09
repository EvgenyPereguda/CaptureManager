#pragma once

#include <string>
#include <mutex>
#include <map>

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../CaptureManagerBroker/SinkCommon.h"
#include "../Common/ConnectionPointContainer.h"
#include "../Common/ConnectionPoint.h"
#include "../Common/EnumConnections.h"
#include "../LogPrintOut/ILogPrintOutCallbackInner.h"

namespace CaptureManager
{
	namespace COMServer
	{
		class CoLogPrintOut :
			public BaseDispatch<ILogPrintOutControl, ILogPrintOutCallbackControl>
		{
		public:
			CoLogPrintOut();
			virtual ~CoLogPrintOut();

			// interface ILogPrintOutControl

			STDMETHOD(setVerbose)(
				DWORD aLevelType,
				BSTR aFilePath,
				boolean aState);

			STDMETHOD(addPrintOutDestination)(
				DWORD aLevelType,
				BSTR aFilePath);

			STDMETHOD(removePrintOutDestination)(
				DWORD aLevelType,
				BSTR aFilePath);

			// interface ILogPrintOutCallbackControl

			STDMETHOD(getIConnectionPointContainer)(
				/* [in] */ REFIID aREFIID,
				/* [out] */ IUnknown** aPtrPtrControl);

			STDMETHOD(setVerbose)(
				/* [in] */ DWORD aLevelType,
				/* [in] */ boolean aState);

			// IDispatch interface stub
			
			STDMETHOD(GetIDsOfNames)(
				__RPC__in REFIID riid,
				/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
				/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
				LCID lcid,
				/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);

			virtual HRESULT invokeMethod(
				/* [annotation][in] */
				_In_  DISPID dispIdMember,
				/* [annotation][out][in] */
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				VARIANT *pVarResult);

		private:

			struct LogPrintOutCallbackInner :
				BaseUnknown<
				ILogPrintOutCallbackInner>
			{
				// ILogPrintOutCallbackInner interface

				virtual void Invoke(
					/* [in] */ DWORD aLevelType,
					/* [in] */ BSTR aPtrLogString)
				{
					switch (aLevelType)
					{
					case LogPrintOut::ERROR_LEVEL:
					{
						if (!mVerbose_ERROR_LEVEL)
							return;

					}
						break;
					case LogPrintOut::INFO_LEVEL:
					{
						if (!mVerbose_INFO_LEVEL)
							return;

					}
						break;
					default:
						break;
					}

					std::lock_guard<std::mutex> lLock(mMutex);

					auto liter = mCallbackMassive.begin();

					for (; liter != mCallbackMassive.end(); liter++)
					{
						(*liter).second->invoke(
							aLevelType,
							aPtrLogString);
					}
				}

				STDMETHOD(setVerbose)(
					/* [in] */ DWORD aLevelType,
					/* [in] */ boolean aState) {

					LogPrintOut::Level lLogPrintOut(LogPrintOut::INFO_LEVEL);

					switch (aLevelType)
					{
					case LogPrintOut::ERROR_LEVEL:
						mVerbose_ERROR_LEVEL = aState;
						break;
					case LogPrintOut::INFO_LEVEL:
						mVerbose_INFO_LEVEL = aState;
						break;
					default:
						break;
					}

					return S_OK;
				}

				STDMETHOD(Advise)(
					ILogPrintOutCallback* aPtrISessionCallback,
					/* [out] */DWORD* pdwCookie)
				{

					HRESULT lresult;

					do
					{
						if (aPtrISessionCallback == nullptr) {

							lresult = E_POINTER;

							break;
						}

						if (pdwCookie == nullptr) {

							lresult = E_POINTER;

							break;
						}

						CComPtrCustom<ILogPrintOutCallback> lILogPrintOutCallback;

						std::lock_guard<std::mutex> lLock(mMutex);

						lILogPrintOutCallback = aPtrISessionCallback;

						if (lLastCookie >= (2 << (sizeof(DWORD) * 7)) - 1)
						{
							lresult = S_FALSE;

							break;
						}

						*pdwCookie = lLastCookie;

						mCallbackMassive[lLastCookie++] = lILogPrintOutCallback;

						lresult = S_OK;

					} while (false);

					return lresult;
				}

				STDMETHOD(Unadvise)(
					DWORD dwCookie)
				{

					HRESULT lresult;

					do
					{
						std::lock_guard<std::mutex> lLock(mMutex);

						auto lfind = mCallbackMassive.find(dwCookie);

						if (lfind == mCallbackMassive.end())
						{
							lresult = S_FALSE;

							break;
						}

						mCallbackMassive.erase(lfind);

						lresult = S_OK;

					} while (false);

					return lresult;
				}


				STDMETHOD(getEnumConnections)(
					__RPC__deref_out_opt IEnumConnections** ppEnum)
				{

					HRESULT lresult;

					do
					{
						CComPtrCustom<EnumConnections<LogPrintOutCallbackInner>> lEnumConnections(new (std::nothrow)EnumConnections<LogPrintOutCallbackInner>(this));

						if (!lEnumConnections)
						{
							lresult = E_OUTOFMEMORY;

							break;
						}

						lresult = lEnumConnections->QueryInterface(IID_PPV_ARGS(ppEnum));

					} while (false);

					return lresult;
				}


				DWORD lLastCookie = 0;

				std::mutex mMutex;

				std::map<DWORD, CComPtrCustom<ILogPrintOutCallback>> mCallbackMassive;

				bool mVerbose_INFO_LEVEL = true;

				bool mVerbose_ERROR_LEVEL = true;

			};

			CComPtrCustom<LogPrintOutCallbackInner> mLogPrintOutCallbackInner;


			CComPtrCustom<ConnectionPointContainer> mConnectionPointContainer;


			HRESULT invokeSetVerbose(
				_In_  DISPPARAMS *pDispParams);

			HRESULT invokeAddPrintOutDestination(
				_In_  DISPPARAMS *pDispParams);

			HRESULT invokeRemovePrintOutDestination(
				_In_ DISPPARAMS *pDispParams);
			
		};
	}
}
