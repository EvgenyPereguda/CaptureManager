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

#include "CoLogPrintOut.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Singleton.h"


namespace CaptureManager
{
	namespace COMServer
	{
		enum class MethodsEnum :DISPID
		{
			SetVerbose = 1,
			AddPrintOutDestination = SetVerbose + 1,
			RemovePrintOutDestination = AddPrintOutDestination + 1
		};
		
		CoLogPrintOut::CoLogPrintOut()
		{

			HRESULT lresult(E_NOTIMPL);

			do
			{

				mLogPrintOutCallbackInner = CComPtrCustom<LogPrintOutCallbackInner>(new (std::nothrow) LogPrintOutCallbackInner);

				LOG_CHECK_PTR_MEMORY(mLogPrintOutCallbackInner);

				mConnectionPointContainer = new (std::nothrow) ConnectionPointContainer();

				LOG_CHECK_PTR_MEMORY(mConnectionPointContainer);

				mConnectionPointContainer->Release();

				LOG_INVOKE_POINTER_METHOD(mConnectionPointContainer, addConnectionPoint,
					new (std::nothrow) ConnectionPoint<LogPrintOutCallbackInner, ILogPrintOutCallback>(
						__uuidof(ILogPrintOutCallback),
						mLogPrintOutCallbackInner));

				LogPrintOut::getInstance().addCallbackInner(mLogPrintOutCallbackInner);

			} while (false);
		}

		CoLogPrintOut::~CoLogPrintOut()
		{
		}

		STDMETHODIMP CoLogPrintOut::setVerbose(
			DWORD aLevelType,
			BSTR aFilePath,
			boolean aState)
		{
			HRESULT lresult;

			do
			{

				LogPrintOut::Level lLogPrintOut(LogPrintOut::INFO_LEVEL);

				switch (aLevelType)
				{
				case LogPrintOut::ERROR_LEVEL:
					lLogPrintOut = LogPrintOut::ERROR_LEVEL;
					break;
				case LogPrintOut::INFO_LEVEL:
				default:
					break;
				}

				bool lstate = aState > 0;

				LogPrintOut::getInstance().setVerbose(
					lLogPrintOut,
					aFilePath,
					lstate);

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		STDMETHODIMP CoLogPrintOut::addPrintOutDestination(
			DWORD aLevelType,
			BSTR aFilePath)
		{
			HRESULT lresult(E_FAIL);

			do
			{

				LogPrintOut::Level lLogPrintOut(LogPrintOut::INFO_LEVEL);

				switch (aLevelType)
				{
				case LogPrintOut::ERROR_LEVEL:
					lLogPrintOut = LogPrintOut::ERROR_LEVEL;
					break;
				case LogPrintOut::INFO_LEVEL:
				default:
					break;
				}

				bool lboolResult = LogPrintOut::getInstance().addFileLogPrintOutStream(
					lLogPrintOut,
					aFilePath);

				if (!lboolResult)
				{
					break;
				}

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		STDMETHODIMP CoLogPrintOut::removePrintOutDestination(
			DWORD aLevelType,
			BSTR aFilePath)
		{
			HRESULT lresult(E_FAIL);

			do
			{

				LogPrintOut::Level lLogPrintOut(LogPrintOut::INFO_LEVEL);

				switch (aLevelType)
				{
				case LogPrintOut::ERROR_LEVEL:
					lLogPrintOut = LogPrintOut::ERROR_LEVEL;
					break;
				case LogPrintOut::INFO_LEVEL:
				default:
					break;
				}

				bool lboolResult = LogPrintOut::getInstance().removeFileLogPrintOutStream(
					lLogPrintOut,
					aFilePath);

				if (!lboolResult)
				{
					break;
				}

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		STDMETHODIMP_(HRESULT __stdcall) CoLogPrintOut::getIConnectionPointContainer(REFIID aREFIID, IUnknown** aPtrPtrControl)
		{

			HRESULT lresult(E_NOTIMPL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrControl);

				lresult = E_NOINTERFACE;

				if (aREFIID == __uuidof(IConnectionPointContainer))
				{

					LOG_CHECK_PTR_MEMORY(mConnectionPointContainer);

					LOG_INVOKE_POINTER_METHOD(mConnectionPointContainer, QueryInterface,
						aREFIID,
						(void**)aPtrPtrControl);
				}

			} while (false);

			return lresult;
		}

		STDMETHODIMP_(HRESULT __stdcall) CoLogPrintOut::setVerbose(
			/* [in] */ DWORD aLevelType,
			/* [in] */ boolean aState)
		{
			if (mLogPrintOutCallbackInner)
				mLogPrintOutCallbackInner->setVerbose(aLevelType, aState);

				return S_OK;
		}


		// IDispatch interface stub

		STDMETHODIMP CoLogPrintOut::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId){

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				if (cNames != 1)
				{
					lresult = E_INVALIDARG;

					break;
				}
				
				if (_wcsicmp(*rgszNames, OLESTR("addPrintOutDestination")) == 0)
				{
					*rgDispId = (int)MethodsEnum::AddPrintOutDestination;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("removePrintOutDestination")) == 0)
				{
					*rgDispId = (int)MethodsEnum::RemovePrintOutDestination;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("setVerbose")) == 0)
				{
					*rgDispId = (int)MethodsEnum::SetVerbose;

					lresult = S_OK;
				}
				
				
			} while (false);
			
			return lresult;
		}

		HRESULT CoLogPrintOut::invokeMethod(
			/* [annotation][in] */
			_In_  DISPID dispIdMember,
			/* [annotation][out][in] */
			_In_  DISPPARAMS *pDispParams,
			/* [annotation][out] */
			VARIANT *pVarResult) 
		{

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				if (pDispParams == nullptr)
				{
					lresult = DISP_E_PARAMNOTFOUND;

					break;
				}
								
				switch (dispIdMember)
				{
				case (int)MethodsEnum::SetVerbose:
				{
					lresult = invokeSetVerbose(pDispParams);
				}
				break;
				case (int)MethodsEnum::AddPrintOutDestination:
				{
					lresult = invokeAddPrintOutDestination(pDispParams);
				}
				break;
				case (int)MethodsEnum::RemovePrintOutDestination:
				{

					lresult = invokeRemovePrintOutDestination(pDispParams);
				}
				break;
								
				default:
					break;
				}


			} while (false);


			return lresult;
		}

