#pragma once

#include <ocidl.h>
#include <map>
#include "BaseUnknown.h"
#include "ComPtrCustom.h"

namespace CaptureManager
{

	template <typename CallbackInner, typename ICallback>
	struct ConnectionPoint :
		public BaseUnknown<IConnectionPoint>
	{
	private:

		IID mIID;

		CComPtrCustom<IConnectionPointContainer> mIConnectionPointContainer;

		CComPtrCustom<CallbackInner> mCallbackInner;

	public:

		ConnectionPoint(
			IID aIID,
			CallbackInner* aPtrSessionCallbackInner) :
			mIID(aIID)
		{
			mCallbackInner = aPtrSessionCallbackInner;
		}

		void registerContainer(
			IConnectionPointContainer* aPtrIConnectionPointContainer)
		{
			mIConnectionPointContainer = aPtrIConnectionPointContainer;
		}

		STDMETHOD(GetConnectionInterface)(
			__RPC__out IID* pIID)
		{

			HRESULT lresult;

			do
			{
				*pIID = mIID;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		STDMETHOD(GetConnectionPointContainer)(
			__RPC__deref_out_opt IConnectionPointContainer** ppCPC)
		{

			HRESULT lresult;

			do
			{
				if (ppCPC == nullptr)
				{
					lresult = E_INVALIDARG;

					break;
				}

				if (!mIConnectionPointContainer)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = mIConnectionPointContainer->QueryInterface(IID_PPV_ARGS(ppCPC));

			} while (false);

			return lresult;
		}

		STDMETHOD(Advise)(
			IUnknown* pUnkSink,
			__RPC__out DWORD* pdwCookie)
		{

			HRESULT lresult;

			do
			{
				if (!mCallbackInner)
				{
					lresult = E_POINTER;

					break;
				}

				if (pUnkSink == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				CComPtrCustom<ICallback> lCallback;

				lresult = pUnkSink->QueryInterface(IID_PPV_ARGS(&lCallback));

				if (FAILED(lresult))
				{
					break;
				}

				lresult = mCallbackInner->Advise(
					lCallback,
					pdwCookie);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			return lresult;
		}

		STDMETHOD(Unadvise)(
			DWORD dwCookie)
		{
			HRESULT lresult;

			do
			{
				if (!mCallbackInner)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = mCallbackInner->Unadvise(
					dwCookie);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			return lresult;
		}

		STDMETHOD(EnumConnections)(
			__RPC__deref_out_opt IEnumConnections** ppEnum)
		{

			HRESULT lresult;

			do
			{
				lresult = mCallbackInner->getEnumConnections(ppEnum);
			} while (false);

			return lresult;
		}

	};
}