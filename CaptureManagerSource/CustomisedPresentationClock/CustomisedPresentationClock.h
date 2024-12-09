#pragma once

#include <map>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "IPresentationClock.h"


namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			class CustomisedPresentationClock :
				public BaseUnknown<IMFPresentationClock, IMFClockStateSink, IPresentationClock>
			{
				CustomisedPresentationClock(IMFPresentationClock* a_InnerPresentationClock);
				virtual ~CustomisedPresentationClock();

				CComPtrCustom<IMFPresentationClock> m_InnerPresentationClock;

				std::map<IMFClockStateSink*, IMFClockStateSink*> m_IMFClockStateSinks;

				std::map<IMediaPipelineProcessorControl*, IMediaPipelineProcessorControl*> m_IMediaPipelineProcessorControls;


			protected:

				virtual bool findInterface(
					REFIID aRefIID,
					void** aPtrPtrVoidObject)
				{
					if (aRefIID == __uuidof(IMFPresentationClock))
					{
						return castInterfaces(
							aRefIID,
							aPtrPtrVoidObject,
							static_cast<IMFPresentationClock*>(this));
					}
					else if (aRefIID == __uuidof(IMFClock))
					{
						return castInterfaces(
							aRefIID,
							aPtrPtrVoidObject,
							static_cast<IMFClock*>(this));
					}
					else
					{
						return BaseUnknown::findInterface(
							aRefIID,
							aPtrPtrVoidObject);
					}
				}


			public:

				static HRESULT create(
					IMFPresentationClock* a_InnerPresentationClock,
					IMFPresentationClock** aPtrPtrIMFPresentationClock);


				// IMFPresentationClock interface

				virtual HRESULT STDMETHODCALLTYPE SetTimeSource(
					/* [in] */ __RPC__in_opt IMFPresentationTimeSource *pTimeSource)override;

				virtual HRESULT STDMETHODCALLTYPE GetTimeSource(
					/* [out] */ __RPC__deref_out_opt IMFPresentationTimeSource **ppTimeSource)override;

				virtual HRESULT STDMETHODCALLTYPE GetTime(
					/* [out] */ __RPC__out MFTIME *phnsClockTime)override;

				virtual HRESULT STDMETHODCALLTYPE AddClockStateSink(
					/* [in] */ __RPC__in_opt IMFClockStateSink *pStateSink)override;

				virtual HRESULT STDMETHODCALLTYPE RemoveClockStateSink(
					/* [in] */ __RPC__in_opt IMFClockStateSink *pStateSink)override;

				virtual HRESULT STDMETHODCALLTYPE Start(
					/* [in] */ LONGLONG llClockStartOffset)override;

				virtual HRESULT STDMETHODCALLTYPE Stop(void)override;

				virtual HRESULT STDMETHODCALLTYPE Pause(void)override;



				// IMFClock interface

				virtual HRESULT STDMETHODCALLTYPE GetClockCharacteristics(
					/* [out] */ __RPC__out DWORD *pdwCharacteristics)override;

				virtual HRESULT STDMETHODCALLTYPE GetCorrelatedTime(
					/* [in] */ DWORD dwReserved,
					/* [out] */ __RPC__out LONGLONG *pllClockTime,
					/* [out] */ __RPC__out MFTIME *phnsSystemTime)override;

				virtual HRESULT STDMETHODCALLTYPE GetContinuityKey(
					/* [out] */ __RPC__out DWORD *pdwContinuityKey)override;

				virtual HRESULT STDMETHODCALLTYPE GetState(
					/* [in] */ DWORD dwReserved,
					/* [out] */ __RPC__out MFCLOCK_STATE *peClockState)override;

				virtual HRESULT STDMETHODCALLTYPE GetProperties(
					/* [out] */ __RPC__out MFCLOCK_PROPERTIES *pClockProperties)override;


				// IMFClockStateSink interface

				virtual HRESULT STDMETHODCALLTYPE OnClockStart(
					/* [in] */ MFTIME hnsSystemTime,
					/* [in] */ LONGLONG llClockStartOffset)override;

				virtual HRESULT STDMETHODCALLTYPE OnClockStop(
					/* [in] */ MFTIME hnsSystemTime)override;

				virtual HRESULT STDMETHODCALLTYPE OnClockPause(
					/* [in] */ MFTIME hnsSystemTime)override;

				virtual HRESULT STDMETHODCALLTYPE OnClockRestart(
					/* [in] */ MFTIME hnsSystemTime)override;

				virtual HRESULT STDMETHODCALLTYPE OnClockSetRate(
					/* [in] */ MFTIME hnsSystemTime,
					/* [in] */ float flRate)override;

			// IPresentationClock interface
			
				virtual HRESULT addIMediaPipelineProcessorControl(
					IMediaPipelineProcessorControl* aPtrIMediaPipelineProcessorControl)override;

				virtual HRESULT removeIMediaPipelineProcessorControl(
					IMediaPipelineProcessorControl* aPtrIMediaPipelineProcessorControl)override;

				virtual HRESULT getPresentationClock(
					IMFPresentationClock** aPtrPtrIMFPresentationClock)override;
				
				virtual HRESULT shutdown() override;
			
			};
		}
	}
}
