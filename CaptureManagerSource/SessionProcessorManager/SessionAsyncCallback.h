#pragma once

#include <mutex>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../CaptureManagerBroker/ISessionCallbackInner.h"

namespace CaptureManager
{
	namespace Core
	{
		class SessionAsyncCallback:
			public BaseUnknown<IMFAsyncCallback>
		{

		public:

			MIDL_INTERFACE("DFF03825-8AD5-4A5E-B7F0-F395F590798D")
			ISessionAsyncCallbackRequest : public IUnknown
			{
			public:

				STDMETHOD(invoke)() = 0;
			};

			class SessionAsyncCallbackRequest :
				public BaseUnknown<ISessionAsyncCallbackRequest>
			{
			public:

				SessionAsyncCallbackRequest(
					CallbackEventCodeDescriptor aCodeDescriptor,
					SessionDescriptor aSessionDescriptor,
					CComPtrCustom<ISessionCallbackInner>& aISessionCallbackInner) :
					mCodeDescriptor(aCodeDescriptor),
					mSessionDescriptor(aSessionDescriptor),
					mISessionCallbackInner(aISessionCallbackInner)
				{}

				STDMETHOD(invoke)()
				{

					if (mISessionCallbackInner)
					{
						mISessionCallbackInner->Invoke(
							mCodeDescriptor,
							mSessionDescriptor);
					}

					return S_OK;
				}

			private:

				CallbackEventCodeDescriptor mCodeDescriptor;

				SessionDescriptor mSessionDescriptor;

				CComPtrCustom<ISessionCallbackInner> mISessionCallbackInner;
			};


			STDMETHOD(GetParameters)(
				__RPC__out DWORD *pdwFlags,
				__RPC__out DWORD *pdwQueue);

			STDMETHOD(Invoke)(
				IMFAsyncResult *aPtrAsyncResult);

		private:


			std::mutex mInvokeMutex;
		};
	}
}