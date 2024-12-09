#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <list>
#include <mutex>
#include <exception>
#include <memory>


#include "../Common/MFHeaders.h"
#include "../DataParser/DataParser.h"
#include "../Common/Common.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/BaseUnknown.h"
#include "ILogPrintOutCallbackInner.h"
#include "ILogPrintOutAsyncCallback.h"



namespace CaptureManager
{
	namespace Log
	{
		class LogPrintOutAsyncCallback :
			public BaseUnknown<IMFAsyncCallback, ILogPrintOutAsyncCallback>
		{

		public:

			MIDL_INTERFACE("30B0D10C-E082-49F7-9662-7CB9F6E808EB")
				ILogPrintOutAsyncCallbackRequest : public IUnknown
			{
			public:

				STDMETHOD(invoke)(ILogPrintOutCallbackInner* aPtrCallbackInner) = 0;
			};

			class LogPrintOutAsyncCallbackRequest :
				public BaseUnknown<ILogPrintOutAsyncCallbackRequest>
			{
			public:

				LogPrintOutAsyncCallbackRequest(wchar_t* aPtrMessage, std::streamsize length);

				STDMETHOD(invoke)(ILogPrintOutCallbackInner* aPtrCallbackInner);

				//LogPrintOutAsyncCallbackRequest()
				//{}

				//STDMETHOD(invoke)()
				//{


				//	return S_OK;
				//}

			private:

				std::wstring mMessage;

			};

			//interface IMFAsyncCallback

			STDMETHOD(GetParameters)(
				__RPC__out DWORD* pdwFlags,
				__RPC__out DWORD* pdwQueue);

			STDMETHOD(Invoke)(
				IMFAsyncResult* aPtrAsyncResult);


			// interface ILogPrintOutAsyncCallback

			STDMETHOD(AddCallbackInner)(
				/* [in] */ IUnknown* aPtrUnkCallbackInner);

		private:


			std::mutex mInvokeMutex;


			std::list<CComPtrCustom<ILogPrintOutCallbackInner>> mCallbackInner;
		};
	}
}
