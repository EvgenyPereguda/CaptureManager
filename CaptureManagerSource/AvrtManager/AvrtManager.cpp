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

#include "AvrtManager.h"



namespace CaptureManager
{
	namespace Core
	{


#ifndef CASTFUNCTION
#define CASTFUNCTION(Function){\
		auto lPtrFunc = GetProcAddress(aModule, #Function);\
 if (lPtrFunc != nullptr){\
auto lFunc = (CaptureManager::Core::Function)(lPtrFunc);\
if (lFunc != nullptr)Function = lFunc;\
   }\
				}\

#endif

#ifndef BINDFUNCTION
#define BINDFUNCTION(Function)Function AvrtManager::Function = AvrtManager::stub##Function
#endif

		std::vector<HMODULE> AvrtManager::mModules;

		HRESULT AvrtManager::mResult = E_FAIL;

		BINDFUNCTION(AvSetMmThreadCharacteristicsW);

		BINDFUNCTION(AvRevertMmThreadCharacteristics);

		BINDFUNCTION(AvSetMmThreadPriority);

		

		AvrtManager::AvrtManager()			
		{
		}

		AvrtManager::~AvrtManager()
		{
		}

		// Initialize Avrt support
		HRESULT AvrtManager::initialize()
		{
			HRESULT lresult = mResult;

			do
			{
				if (SUCCEEDED(lresult))
					break;

				lresult = loadLibraries();

				if (FAILED(lresult))
					break;

				lresult = fillPtrFuncCollection();

				if (FAILED(lresult))
					break;


			} while (false);
			
			return mResult = lresult;
		}

		// load needed libraries dynamically;
		HRESULT AvrtManager::loadLibraries()
		{

			HRESULT lresult = E_FAIL;

			do
			{

				auto lModule = LoadLibrary(L"Avrt.dll");
				
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
		HRESULT AvrtManager::initFunctions(HMODULE aModule)
		{

			HRESULT lresult = E_FAIL;

			do
			{
				CASTFUNCTION(AvSetMmThreadCharacteristicsW);

				CASTFUNCTION(AvRevertMmThreadCharacteristics);

				CASTFUNCTION(AvSetMmThreadPriority);

				lresult = S_OK;
				
			} while (false);

			return lresult;
		}

		// fill collection of pointers on Media Foundation functions
		HRESULT AvrtManager::fillPtrFuncCollection()
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