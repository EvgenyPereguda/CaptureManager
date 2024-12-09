#include "pch.h"
#include "Session.h"
#include "BaseUnknown.h"


namespace CaptureManagerToWinRTProxy
{
	Session::Session(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mISession));

		} while (false);
	}


	Boolean Session::startSession(
		int64 aStartPositionInHundredNanosecondUnits,
		Guid aGUIDTimeFormat)
	{
		Boolean lresult = false;

		do
		{
			if (!mISession)
				break;
			
			auto lhr = mISession->startSession(
				aStartPositionInHundredNanosecondUnits,
				aGUIDTimeFormat);

			if (FAILED(lhr))
				break;
						
			lresult = true;

		} while (false);

		return lresult;
	}

	Boolean Session::stopSession()
	{
		Boolean lresult = false;

		do
		{
			if (!mISession)
				break;
			
			auto lhr = mISession->stopSession();

			if (FAILED(lhr))
				break;

			lresult = true;

		} while (false);

		return lresult;
	}

	Boolean Session::pauseSession()
	{
		Boolean lresult = false;

		do
		{
			if (!mISession)
				break;
			
			auto lhr = mISession->pauseSession();

			if (FAILED(lhr))
				break;

			lresult = true;

		} while (false);

		return lresult;
	}

	Boolean Session::closeSession()
	{
		Boolean lresult = false;

		do
		{
			if (!mISession)
				break;
			
			auto lhr = mISession->closeSession();

			if (FAILED(lhr))
				break;

			lresult = true;

		} while (false);

		return lresult;
	}

	Boolean Session::getSessionDescriptor(uint32 *aPtrSessionDescriptor)
	{
		Boolean lresult = false;

		do
		{
			if (!mISession)
				break;
			
			auto lhr = mISession->getSessionDescriptor((DWORD*)aPtrSessionDescriptor);

			if (FAILED(lhr))
				break;

			lresult = true;

		} while (false);

		return lresult;
	}

	Boolean Session::attachISessionCallback(ISessionCallback^ aISessionCallback)
	{
		class SessionCallbackProxy :
			public BaseUnknown <::ISessionCallback>
		{
			CaptureManagerToWinRTProxy::ISessionCallback^ mISessionCallback;

		public:

			SessionCallbackProxy(
				CaptureManagerToWinRTProxy::ISessionCallback^ aISessionCallback)
			{
				mISessionCallback = aISessionCallback;
			}

			virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE invoke(
				/* [in] */ DWORD aCallbackEventCode,
				/* [in] */ DWORD aSessionDescriptor)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					if (mISessionCallback == nullptr)
						break;
															
					if (!mISessionCallback->invoke(
						aCallbackEventCode,
						aSessionDescriptor))
						break;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

		};

			

		Boolean lresult = false;

		do
		{
			if (!mISession)
				break;
			
			CComPtrCustom<IConnectionPointContainer> lIConnectionPointContainer;

			CComPtrCustom<IUnknown> lIUnknown;

			auto lhr = mISession->getIConnectionPointContainer(
				__uuidof(::IConnectionPointContainer),
				&lIUnknown);
			
			if (FAILED(lhr))
				break;

			if (!lIUnknown)
				break;

			lhr = lIUnknown->QueryInterface(IID_PPV_ARGS(&lIConnectionPointContainer));

			if (FAILED(lhr))
				break;

			if (!lIConnectionPointContainer)
				break;

			CComPtrCustom<IConnectionPoint> lIConnectionPoint;

			lhr = lIConnectionPointContainer->FindConnectionPoint(
				__uuidof(::ISessionCallback),
				&lIConnectionPoint);

			if (FAILED(lhr))
				break;

			if (!lIConnectionPoint)
				break;
					
			CComPtrCustom<::ISessionCallback> lSessionCallbackProxy = 
				new (std::nothrow) SessionCallbackProxy(aISessionCallback);
			
			if (!lSessionCallbackProxy)
				break;

			DWORD lkey;
			
			lhr = lIConnectionPoint->Advise(
				lSessionCallbackProxy,
				&lkey);

			if (FAILED(lhr))
				break;

			mISessionCallbacks[lkey] = lSessionCallbackProxy;

			lresult = true;

		} while (false);

		return lresult;
	}
}