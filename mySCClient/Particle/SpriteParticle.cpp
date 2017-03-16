#include "../Shader.h"
#include "SpriteParticle.h"

CSpriteParticle* CSpriteParticle::instance = nullptr;

CSpriteParticle::CSpriteParticle()
{
	m_pd3dVertexShader = nullptr;
	m_pd3dVertexLayout = nullptr ;
	m_pd3dGeometryShader = nullptr;
	m_pd3dPixelShader = nullptr;

	m_pd3dRasterizerState = nullptr;
	m_pd3dParticleSampler = nullptr;
	
	m_pd3dParticlePositionBuffer = nullptr;
	
	m_mapParticleData.clear();
}


CSpriteParticle::~CSpriteParticle()
{
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if (m_pd3dGeometryShader) m_pd3dGeometryShader->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();

	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
	if (m_pd3dParticleSampler) m_pd3dParticleSampler->Release();

	if (m_pd3dParticlePositionBuffer) m_pd3dParticlePositionBuffer->Release();

	for (auto& d : m_mapParticleData)
		d.second.DeleteParticleData();
	m_mapParticleData.clear();
}


void CSpriteParticle::CreateShader(ID3D11Device *pd3dDevice)
{
	//time���� ���Ͽ� ��������Ʈ �̹����� uv��ǥ�� ��� �������Ѵ�.
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "ELAPSEDTIME", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NUMSPRITES", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "BETWEENTIME", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSSpriteParticle", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CShader::CreateGeometryShaderFromFile(pd3dDevice, "Effect.fx", "GSSpriteParticle", "gs_5_0", &m_pd3dGeometryShader);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSSpriteParticle", "ps_5_0", &m_pd3dPixelShader);
}

void CSpriteParticle::BuildObject(ID3D11Device *pd3dDevice)
{
	//�������� ���� -> 1 ����;
	D3DXVECTOR3 vTemp{ 0,0,0 };
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3);
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = &vTemp;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dParticlePositionBuffer);

	//���÷��� �����̹Ƿ� ����
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &m_pd3dParticleSampler);
	
	/*
	//�׽�Ʈ�� RS
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.CullMode = D3D11_CULL_BACK;
	rd.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&rd, &m_pd3dRasterizerState);
	*/

	
}



void CSpriteParticle::AddProjectileParticle(string particleName, CProjectileParticleObject particle)
{
	auto p = m_mapParticleData.find(particleName);
	if (p != m_mapParticleData.end())
	{
		particle.m_fProjectileSpeed = p->second.m_fProjectileSpeed;
		p->second.m_listProjectileParticleObjects.push_back(particle);
	}
}

void CSpriteParticle::AddPromptParticle(string particleName, CPromptParticleObject particle)
{
	auto p = m_mapParticleData.find(particleName);
	if (p != m_mapParticleData.end())
	{
		particle.m_fMaxParticleTime = p->second.m_fPromptMaintainTime;
		p->second.m_listPromptParticleObjects.push_back(particle);
	}
}

void CSpriteParticle::AddParticleData(string particleName, PARTICLE_DATA particleData)
{
	m_mapParticleData.insert(make_pair(particleName, particleData));	
}



