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

#include "stdafx.h"
#include "CustomisedPresentationClock.h"
#include "../LogPrintOut/LogPrintOut.h"

namespace CaptureManager
{

	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			CustomisedPresentationClock::CustomisedPresentationClock(IMFPresentationClock* a_InnerPresentationClock)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(a_InnerPresentationClock);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(a_InnerPresentationClock, &m_InnerPresentationClock);

					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, AddClockStateSink, this);

				} while (false);
			}


			CustomisedPresentationClock::~CustomisedPresentationClock()
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, RemoveClockStateSink, this);

				} while (false);
			}

			HRESULT CustomisedPresentationClock::create(IMFPresentationClock* a_InnerPresentationClock, IMFPresentationClock** aPtrPtrIMFPresentationClock)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(a_InnerPresentationClock);

					CComPtrCustom<IMFPresentationClock> l_instance(new CustomisedPresentationClock(a_InnerPresentationClock));

					LOG_CHECK_PTR_MEMORY(l_instance);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(l_instance, aPtrPtrIMFPresentationClock);


				} while (false);

				return lresult;
			}




			// IMFPresentationClock interface

			HRESULT CustomisedPresentationClock::SetTimeSource(
				/* [in] */ __RPC__in_opt IMFPresentationTimeSource *pTimeSource)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, SetTimeSource, pTimeSource);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::GetTimeSource(
				/* [out] */ __RPC__deref_out_opt IMFPresentationTimeSource **ppTimeSource)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, GetTimeSource, ppTimeSource);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::GetTime(
				/* [out] */ __RPC__out MFTIME *phnsClockTime)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, GetTime, phnsClockTime);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::AddClockStateSink(
				/* [in] */ __RPC__in_opt IMFClockStateSink *pStateSink)
			{
				HRESULT lresult(E_FAIL);

				do
				{

					auto lfind = m_IMFClockStateSinks.find(pStateSink);

					if (lfind == m_IMFClockStateSinks.end())
						m_IMFClockStateSinks[pStateSink] = pStateSink;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::RemoveClockStateSink(
				/* [in] */ __RPC__in_opt IMFClockStateSink *pStateSink)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					auto lfind = m_IMFClockStateSinks.find(pStateSink);

					if (lfind != m_IMFClockStateSinks.end())
						m_IMFClockStateSinks.erase(lfind);

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::Start(
				/* [in] */ LONGLONG llClockStartOffset)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, Start, llClockStartOffset);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::Stop(void)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, Stop);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::Pause(void)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, Pause);

				} while (false);

				return lresult;
			}



			// IMFClock interface

			HRESULT CustomisedPresentationClock::GetClockCharacteristics(
				/* [out] */ __RPC__out DWORD *pdwCharacteristics)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, GetClockCharacteristics, pdwCharacteristics);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::GetCorrelatedTime(
				/* [in] */ DWORD dwReserved,
				/* [out] */ __RPC__out LONGLONG *pllClockTime,
				/* [out] */ __RPC__out MFTIME *phnsSystemTime)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, GetCorrelatedTime,
						dwReserved,
						pllClockTime,
						phnsSystemTime);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::GetContinuityKey(
				/* [out] */ __RPC__out DWORD *pdwContinuityKey)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, GetContinuityKey,
						pdwContinuityKey);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::GetState(
				/* [in] */ DWORD dwReserved,
				/* [out] */ __RPC__out MFCLOCK_STATE *peClockState)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, GetState,
						dwReserved,
						peClockState);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::GetProperties(
				/* [out] */ __RPC__out MFCLOCK_PROPERTIES *pClockProperties)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_POINTER_METHOD(m_InnerPresentationClock, GetProperties,
						pClockProperties);

				} while (false);

				return lresult;
			}


			// IMFClockStateSink interface

			HRESULT CustomisedPresentationClock::OnClockStart(
				/* [in] */ MFTIME hnsSystemTime,
				/* [in] */ LONGLONG llClockStartOffset)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					for (auto& l_item : m_IMFClockStateSinks)
					{
						lresult = l_item.second->OnClockStart(hnsSystemTime, llClockStartOffset);
					}

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::OnClockStop(
				/* [in] */ MFTIME hnsSystemTime)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					for (auto& l_item : m_IMFClockStateSinks)
					{
						lresult = l_item.second->OnClockStop(hnsSystemTime);
					}

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::OnClockPause(
				/* [in] */ MFTIME hnsSystemTime)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					for (auto& l_item : m_IMediaPipelineProcessorControls)
					{
						lresult = l_item.second->pause();
					}

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::OnClockRestart(
				/* [in] */ MFTIME hnsSystemTime)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					for (auto& l_item : m_IMediaPipelineProcessorControls)
					{
						lresult = l_item.second->restart();
					}

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::OnClockSetRate(
				/* [in] */ MFTIME hnsSystemTime,
				/* [in] */ float flRate)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					for (auto& l_item : m_IMFClockStateSinks)
					{
						lresult = l_item.second->OnClockSetRate(hnsSystemTime, flRate);
					}

				} while (false);

				return lresult;
			}

			// IPresentationClock interface

			HRESULT CustomisedPresentationClock::addIMediaPipelineProcessorControl(
				IMediaPipelineProcessorControl* a_IMediaPipelineProcessorControl)
			{
				HRESULT lresult(E_FAIL);

				do
				{

					auto lfind = m_IMediaPipelineProcessorControls.find(a_IMediaPipelineProcessorControl);

					if (lfind == m_IMediaPipelineProcessorControls.end())
						m_IMediaPipelineProcessorControls[a_IMediaPipelineProcessorControl] = a_IMediaPipelineProcessorControl;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::removeIMediaPipelineProcessorControl(
				IMediaPipelineProcessorControl* a_IMediaPipelineProcessorControl)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					auto lfind = m_IMediaPipelineProcessorControls.find(a_IMediaPipelineProcessorControl);

					if (lfind != m_IMediaPipelineProcessorControls.end())
						m_IMediaPipelineProcessorControls.erase(lfind);

					lresult = S_OK;


				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::getPresentationClock(
				IMFPresentationClock** aPtrPtrIMFPresentationClock)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrIMFPresentationClock);

					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(m_InnerPresentationClock, aPtrPtrIMFPresentationClock);

				} while (false);

				return lresult;
			}

			HRESULT CustomisedPresentationClock::shutdown()
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(m_InnerPresentationClock);

					m_InnerPresentationClock->RemoveClockStateSink(this);

				} while (false);

				return lresult;
			}
		}
	}
}