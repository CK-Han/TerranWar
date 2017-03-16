#include "../stdafx.h"
#include "ProgressImage.h"


CProgressImage::CProgressImage()
{
	m_pd3dBarBuffer = nullptr;

	m_pd3dSRVProgressBar = nullptr;
	m_pd3dProgressSamplerState = nullptr;

	m_pd3dProgressVertexShader = nullptr;
	m_pd3dProgressInputLayout = nullptr;
	m_pd3dProgressPixelShader = nullptr;

	m_pd3dHPBuffer = nullptr;
	m_pd3dSRVHPBar = nullptr;
}


CProgressImage::~CProgressImage()
{
	if(m_pd3dBarBuffer) m_pd3dBarBuffer->Release();
	
	if(m_pd3dSRVProgressBar) m_pd3dSRVProgressBar->Release();
	if(m_pd3dProgressSamplerState) m_pd3dProgressSamplerState->Release();
	
	if(m_pd3dProgressVertexShader) m_pd3dProgressVertexShader->Release();
	if(m_pd3dProgressInputLayout) m_pd3dProgressInputLayout->Release();
	if(m_pd3dProgressPixelShader) m_pd3dProgressPixelShader->Release();

	if (m_pd3dHPBuffer) m_pd3dHPBuffer->Release();
	if (m_pd3dSRVHPBar) m_pd3dSRVHPBar->Release();
}

void		CProgressImage::CreateShader(ID3D11Device* pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSCOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSProgressBar", "vs_5_0", &m_pd3dProgressVertexShader, d3dInputLayout, nElements, &m_pd3dProgressInputLayout);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSProgressBar", "ps_5_0", &m_pd3dProgressPixelShader);
}

void		CProgressImage::CreateResource(ID3D11Device* pd3dDevice)
{
	//Frame과 Bar 둘 다 같은 버퍼를 사용한다. 버퍼 2개 생성 대신 Map을 2 번 선택헀는데 흠...
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(PROGRESS_VERTEX) * 6;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dBarBuffer);

	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;

	//샘플러 및 리소스 생성
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &m_pd3dProgressSamplerState);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/progressbar.png"), NULL, NULL, &m_pd3dSRVProgressBar, NULL);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/hpbar.png"), NULL, NULL, &m_pd3dSRVHPBar, NULL);


	//0904 HP 표현
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(PROGRESS_VERTEX) * 6 * MAX_SELECTED_OBJECTS;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dHPBuffer);	
}

