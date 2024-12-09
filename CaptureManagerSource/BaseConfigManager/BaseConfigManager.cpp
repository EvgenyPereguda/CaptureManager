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

#include "BaseConfigManager.h"
struct IUnknown;
#include <Unknwn.h>

#include "../Common/MFHeaders.h"
#include "../DirectXManager/DXGIManager.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/Singleton.h"
#include <dxgi1_3.h>



namespace CaptureManager
{
		template <typename T>
		class CComPtrCustom
		{
		public:

			CComPtrCustom(T *aPtrElement)
				:element(aPtrElement)
			{
			}

			CComPtrCustom()
				:element(nullptr)
			{
			}

			virtual ~CComPtrCustom()
			{
				Release();
			}

			T* Detach()
			{
				auto lOutPtr = element;

				element = nullptr;

				return lOutPtr;
			}

			T* detach()
			{
				return Detach();
			}

			void Release()
			{
				if (element == nullptr)
					return;

				auto k = element->Release();

				element = nullptr;
			}

			CComPtrCustom& operator = (T *pElement)
			{
				Release();

				if (pElement == nullptr)
					return *this;

				auto k = pElement->AddRef();

				element = pElement;

				return *this;
			}

			void Swap(CComPtrCustom& other)
			{
				T* pTemp = element;
				element = other.element;
				other.element = pTemp;
			}

			T* operator->()
			{
				return element;
			}

			operator T*()
			{
				return element;
			}

			operator T*() const
			{
				return element;
			}


			T* get()
			{
				return element;
			}

			T* get() const
			{
				return element;
			}

			T** operator &()
			{
				return &element;
			}

			bool operator !()const
			{
				return element == nullptr;
			}

			operator bool()const
			{
				return element != nullptr;
			}

			bool operator == (const T *pElement)const
			{
				return element == pElement;
			}

			bool operator == (const CComPtrCustom& aCComPtrCustom)const
			{
				return element == aCComPtrCustom.get();
			}


			CComPtrCustom(const CComPtrCustom& aCComPtrCustom)
			{
				if (aCComPtrCustom.operator!())
				{
					element = nullptr;

					return;
				}

				element = aCComPtrCustom;

				auto h = element->AddRef();

				h++;
			}

			CComPtrCustom& operator = (const CComPtrCustom& aCComPtrCustom)
			{
				Release();

				element = aCComPtrCustom;

				LONG k = 0;

				if (element != nullptr)
					k = element->AddRef();

				return *this;
			}

			_Check_return_ HRESULT CopyTo(T** ppT) throw()
			{
				if (ppT == NULL)
					return E_POINTER;

				*ppT = element;

				if (element)
					element->AddRef();

				return S_OK;
			}

			HRESULT CoCreateInstance(const CLSID aCLSID)
			{
				T* lPtrTemp;

				auto lresult = ::CoCreateInstance(aCLSID, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&lPtrTemp));

				if (SUCCEEDED(lresult))
				{
					if (lPtrTemp != nullptr)
					{
						Release();

						element = lPtrTemp;
					}

				}

				return lresult;
			}

		protected:

