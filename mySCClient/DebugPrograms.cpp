#include "stdafx.h"
#include "DebugPrograms.h"


CDebugPrograms::CDebugPrograms(ID3D11Device *pd3dDevice, CHeightMap *pHeightMap)
{
	m_pd3dPositionBuffer = nullptr;
	m_pd3dVertexShader = nullptr;
	m_pd3dInputLayout = nullptr;
	m_pd3dPixelShader = nullptr;

	m_nVertices = (251 * 2) * 2;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	
	DebugGridVertex* pVertices = new DebugGridVertex[251 * 2 * 2];
	
	float minPos = -125.0f;
	float maxPos = +125.0f;

	D3DXCOLOR defaultColor = D3DXCOLOR(0.5f, 0.0f, 0.5f, 1.0f);
	D3DXCOLOR tenColor = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);
	D3DXCOLOR centerColor = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	float height = pHeightMap->GetHeight(0.0f, 0.0f, false);

	int nVertices = 0;
	for (int i = -125; i <= 125; ++i)
	{	// 세로줄 시작	-z -> +z로 긋는
		if (i % 10 == 0)
		{
			pVertices[nVertices].position = D3DXVECTOR3(i, height, minPos);
			pVertices[nVertices++].color = tenColor;

			pVertices[nVertices].position = D3DXVECTOR3(i, height, maxPos);
			pVertices[nVertices++].color = tenColor;
		}
		else
		{
			pVertices[nVertices].position = D3DXVECTOR3(i, height, minPos);
			pVertices[nVertices++].color = defaultColor;

			pVertices[nVertices].position = D3DXVECTOR3(i, height, maxPos);
			pVertices[nVertices++].color = defaultColor;
		}
	}

	for (int i = -125; i <= 125; ++i)
	{	// 가로줄 시작	-x -> +x로 긋는
		if (i % 10 == 0)
		{
			pVertices[nVertices].position = D3DXVECTOR3(minPos, height, i);
			pVertices[nVertices++].color = tenColor;

			pVertices[nVertices].position = D3DXVECTOR3(maxPos, height, i);
			pVertices[nVertices++].color = tenColor;
		}
		else
		{
			pVertices[nVertices].position = D3DXVECTOR3(minPos, height, i);
			pVertices[nVertices++].color = defaultColor;

			pVertices[nVertices].position = D3DXVECTOR3(maxPos, height, i);
			pVertices[nVertices++].color = defaultColor;
		}
	}

	pVertices[(125 * 2)].color = centerColor;
	pVertices[(125 * 2) + 1].color = centerColor;
	pVertices[(125 * 2) + (251*2)].color = centerColor;
	pVertices[(125 * 2) + 1 + (251 * 2)].color = centerColor;

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(DebugGridVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	d3dBufferData.SysMemPitch = sizeof(DebugGridVertex) * m_nVertices;

	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	delete pVertices;
	//정점 생성 완료


	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);

	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED(hResult = D3DX11CompileFromFile("Effect.fx", NULL, NULL, "VSDebugGrid", "vs_5_0", dwShaderFlags, 0, NULL, &pd3dVertexShaderBlob, &pd3dErrorBlob, NULL)))
	{
		pd3dDevice->CreateVertexShader(pd3dVertexShaderBlob->GetBufferPointer(), pd3dVertexShaderBlob->GetBufferSize(), NULL, &m_pd3dVertexShader);
		pd3dDevice->CreateInputLayout(d3dInputElements, nElements, pd3dVertexShaderBlob->GetBufferPointer(), pd3dVertexShaderBlob->GetBufferSize(), &m_pd3dInputLayout);
		pd3dVertexShaderBlob->Release();
	}

	if (SUCCEEDED(hResult = D3DX11CompileFromFile("Effect.fx", NULL, NULL, "PSDebugGrid", "ps_5_0", dwShaderFlags, 0, NULL, &pd3dPixelShaderBlob, &pd3dErrorBlob, NULL)))
	{
		pd3dDevice->CreatePixelShader(pd3dPixelShaderBlob->GetBufferPointer(), pd3dPixelShaderBlob->GetBufferSize(), NULL, &m_pd3dPixelShader);
		pd3dPixelShaderBlob->Release();
	}
}


CDebugPrograms::~CDebugPrograms()
{
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dInputLayout) m_pd3dInputLayout->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
}

void CDebugPrograms::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dInputLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
	pd3dDeviceContext->GSSetShader(NULL, NULL, 0);

	unsigned int nVertesStride = sizeof(DebugGridVertex);
	unsigned int nVertexOffset = 0;

	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dPositionBuffer, &nVertesStride, &nVertexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->Draw(m_nVertices, 0);
}