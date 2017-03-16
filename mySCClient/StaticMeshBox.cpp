#include "stdafx.h"
#include "StaticMeshBox.h"



CStaticMeshBox::CStaticMeshBox(ID3D11Device* pd3dDevice)
{
}


CStaticMeshBox::~CStaticMeshBox()
{
	for (auto d : m_mapMeshBox)
		if (d.second) d.second->Release();
	m_mapMeshBox.clear();

	for (auto d : m_mapMaterialBox)
		if (d.second) d.second->Release();
	m_mapMaterialBox.clear();

	for (auto d : m_mapTextureBox)
		if (d.second) d.second->Release();
	m_mapTextureBox.clear();


	for (auto d : m_mapSpecularMapBox)
		if (d.second) d.second->Release();
	m_mapSpecularMapBox.clear();

	for (auto d : m_mapNormalMapBox)
		if (d.second) d.second->Release();
	m_mapNormalMapBox.clear();
}


//**********************************************************************
//정적 메시 모두를 불러와서 map에 저장합니다.
void CStaticMeshBox::LoadModelMesh(ID3D11Device* pd3dDevice)
{
	////////////////////////////////////////////////////////////////////////////////
	//모델 정보 생성
	CStaticMesh* pMesh = nullptr;
	CStaticMesh* copyMesh = nullptr;
	D3DXVECTOR3 vScale = D3DXVECTOR3(1.5f, 1.5f, 1.5f);
	
	vScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//Commandcenter
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Commandcenter.data", vScale, pd3dDevice, D3DXVECTOR3(0,0,0), 4.0f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Commandcenter, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Commandcenter_Enemy, copyMesh));
	copyMesh->AddRef();

	//Supplydepot
	vScale = D3DXVECTOR3(1.25f, 1.25f, 1.25f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Supplydepot.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 2.5f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Supplydepot, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Supplydepot_Enemy, copyMesh));
	copyMesh->AddRef();

	//Refinery
	vScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Refinery.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 3.0f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Refinery, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Refinery_Enemy, copyMesh));
	copyMesh->AddRef();

	vScale = D3DXVECTOR3(0.95f, 0.95f, 0.95f);
	//Barrack
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Barrack.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 3.5f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Barrack, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Barrack_Enemy, copyMesh));
	copyMesh->AddRef();

	//Engineeringbay
	vScale = D3DXVECTOR3(0.95f, 0.95f, 0.95f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Engineeringbay.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 3.5f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Engineeringbay, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Engineeringbay_Enemy, copyMesh));
	copyMesh->AddRef();

	//Ghostacademy
	vScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Ghostacademy.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 3.0f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Ghostacademy, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Ghostacademy_Enemy, copyMesh));
	copyMesh->AddRef();

