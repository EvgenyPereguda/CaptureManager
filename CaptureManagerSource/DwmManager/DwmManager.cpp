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

#include "DwmManager.h"
#include "DwmManager.h"
#include "../Common/Common.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include <dwmapi.h>


namespace CaptureManager
{
	namespace Core
	{
		namespace Dwm
		{

			HMODULE g_hDwmApiDLL = NULL;
			PVOID g_pfnDwmIsCompositionEnabled = NULL;
			PVOID g_pfnDwmGetCompositionTimingInfo = NULL;
			PVOID g_pfnDwmSetPresentParameters = NULL;

			BOOL g_bDwmQueuing = FALSE;
			const UINT DWM_BUFFER_COUNT = 4;

			//
			// Type definitions.
			//

			typedef HRESULT(WINAPI * PFNDWMISCOMPOSITIONENABLED)(
				__out BOOL* pfEnabled
				);

			typedef HRESULT(WINAPI * PFNDWMGETCOMPOSITIONTIMINGINFO)(
				__in HWND hwnd,
				__out DWM_TIMING_INFO* pTimingInfo
				);

			typedef HRESULT(WINAPI * PFNDWMSETPRESENTPARAMETERS)(
				__in HWND hwnd,
				__inout DWM_PRESENT_PARAMETERS* pPresentParams
				);
			
			BOOL DwmManager::EnableDwmQueuing(HWND a_Hwnd)
			{
				HRESULT hr;

				//
				// DWM is not available.
				//
				if (!g_hDwmApiDLL)
				{
					return TRUE;
				}

				//
				// Check to see if DWM is currently enabled.
				//
				BOOL bDWM = FALSE;

				hr = ((PFNDWMISCOMPOSITIONENABLED)g_pfnDwmIsCompositionEnabled)(&bDWM);

				if (FAILED(hr))
				{
					return FALSE;
				}

				//
				// DWM queuing is disabled when DWM is disabled.
				//
				if (!bDWM)
				{
					g_bDwmQueuing = FALSE;
					return TRUE;
				}

				//
				// DWM queuing is enabled already.
				//
				if (g_bDwmQueuing)
				{
					return TRUE;
				}

				//
				// Retrieve DWM refresh count of the last vsync.
				//
				DWM_TIMING_INFO dwmti = { 0 };

				dwmti.cbSize = sizeof(dwmti);

				hr = ((PFNDWMGETCOMPOSITIONTIMINGINFO)g_pfnDwmGetCompositionTimingInfo)(NULL, &dwmti);

				if (FAILED(hr))
				{
					return FALSE;
				}

				//
				// Enable DWM queuing from the next refresh.
				//
				DWM_PRESENT_PARAMETERS dwmpp = { 0 };

				dwmpp.cbSize = sizeof(dwmpp);
				dwmpp.fQueue = TRUE;
				dwmpp.cRefreshStart = dwmti.cRefresh + 1;
				dwmpp.cBuffer = DWM_BUFFER_COUNT;
				dwmpp.fUseSourceRate = FALSE;
				dwmpp.cRefreshesPerFrame = 1;
				dwmpp.eSampling = DWM_SOURCE_FRAME_SAMPLING_POINT;

				hr = ((PFNDWMSETPRESENTPARAMETERS)g_pfnDwmSetPresentParameters)(a_Hwnd, &dwmpp);

				if (FAILED(hr))
				{
					return FALSE;
				}

				//
				// DWM queuing is enabled.
				//
				g_bDwmQueuing = TRUE;

				return TRUE;
			}

			DwmManager::DwmManager(){ initialize(); }
			DwmManager::~DwmManager(){}

			void DwmManager::initialize()
			{
				HRESULT lresult(E_FAIL);

				do
				{

					g_hDwmApiDLL = LoadLibrary(TEXT("dwmapi.dll"));

					LOG_CHECK_PTR_MEMORY(g_hDwmApiDLL);

					g_pfnDwmIsCompositionEnabled = GetProcAddress(g_hDwmApiDLL, "DwmIsCompositionEnabled");

					LOG_CHECK_PTR_MEMORY(g_pfnDwmIsCompositionEnabled);

					g_pfnDwmGetCompositionTimingInfo = GetProcAddress(g_hDwmApiDLL, "DwmGetCompositionTimingInfo");

					LOG_CHECK_PTR_MEMORY(g_pfnDwmGetCompositionTimingInfo);

					g_pfnDwmSetPresentParameters = GetProcAddress(g_hDwmApiDLL, "DwmSetPresentParameters");

					LOG_CHECK_PTR_MEMORY(g_pfnDwmSetPresentParameters);

				} while (false);
			}
		}
	}
}