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

#include "CoCaptureManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Common.h"
#include "SourceControl.h"
#include "SinkControl.h"
#include "SessionControl.h"
#include "StreamControl.h"
#include "EncoderControl.h"
#include "MediaTypeParser.h"
#include "StrideForBitmap.h"
#include "VersionControl.h"
#include "EVRStreamControl.h"
#include "RenderingControl.h"
#include "SwitcherControl.h"
#include "VideoMixerControl.h"
#include "AudioMixerControl.h"
#include "SARVolumeControl.h"





namespace CaptureManager
{
	namespace COMServer
	{

		enum class MethodsEnum :DISPID
		{
			CreateControl = 1,
			CreateMisc = CreateControl + 1
		};

		CoCaptureManager::CoCaptureManager()
		{
		}

		CoCaptureManager::~CoCaptureManager()
		{
		}
		
		// ICaptureManagerControl interface implementation
		
		STDMETHODIMP CoCaptureManager::createControl(
			REFIID aREFIID,
			IUnknown **aPtrPtrControl)
		{

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrControl);
				
				CComPtrCustom<IUnknown> ltempInstance;

				if (aREFIID == __uuidof(ISourceControl))
				{
					ltempInstance = CComPtrCustom<SourceControl>(new (std::nothrow)SourceControl);
				}
				else if (aREFIID == __uuidof(ISinkControl))
				{
					ltempInstance = CComPtrCustom<SinkControl>(new (std::nothrow)SinkControl);
				}
				else if (aREFIID == __uuidof(ISessionControl))
				{
					ltempInstance = CComPtrCustom<SessionControl>(new (std::nothrow)SessionControl);
				}
				else if (aREFIID == __uuidof(IStreamControl))
				{
					ltempInstance = CComPtrCustom<StreamControl>(new (std::nothrow)StreamControl);
				}
				else if (aREFIID == __uuidof(IEncoderControl))
				{
					ltempInstance = CComPtrCustom<EncoderControl>(new (std::nothrow)EncoderControl);
				}				
				else
				{
					lresult = E_NOINTERFACE;

					break;
				}
				
				LOG_CHECK_STATE_DESCR(!ltempInstance, E_OUTOFMEMORY);
												
				LOG_INVOKE_POINTER_METHOD(ltempInstance, QueryInterface,
					aREFIID,
					(void**)aPtrPtrControl);
								
			} while (false);

			return lresult;
		}

		STDMETHODIMP CoCaptureManager::createMisc(
			REFIID aREFIID,
			IUnknown **aPtrPtrMisc)
		{

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrMisc);
				
				CComPtrCustom<IUnknown> ltempInstance;

				if (aREFIID == __uuidof(IMediaTypeParser))
				{
					ltempInstance = CComPtrCustom<MediaTypeParser>(new (std::nothrow)MediaTypeParser);
				}
				else if (aREFIID == __uuidof(IStrideForBitmap))
				{
					ltempInstance = CComPtrCustom<StrideForBitmap>(new (std::nothrow)StrideForBitmap);
				}
				else if (aREFIID == __uuidof(IVersionControl))
				{
					ltempInstance = CComPtrCustom<VersionControl>(new (std::nothrow)VersionControl);
				}
				else if (aREFIID == __uuidof(IEVRStreamControl))
				{
					ltempInstance = CComPtrCustom<EVRStreamControl>(new (std::nothrow)EVRStreamControl);
				}
				else if (aREFIID == __uuidof(IRenderingControl))
				{
					ltempInstance = CComPtrCustom<RenderingControl>(new (std::nothrow)RenderingControl);
				}
				else if (aREFIID == __uuidof(ISwitcherControl))
				{
					ltempInstance = CComPtrCustom<SwitcherControl>(new (std::nothrow)SwitcherControl);
				}
				else if (aREFIID == __uuidof(IVideoMixerControl))
				{
					ltempInstance = CComPtrCustom<VideoMixerControl>(new (std::nothrow)VideoMixerControl);
				}
				else if (aREFIID == __uuidof(IAudioMixerControl))
				{
					ltempInstance = CComPtrCustom<AudioMixerControl>(new (std::nothrow)AudioMixerControl);
				}
				else if (aREFIID == __uuidof(ISARVolumeControl))
				{
					ltempInstance = CComPtrCustom<SARVolumeControl>(new (std::nothrow)SARVolumeControl);
				}
				else
				{					
					lresult = E_NOINTERFACE;

					break;
				}

				

				LOG_CHECK_STATE_DESCR(!ltempInstance, E_OUTOFMEMORY);
				
				LOG_INVOKE_POINTER_METHOD(ltempInstance, QueryInterface,
					aREFIID,
					(void**)aPtrPtrMisc);
				
			} while (false);

			return lresult;
		}
	

		// IDispatch interface stub

		STDMETHODIMP CoCaptureManager::GetIDsOfNames(
			__RPC__in REFIID riid,
			/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
			/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
			LCID lcid,
			/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

			HRESULT lresult(DISP_E_UNKNOWNNAME);

			do
			{
				LOG_CHECK_STATE(cNames != 1);

				if (_wcsicmp(*rgszNames, OLESTR("createControl")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateControl;

					lresult = S_OK;
				}
				else if (_wcsicmp(*rgszNames, OLESTR("createMisc")) == 0)
				{
					*rgDispId = (int)MethodsEnum::CreateMisc;

					lresult = S_OK;
				}


			} while (false);

			return lresult;
		}

		HRESULT CoCaptureManager::invokeMethod(
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

				LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);
				
				switch (dispIdMember)
				{
				case (int)MethodsEnum::CreateControl:
				{

					if (pDispParams->rgvarg[0].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != nullptr)
					{
						CLSID lControlID;

						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[0].bstrVal, &lControlID);

						CComPtrCustom<IUnknown> lControl;

						LOG_INVOKE_FUNCTION(createControl, lControlID, &lControl);
						
						pVarResult->vt = VT_UNKNOWN;

						pVarResult->punkVal = lControl.detach();
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}
				}
				break;
				case (int)MethodsEnum::CreateMisc:
				{

					if (pDispParams->rgvarg[0].vt == VT_CLSID)
					{
					}
					else if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[0].bstrVal != nullptr)
					{
						CLSID lControlID;

						LOG_INVOKE_FUNCTION(CLSIDFromString, pDispParams->rgvarg[0].bstrVal, &lControlID);

						CComPtrCustom<IUnknown> lControl;

						LOG_INVOKE_FUNCTION(createMisc, lControlID, &lControl);
						
						pVarResult->vt = VT_UNKNOWN;

						pVarResult->punkVal = lControl.detach();
					}
					else
					{
						lresult = DISP_E_BADVARTYPE;

						break;
					}
				}
				break;

				default:
					break;
				}


			} while (false);

			return lresult;
		}
	}
}