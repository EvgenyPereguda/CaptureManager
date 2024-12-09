#pragma once

#include <string>
#include <mutex>
#include <map>

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/BaseUnknown.h"
#include "../CaptureManagerBroker/ISessionCallbackInner.h"
#include "../CaptureManagerBroker/SinkCommon.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/ConnectionPointContainer.h"
#include "../Common/ConnectionPoint.h"
#include "../Common/EnumConnections.h"


namespace CaptureManager
{
	namespace COMServer
	{
		class Session:
			public BaseDispatch<
			ISession
			>
		{
		private:
									
			struct SessionCallbackInner :
				BaseUnknown<
				ISessionCallbackInner>
			{
				// ISessionCallbackInner interface

				virtual void Invoke(
				CallbackEventCodeDescriptor aCallbackEventCode,
				SessionDescriptor aSessionDescriptor)
				{
					std::lock_guard<std::mutex> lLock(mMutex);

					auto liter = mCallbackMassive.begin();

					for (; liter != mCallbackMassive.end(); liter++)
					{
						(*liter).second->invoke(
							aCallbackEventCode,
							aSessionDescriptor);
					}
				}				
				
				STDMETHOD(Advise)(
					ISessionCallback *aPtrISessionCallback,
					/* [out] */DWORD *pdwCookie)
				{

					HRESULT lresult;

					do
					{
						LOG_CHECK_PTR_MEMORY(aPtrISessionCallback);

						LOG_CHECK_PTR_MEMORY(pdwCookie);

						CComPtrCustom<ISessionCallback> lISessionCallback;

						std::lock_guard<std::mutex> lLock(mMutex);
						
						lISessionCallback = aPtrISessionCallback;

						if (lLastCookie >= (2 << (sizeof(DWORD)* 7)) - 1)
						{
							lresult = S_FALSE;

							break;
						}
												
						*pdwCookie = lLastCookie;
												
						mCallbackMassive[lLastCookie++] = lISessionCallback;

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
					__RPC__deref_out_opt IEnumConnections **ppEnum)
				{

					HRESULT lresult;

					do
					{
						CComPtrCustom<EnumConnections<SessionCallbackInner>> lEnumConnections(new (std::nothrow)EnumConnections<SessionCallbackInner>(this));

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

				std::map<DWORD, CComPtrCustom<ISessionCallback>> mCallbackMassive;

			};

			SessionDescriptor mSessionDescriptor;

			CComPtrCustom<SessionCallbackInner> mSessionCallback;

			CComPtrCustom<ConnectionPointContainer> mConnectionPointContainer;

		public:
			
			HRESULT init(VARIANT aArrayPtrSourceNodesOfTopology);

			

			// ISession interface

			STDMETHOD(startSession)(
				LONGLONG aStartPositionInHundredNanosecondUnits,
				REFGUID aGUIDTimeFormat);

			STDMETHOD(stopSession)(void);

			STDMETHOD(pauseSession)(void);

			STDMETHOD(closeSession)(void);
			
			STDMETHOD(getSessionDescriptor)(
				DWORD *aPtrSessionDescriptor);

			STDMETHOD(getIConnectionPointContainer)(
				REFIID aREFIID,
				IUnknown **aPtrPtrControl);


			// IDispatch interface stub

			STDMETHOD(GetIDsOfNames)(
				__RPC__in REFIID riid,
				/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
				/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
				LCID lcid,
				/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);

			virtual HRESULT invokeMethod(
				/* [annotation][in] */
				DISPID dispIdMember,
				/* [annotation][out][in] */
				DISPPARAMS *pDispParams,
				/* [annotation][out] */
				 VARIANT *pVarResult);

		};
	}
}