			T* element;
		};

		template <class T, const IID* piid = &__uuidof(T)>
		class CComQIPtrCustom :
			public CComPtrCustom<T>
		{
		public:
			CComQIPtrCustom() throw()
			{
			}
			CComQIPtrCustom(decltype(__nullptr)) throw()
			{
			}
			CComQIPtrCustom(_Inout_opt_ T* lp) throw() :
				CComPtrCustom<T>(lp)
			{
			}

			CComQIPtrCustom(_Inout_opt_ IUnknown* lp) throw()
			{
				if (lp != NULL)
				{
					if (FAILED(lp->QueryInterface(*piid, (void **)&T::element)))
						T::element = NULL;
				}
			}
			T* operator=(decltype(__nullptr)) throw()
			{
				CComQIPtrCustom(nullptr).Swap(*this);
				return nullptr;
			}
			T* operator=(_Inout_opt_ T* lp) throw()
			{
				if (*this != lp)
				{
					CComQIPtrCustom(lp).Swap(*this);
				}
				return *this;
			}

			T* operator=(_Inout_opt_ IUnknown* lp) throw()
			{
				if (*this != lp)
				{
					if (FAILED(lp->QueryInterface(*piid, (void **)&T::element)))
						T::element = NULL;
					else
						return T::element;
				}
				return *this;
			}
		};

		template<>
		class CComQIPtrCustom<IUnknown, &IID_IUnknown> :
			public CComPtrCustom<IUnknown>
		{
		public:
			CComQIPtrCustom() throw()
			{
			}
			CComQIPtrCustom(_Inout_opt_ IUnknown* lp) throw()
			{
				if (lp != NULL)
				{
					if (FAILED(lp->QueryInterface(__uuidof(IUnknown), (void **)&element)))
						element = NULL;
				}
			}
			CComQIPtrCustom(_Inout_ const CComQIPtrCustom<IUnknown, &IID_IUnknown>& lp) throw()
			{
				this->element = lp.get();
			}

			IUnknown* operator=(_Inout_ const CComQIPtrCustom<IUnknown, &IID_IUnknown>& lp) throw()
			{
				if (this->get() != lp.get())
				{
					this->element = lp.get();

				}
				return *this;
			}
		};

	
	namespace Core
	{
		using namespace Core::DXGI;
		using namespace Core::Direct3D11;

		BaseConfigManager::BaseConfigManager():
			m_IsWindows8_Or_Greater(false),
			m_IsWindows8Point1_Or_Greater(false),
			m_IsWindows10_Or_Greater(false),
			m_IsWindows10_With_VP_Or_Greater(false)
		{
			m_IsWindows8_Or_Greater = isWindows8_Or_Greater_Inner();

			m_IsWindows8Point1_Or_Greater = isWindows8Point1_Or_Greater_Inner();

			m_IsWindows10_Or_Greater = isWindows10_Or_Greater_Inner();

			m_IsWindows10_With_VP_Or_Greater = isWindows10_With_VP_Or_Greater_Inner();
		}
		
		BaseConfigManager::~BaseConfigManager()
		{
		}

		bool BaseConfigManager::isWindows8_Or_Greater()
		{
			return m_IsWindows8_Or_Greater;
		}

		bool BaseConfigManager::isWindows8Point1_Or_Greater()
		{
			return m_IsWindows8Point1_Or_Greater;
		}

		bool BaseConfigManager::isWindows10_Or_Greater()
		{
			return m_IsWindows10_Or_Greater;
		}

		bool BaseConfigManager::isWindows10_With_VP_Or_Greater()
		{
			return m_IsWindows10_With_VP_Or_Greater;
		}

		bool BaseConfigManager::isWindows8_Or_Greater_Inner()
		{
			HRESULT lresult(E_FAIL);

			do
			{

				LOG_INVOKE_FUNCTION(Singleton<DXGIManager>::getInstance().getState);

				CComPtrCustom<IDXGIFactory1> lFactory;

				LOG_INVOKE_DXGI_FUNCTION(CreateDXGIFactory1,
					IID_PPV_ARGS(&lFactory));

				LOG_CHECK_PTR_MEMORY(lFactory);

				CComPtrCustom<IDXGIFactory2> lDxgiAdapter1;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lFactory, &lDxgiAdapter1);

			} while (false);

			return SUCCEEDED(lresult);
		}

		bool BaseConfigManager::isWindows8Point1_Or_Greater_Inner()
		{
			HRESULT lresult(E_FAIL);

			do
			{

				LOG_INVOKE_FUNCTION(Singleton<DXGIManager>::getInstance().getState);

				CComPtrCustom<IDXGIFactory1> lFactory;

				LOG_INVOKE_DXGI_FUNCTION(CreateDXGIFactory1,
					IID_PPV_ARGS(&lFactory));

				LOG_CHECK_PTR_MEMORY(lFactory);

				CComPtrCustom<IDXGIFactory3> lDxgiAdapter1;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lFactory, &lDxgiAdapter1);

			} while (false);

			return SUCCEEDED(lresult);
		}

		bool BaseConfigManager::isWindows10_Or_Greater_Inner()
		{
			HRESULT lresult(E_FAIL);

			do
			{

				LOG_INVOKE_FUNCTION(Singleton<DXGIManager>::getInstance().getState);

				CComPtrCustom<IDXGIFactory1> lFactory;

				LOG_INVOKE_DXGI_FUNCTION(CreateDXGIFactory1,
					IID_PPV_ARGS(&lFactory));

				LOG_CHECK_PTR_MEMORY(lFactory);

				MIDL_INTERFACE("1bc6ea02-ef36-464f-bf0c-21ca39e5168a")
				IDXGIFactory4_temp : public IDXGIFactory3{};

				CComPtrCustom<IDXGIFactory4_temp> lDxgiAdapter1;

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lFactory, &lDxgiAdapter1);

			} while (false);

			return SUCCEEDED(lresult);
		}

		bool BaseConfigManager::isWindows10_With_VP_Or_Greater_Inner()
		{
			HRESULT lresult(E_FAIL);

			do
			{
				try
				{
					if (!isWindows10_Or_Greater())
						break;

					class DECLSPEC_UUID("AEB6C755-2546-4881-82CC-E15AE5EBFF3D")
						CVPXEncoderMFTProxy;

					CComPtrCustom<IUnknown> ltestObject;

					lresult = ltestObject.CoCreateInstance(__uuidof(CVPXEncoderMFTProxy));
				}
				catch (const std::exception&)
				{
					lresult = E_FAIL;
				}

			} while (false);

			return SUCCEEDED(lresult);
		}
	}
}