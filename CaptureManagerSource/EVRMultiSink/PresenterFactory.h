#pragma once
#include "IPresenter.h"

namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			class PresenterFactory
			{
			public:

				static HRESULT createPresenter(
					HANDLE aHandle,
					IUnknown* aPtrUnkTarget,
					DWORD aOutputNodeAmount,
					IPresenter** aPtrPresenter,
					IMFTransform** aPtrPtrMixer);

				static HRESULT getMaxInputStreamCount(
					DWORD* aPtrMaxInputStreamCount);

			private:
				PresenterFactory() = delete;
				~PresenterFactory() = delete;

				static HRESULT createFromWindowHandler(
					HANDLE aHandle,
					DWORD aOutputNodeAmount,
					IPresenter** aPtrPresenter,
					IMFTransform** aPtrPtrMixer);

				static HRESULT createFromSharedHandler(
					HANDLE aHandle,
					DWORD aOutputNodeAmount,
					IPresenter** aPtrPresenter,
					IMFTransform** aPtrPtrMixer);

				static HRESULT createDirect3D11PresenterFromWindowHandler(
					HANDLE aHandle,
					DWORD aOutputNodeAmount,
					IPresenter** aPtrPresenter,
					IMFTransform** aPtrPtrMixer);

				static HRESULT createDirect3D11PresenterFromSharedHandler(
					HANDLE aHandle,
					DWORD aOutputNodeAmount,
					IPresenter** aPtrPresenter,
					IMFTransform** aPtrPtrMixer);

				static HRESULT createDirect3D11PresenterFromTexture(
					IUnknown* aPtrTarget,
					DWORD aOutputNodeAmount,
					IPresenter** aPtrPresenter,
					IMFTransform** aPtrPtrMixer);
				
				static HRESULT createDirect3D11Presenter(
					HANDLE aHandle,
					IUnknown* aPtrUnkTarget,
					DWORD aOutputNodeAmount,
					IPresenter** aPtrPresenter,
					IMFTransform** aPtrPtrMixer);
			};
		}
	}
}