void		CProgressImage::RenderProgressImage(ID3D11DeviceContext* pd3dDeviceContext, float fProgress)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dProgressInputLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dProgressVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dProgressPixelShader, NULL, 0);
	pd3dDeviceContext->GSSetShader(nullptr, NULL, 0);
	pd3dDeviceContext->HSSetShader(nullptr, NULL, 0);
	pd3dDeviceContext->DSSetShader(nullptr, NULL, 0);

	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_TEXTURE, 1, &m_pd3dSRVProgressBar);
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_SAMPLER, 1, &m_pd3dProgressSamplerState);

	//프로그레스 바 렌더링 시엔 posColor의 w값을 0.0f로 해서 구분한다.
	//LB LT RT RT RB LB
	// 1. 프레임 렌더링
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dBarBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	PROGRESS_VERTEX* pVertices = (PROGRESS_VERTEX*)d3dMappedResource.pData;	
	pVertices[0].posColor = D3DXVECTOR4(m_d3dxvLeftBottom.x, m_d3dxvLeftBottom.y, 0.0f, 0.0f);	//LB
	pVertices[1].posColor = D3DXVECTOR4(m_d3dxvLeftBottom.x, m_d3dxvRightTop.y, 0.0f, 0.0f);		//LT
	pVertices[2].posColor = D3DXVECTOR4(m_d3dxvRightTop.x, m_d3dxvRightTop.y, 0.0f, 0.0f);		//RT
	pVertices[3].posColor = D3DXVECTOR4(m_d3dxvRightTop.x, m_d3dxvRightTop.y, 0.0f, 0.0f);		//RT
	pVertices[4].posColor = D3DXVECTOR4(m_d3dxvRightTop.x, m_d3dxvLeftBottom.y, 0.0f, 0.0f);		//RB
	pVertices[5].posColor = D3DXVECTOR4(m_d3dxvLeftBottom.x, m_d3dxvLeftBottom.y, 0.0f, 0.0f);	//LB
		//프레임 텍스쳐는 0.5f부터 시작
	pVertices[0].texCoord = D3DXVECTOR2(0.5f, 1.0f);
	pVertices[1].texCoord = D3DXVECTOR2(0.5f, 0.0f);
	pVertices[2].texCoord = D3DXVECTOR2(1.0f, 0.0f);
	pVertices[3].texCoord = D3DXVECTOR2(1.0f, 0.0f);
	pVertices[4].texCoord = D3DXVECTOR2(1.0f, 1.0f);
	pVertices[5].texCoord = D3DXVECTOR2(0.5f, 1.0f);
	
	pd3dDeviceContext->Unmap(m_pd3dBarBuffer, 0);

	UINT vertexStrides[1] = { sizeof(PROGRESS_VERTEX) };
	UINT vertexOffsets[1] = { 0 };
	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dBarBuffer, vertexStrides, vertexOffsets);
	pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dDeviceContext->Draw(6, 0);
	
	// 2. 프로그레스 바 렌더링
	float length = (m_d3dxvRightTop.x - m_d3dxvLeftBottom.x) * fProgress;
	pd3dDeviceContext->Map(m_pd3dBarBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pVertices = (PROGRESS_VERTEX*)d3dMappedResource.pData;
	pVertices[0].posColor = D3DXVECTOR4(m_d3dxvLeftBottom.x, m_d3dxvLeftBottom.y, 0.0f, 0.0f);			//LB
	pVertices[1].posColor = D3DXVECTOR4(m_d3dxvLeftBottom.x, m_d3dxvRightTop.y, 0.0f, 0.0f);				//LT
	pVertices[2].posColor = D3DXVECTOR4(m_d3dxvLeftBottom.x + length, m_d3dxvRightTop.y, 0.0f, 0.0f);		//RT
	pVertices[3].posColor = D3DXVECTOR4(m_d3dxvLeftBottom.x + length, m_d3dxvRightTop.y, 0.0f, 0.0f);		//RT
	pVertices[4].posColor = D3DXVECTOR4(m_d3dxvLeftBottom.x + length, m_d3dxvLeftBottom.y, 0.0f, 0.0f);	//RB
	pVertices[5].posColor = D3DXVECTOR4(m_d3dxvLeftBottom.x, m_d3dxvLeftBottom.y, 0.0f, 0.0f);			//LB
		//프로그래스 바 텍스쳐는 0.0f부터 0.5f까지
	pVertices[0].texCoord = D3DXVECTOR2(0.0f, 1.0f);
	pVertices[1].texCoord = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[2].texCoord = D3DXVECTOR2(0.5f, 0.0f);
	pVertices[3].texCoord = D3DXVECTOR2(0.5f, 0.0f);
	pVertices[4].texCoord = D3DXVECTOR2(0.5f, 1.0f);
	pVertices[5].texCoord = D3DXVECTOR2(0.0f, 1.0f);
	pd3dDeviceContext->Unmap(m_pd3dBarBuffer, 0);

	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dBarBuffer, vertexStrides, vertexOffsets);
	pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dDeviceContext->Draw(6, 0);	//깊이검사를 하지 않아야 렌더링될 것이다.
}

