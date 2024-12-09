#pragma once

#include <VersionHelpers.h>
#include "../Common/ComPtrCustom.h"
#include "../Common/BaseMFAttributes.h"
#include "../Common/MFHeaders.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../DirectXManager/DXGIManager.h"
#include "IEVRStreamControl.h"
#include "IMixerStreamPositionControl.h"
#include "IStreamFilterControl.h"
#include "IPresenter.h"

namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			enum class MethodsEnum :DISPID
			{
				SetPosition = 1,
				SetZOrder = SetPosition + 1,
				GetPosition = SetZOrder + 1,
				GetZOrder = GetPosition + 1,
				Flush = GetZOrder + 1,
				GetCollectionOfFilters = Flush + 1,
				SetFilterParametr = GetCollectionOfFilters + 1,
				GetCollectionOfOutputFeatures = SetFilterParametr + 1,
				SetOutputFeatureParametr = GetCollectionOfOutputFeatures + 1,
				SetOpacity = SetOutputFeatureParametr + 1,
				SetSrcPosition = SetOpacity + 1,
				Count = SetSrcPosition
			};

			using namespace CaptureManager;

			using namespace CaptureManager::Core;

			class DECLSPEC_UUID("5A8E4C43-38F8-4193-B8B2-E7180D02E6B7")
				CLSID_SetDirectXDeviceProxy;

									
			template <typename MediaSinkFactory>
			class EVRActivate :
				public BaseMFAttributes<IMFActivate, IEVRStreamControl, IDispatch>
			{

				
			public:
			
				EVRActivate(IMFAttributes* aPtrIMFAttributes) :
					BaseMFAttributes<IMFActivate, IEVRStreamControl, IDispatch>(aPtrIMFAttributes)
				{}

				virtual ~EVRActivate() 
				{
					if (mUnkVideoRenderingClass)
					{
						mUnkVideoRenderingClass.Release();
					}

				}

				virtual ULONG STDMETHODCALLTYPE AddRef(void)
				{
					auto l = BaseMFAttributes::AddRef();

					return l;
				}

				virtual ULONG STDMETHODCALLTYPE Release(void)
				{
					auto l = BaseMFAttributes::Release();

					return l;
				}

				// IMFActivate implementation
				STDMETHODIMP ActivateObject(__RPC__in REFIID riid, __RPC__deref_out_opt void** ppvObject)
				{

					HRESULT lresult;

					do
					{
						if (!mUnkVideoRenderingClass)
						{

							HRESULT lhrCreateMediaSink(E_FAIL);

							CComPtrCustom<IUnknown> lUnkVideoRenderingClass;

							lhrCreateMediaSink = Singleton<MediaSinkFactory>::getInstance().createVideoRenderingClass(
								mPresenter,
								mMixer,
								mMixerStreamID,
								riid,
								&lUnkVideoRenderingClass
								);

							if (SUCCEEDED(lhrCreateMediaSink))
							{
								mUnkVideoRenderingClass = lUnkVideoRenderingClass;
							}
							else
							{
								LOG_CHECK_STATE_DESCR(FAILED(lhrCreateMediaSink), lhrCreateMediaSink);
							}
						}

						LOG_CHECK_PTR_MEMORY(mUnkVideoRenderingClass);

						LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(mUnkVideoRenderingClass, riid, ppvObject);
						
					} while (false);

					return lresult;					
				}						

				STDMETHODIMP DetachObject()
				{
					return S_OK;
				}

				STDMETHODIMP ShutdownObject()
				{
					HRESULT lresult;
					
					if (mUnkVideoRenderingClass)
					{
						CComQIPtrCustom<IMFMediaSink> lMediaSink(mUnkVideoRenderingClass);

						if (lMediaSink)
						{
							do
							{
								LOG_INVOKE_MF_METHOD(Shutdown, lMediaSink);

							} while (true);
						}

						mUnkVideoRenderingClass.Release();
					}
					
					return S_OK;
				}		

				static HRESULT createInstance(
					IPresenter* aPtrPresenter,
					IMFTransform* aPtrMixer,
					DWORD aMixerStreamID,
					IMFActivate** aPtrPtrActivate)
				{
					using namespace Core;

					HRESULT lresult;

					do
					{

						LOG_CHECK_PTR_MEMORY(aPtrPtrActivate);

						LOG_CHECK_PTR_MEMORY(aPtrPresenter);

						LOG_CHECK_PTR_MEMORY(aPtrMixer);

						CComPtrCustom<IMFAttributes> lAttributes;

						LOG_INVOKE_MF_FUNCTION(MFCreateAttributes, &lAttributes, 0);

						LOG_CHECK_PTR_MEMORY(lAttributes);

						CComPtrCustom<EVRActivate> lActivate(new (std::nothrow) EVRActivate(lAttributes));

						LOG_CHECK_PTR_MEMORY(lActivate);

						lActivate->mMixer = aPtrMixer;

						lActivate->mPresenter = aPtrPresenter;

						lActivate->mMixerStreamID = aMixerStreamID;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lActivate, aPtrPtrActivate);


					} while (false);
					
					return lresult;
				}
							   				 			  
				// IDispatch
				virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
					/* [out] */ __RPC__out UINT *pctinfo) {

					if(pctinfo != nullptr)
						*pctinfo = 0;

					return S_OK;
				}

				virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(
					/* [in] */ UINT iTInfo,
					/* [in] */ LCID lcid,
					/* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo) {
					do
					{
						if (ppTInfo != nullptr)
							*ppTInfo = nullptr;

					} while (false);

					return S_OK;
				}

				virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
					/* [in] */ __RPC__in REFIID riid,
					/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
					/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
					/* [in] */ LCID lcid,
					/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {

					HRESULT lresult(DISP_E_UNKNOWNNAME);

					do
					{
						LOG_CHECK_STATE(cNames != 1);

						if (_wcsicmp(*rgszNames, OLESTR("setPosition")) == 0)
						{
							*rgDispId = (int)MethodsEnum::SetPosition;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("setZOrder")) == 0)
						{
							*rgDispId = (int)MethodsEnum::SetZOrder;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("getPosition")) == 0)
						{
							*rgDispId = (int)MethodsEnum::GetPosition;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("getZOrder")) == 0)
						{
							*rgDispId = (int)MethodsEnum::GetZOrder;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("flush")) == 0)
						{
							*rgDispId = (int)MethodsEnum::Flush;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("getCollectionOfFilters")) == 0)
						{
							*rgDispId = (int)MethodsEnum::GetCollectionOfFilters;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("setFilterParametr")) == 0)
						{
							*rgDispId = (int)MethodsEnum::SetFilterParametr;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("getCollectionOfOutputFeatures")) == 0)
						{
							*rgDispId = (int)MethodsEnum::GetCollectionOfOutputFeatures;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("setOutputFeatureParametr")) == 0)
						{
							*rgDispId = (int)MethodsEnum::SetOutputFeatureParametr;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("setOpacity")) == 0)
						{
							*rgDispId = (int)MethodsEnum::SetOpacity;

							lresult = S_OK;
						}
						else if (_wcsicmp(*rgszNames, OLESTR("setSrcPosition")) == 0)
						{
							*rgDispId = (int)MethodsEnum::SetSrcPosition;

							lresult = S_OK;
						}

						

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
					_Out_opt_  VARIANT *pVarResult,
					/* [annotation][out] */
					_Out_opt_  EXCEPINFO *pExcepInfo,
					/* [annotation][out] */
					_Out_opt_  UINT *puArgErr) {

					HRESULT lresult = E_NOTIMPL;

					do
					{

						if (lcid != 0 && lcid != 2048)
						{
							lresult = DISP_E_UNKNOWNLCID;

							break;
						}

						LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

						LOG_CHECK_PTR_MEMORY(pVarResult);

						switch (dispIdMember)
						{
						case (int)MethodsEnum::SetPosition:
						{
							lresult = invokeSetPosition(
								pDispParams,
								pVarResult);
						}
						break;
						case (int)MethodsEnum::SetZOrder:
						{
							lresult = invokeSetZOrder(
								pDispParams,
								pVarResult);
						}
						break;
						case (int)MethodsEnum::GetPosition:
						{
							lresult = invokeGetPosition(
								pDispParams,
								pVarResult);
						}
						break;
						case (int)MethodsEnum::GetZOrder:
						{
							lresult = invokeGetZOrder(
								pDispParams,
								pVarResult);
						}
						break;
						case (int)MethodsEnum::Flush:
						{
							lresult = invokeFlush(
								pDispParams,
								pVarResult);
						}
						break;
						case (int)MethodsEnum::SetOpacity:
						{
							lresult = invokeSetOpacity(
								pDispParams,
								pVarResult);
						}
						break;
						case (int)MethodsEnum::SetSrcPosition:
						{
							lresult = invokeSetSrcPosition(
								pDispParams,
								pVarResult);
						}
						break;
						default:
							break;
						}

						
					} while (false);

					return lresult;
				}



				HRESULT STDMETHODCALLTYPE invokeSetPosition(
					_In_  DISPPARAMS *pDispParams,
					/* [annotation][out] */
					_Out_opt_  VARIANT *pVarResult)
				{

					HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

					do
					{


						LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

						LOG_CHECK_PTR_MEMORY(pVarResult);

						LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 4, DISP_E_BADPARAMCOUNT);

						

						FLOAT lLeft = 0.0f;
						FLOAT lRight = 1.0f;
						FLOAT lTop = 0.0f;
						FLOAT lBottom = 1.0f;

						VARIANT lFifthArg = pDispParams->rgvarg[0];

						VARIANT lFouthArg = pDispParams->rgvarg[1];

						VARIANT lThirdArg = pDispParams->rgvarg[2];

						VARIANT lSecondArg = pDispParams->rgvarg[3];

						

						if (lSecondArg.vt == VT_R4)
						{
							lLeft = lSecondArg.fltVal;
						}

						if (lThirdArg.vt == VT_R4)
						{
							lRight = lThirdArg.fltVal;
						}

						if (lFouthArg.vt == VT_R4)
						{
							lTop = lFouthArg.fltVal;
						}

						if (lFifthArg.vt == VT_R4)
						{
							lBottom = lFifthArg.fltVal;
						}

						LOG_INVOKE_FUNCTION(setPosition,
							lLeft,
							lRight,
							lTop,
							lBottom);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE invokeSetSrcPosition(
					_In_  DISPPARAMS *pDispParams,
					/* [annotation][out] */
					_Out_opt_  VARIANT *pVarResult)
				{

					HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

					do
					{


						LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

						LOG_CHECK_PTR_MEMORY(pVarResult);

						LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 4, DISP_E_BADPARAMCOUNT);



						FLOAT lLeft = 0.0f;
						FLOAT lRight = 1.0f;
						FLOAT lTop = 0.0f;
						FLOAT lBottom = 1.0f;

						VARIANT lFifthArg = pDispParams->rgvarg[0];

						VARIANT lFouthArg = pDispParams->rgvarg[1];

						VARIANT lThirdArg = pDispParams->rgvarg[2];

						VARIANT lSecondArg = pDispParams->rgvarg[3];



						if (lSecondArg.vt == VT_R4)
						{
							lLeft = lSecondArg.fltVal;
						}

						if (lThirdArg.vt == VT_R4)
						{
							lRight = lThirdArg.fltVal;
						}

						if (lFouthArg.vt == VT_R4)
						{
							lTop = lFouthArg.fltVal;
						}

						if (lFifthArg.vt == VT_R4)
						{
							lBottom = lFifthArg.fltVal;
						}

						LOG_INVOKE_FUNCTION(setSrcPosition,
							lLeft,
							lRight,
							lTop,
							lBottom);

					} while (false);

					return lresult;
				}
				

				HRESULT STDMETHODCALLTYPE invokeSetZOrder(
					_In_  DISPPARAMS *pDispParams,
					/* [annotation][out] */
					_Out_opt_  VARIANT *pVarResult)
				{
					HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

					do
					{

						LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

						LOG_CHECK_PTR_MEMORY(pVarResult);

						LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);




						DWORD lZOrder = 0;

						VARIANT lSecondArg = pDispParams->rgvarg[0];
						



						if (lSecondArg.vt == VT_I4)
						{
							lZOrder = lSecondArg.intVal;
						}
						else if (lSecondArg.vt == VT_UI4)
						{
							lZOrder = lSecondArg.uintVal;
						}

						LOG_INVOKE_FUNCTION(setZOrder,
							lZOrder);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE invokeSetOpacity(
					_In_  DISPPARAMS *pDispParams,
					/* [annotation][out] */
					_Out_opt_  VARIANT *pVarResult)
				{
					HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

					do
					{

						LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

						LOG_CHECK_PTR_MEMORY(pVarResult);

						LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 1, DISP_E_BADPARAMCOUNT);




						FLOAT lOpacity = 0;

						VARIANT lSecondArg = pDispParams->rgvarg[0];




						if (lSecondArg.vt == VT_R4)
						{
							lOpacity = lSecondArg.fltVal;
						}
						else
						{
							lresult = DISP_E_BADVARTYPE;

							break;
						}

						LOG_INVOKE_FUNCTION(setOpacity,
							lOpacity);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE invokeGetPosition(
					_In_  DISPPARAMS *pDispParams,
					/* [annotation][out] */
					_Out_opt_  VARIANT *pVarResult)
				{
					HRESULT lresult(E_NOTIMPL);

					do
					{

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE invokeGetZOrder(
					_In_  DISPPARAMS *pDispParams,
					/* [annotation][out] */
					_Out_opt_  VARIANT *pVarResult)
				{
					HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

					do
					{

						LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

						LOG_CHECK_PTR_MEMORY(pVarResult);

						LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 0, DISP_E_BADPARAMCOUNT);
												
						DWORD lZOrder = 0;

						LOG_INVOKE_FUNCTION(getZOrder,
							&lZOrder);

						pVarResult->vt = VT_UI4;

						pVarResult->uintVal = lZOrder;

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE invokeFlush(
					_In_  DISPPARAMS *pDispParams,
					/* [annotation][out] */
					_Out_opt_  VARIANT *pVarResult)
				{
					HRESULT lresult(DISP_E_UNKNOWNINTERFACE);

					do
					{

						LOG_CHECK_STATE_DESCR(pDispParams == nullptr, DISP_E_PARAMNOTFOUND);

						LOG_CHECK_PTR_MEMORY(pVarResult);

						LOG_CHECK_STATE_DESCR(pDispParams->cArgs != 0, DISP_E_BADPARAMCOUNT);
						
						LOG_INVOKE_FUNCTION(flush);

					} while (false);

					return lresult;
				}


				// IEVRStreamControl methods


				HRESULT STDMETHODCALLTYPE setPosition(
					/* [in] */ FLOAT aLeft,
					/* [in] */ FLOAT aRight,
					/* [in] */ FLOAT aTop,
					/* [in] */ FLOAT aBottom) override
				{

					HRESULT lresult(E_NOTIMPL);

					do
					{
						CComPtrCustom<IMixerStreamPositionControl> lIMixerStreamPositionControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIMixerStreamPositionControl);

						LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

						LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, setPosition,
							mMixerStreamID,
							aLeft,
							aRight,
							aTop,
							aBottom);

						if (mUnkVideoRenderingClass)
						{
							CComPtrCustom<IPresenter> lPresenter;

							LOG_INVOKE_QUERY_INTERFACE_METHOD(mPresenter, &lPresenter);

							if (lPresenter)
								lPresenter->ProcessFrame(FALSE);
						}

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE setSrcPosition(
					/* [in] */ FLOAT aLeft,
					/* [in] */ FLOAT aRight,
					/* [in] */ FLOAT aTop,
					/* [in] */ FLOAT aBottom) override
				{

					HRESULT lresult(E_NOTIMPL);

					do
					{
						CComPtrCustom<IMixerStreamPositionControl> lIMixerStreamPositionControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIMixerStreamPositionControl);

						LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

						LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, setSrcPosition,
							mMixerStreamID,
							aLeft,
							aRight,
							aTop,
							aBottom);

					} while (false);

					return lresult;
				}
				
				HRESULT STDMETHODCALLTYPE getSrcPosition(
					/* [out] */ FLOAT *aPtrLeft,
					/* [out] */ FLOAT *aPtrRight,
					/* [out] */ FLOAT *aPtrTop,
					/* [out] */ FLOAT *aPtrBottom) override
				{

					HRESULT lresult(E_NOTIMPL);

					do
					{
						CComPtrCustom<IMixerStreamPositionControl> lIMixerStreamPositionControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIMixerStreamPositionControl);

						LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

						LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, getSrcPosition,
							mMixerStreamID,
							aPtrLeft,
							aPtrRight,
							aPtrTop,
							aPtrBottom);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE setOpacity(
					/* [in] */ FLOAT aOpacity) override
				{
					HRESULT lresult(E_FAIL);

					do
					{
						CComPtrCustom<IMixerStreamPositionControl> lIMixerStreamPositionControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIMixerStreamPositionControl);

						LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

						LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, setOpacity,
							mMixerStreamID,
							aOpacity);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE setZOrder(
					/* [out] */ DWORD aZOrder) override
				{
					HRESULT lresult(E_FAIL);

					do
					{
						CComPtrCustom<IMixerStreamPositionControl> lIMixerStreamPositionControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIMixerStreamPositionControl);

						LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

						LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, setZOrder,
							mMixerStreamID,
							aZOrder);		
						
						CComPtrCustom<IPresenter> lPresenter;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mPresenter, &lPresenter);

						if (lPresenter)
							lPresenter->ProcessFrame(FALSE);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE getPosition(
					/* [out] */ FLOAT *aPtrLeft,
					/* [out] */ FLOAT *aPtrRight,
					/* [out] */ FLOAT *aPtrTop,
					/* [out] */ FLOAT *aPtrBottom) override
				{
					HRESULT lresult(E_FAIL);

					do
					{
						CComPtrCustom<IMixerStreamPositionControl> lIMixerStreamPositionControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIMixerStreamPositionControl);

						LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

						LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, getPosition,
							mMixerStreamID,
							aPtrLeft,
							aPtrRight,
							aPtrTop,
							aPtrBottom);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE getOpacity(
					/* [out] */ FLOAT *aPtrOpacity) override
				{
					HRESULT lresult(E_FAIL);

					do
					{
						CComPtrCustom<IMixerStreamPositionControl> lIMixerStreamPositionControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIMixerStreamPositionControl);

						LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

						LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, getOpacity,
							mMixerStreamID,
							aPtrOpacity);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE getZOrder(
					/* [out] */ DWORD *aPtrZOrder) override
				{
					HRESULT lresult(E_FAIL);

					do
					{
						CComPtrCustom<IMixerStreamPositionControl> lIMixerStreamPositionControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIMixerStreamPositionControl);

						LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

						LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, getZOrder,
							mMixerStreamID,
							aPtrZOrder);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE flush() override
				{
					HRESULT lresult(E_FAIL);

					do
					{
						CComPtrCustom<IMixerStreamPositionControl> lIMixerStreamPositionControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIMixerStreamPositionControl);

						LOG_CHECK_PTR_MEMORY(lIMixerStreamPositionControl);

						LOG_INVOKE_POINTER_METHOD(lIMixerStreamPositionControl, flush,
							mMixerStreamID);

						CComPtrCustom<IPresenter> lPresenter;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mPresenter, &lPresenter);

						if (lPresenter)
							lPresenter->ProcessFrame(FALSE);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE getCollectionOfFilters(
					/* [out] */ BSTR *aPtrPtrXMLstring) override
				{
					HRESULT lresult(E_FAIL);

					do
					{
						CComPtrCustom<IStreamFilterControl> lIStreamFilterControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIStreamFilterControl);

						LOG_CHECK_PTR_MEMORY(lIStreamFilterControl);

						LOG_INVOKE_POINTER_METHOD(lIStreamFilterControl, getCollectionOfFilters,
							mMixerStreamID,
							aPtrPtrXMLstring);


					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE setFilterParametr(
					/* [in] */ DWORD aParametrIndex,
					/* [in] */ LONG aNewValue,
					/* [in] */ BOOL aIsEnabled) override
				{
					HRESULT lresult(E_FAIL);

					do
					{

						CComPtrCustom<IStreamFilterControl> lIStreamFilterControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIStreamFilterControl);

						LOG_CHECK_PTR_MEMORY(lIStreamFilterControl);

						LOG_INVOKE_POINTER_METHOD(lIStreamFilterControl, setFilterParametr,
							mMixerStreamID,
							aParametrIndex,
							aNewValue,
							aIsEnabled);

						CComPtrCustom<IPresenter> lPresenter;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mPresenter, &lPresenter);

						if (lPresenter)
							lPresenter->ProcessFrame(FALSE);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE getCollectionOfOutputFeatures(
					/* [out] */ BSTR *aPtrPtrXMLstring) override
				{
					HRESULT lresult(E_FAIL);

					do
					{
						CComPtrCustom<IStreamFilterControl> lIStreamFilterControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIStreamFilterControl);

						LOG_CHECK_PTR_MEMORY(lIStreamFilterControl);

						LOG_INVOKE_POINTER_METHOD(lIStreamFilterControl, getCollectionOfOutputFeatures,
							aPtrPtrXMLstring);

					} while (false);

					return lresult;
				}

				HRESULT STDMETHODCALLTYPE setOutputFeatureParametr(
					/* [in] */ DWORD aParametrIndex,
					/* [in] */ LONG aNewValue) override
				{
					HRESULT lresult(E_FAIL);

					do
					{
						CComPtrCustom<IStreamFilterControl> lIStreamFilterControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, &lIStreamFilterControl);

						LOG_CHECK_PTR_MEMORY(lIStreamFilterControl);

						LOG_INVOKE_POINTER_METHOD(lIStreamFilterControl, setOutputFeatureParametr,
							aParametrIndex,
							aNewValue);

						CComPtrCustom<IPresenter> lPresenter;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mPresenter, &lPresenter);

						if (lPresenter)
							lPresenter->ProcessFrame(FALSE);

					} while (false);

					return lresult;
				}
				
				
			private:

				CComPtrCustom<IPresenter> mPresenter;

				CComPtrCustom<IMFTransform> mMixer;

				//CComPtrCustom<IMFMediaSink> mMediaSink;

				//CComPtrCustom<IBaseFilter> mBaseFilter;

				CComPtrCustom<IUnknown> mUnkVideoRenderingClass;

				DWORD mMixerStreamID;
			};
		}
	}
}