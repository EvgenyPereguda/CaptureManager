#include "Direct3D11Renderer.h"
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3d9types.h>
#include <d3d9.h>
#include <math.h>

#pragma comment(lib, "D3D11.lib")

// Driver types supported
D3D_DRIVER_TYPE gDriverTypes[] =
{
	D3D_DRIVER_TYPE_HARDWARE
};
UINT gNumDriverTypes = ARRAYSIZE(gDriverTypes);

// Feature levels supported
D3D_FEATURE_LEVEL gFeatureLevels[] =
{
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0
	//D3D_FEATURE_LEVEL_10_1,
	//D3D_FEATURE_LEVEL_10_0,
	//D3D_FEATURE_LEVEL_9_1
};

UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);


UINT mUseDebugLayer(D3D11_CREATE_DEVICE_VIDEO_SUPPORT);

//Transformed vertex with 1 set of texture coordinates
const DWORD tri_fvf = D3DFVF_XYZRHW | D3DFVF_TEX1;

struct textured_vertex{
	float x, y, z, rhw;  // The transformed(screen space) position for the vertex.
	float tu, tv;         // Texture coordinates
};

Direct3D11Renderer::Direct3D11Renderer() :
mRenderTargetType(RenderTargetType::Texture)
{
}


Direct3D11Renderer::~Direct3D11Renderer()
{
}

HRESULT Direct3D11Renderer::init(HWND aWindowHandler)
{
	HRESULT lhresult(E_FAIL);

	do
	{

		D3D_FEATURE_LEVEL lfeatureLevel;

		CComPtrCustom<ID3D11Device> l_Device;

		D3D11CreateDevice(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			mUseDebugLayer,
			gFeatureLevels,
			gNumFeatureLevels,
			D3D11_SDK_VERSION,
			&l_Device,
			&lfeatureLevel,
			NULL);




		CComPtrCustom<ID3D11DeviceContext> mImmediateContext;

		l_Device->GetImmediateContext(&mImmediateContext);
		
		CComPtrCustom<ID3D10Multithread> lMultiThread;

		// Need to explitly set the multithreaded mode for this device
		mImmediateContext->QueryInterface(IID_PPV_ARGS(&lMultiThread));
		
		BOOL lbRrsult = lMultiThread->SetMultithreadProtected(TRUE);

		CComPtrCustom<IDXGIDevice1> lDXGIDev;

		l_Device->QueryInterface(IID_PPV_ARGS(&lDXGIDev));
		
		CComPtrCustom<IDXGIAdapter> lTempAdapter;

		lDXGIDev->GetAdapter(&lTempAdapter);
		
		CComPtrCustom<IDXGIAdapter1> lAdapter;

		lTempAdapter->QueryInterface(IID_PPV_ARGS(&lAdapter));


		CComPtrCustom<IDXGIFactory2> mDXGIFactory2;

		CComPtrCustom<IDXGIOutput1> mDXGIOutput1;

		CComPtrCustom<IDXGISwapChain1> mSwapChain1;

		lAdapter->GetParent(
			IID_PPV_ARGS(&mDXGIFactory2));
		
		CComPtrCustom<IDXGIOutput> lDXGIOutput;

		lAdapter->EnumOutputs(
			0, &lDXGIOutput);
		
		mDXGIOutput1.Release();

		lDXGIOutput->QueryInterface(IID_PPV_ARGS(&mDXGIOutput1));
		
		// Get the DXGISwapChain1
		DXGI_SWAP_CHAIN_DESC1 scd;
		ZeroMemory(&scd, sizeof(scd));
		scd.SampleDesc.Count = 1;
		scd.SampleDesc.Quality = 0;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Scaling = DXGI_SCALING_STRETCH;
		scd.Width = 800;
		scd.Height = 600;
		scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scd.Stereo = FALSE;
		scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.Flags = 0;// m_bStereoEnabled ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0; //opt in to do direct flip;
		scd.BufferCount = 2;

		mSwapChain1.Release();

		mDXGIFactory2->CreateSwapChainForHwnd(
			l_Device,
			aWindowHandler,
			&scd,
			NULL,
			NULL,
			&mSwapChain1);


		m_SwapChain = mSwapChain1;

		CComPtrCustom<ID3D11Texture2D> lSurface;

		// Get Backbuffer
		mSwapChain1->GetBuffer(
			0, IID_PPV_ARGS(&lSurface));
		
		m_RenderTexture = lSurface;


	} while (false);

	return lhresult;
}

HRESULT Direct3D11Renderer::getRenderTarget(RenderTargetType aRenderTargetType, IUnknown** a_pRenderTarget)
{
	if (m_RenderTexture == nullptr)
		return E_POINTER;

	mRenderTargetType = aRenderTargetType;

	if (aRenderTargetType == RenderTargetType::Texture)
		return m_RenderTexture->QueryInterface(a_pRenderTarget);
	else
		return m_SwapChain->QueryInterface(a_pRenderTarget);
}

HRESULT Direct3D11Renderer::presents()
{
	HRESULT hr = S_OK;

	if (m_SwapChain == nullptr)
		return E_POINTER;

	CComQIPtrCustom<IDXGISwapChain1> lSwapChain(m_SwapChain);

	if (lSwapChain == nullptr)
		return E_POINTER;


	if (mRenderTargetType == RenderTargetType::Texture)
	{		
		hr = lSwapChain->Present(0, 0);
		
		return hr;
	}
	else
	{
		//hr = lSwapChain->Present(0, 0);

		return hr;
	}

}

HRESULT Direct3D11Renderer::shutdown()
{
	m_SwapChain.Release();

	m_RenderTexture.Release();

	m_list_vb.Release();

	return S_OK;
}