void		CProgressImage::RenderHPImage(ID3D11DeviceContext* pd3dDeviceContext, CGameObject** ppObjects, int nValidObject)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dProgressInputLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dProgressVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dProgressPixelShader, NULL, 0);
	pd3dDeviceContext->GSSetShader(nullptr, NULL, 0);
	pd3dDeviceContext->HSSetShader(nullptr, NULL, 0);
	pd3dDeviceContext->DSSetShader(nullptr, NULL, 0);

	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_TEXTURE, 1, &m_pd3dSRVHPBar);
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_SAMPLER, 1, &m_pd3dProgressSamplerState);

	float hpColor, length;
	//체력 바 렌더링 시엔 posColor의 w값을 0.0f이 아닌 Black Green Yellow Red 순서대로 1.0f, 2.0f, 3.0f, 4.0f로 구분한다.
	//LB LT RT RT RB LB
	// 1. 프레임 렌더링
	float barWidth = 2.0f;
	float barHeight = 0.3f;
	D3DXVECTOR3 vLB, vRT;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dHPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	PROGRESS_VERTEX* pVertices = (PROGRESS_VERTEX*)d3dMappedResource.pData;
	for (int i = 0; i < nValidObject; ++i)
	{
		length =  (float)ppObjects[i]->m_nHP / (float)ppObjects[i]->m_nMaxHP;

		if (isAnimateObject(ppObjects[i]->m_nObjectType))
			barWidth = 2.0f;
		else
			barWidth = 3.0f;

		D3DXVECTOR3 objPos = ppObjects[i]->GetPosition();
		if(!isAirObject(ppObjects[i]->m_nObjectType) && isAnimateObject(ppObjects[i]->m_nObjectType))
			objPos.y += ppObjects[i]->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f;
		objPos.z += ppObjects[i]->m_bcMeshBoundingCube.m_d3dxvMaximum.z * 2.0f;
		vLB.x = objPos.x - barWidth / 2.0f;	vLB.y = objPos.y; vLB.z = objPos.z - barHeight / 2.0f;
		vRT.x = objPos.x + barWidth / 2.0f;	vRT.y = objPos.y; vRT.z = objPos.z + barHeight / 2.0f;

		pVertices[0 + i * 6].posColor = D3DXVECTOR4(vLB.x, vLB.y, vLB.z, 1.0f);		//LB
		pVertices[1 + i * 6].posColor = D3DXVECTOR4(vLB.x, vRT.y, vRT.z, 1.0f);		//LT
		pVertices[2 + i * 6].posColor = D3DXVECTOR4(vRT.x, vRT.y, vRT.z, 1.0f);			//RT
		pVertices[3 + i * 6].posColor = D3DXVECTOR4(vRT.x, vRT.y, vRT.z, 1.0f);		//RT
		pVertices[4 + i * 6].posColor = D3DXVECTOR4(vRT.x, vLB.y, vLB.z, 1.0f);		//RB
		pVertices[5 + i * 6].posColor = D3DXVECTOR4(vLB.x, vLB.y, vLB.z, 1.0f);	//LB
																									
		//프레임 텍스쳐는 0.75f부터 시작
		pVertices[0 + i * 6].texCoord = D3DXVECTOR2(0.75f, 1.0f);
		pVertices[1 + i * 6].texCoord = D3DXVECTOR2(0.75f, 0.0f);
		pVertices[2 + i * 6].texCoord = D3DXVECTOR2(1.0f, 0.0f);
		pVertices[3 + i * 6].texCoord = D3DXVECTOR2(1.0f, 0.0f);
		pVertices[4 + i * 6].texCoord = D3DXVECTOR2(1.0f, 1.0f);
		pVertices[5 + i * 6].texCoord = D3DXVECTOR2(0.75f, 1.0f);
	}
	pd3dDeviceContext->Unmap(m_pd3dHPBuffer, 0);

	UINT vertexStrides[1] = { sizeof(PROGRESS_VERTEX) };
	UINT vertexOffsets[1] = { 0 };
	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dHPBuffer, vertexStrides, vertexOffsets);
	pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dDeviceContext->Draw(6 * nValidObject, 0);

	// 2. 프로그레스 바 렌더링
	pd3dDeviceContext->Map(m_pd3dHPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pVertices = (PROGRESS_VERTEX*)d3dMappedResource.pData;
	for (int i = 0; i < nValidObject; ++i)
	{
		if (isAnimateObject(ppObjects[i]->m_nObjectType))
			barWidth = 2.0f;
		else
			barWidth = 3.0f;

		length = hpColor = (float)ppObjects[i]->m_nHP / (float)ppObjects[i]->m_nMaxHP;
		if (hpColor >= 0.5f)
			hpColor = 0.0f; // green
		else if (hpColor >= 0.25f)
			hpColor = 0.25f; // yellow
		else
			hpColor = 0.5f; // red

		length = length > 1.0f ? 1.0f : length;
		D3DXVECTOR3 objPos = ppObjects[i]->GetPosition();
		if (!isAirObject(ppObjects[i]->m_nObjectType) && isAnimateObject(ppObjects[i]->m_nObjectType))
			objPos.y += ppObjects[i]->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f;
		objPos.z += ppObjects[i]->m_bcMeshBoundingCube.m_d3dxvMaximum.z * 2.0f;
		vLB.x = objPos.x - barWidth / 2.0f;	vLB.y = objPos.y; vLB.z = objPos.z - barHeight / 2.0f;
		vRT.x = vLB.x + (length * barWidth);	vRT.y = objPos.y; vRT.z = objPos.z + barHeight / 2.0f;

		pVertices[0 + i * 6].posColor = D3DXVECTOR4(vLB.x, vLB.y, vLB.z, 1.0f);		//LB
		pVertices[1 + i * 6].posColor = D3DXVECTOR4(vLB.x, vRT.y, vRT.z, 1.0f);		//LT
		pVertices[2 + i * 6].posColor = D3DXVECTOR4(vRT.x, vRT.y, vRT.z, 1.0f);			//RT
		pVertices[3 + i * 6].posColor = D3DXVECTOR4(vRT.x, vRT.y, vRT.z, 1.0f);		//RT
		pVertices[4 + i * 6].posColor = D3DXVECTOR4(vRT.x, vLB.y, vLB.z, 1.0f);		//RB
		pVertices[5 + i * 6].posColor = D3DXVECTOR4(vLB.x, vLB.y, vLB.z, 1.0f);	//LB

																				//프레임 텍스쳐는 0.5f부터 시작
		pVertices[0 + i * 6].texCoord = D3DXVECTOR2(hpColor, 1.0f);
		pVertices[1 + i * 6].texCoord = D3DXVECTOR2(hpColor, 0.0f);
		pVertices[2 + i * 6].texCoord = D3DXVECTOR2(hpColor + 0.25f, 0.0f);
		pVertices[3 + i * 6].texCoord = D3DXVECTOR2(hpColor + 0.25f, 0.0f);
		pVertices[4 + i * 6].texCoord = D3DXVECTOR2(hpColor + 0.25f, 1.0f);
		pVertices[5 + i * 6].texCoord = D3DXVECTOR2(hpColor, 1.0f);
	}
	pd3dDeviceContext->Unmap(m_pd3dHPBuffer, 0);

	pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dHPBuffer, vertexStrides, vertexOffsets);
	pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dDeviceContext->Draw(6 * nValidObject, 0);
}