		HRESULT CoLogPrintOut::invokeSetVerbose(
			_In_ DISPPARAMS *pDispParams)
		{

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				if (pDispParams == nullptr)
				{
					lresult = DISP_E_PARAMNOTFOUND;

					break;
				}

				if (pDispParams->cArgs != 3)
				{
					lresult = DISP_E_BADPARAMCOUNT;

					break;
				}
								
				DWORD lLevelType;

				BSTR lFilePath;

				boolean lState = true;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				VARIANT lThirdArg = pDispParams->rgvarg[2];

				if (lFirstArg.vt == VT_BOOL)
				{
					lState = lFirstArg.boolVal != 0;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lSecondArg.vt == VT_BSTR)
				{
					lFilePath = lSecondArg.bstrVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lThirdArg.vt == VT_UI4 ||
					lThirdArg.vt == VT_I4)
				{
					lLevelType = lThirdArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				lresult = this->setVerbose(
					lLevelType,
					lFilePath,
					lState);


			} while (false);


			return lresult;
		}

		HRESULT CoLogPrintOut::invokeAddPrintOutDestination(
			_In_ DISPPARAMS *pDispParams)
		{

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				if (pDispParams == nullptr)
				{
					lresult = DISP_E_PARAMNOTFOUND;

					break;
				}

				if (pDispParams->cArgs != 2)
				{
					lresult = DISP_E_BADPARAMCOUNT;

					break;
				}

				DWORD lLevelType;

				BSTR lFilePath;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				if (lSecondArg.vt == VT_UI4 ||
					lSecondArg.vt == VT_I4)
				{
					lLevelType = lSecondArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lFirstArg.vt == VT_BSTR)
				{
					lFilePath = lFirstArg.bstrVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}
				




				lresult = this->addPrintOutDestination(
					lLevelType,
					lFilePath);


			} while (false);


			return lresult;
		}

		HRESULT CoLogPrintOut::invokeRemovePrintOutDestination(
			_In_ DISPPARAMS *pDispParams)
		{

			HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

			do
			{
				if (pDispParams == nullptr)
				{
					lresult = DISP_E_PARAMNOTFOUND;

					break;
				}

				if (pDispParams->cArgs != 2)
				{
					lresult = DISP_E_BADPARAMCOUNT;

					break;
				}

				DWORD lLevelType;

				BSTR lFilePath;

				VARIANT lFirstArg = pDispParams->rgvarg[0];

				VARIANT lSecondArg = pDispParams->rgvarg[1];

				if (lSecondArg.vt == VT_UI4 ||
					lSecondArg.vt == VT_I4)
				{
					lLevelType = lSecondArg.uintVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				if (lFirstArg.vt == VT_BSTR)
				{
					lFilePath = lFirstArg.bstrVal;
				}
				else
				{
					lresult = DISP_E_BADVARTYPE;

					break;
				}

				//std::wstringstream lwstringstream;

				//lwstringstream << lLevelType << lFilePath;

				//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);



				lresult = this->removePrintOutDestination(
					lLevelType,
					lFilePath);
				
			} while (false);

			return lresult;
		}
	}
}