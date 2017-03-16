#include "../stdafx.h"
#include "ShadowMap.h"


CShadowMap::CShadowMap(ID3D11Device* pd3dDevice, UINT width, UINT height)
	: mWidth(width * UPSIZE_SHADOW_MAP), mHeight(height * UPSIZE_SHADOW_MAP), m_pd3dProjectedTextureSRV(0), m_pd3dProjectedTextureRTV(0)
{
	m_d3dViewport.TopLeftX = 0.0f;
	m_d3dViewport.TopLeftY = 0.0f;
	m_d3dViewport.Width = static_cast<float>(mWidth);
	m_d3dViewport.Height = static_cast<float>(mHeight);
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.MaxDepth = 1.0f;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	//texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* projectedTexture = 0;
	(pd3dDevice->CreateTexture2D(&texDesc, NULL, &projectedTexture));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	(pd3dDevice->CreateRenderTargetView(projectedTexture, &rtvDesc, &m_pd3dProjectedTextureRTV));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	//srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	(pd3dDevice->CreateShaderResourceView(projectedTexture, &srvDesc, &m_pd3dProjectedTextureSRV));

	projectedTexture->Release();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.ByteWidth = sizeof(VS_CB_TEXTURE_PROJECTION);
	pd3dDevice->CreateBuffer(&bd, nullptr, &m_pd3dcbShadowMap);
	
	CreateShadowSamplerAndRasterizer(pd3dDevice);
}

CShadowMap::~CShadowMap()
{
	if (m_pd3dProjectedTextureSRV)	m_pd3dProjectedTextureSRV->Release();
	if (m_pd3dProjectedTextureRTV) m_pd3dProjectedTextureRTV->Release();
	if (m_pd3dcbShadowMap) m_pd3dcbShadowMap->Release();
	if (m_pd3dShadowSamplerState) m_pd3dShadowSamplerState->Release();
	if (m_pd3dShadowRasterizerState) m_pd3dShadowRasterizerState->Release();
}

//********************************************
void CShadowMap::ResizeWindow(ID3D11Device *pd3dDevice, float width, float height)
{
	mWidth = width * UPSIZE_SHADOW_MAP;
	mHeight = height * UPSIZE_SHADOW_MAP;

	m_d3dViewport.TopLeftX = 0.0f;
	m_d3dViewport.TopLeftY = 0.0f;
	m_d3dViewport.Width = static_cast<float>(mWidth);
	m_d3dViewport.Height = static_cast<float>(mHeight);
	m_d3dViewport.MinDepth = 0.0f;
	m_d3dViewport.MaxDepth = 1.0f;

	if (m_pd3dProjectedTextureRTV) m_pd3dProjectedTextureRTV->Release();
	if (m_pd3dProjectedTextureSRV) m_pd3dProjectedTextureSRV->Release();
	
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* projectedTexture = 0;
	(pd3dDevice->CreateTexture2D(&texDesc, NULL, &projectedTexture));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	(pd3dDevice->CreateRenderTargetView(projectedTexture, &rtvDesc, &m_pd3dProjectedTextureRTV));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	(pd3dDevice->CreateShaderResourceView(projectedTexture, &srvDesc, &m_pd3dProjectedTextureSRV));

	projectedTexture->Release();
}

void CShadowMap::CreateShadowSamplerAndRasterizer(ID3D11Device *pd3dDevice)
{
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	//sd.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sd.MaxAnisotropy = 1;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	pd3dDevice->CreateSamplerState(&sd, &m_pd3dShadowSamplerState);

	/*
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_BACK;
	rd.FrontCounterClockwise = false;
	rd.DepthClipEnable = true;
	rd.DepthBias = 100000; //100000 / 2^24 = 0.006
	rd.DepthBiasClamp = 0.0f;
	rd.SlopeScaledDepthBias = 1.0f;
	pd3dDevice->CreateRasterizerState(&rd, &m_pd3dShadowRasterizerState);
	*/
}

