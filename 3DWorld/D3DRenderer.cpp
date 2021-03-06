#include "stdafx.h"
#include "D3DRenderer.h"


D3DRenderer::D3DRenderer()
{
}

D3DRenderer::~D3DRenderer()
{
	Release();
}

void D3DRenderer::Release()
{
	DWORD dwRef = m_Device->Release();
	SafeRelease(m_D3D);
}

bool D3DRenderer::Initialize(int width, int height, HWND hWnd, bool windowed)
{
	m_bWireFrame = false;

	if (!(m_D3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return false;

	D3DPRESENT_PARAMETERS pp = {};
	pp.AutoDepthStencilFormat = D3DFMT_D16;
	pp.BackBufferWidth = width;
	pp.BackBufferHeight = height;
	pp.BackBufferFormat = D3DFMT_A8R8G8B8;
	pp.Windowed = !windowed;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.EnableAutoDepthStencil = true;

	HRESULT hr = m_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &m_Device);

	if (FAILED(hr))
		return false;

	m_Device->SetRenderState(D3DRS_ZENABLE, true);
	m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_Device->SetRenderState(D3DRS_LIGHTING, true);
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); //텍스쳐가 가진 알파 그대로 씀
	m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); //텍스쳐가 가진 알파를 1 - srcalpha

	m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR); //멀리 있는 텍스쳐 축소
	m_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); //가까이 있는 텍스쳐 확대
	m_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR); //자연스러운 보간

	//알파블렌딩 할때는 알파블렌딩 하지않는 객체를 그리고, 제일 멀리 있는 것부터 가까이 있는 객체까지 그린다 (뷰 기준).

	//m_Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	//

	ZeroMemory(&Light, sizeof(D3DLIGHT9));

	Light.Type = D3DLIGHT_DIRECTIONAL;

	Light.Direction = D3DXVECTOR3(0.f,-1.f,1.f);

	Light.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f); //난반사광 (다수의 방향으로 반사됨)
	Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f); //주변광 (특별한 방향없이 주변을 덮고 있는 빛)
	Light.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f); //정반사광 (한 방향으로만 반사되는 빛)

	m_Device->SetLight(0, &Light);
	m_Device->LightEnable(0, true);

	m_fX = 0.01f;

	return true;
}

void D3DRenderer::RenderBegin()
{
	if (3.f < Light.Direction.x || 0.f > Light.Direction.x) //3을 넣어서 노멀라이즈 했을 때 X가 Y, Z보다 더 큰 값
		m_fX *= -1.f;

	D3DXVECTOR3 vNormal = *D3DXVec3Normalize(&D3DXVECTOR3() ,&D3DXVECTOR3(1, -1, 1)); //노멀라이즈는 각 성분을 길이로 나누기 때문에 여기선 각각 비중이 비슷하다.
	D3DXVECTOR3 vNormal2 = *D3DXVec3Normalize(&D3DXVECTOR3(), &D3DXVECTOR3(5, -1, 1)); //하지만 길이에 비해 x인 5가 제일 비중이 크기 때문에 y ,z는 비중이 적다, x가 가장 비중이 크다.

	Light.Direction.x += m_fX; //빛의 방향벡터와 정점의 노말벡터가 내적 했을 때  90 이상 270 도 이하이면 빛이 반사되지 않는다.

	m_Device->SetLight(0, &Light);
	

	if (m_bWireFrame)
	{
		m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}
	else
	{
		m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	if (Input::GetInstance()->GetKeyState(VK_SPACE) == KeyState::Up)
	{
		m_bWireFrame ^= true;
	}

	m_Device->BeginScene();
	m_Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 125, 125), 1.f, 0.f);
}

void D3DRenderer::RenderEnd()
{
	m_Device->EndScene();
	m_Device->Present(NULL, NULL, NULL, NULL);
}
