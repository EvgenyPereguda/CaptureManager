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

#include "Direct3D11Manager.h"

namespace CaptureManager
{
	namespace Core
	{
		namespace Direct3D11
		{

#ifndef CASTFUNCTION
#define CASTFUNCTION(Function){\
		auto lPtrFunc = GetProcAddress(aModule, #Function);\
 if (lPtrFunc != nullptr){\
auto lFunc = (CaptureManager::Core::Direct3D11::Function)(lPtrFunc);\
if (lFunc != nullptr)Function = lFunc;\
     }\
							}\

#endif



#ifndef BINDFUNCTION
#define BINDFUNCTION(Function)Function Direct3D11Manager::Function = Direct3D11Manager::stub##Function
#endif


			BINDFUNCTION(D3D11CreateDevice);

			Direct3D11Manager::Direct3D11Manager()
			{
				HRESULT lresult = E_FAIL;

				do
				{

					auto lModule = LoadLibrary(L"d3d11.dll");

					if (lModule == nullptr)
					{
						lresult = E_HANDLE;

						break;
					}

					lresult = initFunctions(lModule);

					//lModule = LoadLibrary(L"Dxva2.dll");

					//if (lModule == nullptr)
					//{
					//	lresult = E_HANDLE;

					//	break;
					//}

					//lresult = initFunctions(lModule);

				} while (false);

				mState = lresult;
			}

			Direct3D11Manager::~Direct3D11Manager()
			{
			}

			HRESULT Direct3D11Manager::getState()
			{
				return mState;
			}
			
			// init collection of pointers on Direct3D11 functions
			HRESULT Direct3D11Manager::initFunctions(HMODULE aModule)
			{

				HRESULT lresult = E_FAIL;

				do
				{
					CASTFUNCTION(D3D11CreateDevice);

					lresult = S_OK;

				} while (false);

				return lresult;
			}
		}
	}
}