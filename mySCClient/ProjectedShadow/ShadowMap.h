#pragma once

#include "../Scene.h"

struct VS_CB_TEXTURE_PROJECTION
{
	D3DXMATRIX mtxProjectView;
	D3DXMATRIX mtxProjectProjection;
	D3DXVECTOR4 vProjectorPosition;
	//16 packing
};

class CShadowMap
{
public:
	CShadowMap(ID3D11Device* pd3dDevice, UINT width, UINT height);
	~CShadowMap();

	ID3D11ShaderResourceView*	GetProjectedTextureSRV() { return m_pd3dProjectedTextureSRV; }
	ID3D11RenderTargetView*		GetProjectedTextureRTV() { return m_pd3dProjectedTextureRTV; }

	void OnCreateShadowMap(ID3D11DeviceContext* pd3dDeviceContext, CScene* pScene, ID3D11DepthStencilView* pd3dDSV);
	void CreateShadowSamplerAndRasterizer(ID3D11Device *pd3dDevice);

	void UpdateShadowConstantBuffer(ID3D11DeviceContext *pd3dDeviceContext);

	void ResizeWindow(ID3D11Device *pd3dDevice, float width, float height);

	D3DXMATRIX		m_d3dxmtxShadowMap;
	D3DXMATRIX		m_d3dxmtxShadowView;
	D3DXMATRIX		m_d3dxmtxShadowProjection;

	D3DXVECTOR3		m_d3dxvLightPosition;

private:
	CShadowMap(const CShadowMap& rhs);
	
public:
	UINT mWidth;
	UINT mHeight;

	D3D11_VIEWPORT					m_d3dViewport;
	ID3D11ShaderResourceView*		m_pd3dProjectedTextureSRV{ nullptr };
	ID3D11RenderTargetView*			m_pd3dProjectedTextureRTV{ nullptr };
	
	ID3D11Buffer*					m_pd3dcbShadowMap{nullptr};

	ID3D11SamplerState*				m_pd3dShadowSamplerState{nullptr};
	ID3D11RasterizerState*			m_pd3dShadowRasterizerState{ nullptr };
};