#pragma once

#include <Unknwnbase.h>
#include <dxgi1_2.h>
#include "Common\ComPtrCustom.h"
#include "IRenderer.h"
#include "Common.h"

class Direct3D11Renderer : 
	public BaseUnknown<IRenderer>
{
public:
	Direct3D11Renderer();
	virtual ~Direct3D11Renderer();

	HRESULT init(HWND aWindowHandler);

	HRESULT getRenderTarget(RenderTargetType aRenderTargetType, IUnknown** a_pRenderTarget) override;

	HRESULT presents() override;

	HRESULT shutdown() override;

private:

	int list_count;

	RenderTargetType mRenderTargetType;

	CComPtrCustom<IUnknown> m_list_vb;

	CComPtrCustom<IUnknown> m_RenderTexture;

	CComPtrCustom<IUnknown> m_SwapChain;
};

