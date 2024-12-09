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

#include "DXGIManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/MFHeaders.h"

namespace CaptureManager
{
	namespace Core
	{
		namespace DXGI
		{

#ifndef CASTFUNCTION
#define CASTFUNCTION(Function){\
		auto lPtrFunc = GetProcAddress(aModule, #Function);\
 if (lPtrFunc != nullptr){\
auto lFunc = (CaptureManager::Core::DXGI::Function)(lPtrFunc);\
if (lFunc != nullptr)\
	{\
			Function = lFunc;\
			lresult = S_OK;\
}\
       }\
		  	  	  else\
					{\
			lresult = E_FAIL;\
			break;\
		}\
									}\

#endif



#ifndef BINDFUNCTION
#define BINDFUNCTION(Function)Function DXGIManager::Function = DXGIManager::stub##Function
#endif

			BINDFUNCTION(CreateDXGIFactory1);

			DXGIManager::DXGIManager()
			{
				HRESULT lresult = E_FAIL;

				do
				{

					auto lModule = LoadLibrary(L"Dxgi.dll");

					if (lModule == nullptr)
					{
						lresult = E_HANDLE;

						break;
					}

					lresult = initFunctions(lModule);

				} while (false);

				mState = lresult;
			}


			DXGIManager::~DXGIManager()
			{
			}


			HRESULT DXGIManager::getState()
			{
				return mState;
			}

			HRESULT DXGIManager::GetResource(
				IMFDXGIBuffer* aPtrIMFDXGIBuffer,
				/* [annotation][in] */
				REFIID riid,
				/* [annotation][out] */
				LPVOID *ppvObject)
			{

				HRESULT lresult = E_FAIL;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrIMFDXGIBuffer);

					LOG_CHECK_PTR_MEMORY(ppvObject);

					lresult = aPtrIMFDXGIBuffer->GetResource(
						riid,
						ppvObject);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

				return lresult;
			}

			// init collection of pointers on Direct3D11 functions
			HRESULT DXGIManager::initFunctions(HMODULE aModule)
			{

				HRESULT lresult = E_FAIL;

				do
				{
					CASTFUNCTION(CreateDXGIFactory1);
					
				} while (false);

				return lresult;
			}
		}
	}
}