#ifndef DEBUG_SIMPLE_LOAD

	//Armory
	vScale = D3DXVECTOR3(1.1f, 1.1f, 1.1f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Armory.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 2.5f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Armory, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Armory_Enemy, copyMesh));
	copyMesh->AddRef();

	//Bunker
	vScale = D3DXVECTOR3(1.15f, 1.15f, 1.15f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Bunker.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 2.0f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Bunker, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Bunker_Enemy, copyMesh));
	copyMesh->AddRef();
	
	//Factory
	vScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Factory.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 3.5f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Factory, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Factory_Enemy, copyMesh));
	copyMesh->AddRef();

	//Fusioncore
	vScale = D3DXVECTOR3(0.9f, 0.9f, 0.9f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Fusioncore.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 3.0f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Fusioncore, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Fusioncore_Enemy, copyMesh));
	copyMesh->AddRef();

	//Missileturret
	vScale = D3DXVECTOR3(1.15f, 1.15f, 1.15f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Missileturret.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 2.0f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Missileturret, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Missileturret_Enemy, copyMesh));
	copyMesh->AddRef();
	
	vScale = D3DXVECTOR3(0.875f, 0.875f, 0.875f);
	//Reactor
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Reactor.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 2.0f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Reactor, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Reactor_Enemy, copyMesh));
	copyMesh->AddRef();

	//Sensortower
	vScale = D3DXVECTOR3(1.3f, 1.3f, 1.3f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Sensortower.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 2.0f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Sensortower, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Sensortower_Enemy, copyMesh));
	copyMesh->AddRef();
	
	//Starport
	vScale = D3DXVECTOR3(0.95f, 0.95f, 0.95f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Starport.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 3.5f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Starport, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Starport_Enemy, copyMesh));
	copyMesh->AddRef();

	//Techlab
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Building/Techlab.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 2.0f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Techlab, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Techlab_Enemy, copyMesh));
	copyMesh->AddRef();

	//Mineral
	vScale = D3DXVECTOR3(1.5f, 1.5f, 1.5f);
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Neutral/Mineral.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 1.8f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Mineral, pMesh));
	pMesh->AddRef();

	//Gas
	pMesh = new CStaticMesh(pd3dDevice);
	pMesh->LoadStaticObjectFromFBX("Assets/ModelFile/Neutral/Gas.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 2.3f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Gas, pMesh));
	pMesh->AddRef();

#endif

	////////////////////////////////////////////////////////////////////////////////
	//텍스쳐 및 재질 생성
	ID3D11SamplerState *pd3dSamplerState = nullptr;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	CTexture *pTexture = nullptr;
	CMATERIAL* pMaterial = nullptr;
	ID3D11ShaderResourceView *pd3dsrvTexture = nullptr;

	D3DXCOLOR matDif = D3DXCOLOR(0.59f, 0.59f, 0.59f, 1.0f);
	D3DXCOLOR matAmb = D3DXCOLOR(0.59f, 0.59f, 0.59f, 1.0f);
	D3DXCOLOR matSpe = D3DXCOLOR(0.8f, 0.8f, 0.8f, 20.0f);
	D3DXCOLOR matEmi = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	pMaterial = new CMATERIAL{ matAmb, matDif, matEmi, matSpe };


	//commandcenter_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/commandcenter_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Commandcenter, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Commandcenter, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();
	//commandcenter_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/commandcenter_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Commandcenter_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Commandcenter_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Supplydepot_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Supplydepot_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Supplydepot, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Supplydepot, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Supplydepot_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Supplydepot_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Supplydepot_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Supplydepot_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Refinery_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Refinery_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Refinery, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Refinery, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Refinery_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Refinery_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Refinery_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Refinery_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Barrack_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Barrack_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Barrack, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Barrack, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Barrack_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Barrack_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Barrack_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Barrack_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Engineeringbay_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Engineeringbay_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Engineeringbay, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Engineeringbay, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Engineeringbay_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Engineeringbay_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Engineeringbay_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Engineeringbay_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Ghostacademy_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Ghostacademy_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Ghostacademy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Ghostacademy, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Ghostacademy_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Ghostacademy_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Ghostacademy_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Ghostacademy_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();


#ifndef DEBUG_SIMPLE_LOAD

	//Armory_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Armory_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Armory, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Armory, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Armory_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Armory_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Armory_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Armory_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Bunker_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Bunker_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Bunker, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Bunker, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Bunker_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Bunker_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Bunker_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Bunker_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Factory_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Factory_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Factory, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Factory, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Factory_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Factory_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Factory_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Factory_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Fusioncore_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Fusioncore_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Fusioncore, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Fusioncore, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Fusioncore_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Fusioncore_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Fusioncore_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Fusioncore_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Missileturret_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Missileturret_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Missileturret, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Missileturret, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Missileturret_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Missileturret_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Missileturret_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Missileturret_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Reactor_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Reactor_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Reactor, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Reactor, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Reactor_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Reactor_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Reactor_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Reactor_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Sensortower_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Sensortower_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Sensortower, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Sensortower, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Sensortower_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Sensortower_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Sensortower_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Sensortower_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Starport_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Starport_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Starport, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Starport, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Starport_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Starport_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Starport_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Starport_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Techlab_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Techlab_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Techlab, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Techlab, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Techlab_enemy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Building/Techlab_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Techlab_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Techlab_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//mineral_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Neutral/mineral_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Mineral, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Mineral, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();

	//gas_diffuse
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Neutral/gas_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Gas, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Gas, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();

#endif
	
	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();

	LoadSpecularMap(pd3dDevice);
	LoadNormalMap(pd3dDevice);
}


