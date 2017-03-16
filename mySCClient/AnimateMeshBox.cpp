#include "stdafx.h"
#include "AnimateMeshBox.h"


CAnimationMeshBox::CAnimationMeshBox(ID3D11Device* pd3dDevice)
{
}


CAnimationMeshBox::~CAnimationMeshBox()
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
//애니메이션 메시 모두를 불러와서 map에 저장합니다.
void CAnimationMeshBox::LoadModelMesh(ID3D11Device* pd3dDevice)
{
	////////////////////////////////////////////////////////////////////////////////
	//모델 정보 생성
	CAnimationMesh* pMesh = nullptr;
	CAnimationMesh* copyMesh = nullptr;
	D3DXVECTOR3 vScale = D3DXVECTOR3(1.5f, 1.5f, 1.5f);
	//D3DXVECTOR3 vScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//Scv
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/scv.data", vScale, pd3dDevice, D3DXVECTOR3(0,0,0), 0.75f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Scv, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Scv_Enemy, copyMesh));
	copyMesh->AddRef();
	
	//Marine
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/marine.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 0.75f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Marine, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Marine_Enemy, copyMesh));
	copyMesh->AddRef();

	//Reaper
	vScale = D3DXVECTOR3(1.25f, 1.25f, 1.25f);
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/reaper.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 0.75f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Reaper, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Reaper_Enemy, copyMesh));
	copyMesh->AddRef();

	//Marauder
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/marauder.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 1.25f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Marauder, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Marauder_Enemy, copyMesh));
	copyMesh->AddRef();

	/*
	//Ghost
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/ghost.data", vScale, pd3dDevice);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Ghost, pMesh));
	pMesh->AddRef();
	*/

#ifndef DEBUG_SIMPLE_LOAD

	//Medivac
	vScale = D3DXVECTOR3(0.75f, 0.75f, 0.75f);
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/Medivac.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 1.8f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Medivac, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Medivac_Enemy, copyMesh));
	copyMesh->AddRef();

	//Banshee
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/Banshee.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 1.8f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Banshee, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Banshee_Enemy, copyMesh));
	copyMesh->AddRef();

	//Battlecruiser
	vScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/BattleCruiser.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 2.3f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::BattleCruiser, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::BattleCruiser_Enemy, copyMesh));
	copyMesh->AddRef();

	//Mule
	vScale = D3DXVECTOR3(0.75f, 0.75f, 0.75f);
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/Mule.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 1.5f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Mule, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Mule_Enemy, copyMesh));
	copyMesh->AddRef();

	//Raven
	vScale = D3DXVECTOR3(1.25f, 1.25f, 1.25f);
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/Raven.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 1.8f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Raven, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Raven_Enemy, copyMesh));
	copyMesh->AddRef();

	//Thor
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/Thor.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 2.2f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Thor, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Thor_Enemy, copyMesh));
	copyMesh->AddRef();

	//Viking
	vScale = D3DXVECTOR3(0.008f, 0.008f, 0.008f);
	pMesh = new CAnimationMesh(pd3dDevice);
	pMesh->LoadAnimateObjectFromFBX("Assets/ModelFile/Unit/Viking.data", vScale, pd3dDevice, D3DXVECTOR3(0, 0, 0), 1.8f);
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Viking, pMesh));
	pMesh->AddRef();
	copyMesh = pMesh->CreateAndCopyMeshData();
	m_mapMeshBox.insert(make_pair((int)OBJECT_TYPE::Viking_Enemy, copyMesh));
	copyMesh->AddRef();

	vScale = D3DXVECTOR3(1.5f, 1.5f, 1.5f);

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

	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	CTexture *pTexture = nullptr;
	CMATERIAL* pMaterial = nullptr;

	D3DXCOLOR matDif = D3DXCOLOR(0.59f, 0.59f, 0.59f, 1.0f);
	D3DXCOLOR matAmb = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
	D3DXCOLOR matSpe = D3DXCOLOR(0.9f, 0.9f, 0.9f, 20.0f);
	D3DXCOLOR matEmi = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	pMaterial = new CMATERIAL{ matAmb, matDif, matEmi, matSpe };

	//Scv
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/scv_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Scv, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Scv, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Scv_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/scv_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Scv_Enemy, pTexture));	
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Scv_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();


	//Marine
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Marine_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Marine, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Marine, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Marine_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Marine_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Marine_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Marine_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Reaper
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Reaper_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Reaper, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Reaper, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Reaper_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Reaper_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Reaper_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Reaper_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Marauder
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Marauder_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Marauder, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Marauder, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Marauder_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Marauder_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Marauder_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Marauder_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	/*
	//GHOST
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/ghost_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Ghost, pTexture));
	pTexture->AddRef();

	matDif = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	matAmb = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	matSpe = D3DXCOLOR(0.9f, 0.9f, 0.9f, 20.0f);
	matEmi = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
	pMaterial = new CMATERIAL{ matAmb, matDif, matEmi, matSpe };
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Ghost, pMaterial));
	pMaterial->AddRef();
	*/

