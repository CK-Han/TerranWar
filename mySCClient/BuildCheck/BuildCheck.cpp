#include "../stdafx.h"
#include "BuildCheck.h"

CBuildCheck* CBuildCheck::instance = nullptr;

CBuildCheck::CBuildCheck()
{
}


CBuildCheck::~CBuildCheck()
{
	if(m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();

	if(m_ppBuildingTextures)
	{
		for (int i = 0; i < m_nTextures; ++i)
			m_ppBuildingTextures[i]->Release();
		delete[] m_ppBuildingTextures;
	}
	
	if(m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if(m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if(m_pd3dPixelShader) m_pd3dPixelShader->Release();
	if(m_pd3dGeometryShader) m_pd3dGeometryShader->Release();

	m_mapBuildingTextureBox.clear();
}

void CBuildCheck::CreateShader(ID3D11Device* pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSSIZE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSBuildCheck", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CShader::CreateGeometryShaderFromFile(pd3dDevice, "Effect.fx", "GSBuildCheck", "gs_5_0", &m_pd3dGeometryShader);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSBuildCheck", "ps_5_0", &m_pd3dPixelShader);
}


void CBuildCheck::CreateResource(ID3D11Device* pd3dDevice)
{
	//정점 1개 버퍼 생성
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(CREATE_BUILDING_VERTEX);
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dVertexBuffer);

	//텍스쳐 리소스들을 부름
	m_nTextures = 13;
	m_ppBuildingTextures = new ID3D11ShaderResourceView*[m_nTextures];

	int nCount = 0;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/armory.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("armory", make_pair(nCount, 2.5)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/barrack.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("barrack", make_pair(nCount, 3.5)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/bunker.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("bunker", make_pair(nCount, 2.5)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/commandcenter.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("commandcenter", make_pair(nCount, 4)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/engineeringbay.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("engineeringbay", make_pair(nCount, 3.5)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/factory.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("factory", make_pair(nCount, 3.5)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/fusioncore.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("fusioncore", make_pair(nCount, 2)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/ghostacademy.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("ghostacademy", make_pair(nCount, 3.5)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/sensortower.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("ghostacademy", make_pair(nCount, 2.0)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/missileturret.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("missileturret", make_pair(nCount, 1.5)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/refinery.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("refinery", make_pair(nCount, 3)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/starport.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("starport", make_pair(nCount, 3.5)));
	nCount++;
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/BuildCheck/supplydepot.png"), NULL, NULL, &(m_ppBuildingTextures[nCount]), NULL);
	m_mapBuildingTextureBox.insert(make_pair("supplydepot", make_pair(nCount, 3)));
	nCount++;

	m_nCurrentTexture = INVALIDATE_BUILDCHECK;
}

void CBuildCheck::SetBuildingImage(string buildingName, D3DXVECTOR3 pos)
{
	auto p = m_mapBuildingTextureBox.find(buildingName);
	if (p != m_mapBuildingTextureBox.end())
	{
		m_nCurrentTexture = p->second.first;
		m_d3dxvPosSize = D3DXVECTOR4(pos, p->second.second);
	}
}

void CBuildCheck::InvalidateBuildingImage()
{
	m_nCurrentTexture = INVALIDATE_BUILDCHECK;
}

void CBuildCheck::Render(ID3D11DeviceContext* pd3dDeviceContext)
{
	if (INVALIDATE_BUILDCHECK == m_nCurrentTexture)
		return;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	CREATE_BUILDING_VERTEX* pPos = (CREATE_BUILDING_VERTEX*)d3dMappedResource.pData;
	pPos->posSize = m_d3dxvPosSize;
	pd3dDeviceContext->Unmap(m_pd3dVertexBuffer, 0);

	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
	pd3dDeviceContext->GSSetShader(m_pd3dGeometryShader, NULL, 0);
	pd3dDeviceContext->HSSetShader(nullptr, NULL, 0);
	pd3dDeviceContext->DSSetShader(nullptr, NULL, 0);

	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_TEXTURE, 1, &(m_ppBuildingTextures[m_nCurrentTexture]));
	
	UINT vertexStrides[1] = { sizeof(CREATE_BUILDING_VERTEX) };
	UINT vertexOffsets[1] = { 0 };
	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer, vertexStrides, vertexOffsets);
	pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	pd3dDeviceContext->Draw(1, 0);
}


