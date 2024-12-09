#pragma once

#include <mutex>
#include <memory>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "IStreamSink.h"
#include "IScheduler.h"
#include  "SchedulerFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "IPresenter.h"
#include "IEVRStreamControl.h"
#include "IMixerStreamPositionControl.h"
#include "IStreamFilterControl.h"


namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			using namespace CaptureManager;

			template<class T>
			class Scheduler :
				public ISchedulerCallback
			{
			private:

				std::unique_ptr<IScheduler> mIScheduler;

			public:
				typedef void(T::*InvokeFn)();

				Scheduler(T *pParent, InvokeFn fn) : m_pParent(pParent), m_pInvokeFn(fn)
				{
				}

				HRESULT init(INT64 aFrameDuration100nseconds)
				{
					HRESULT lresult;

					do
					{
						IScheduler* lIScheduler;

						LOG_INVOKE_FUNCTION(SchedulerFactory::createScheduler,
							this,
							aFrameDuration100nseconds,
							&lIScheduler);

						LOG_CHECK_PTR_MEMORY(lIScheduler);

						mIScheduler.reset(lIScheduler);

					} while (false);

					return lresult;
				}

				HRESULT start()
				{
					HRESULT lresult;

					do
					{
						LOG_CHECK_PTR_MEMORY(mIScheduler.get());

						LOG_INVOKE_POINTER_METHOD(mIScheduler, start);

					} while (false);

					return lresult;
				}

				HRESULT stop()
				{
					HRESULT lresult;

					do
					{
						LOG_CHECK_PTR_MEMORY(mIScheduler.get());

						LOG_INVOKE_POINTER_METHOD(mIScheduler, stop);

					} while (false);

					return lresult;
				}

				virtual void callback()
				{
					(m_pParent->*m_pInvokeFn)();
				}

				T *m_pParent;
				InvokeFn m_pInvokeFn;
			};



			class EVRStreamSink:
				public BaseUnknown<
				IMFStreamSink,
				IStreamSink,
				IMFMediaTypeHandler,
				IMFGetService,
				IMFAsyncCallback,
				IMFClockStateSink,
				IEVRStreamControl
				>
			{

				enum StreamEnum
				{
					Uninitialized,
					Ready,
					Started,
					Paused,
					Stoped
				};

			public:
				static HRESULT createEVRStreamSink(
					DWORD aStreamID,
					IMFMediaSink* aPtrMediaSink,
					IPresenter* aPtrPresenter,
					DWORD aMixerStreamID,
					IStreamSink** aPtrPtrStreamSink);

				HRESULT initialize(
					IMFMediaSink* aPtrMediaSink,
					IPresenter* aPtrPresenter);



				// IMFClockStateSink methods
				virtual HRESULT STDMETHODCALLTYPE OnClockStart(
					MFTIME aHNSSystemTime,
					LONGLONG aClockStartOffset);
				virtual HRESULT STDMETHODCALLTYPE OnClockStop(
					MFTIME aHNSSystemTime);
				virtual HRESULT STDMETHODCALLTYPE OnClockPause(
					MFTIME aHNSSystemTime);
				virtual HRESULT STDMETHODCALLTYPE OnClockRestart(
					MFTIME aHNSSystemTime);
				virtual HRESULT STDMETHODCALLTYPE OnClockSetRate(
					MFTIME aHNSSystemTime,
					float aRate);



				// IMFMediaTypeHandler implementation
				virtual HRESULT STDMETHODCALLTYPE IsMediaTypeSupported(
					IMFMediaType* aPtrMediaType,
					IMFMediaType** aPtrPtrMediaType);
				virtual HRESULT STDMETHODCALLTYPE GetMediaTypeCount(
					DWORD* aPtrTypeCount);
				virtual HRESULT STDMETHODCALLTYPE GetMediaTypeByIndex(
					DWORD aIndex,
					IMFMediaType** aPtrPtrType);
				virtual HRESULT STDMETHODCALLTYPE SetCurrentMediaType(
					IMFMediaType* aPtrMediaType);
				virtual HRESULT STDMETHODCALLTYPE GetCurrentMediaType(
					IMFMediaType** aPtrPtrMediaType);
				virtual HRESULT STDMETHODCALLTYPE GetMajorType(
					GUID* PtrGUIDMajorType);



				// IMFStreamSink implementation
				virtual HRESULT STDMETHODCALLTYPE Flush();
				virtual HRESULT STDMETHODCALLTYPE GetIdentifier(
					DWORD* aPtrIdentifier);
				virtual HRESULT STDMETHODCALLTYPE GetMediaSink(
					IMFMediaSink** aPtrPtrMediaSink);
				virtual HRESULT STDMETHODCALLTYPE GetMediaTypeHandler(
					IMFMediaTypeHandler** aPtrPtrHandler);
				virtual HRESULT STDMETHODCALLTYPE PlaceMarker(
					MFSTREAMSINK_MARKER_TYPE aMarkerType, 
					const PROPVARIANT* aPtrVarMarkerValue, 
					const PROPVARIANT* aPtrVarContextValue);
				virtual HRESULT STDMETHODCALLTYPE ProcessSample(
					IMFSample* aPtrSample);


				// IMFMediaEventGenerator implementation
				virtual HRESULT STDMETHODCALLTYPE BeginGetEvent(
					IMFAsyncCallback* aPtrCallback, 
					IUnknown* aPtrUnkState);
				virtual HRESULT STDMETHODCALLTYPE EndGetEvent(
					IMFAsyncResult* aPtrResult, 
					IMFMediaEvent** aPtrPtrEvent);
				virtual HRESULT STDMETHODCALLTYPE GetEvent(
					DWORD aFlags, 
					IMFMediaEvent** aPtrPtrEvent);
				virtual HRESULT STDMETHODCALLTYPE QueueEvent(
					MediaEventType aMediaEventType,
					REFGUID aRefGUIDExtendedType,
					HRESULT aHRStatus, 
					const PROPVARIANT* aPtrValue);


				// IStreamSink implementation
				virtual HRESULT STDMETHODCALLTYPE getMaxRate(
					BOOL aThin, 
					float* aPtrRate);
				virtual HRESULT STDMETHODCALLTYPE preroll();
				virtual HRESULT STDMETHODCALLTYPE shutdown();


				// IMFGetService
				virtual STDMETHODIMP GetService(REFGUID guidService, REFIID riid, LPVOID* ppvObject) override;


				// IMFAsyncCallback methods
				virtual HRESULT STDMETHODCALLTYPE GetParameters(
					DWORD* aPtrFlags, 
					DWORD* aPtrQueue);
				virtual HRESULT STDMETHODCALLTYPE Invoke(
					IMFAsyncResult* aPtrAsyncResult);


				// IDispatch
				virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
					/* [out] */ __RPC__out UINT *pctinfo) {
					return E_NOTIMPL;
				}

				virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(
					/* [in] */ UINT iTInfo,
					/* [in] */ LCID lcid,
					/* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo) {
					return E_NOTIMPL;
				}

				virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
					/* [in] */ __RPC__in REFIID riid,
					/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
					/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
					/* [in] */ LCID lcid,
					/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId) {
					return E_NOTIMPL;
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
					return E_NOTIMPL;
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


						CComPtrCustom<IPresenter> lPresenter;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mPresenter, &lPresenter);

						if (lPresenter)
							lPresenter->ProcessFrame(FALSE);

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

			protected:
				
				virtual bool findInterface(
					REFIID aRefIID,
					void** aPtrPtrVoidObject)
				{
					auto lresult = castInterfaces(
						aRefIID,
						aPtrPtrVoidObject,
						static_cast<IMFMediaEventGenerator*>(this));

					if (!lresult)
					{
						return BaseUnknown::findInterface(
							aRefIID,
							aPtrPtrVoidObject);
					}

					return lresult;
				}

			private:


				const DWORD mStreamID;
				const DWORD mMixerStreamID;
				DWORD mWorkQueueId;
				bool mIsShutdown;

				CComPtrCustom<IMFMediaSink> mMediaSink;
				CComPtrCustom<IMFMediaEventQueue> mEventQueue;  
				CComPtrCustom<IPresenter> mPresenter;
				CComPtrCustom<IMFMediaType> mCurrentType;

				std::unique_ptr<Scheduler<EVRStreamSink>> mScheduler;
				
				CComPtrCustom<IMFTransform> mMixer;

				std::mutex mAccessMutex;
				std::mutex mClockStateAccessMutex;
				std::mutex mProcessMutex;


				BOOL m_fPrerolling;
				BOOL m_fWaitingForOnClockStart;


				StreamEnum mStreamState;
				INT64 mVideoFrameDuration;

				LONGLONG mPrevTimeStamp;

				LONGLONG mDeltaTimeDuration;

				BOOL mNewSample;

				MFTIME mTimeStamp;

				MFTIME mSampleDuration;


				EVRStreamSink(
					DWORD aStreamID,
					DWORD aMixerStreamID);
				virtual ~EVRStreamSink();
				HRESULT checkShutdown() const;
				HRESULT createVideoAllocator(
					IMFVideoSampleAllocator** aPtrPtrVideoSampleAllocator);
				void update();

				HRESULT checkTimeStamp(MFTIME aTimeStamp, MFTIME aSampleDuration);

				HRESULT createMediaType(
					IMFMediaType* aPtrUpStreamMediaType,
					GUID aMFVideoFormat,
					IMFMediaType** aPtrPtrMediaSinkMediaType);
			};
		}
	}
}