#ifndef DEBUG_SIMPLE_LOAD

	//Viking
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Viking_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Viking, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Viking, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Viking_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Viking_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Viking_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Viking_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Banshee
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Banshee_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Banshee, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Banshee, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Banshee_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Banshee_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Banshee_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Banshee_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//BattleCruiser
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/BattleCruiser_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::BattleCruiser, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::BattleCruiser, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//BattleCruiser_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/BattleCruiser_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::BattleCruiser_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::BattleCruiser_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Medivac
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Medivac_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Medivac, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Medivac, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Medivac_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Medivac_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Medivac_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Medivac_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Mule
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Mule_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Mule, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Mule, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Mule_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Mule_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Mule_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Mule_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Raven
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Raven_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Raven, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Raven, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Raven_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Raven_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Raven_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Raven_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

	//Thor
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Thor_diffuse.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Thor, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Thor, pMaterial));
	pTexture->AddRef(); pMaterial->AddRef();
	//Thor_Enemy Texture
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/ModelImage/Unit/Thor_enemy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapTextureBox.insert(make_pair((int)OBJECT_TYPE::Thor_Enemy, pTexture));
	m_mapMaterialBox.insert(make_pair((int)OBJECT_TYPE::Thor_Enemy, pMaterial));
	pMaterial->AddRef(); pTexture->AddRef();

#endif

	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();

	LoadSpecularMap(pd3dDevice);
	LoadNormalMap(pd3dDevice);
}

///////////////////////////////////////////////////////////////////////
//1213 유닛 스펙큘러 맵
void CAnimationMeshBox::LoadSpecularMap(ID3D11Device* pd3dDevice)
{
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

	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	CTexture *pTexture = nullptr;

	//Scv
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/scv_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Scv, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Scv_Enemy, pTexture)); pTexture->AddRef();

	//Marine
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Marine_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Marine, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Marine_Enemy, pTexture)); pTexture->AddRef();

	//Reaper
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Reaper_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Reaper, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Reaper_Enemy, pTexture)); pTexture->AddRef();

	//Marauder
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Marauder_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Marauder, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Marauder_Enemy, pTexture)); pTexture->AddRef();

	//Viking
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Viking_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Viking, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Viking_Enemy, pTexture)); pTexture->AddRef();

	//Banshee
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Banshee_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Banshee, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Banshee_Enemy, pTexture)); pTexture->AddRef();

	//Battlecruiser
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Battlecruiser_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::BattleCruiser, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::BattleCruiser_Enemy, pTexture)); pTexture->AddRef();

	//Medivac
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Medivac_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Medivac, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Medivac_Enemy, pTexture)); pTexture->AddRef();

	//Raven
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Raven_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Raven, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Raven_Enemy, pTexture)); pTexture->AddRef();

	//Thor
	pTexture = new CTexture(1, 1, PS_SLOT_SPECULAR_TEXTURE, PS_SLOT_SPECULAR_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/SpecularMap/Thor_specular.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Thor, pTexture)); pTexture->AddRef();
	m_mapSpecularMapBox.insert(make_pair((int)OBJECT_TYPE::Thor_Enemy, pTexture)); pTexture->AddRef();

	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();
}

///////////////////////////////////////////////////////////////////////
//1214 유닛 노말 맵
void CAnimationMeshBox::LoadNormalMap(ID3D11Device* pd3dDevice)
{

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

	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	CTexture *pTexture = nullptr;

	//Scv
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/scv_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Scv, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Scv_Enemy, pTexture)); pTexture->AddRef();

	//Marine
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Marine_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Marine, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Marine_Enemy, pTexture)); pTexture->AddRef();

	//Reaper
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Reaper_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Reaper, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Reaper_Enemy, pTexture)); pTexture->AddRef();

	//Marauder
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Marauder_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Marauder, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Marauder_Enemy, pTexture)); pTexture->AddRef();

	//Viking
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Viking_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Viking, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Viking_Enemy, pTexture)); pTexture->AddRef();

	//Banshee
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Banshee_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Banshee, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Banshee_Enemy, pTexture)); pTexture->AddRef();

	//Battlecruiser
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Battlecruiser_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::BattleCruiser, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::BattleCruiser_Enemy, pTexture)); pTexture->AddRef();

	//Medivac
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Medivac_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Medivac, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Medivac_Enemy, pTexture)); pTexture->AddRef();

	//Raven
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Raven_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Raven, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Raven_Enemy, pTexture)); pTexture->AddRef();

	//Thor
	pTexture = new CTexture(1, 1, PS_SLOT_NORMAL_TEXTURE, PS_SLOT_NORMAL_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/NormalMap/Thor_normal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture); pTexture->SetSampler(0, pd3dSamplerState);
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Thor, pTexture)); pTexture->AddRef();
	m_mapNormalMapBox.insert(make_pair((int)OBJECT_TYPE::Thor_Enemy, pTexture)); pTexture->AddRef();

	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();
}