void CShadowMap::UpdateShadowConstantBuffer(ID3D11DeviceContext *pd3dDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE mr;
	pd3dDeviceContext->Map(m_pd3dcbShadowMap, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
	VS_CB_TEXTURE_PROJECTION* mtxShadow = (VS_CB_TEXTURE_PROJECTION*)mr.pData;
	D3DXMatrixTranspose(&(mtxShadow->mtxProjectView), &m_d3dxmtxShadowView);
	D3DXMatrixTranspose(&(mtxShadow->mtxProjectProjection), &m_d3dxmtxShadowProjection);
	mtxShadow->vProjectorPosition = D3DXVECTOR4(m_d3dxvLightPosition, 1.0f);
	pd3dDeviceContext->Unmap(m_pd3dcbShadowMap, 0);
	
	pd3dDeviceContext->VSSetConstantBuffers(CB_SLOT_SHADOW, 1, &m_pd3dcbShadowMap);
	pd3dDeviceContext->DSSetConstantBuffers(CB_SLOT_SHADOW, 1, &m_pd3dcbShadowMap);//DS 테셀레이션
}


void CShadowMap::OnCreateShadowMap(ID3D11DeviceContext* pd3dDeviceContext, CScene* pScene, ID3D11DepthStencilView* pd3dDSV)
{
	//0907 수정사항
	//본래는 맵의 항상 좌측 위에 존재하여 큰 절두체로 그림자맵을 만들었는데 이로 인해 디테일이 깨진다.
	//고로, 화면에 보이는 녀석들만 그림자를 만드는 게 맞다! 카메라의 WorldLootAt을 이용하여 적절한 위치에 배치하고
	//절두체 역시 적절한 크기로 만든다면, 그림자가 좀 더 잘 그려질것이다.

	LIGHT *pLight = pScene->GetLight(0);
	D3DXVECTOR3 d3dxvUp{ 0.0f, 1.0f, 0.0f };

	//float fHalfWidth = (float)pScene->GetHeightMap()->GetHeightMapWidth() * 0.5f * UPSIZE_SHADOW_MAP;
	//float fHalfLength = (float)pScene->GetHeightMap()->GetHeightMapLength() * 0.5f * UPSIZE_SHADOW_MAP;
	float fHalfWidth = 20.0f;
	float fHalfLength = 15.0f;

	//그림자 만드는 조명의 위치를 조정한다.
	D3DXVECTOR3 d3dxvLightPosition = pScene->m_pCamera->GetLookAtPosition() - (pLight->m_d3dxvDirection * SUN_DISTANCE); //+ D3DXVECTOR3(-fHalfWidth * 2.0f, 20.0f, 0.0f);
	m_d3dxvLightPosition = d3dxvLightPosition;
	D3DXVECTOR3 d3dxvTarget = d3dxvLightPosition + pLight->m_d3dxvDirection;

	D3DXMatrixLookAtLH(&m_d3dxmtxShadowView, &d3dxvLightPosition, &d3dxvTarget, &d3dxvUp);
	D3DXVec3TransformCoord(&d3dxvLightPosition, &d3dxvLightPosition, &m_d3dxmtxShadowView);

	float l = d3dxvLightPosition.x - fHalfWidth * 2;
	float r = d3dxvLightPosition.x + fHalfWidth * 2;
	float t = d3dxvLightPosition.y + fHalfLength * 2;
	float b = d3dxvLightPosition.y - fHalfLength * 2;
	float zn = d3dxvLightPosition.z;
	float zf = d3dxvLightPosition.z + 100.0f * UPSIZE_SHADOW_MAP;
	D3DXMatrixOrthoOffCenterLH(&m_d3dxmtxShadowProjection, l, r, b, t, zn, zf);

	D3DXMATRIX mtxTexture(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	m_d3dxmtxShadowMap = m_d3dxmtxShadowView * m_d3dxmtxShadowProjection * mtxTexture;
	pScene->m_pCamera->UpdateShaderVariables(pd3dDeviceContext, m_d3dxmtxShadowView, m_d3dxmtxShadowProjection);


	float cBackColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//float cBackColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	pd3dDeviceContext->RSSetViewports(1, &m_d3dViewport);
	pd3dDeviceContext->ClearDepthStencilView(pd3dDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pd3dDeviceContext->ClearRenderTargetView(m_pd3dProjectedTextureRTV, cBackColor);

	//pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dProjectedTextureRTV, pd3dDSV);
	pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dProjectedTextureRTV, nullptr);
	pScene->RenderObjectShadow(pd3dDeviceContext);
	pd3dDeviceContext->OMSetRenderTargets(0, nullptr, nullptr); //RTV를 떼어낸다.
	

	//0907 수정 전 코드
	/*
	LIGHT *pLight = pScene->GetLight(0);
	D3DXVECTOR3 d3dxvUp{ 1.0f, 0.0f, 0.0f }; 
	
	float fHalfWidth = (float)pScene->GetHeightMap()->GetHeightMapWidth() * 0.5f * UPSIZE_SHADOW_MAP;
	float fHalfLength = (float)pScene->GetHeightMap()->GetHeightMapLength() * 0.5f * UPSIZE_SHADOW_MAP;

	D3DXVECTOR3 d3dxvLightPosition = D3DXVECTOR3(-fHalfWidth * 2, 50.0f, 0.0f);
	m_d3dxvLightPosition = d3dxvLightPosition;
	D3DXVECTOR3 d3dxvTarget = d3dxvLightPosition + pLight->m_d3dxvDirection;
	
	D3DXMatrixLookAtLH(&m_d3dxmtxShadowView, &d3dxvLightPosition, &d3dxvTarget, &d3dxvUp);
	D3DXVec3TransformCoord(&d3dxvLightPosition, &d3dxvLightPosition, &m_d3dxmtxShadowView);

	float l = d3dxvLightPosition.x - fHalfWidth * 2 ;
	float r = d3dxvLightPosition.x + fHalfWidth * 2 ;
	float t = d3dxvLightPosition.y + fHalfLength * 2;
	float b = d3dxvLightPosition.y - fHalfLength * 2;
	float zn = d3dxvLightPosition.z;
	float zf = d3dxvLightPosition.z + 600.0f * UPSIZE_SHADOW_MAP;
	D3DXMatrixOrthoOffCenterLH(&m_d3dxmtxShadowProjection, l, r, b, t, zn, zf);
	
	D3DXMATRIX mtxTexture(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	m_d3dxmtxShadowMap = m_d3dxmtxShadowView * m_d3dxmtxShadowProjection * mtxTexture;
	pScene->m_pCamera->UpdateShaderVariables(pd3dDeviceContext, m_d3dxmtxShadowView, m_d3dxmtxShadowProjection);
	
	
	float cBackColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//float cBackColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	pd3dDeviceContext->RSSetViewports(1, &m_d3dViewport);
	pd3dDeviceContext->ClearDepthStencilView(pd3dDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pd3dDeviceContext->ClearRenderTargetView(m_pd3dProjectedTextureRTV, cBackColor);
	
	//pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dProjectedTextureRTV, pd3dDSV);
	pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dProjectedTextureRTV, nullptr);
	pScene->RenderObjectShadow(pd3dDeviceContext);
	pd3dDeviceContext->OMSetRenderTargets(0, nullptr, nullptr); //RTV를 떼어낸다.
	*/
}