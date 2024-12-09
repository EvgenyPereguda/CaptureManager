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

#include "WinmmManager.h"
#include "../LogPrintOut/LogPrintOut.h"

namespace CaptureManager
{
	namespace Core
	{
		namespace Winmm
		{

#ifndef CASTFUNCTION
#define CASTFUNCTION(Function){\
		auto lPtrFunc = GetProcAddress(aModule, #Function);\
 if (lPtrFunc != nullptr){\
auto lFunc = (CaptureManager::Core::Winmm::Function)(lPtrFunc);\
if (lFunc != nullptr)Function = lFunc;\
  }\
			}\

#endif

#ifndef BINDFUNCTION
#define BINDFUNCTION(Function)Function WinmmManager::Function = WinmmManager::stub##Function
#endif

			std::vector<HMODULE> WinmmManager::mModules;

			HRESULT WinmmManager::mResult = E_FAIL;

			BINDFUNCTION(timeKillEvent);

			BINDFUNCTION(timeGetTime);

			BINDFUNCTION(timeBeginPeriod);

			BINDFUNCTION(timeEndPeriod);

			BINDFUNCTION(timeSetEvent);

			BINDFUNCTION(timeGetDevCaps);

			
			


			WinmmManager::WinmmManager()
			{
			}


			WinmmManager::~WinmmManager()
			{
			}

			// Initialize Winmm support
			HRESULT WinmmManager::initialize()
			{
				HRESULT lresult = E_FAIL;

				do
				{
					lresult = loadLibraries();

					if (FAILED(lresult))
						break;

					lresult = fillPtrFuncCollection();

					if (FAILED(lresult))
						break;


				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(LogPrintOut::ERROR_LEVEL, L"WinmmManager: Winmm cannot be initialized!!!");
				}
				else
				{
					LogPrintOut::getInstance().printOutln(LogPrintOut::INFO_LEVEL, L"WinmmManager: Winmm is initialized!!!");
				}

				return mResult = lresult;
			}

			// load needed libraries dynamically;
			HRESULT WinmmManager::loadLibraries()
			{

				HRESULT lresult = E_FAIL;

				do
				{

					auto lModule = LoadLibrary(L"Winmm.dll");
					
					if (lModule == nullptr)
					{
						lresult = E_HANDLE;
					}

					mModules.push_back(lModule);

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			// init collection of pointers on Media Foundation functions
			HRESULT WinmmManager::initFunctions(HMODULE aModule)
			{

				HRESULT lresult = E_FAIL;

				do
				{
					CASTFUNCTION(timeKillEvent);

					CASTFUNCTION(timeGetTime);

					CASTFUNCTION(timeBeginPeriod);

					CASTFUNCTION(timeEndPeriod);

					CASTFUNCTION(timeSetEvent);

					CASTFUNCTION(timeGetDevCaps);					
					
					
					lresult = S_OK;

				} while (false);

				return lresult;
			}

			// fill collection of pointers on Media Foundation functions
			HRESULT WinmmManager::fillPtrFuncCollection()
			{
				HRESULT lresult = E_FAIL;

				do
				{
					for (auto& lModile : mModules)
					{
						initFunctions(lModile);
					}

					lresult = S_OK;

				} while (false);

				return lresult;
			}
		}
	}
}