///////////////////////////////////////////////////////////////////////
//1213 건물 스펙큘러 맵
void CStaticMeshBox::LoadSpecularMap(ID3D11Device* pd3dDevice)
{
	ID3D11SamplerState *pd3dSamplerState = nullptr;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	CTexture *pTexture = nullptr;
	ID3D11ShaderResourceView *pd3dsrvTexture = nullptr;

	//Commandcenter
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/commandcenter_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Commandcenter, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Commandcenter_Enemy, pTexture)); pTexture->AddRef();
	
	//Supplydepot
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/supplydepot_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Supplydepot, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Supplydepot_Enemy, pTexture)); pTexture->AddRef();
	
	//Refinery
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Refinery_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Refinery, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Refinery_Enemy, pTexture)); pTexture->AddRef();

	//Barrack
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Barrack_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Barrack, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Barrack_Enemy, pTexture)); pTexture->AddRef();

	//Engineeringbay
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Engineeringbay_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Engineeringbay, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Engineeringbay_Enemy, pTexture)); pTexture->AddRef();

	//Ghostacademy
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Ghostacademy_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Ghostacademy, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Ghostacademy_Enemy, pTexture)); pTexture->AddRef();

	//Armory
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Armory_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Armory, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Armory_Enemy, pTexture)); pTexture->AddRef();

	//Bunker
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Bunker_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Bunker, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Bunker_Enemy, pTexture)); pTexture->AddRef();

	//Factory
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Factory_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Factory, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Factory_Enemy, pTexture)); pTexture->AddRef();

	//Fusioncore
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Fusioncore_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Fusioncore, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Fusioncore_Enemy, pTexture)); pTexture->AddRef();
	
	//Missileturret
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Missileturret_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Missileturret, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Missileturret_Enemy, pTexture)); pTexture->AddRef();
	
	//Sensortower
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Sensortower_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Sensortower, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Sensortower_Enemy, pTexture)); pTexture->AddRef();

	//Starport
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Starport_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Starport, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Starport_Enemy, pTexture)); pTexture->AddRef();

	//Mineral
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Mineral_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Mineral, pTexture)); pTexture->AddRef();
	
	//Gas
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Gas_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Gas, pTexture)); pTexture->AddRef();


	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();
}


///////////////////////////////////////////////////////////////////////
//1214 건물 노말 맵
void CStaticMeshBox::LoadNormalMap(ID3D11Device* pd3dDevice)
{
	ID3D11SamplerState *pd3dSamplerState = nullptr;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	CTexture *pTexture = nullptr;
	ID3D11ShaderResourceView *pd3dsrvTexture = nullptr;

	//Commandcenter
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/commandcenter_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Commandcenter, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Commandcenter_Enemy, pTexture)); pTexture->AddRef();

	//Supplydepot
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/supplydepot_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Supplydepot, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Supplydepot_Enemy, pTexture)); pTexture->AddRef();

	//Refinery
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Refinery_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Refinery, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Refinery_Enemy, pTexture)); pTexture->AddRef();

	//Barrack
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Barrack_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Barrack, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Barrack_Enemy, pTexture)); pTexture->AddRef();

	//Engineeringbay
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Engineeringbay_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Engineeringbay, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Engineeringbay_Enemy, pTexture)); pTexture->AddRef();

	//Ghostacademy
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Ghostacademy_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Ghostacademy, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Ghostacademy_Enemy, pTexture)); pTexture->AddRef();

	//Armory
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Armory_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Armory, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Armory_Enemy, pTexture)); pTexture->AddRef();

	//Bunker
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Bunker_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Bunker, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Bunker_Enemy, pTexture)); pTexture->AddRef();

	//Factory
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Factory_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Factory, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Factory_Enemy, pTexture)); pTexture->AddRef();

	//Fusioncore
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Fusioncore_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Fusioncore, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Fusioncore_Enemy, pTexture)); pTexture->AddRef();

	//Missileturret
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Missileturret_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Missileturret, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Missileturret_Enemy, pTexture)); pTexture->AddRef();

	//Sensortower
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Sensortower_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Sensortower, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Sensortower_Enemy, pTexture)); pTexture->AddRef();

	//Starport
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Starport_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Starport, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Starport_Enemy, pTexture)); pTexture->AddRef();

	//Mineral
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Mineral_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Mineral, pTexture)); pTexture->AddRef();

	//Gas
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Gas_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Gas, pTexture)); pTexture->AddRef();


	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();
}