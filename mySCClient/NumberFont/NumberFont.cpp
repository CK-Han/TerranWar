#include "../stdafx.h"
#include "NumberFont.h"

CNumberFont* CNumberFont::instance = nullptr;

CNumberFont::CNumberFont()
{
	m_pd3dNumberBuffer = nullptr;
	//m_pd3dNumberIndexBuffer = nullptr;
	m_pd3dSRVNumber = nullptr;
	m_pd3dSamplerState = nullptr;

	m_pd3dVertexShader = nullptr;
	m_pd3dVertexLayout = nullptr;
	m_pd3dPixelShader = nullptr;
}


CNumberFont::~CNumberFont()
{
	if(m_pd3dNumberBuffer) m_pd3dNumberBuffer->Release();
	//if(m_pd3dNumberIndexBuffer) m_pd3dNumberIndexBuffer->Release();
	if(m_pd3dSRVNumber) m_pd3dSRVNumber->Release();
	if(m_pd3dSamplerState) m_pd3dSamplerState->Release();

	if(m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if(m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if(m_pd3dPixelShader) m_pd3dPixelShader->Release();

	m_mapNumberData.clear();
}


void CNumberFont::CreateShader(ID3D11Device* pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSNumberFont", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSNumberFont", "ps_5_0", &m_pd3dPixelShader);
}

void CNumberFont::CreateResource(ID3D11Device* pd3dDevice)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(FONT_DATA) * 6 * MAX_NUMBER_DIGIT * MAX_NUM_FONT;	
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dNumberBuffer);

	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &m_pd3dSamplerState);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Font/status_font.png"), NULL, NULL, &m_pd3dSRVNumber, NULL);

	//test
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.CullMode = D3D11_CULL_NONE;
	rd.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&rd, &m_pd3dRS);
}


void CNumberFont::AddNumber(string& str, D3DXVECTOR4& screenPosColor, int number)
{
	m_mapNumberData.insert(make_pair(str, make_pair(screenPosColor, number)));
}

void CNumberFont::DelNumber(string& str)
{
	m_mapNumberData.erase(str);
}

void CNumberFont::InvalidateNumber(string& str)
{
	auto p = m_mapNumberData.find(str);
	if (p != m_mapNumberData.end())
	{
		p->second.second = INVALIDATE_FONT;
	}
}

void CNumberFont::InvalidateAll()
{
	for (auto& d : m_mapNumberData)
	{
		string str = d.first;
		InvalidateNumber(str);
	}
}

void CNumberFont::UpdateNumber(string& str, int number)
{
	auto p = m_mapNumberData.find(str);
	if (p != m_mapNumberData.end())
	{
		p->second.second = number;
	}
}

void CNumberFont::Render(ID3D11DeviceContext* pd3dDeviceContext, float clientWidth, float clientHeight)
{
	if (m_mapNumberData.empty())
		return;

	float pixel_width = (22.0f / 2.0f) / clientWidth;
	float pixel_height = (24.0f / 2.0f) / clientHeight;
	D3DXVECTOR4 LT, RT, LB, RB;
	float offset;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dNumberBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	FONT_DATA* pFontData = (FONT_DATA*)d3dMappedResource.pData;
	int nVertices = 0;
	for (auto& d : m_mapNumberData)
	{
		int number = d.second.second;
		if (number == INVALIDATE_FONT) continue;

		offset = pixel_width * 2 * MAX_NUMBER_DIGIT / 2;
		
		do
		{	//LT 0 RT 1 LB 2 RB 3
			LT = D3DXVECTOR4(d.second.first.x - pixel_width + offset, d.second.first.y - pixel_height, 0.0f, d.second.first.w);
			RT = D3DXVECTOR4(d.second.first.x + pixel_width + offset, d.second.first.y - pixel_height, 0.0f, d.second.first.w);
			LB = D3DXVECTOR4(d.second.first.x - pixel_width + offset, d.second.first.y + pixel_height, 0.0f, d.second.first.w);
			RB = D3DXVECTOR4(d.second.first.x + pixel_width + offset, d.second.first.y + pixel_height, 0.0f, d.second.first.w);

			int num = number % MAX_NUMBER_DIGIT;

			pFontData[nVertices].posColor = LT;
			pFontData[nVertices].texCoord = D3DXVECTOR2(0.1f * num, 1.0f);
			pFontData[nVertices + 1].posColor = RT;
			pFontData[nVertices + 1].texCoord = D3DXVECTOR2(0.1f * num + 0.1f, 1.0f);
			pFontData[nVertices + 2].posColor = LB;
			pFontData[nVertices + 2].texCoord = D3DXVECTOR2(0.1f * num, 0.0f);

			pFontData[nVertices + 3].posColor = RT;
			pFontData[nVertices + 3].texCoord = D3DXVECTOR2(0.1f * num + 0.1f, 1.0f);
			pFontData[nVertices + 4].posColor = RB;
			pFontData[nVertices + 4].texCoord = D3DXVECTOR2(0.1f * num + 0.1f, 0.0f);
			pFontData[nVertices + 5].posColor = LB;
			pFontData[nVertices + 5].texCoord = D3DXVECTOR2(0.1f * num, 0.0f);
			

			offset -= pixel_width * 2;
			nVertices += 6;
			number /= 10;
		} while (number > 0);
	}
	pd3dDeviceContext->Unmap(m_pd3dNumberBuffer, 0);

	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
	pd3dDeviceContext->GSSetShader(nullptr, NULL, 0);
	pd3dDeviceContext->HSSetShader(nullptr, NULL, 0);
	pd3dDeviceContext->DSSetShader(nullptr, NULL, 0);

	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_TEXTURE, 1, &m_pd3dSRVNumber);
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_SAMPLER, 1, &m_pd3dSamplerState);

	UINT vertexStrides[1] = { sizeof(FONT_DATA) };
	UINT vertexOffsets[1] = { 0 };
	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dNumberBuffer, vertexStrides, vertexOffsets);
	pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dDeviceContext->RSSetState(m_pd3dRS);
	
	pd3dDeviceContext->Draw(nVertices, 0);
}