void CSpriteParticle::Update(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	VS_SPRITE_PARTICLE_INSTANCE_DATA *pInstances;

	for(auto& pData : m_mapParticleData)
	{
		if (pData.second.m_listProjectileParticleObjects.empty() && pData.second.m_listPromptParticleObjects.empty())
			continue;

		pd3dDeviceContext->Map(pData.second.m_pd3dParticleInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		pInstances = (VS_SPRITE_PARTICLE_INSTANCE_DATA *)d3dMappedResource.pData;

		if (pData.second.m_bProjectile)
		{
			//1. ����ð��� �����Ͽ� ��ȿ�� ��ƼŬ�� ��������.
			list<CProjectileParticleObject> hitParticleObjectList;
			list<CProjectileParticleObject> newParticleObjectList;
			for (auto& d : pData.second.m_listProjectileParticleObjects)
			{
				if (d.UpdateAndGetValid(fTimeElapsed))
					newParticleObjectList.push_back(d);
				else
					hitParticleObjectList.push_back(d);
			}

			//2. ��ȿ�� ��ƼŬ�� �������ϵ��� Map�����͸� ����Ѵ�.
			pData.second.m_listProjectileParticleObjects = newParticleObjectList;
			int nObjects = 0;
			for (auto d : pData.second.m_listProjectileParticleObjects)
			{
				pInstances[nObjects].mtxWorld = d.m_d3dxmtxWorld; //row major
				pInstances[nObjects].elapsedTime = d.m_fParticleElapsedTime;
				pInstances[nObjects].numSprites = pData.second.m_fNumSprites;
				pInstances[nObjects].betweenTime = pData.second.m_fBetweenTime;
				pInstances[nObjects].particleSize = pData.second.m_fParticleSize;
				nObjects++;
			}

			//3. Ÿ�ݵ� ����Ʈ�κ��� ���� ��Ŷ�� �����Ѵ�.
			for (auto d : hitParticleObjectList)
			{
				//d.m_pMyObject->CS_Packet_Attack(d.m_pTargetObject);
			}
		}
		else
		{
			//1. ����ð��� �����Ͽ� ��ȿ�� ��ƼŬ�� ��������.
			list<CPromptParticleObject> newParticleObjectList;
			for (auto& d : pData.second.m_listPromptParticleObjects)
			{
				if (d.UpdateAndGetValid(fTimeElapsed))
					newParticleObjectList.push_back(d);
			}

			//2. ��ȿ�� ��ƼŬ�� �������ϵ��� Map�����͸� ����Ѵ�.
			//��� ��ƼŬ�� 2 ���� �̹����� �׷����� �Ѵ�.
			pData.second.m_listPromptParticleObjects = newParticleObjectList;
			int nObjects = 0;
			for (auto d : pData.second.m_listPromptParticleObjects)
			{
				D3DXMATRIX mtxWorld = d.m_d3dxmtxWorld;
				pInstances[nObjects].mtxWorld = mtxWorld; //row major
				pInstances[nObjects].elapsedTime = d.m_fParticleElapsedTime;
				pInstances[nObjects].numSprites = pData.second.m_fNumSprites;
				pInstances[nObjects].betweenTime = pData.second.m_fBetweenTime;
				pInstances[nObjects].particleSize = pData.second.m_fParticleSize;
				nObjects++;

				//����� �ٿ���ڽ� �� ������ ��ġ�� ��ƼŬ�� �߻��ϵ��� �Ѵ�.
				if(d.m_pTargetObject)
				{
					mtxWorld._41 = d.m_pTargetObject->m_d3dxmtxWorld._41 + d.m_fRandXOffset;
					mtxWorld._42 = d.m_pTargetObject->m_d3dxmtxWorld._42 + d.m_fRandYOffset;
					mtxWorld._43 = d.m_pTargetObject->m_d3dxmtxWorld._43 + d.m_fRandZOffset;

					pInstances[nObjects].mtxWorld = mtxWorld; //row major
					pInstances[nObjects].elapsedTime = d.m_fParticleElapsedTime;
					pInstances[nObjects].numSprites = pData.second.m_fNumSprites;
					pInstances[nObjects].betweenTime = pData.second.m_fBetweenTime;
					pInstances[nObjects].particleSize = pData.second.m_fParticleSize;
					nObjects++;
				}
				else //pTarget is nullptr
				{
					pInstances[nObjects].mtxWorld = mtxWorld; //row major
					pInstances[nObjects].elapsedTime = d.m_fParticleElapsedTime;
					pInstances[nObjects].numSprites = pData.second.m_fNumSprites;
					pInstances[nObjects].betweenTime = pData.second.m_fBetweenTime;
					pInstances[nObjects].particleSize = pData.second.m_fParticleSize;
					nObjects++;
				}
			}
		}
		pd3dDeviceContext->Unmap(pData.second.m_pd3dParticleInstanceBuffer, 0);
	}
}

void CSpriteParticle::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//�������� �κ�
	pd3dDeviceContext->PSSetSamplers(PS_SLOT_PARTICLE, 1, &m_pd3dParticleSampler);

	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->GSSetShader(m_pd3dGeometryShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
	pd3dDeviceContext->HSSetShader(nullptr, NULL, 0);
	pd3dDeviceContext->DSSetShader(nullptr, NULL, 0);


	for(auto& pData : m_mapParticleData)
	{
		if (pData.second.m_listProjectileParticleObjects.empty() && pData.second.m_listPromptParticleObjects.empty())
			continue;

		pd3dDeviceContext->PSSetShaderResources(PS_SLOT_PARTICLE, 1, &pData.second.m_pd3dparticleTextureSRV);
	
		ID3D11Buffer* vertexBuffers[2] = { m_pd3dParticlePositionBuffer, pData.second.m_pd3dParticleInstanceBuffer };
		UINT vertexStride[2] = { sizeof(D3DXVECTOR3), sizeof(VS_SPRITE_PARTICLE_INSTANCE_DATA) };
		UINT vertexOffset[2] = { 0, 0 };
		pd3dDeviceContext->IASetVertexBuffers(0, 2, vertexBuffers, vertexStride, vertexOffset);
		pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		pd3dDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
		//pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);
		//pd3dDeviceContext->RSSetState(nullptr);

		int nInstance = pData.second.m_bProjectile ? pData.second.m_listProjectileParticleObjects.size() : 
																	(pData.second.m_listPromptParticleObjects.size() * 2);
		pd3dDeviceContext->DrawInstanced(1, nInstance, 0, 0);
	}
}