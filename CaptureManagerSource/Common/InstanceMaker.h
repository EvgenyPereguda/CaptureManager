#pragma once
#include "IMaker.h"
#include <guiddef.h>

namespace CaptureManager
{
	namespace Core
	{
		template<typename Class, typename Factory>
		class InstanceMaker :
			public IMaker
		{
		public:
			
			template<typename... Keys>
			InstanceMaker(const Keys... aKeys)
			{
				registerInFactory(aKeys...);
			}
			
			virtual IUnknown * make() const
			{
				CComPtrCustom<Class> lInstance = new(std::nothrow) Class;

				if (!lInstance)
				{
					return nullptr;
				}

				IUnknown* lPtrIUnknown;

				lInstance->QueryInterface(IID_PPV_ARGS(&lPtrIUnknown));

				return lPtrIUnknown;
			}

			virtual ~InstanceMaker()
			{
			}

		private:

			template<typename Key>
			void registerInFactory(const Key& aRefKey)
			{
				Factory::getInstance().registerInstanceMaker(
					aRefKey,
					this);
			}

			template<typename Key, typename... Keys>
			void registerInFactory(const Key& aRefKey, const Keys... aKeys)
			{
				registerInFactory(aRefKey);

				registerInFactory(aKeys...);
			}

		};
		
	}
}