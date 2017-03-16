#include "stdafx.h"
#include "Scene.h"

//��Ŷ ��ſ�
#include "GameFramework.h"
#include "GlobalFunctions.h"

extern int gGameMineral;
extern int gGameGas;
extern int nPacketCount;
extern void* gGameCamera;

//**********************************************************************
CScene::CScene(void* pGameFramework)
{
	m_pGameFramework = (CGameFramework*)pGameFramework;

	m_ppShaders = nullptr;
	m_nShaders = 0;

	m_pCamera = nullptr;

	m_pLights = nullptr;
	m_pd3dcbLights = nullptr;

	//���콺 �̺�Ʈ
	m_pMouseRectTexture = nullptr;
	m_pMouseRectMesh = nullptr;

	m_pd3dMouseRectVertexShader = nullptr;
	m_pd3dMouseRectVertexLayout = nullptr;
	m_pd3dMouseRectPixelShader = nullptr;

	m_bLeftClicked = false;
	m_bMouseRectDraw = false;

	//���õ� ��ü ���� �̺�Ʈ
	m_pSelectedCircleTexture = nullptr;
	m_pSelectedCircleMesh = nullptr;
	m_pd3dSelectedCircleVertexShader = nullptr;
	m_pd3dSelectedCircleVertexLayout = nullptr;
	m_pd3dSelectedCircleGeometryShader = nullptr;
	m_pd3dSelectedCirclePixelShader = nullptr;

	//UI ���� �̹��� map
	m_mapCommandTexture.clear();
	m_mapPortraitTexture.clear();
	m_mapStatusTexture.clear();

	m_mapCommandMesh.clear();

	m_nValidSelectedObjects = 0;
	for (auto i = 0; i < MAX_SELECTED_OBJECTS; ++i)
		m_ppSelectedObjects[i] = nullptr;

	m_pStaticShader = nullptr;
	m_pAnimationShader = nullptr;

	//��ã��
	m_pAStarComponent = nullptr;

	//����׿�
	m_pDebugPrograms = nullptr;

	//��ƼŬ
	m_pParticleBox = nullptr;

	//����
	m_pSoundManager = nullptr;

	//��Ʈ
	m_pNumberFont = nullptr;

	//�ǹ� ����
	m_pBuildCheck = nullptr;

	//Ŀ�ǵ� â ��ġ
	m_mapCommandPosition.clear();
	
	//���� ���� ǥ�� ��ġ
	m_mapCrowdPosition.clear();

	//���α׷��� ��
	m_pProgressIconMesh = nullptr;
	m_pProgressImage = nullptr;

	//���콺 �̹���
	m_pMouseImageTexture = nullptr;
	m_pMouseImageMesh = nullptr;

	//���� ���� �̹���
	m_pCrowdFrameTexture = nullptr;
	m_mapCrowdUnitTexture.clear();
	m_vecCrowdMeshes.clear();
	m_pd3dCrowdImageVertexShader = nullptr;
	m_pd3dCrowdImageVertexLayout = nullptr;
	m_pd3dCrowdImagePixelShader = nullptr;
	
	m_mapAttachedCrowdPosition.clear();
	m_pCrowdAttachedFrameTexture = nullptr;
	m_vecAttachedCrowdMeshes.clear();

	m_mapIsBuildingExist.clear();
}

CScene::~CScene()
{
	m_pd3dDeviceContext = nullptr;

	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
	ReleaseObjects();

	//HeightMapTerrain �Ҹ��ڿ��� ����
	//if (m_pHeightMap) delete m_pHeightMap;

	if (m_pd3dUIDepthStencilState) m_pd3dUIDepthStencilState->Release();

	//���콺 �巡��
	if (m_pMouseRectMesh) m_pMouseRectMesh->Release();
	if (m_pMouseRectTexture) m_pMouseRectTexture->Release();
	if (m_pd3dMouseRectVertexShader) m_pd3dMouseRectVertexShader->Release();
	if (m_pd3dMouseRectVertexLayout) m_pd3dMouseRectVertexLayout->Release();
	if (m_pd3dMouseRectPixelShader)  m_pd3dMouseRectPixelShader->Release();

	//���õ� ������Ʈ��
	if (m_pSelectedCircleTexture) m_pSelectedCircleTexture->Release();
	if (m_pSelectedCircleMesh) m_pSelectedCircleMesh->Release();
	if (m_pd3dSelectedCircleVertexShader) m_pd3dSelectedCircleVertexShader->Release();
	if (m_pd3dSelectedCircleVertexLayout) m_pd3dSelectedCircleVertexLayout->Release();
	if (m_pd3dSelectedCircleGeometryShader) m_pd3dSelectedCircleGeometryShader->Release();
	if (m_pd3dSelectedCirclePixelShader) m_pd3dSelectedCirclePixelShader->Release();

	//UI ���� ������
	if (m_pMainFrameTexture) m_pMainFrameTexture->Release();
	if (m_pMainFrameMesh) m_pMainFrameMesh->Release();
	if (m_pd3dMainFrameVertexShader) m_pd3dMainFrameVertexShader->Release();
	if (m_pd3dMainFrameVertexLayout) m_pd3dMainFrameVertexLayout->Release();
	if (m_pd3dMainFramePixelShader) m_pd3dMainFramePixelShader->Release();

	//AttachedMainFrame
	if (m_pd3dAttachedMainFrameVertexShader) m_pd3dAttachedMainFrameVertexShader->Release();
	if (m_pd3dAttachedMainFrameVertexLayout) m_pd3dAttachedMainFrameVertexLayout->Release();
	if (m_pd3dAttachedMainFramePixelShader) m_pd3dAttachedMainFramePixelShader->Release();

	//UI �̴ϸ�
	if (m_pMiniMapTexture) m_pMiniMapTexture->Release();
	if (m_pMiniMapMesh) m_pMiniMapMesh->Release();
	if (m_pd3dMiniMapVertexShader) m_pd3dMiniMapVertexShader->Release();
	if (m_pd3dMiniMapVertexLayout) m_pd3dMiniMapVertexLayout->Release();
	if (m_pd3dMiniMapPixelShader) m_pd3dMiniMapPixelShader->Release();

	

	//�̴ϸ� ��ü
	if (m_pMiniMapObjectMesh) m_pMiniMapObjectMesh->Release();
	if (m_pd3dMiniMapObjectVertexShader) m_pd3dMiniMapObjectVertexShader->Release();
	if (m_pd3dMiniMapObjectVertexLayout) m_pd3dMiniMapObjectVertexLayout->Release();
	if (m_pd3dMiniMapObjectGeometryShader) m_pd3dMiniMapObjectGeometryShader->Release();
	if (m_pd3dMiniMapObjectPixelShader) m_pd3dMiniMapObjectPixelShader->Release();

	//�̴ϸ� �簢��
	if (m_pMiniMapRectTexture) m_pMiniMapRectTexture->Release();
	if (m_pMiniMapRectMesh) m_pMiniMapRectMesh->Release();
	if (m_pd3dMiniMapRectVertexShader) m_pd3dMiniMapRectVertexShader->Release();
	if (m_pd3dMiniMapRectPixelShader) m_pd3dMiniMapRectPixelShader->Release();
	if (m_pd3dMiniMapRectVertexLayout) m_pd3dMiniMapRectVertexLayout->Release();
	
	//scv�� ä���� �߸� �ڿ�
	if (m_pNeutralResourceTexture) m_pNeutralResourceTexture->Release();
	if (m_pNeutralResourceMesh) m_pNeutralResourceMesh->Release();
	if (m_pd3dNeutralResourceVertexShader) m_pd3dNeutralResourceVertexShader->Release();
	if (m_pd3dNeutralResourcePixelShader) m_pd3dNeutralResourcePixelShader->Release();
	if (m_pd3dNeutralResourceVertexLayout) m_pd3dNeutralResourceVertexLayout->Release();

	//UI ���� �̹���
	for (auto d : m_mapCommandTexture)
		d.second->Release();
	for (auto d : m_mapStatusTexture)
		d.second->Release();
	for (auto d : m_mapPortraitTexture)
		d.second->Release();

	for (auto d : m_mapCommandMesh)
		d.second->Release();
	if (m_pStatusMesh) m_pStatusMesh->Release();
	if (m_pPortraitMesh) m_pPortraitMesh->Release();

	//��ã��
	if (m_pAStarComponent)
		//	delete m_pAStarComponent;
		m_pAStarComponent->DeleteInstance();
	//�����
	if(m_pDebugPrograms)
		delete m_pDebugPrograms;

	//�Ȱ� �������
	if (m_pd3dcbFogBuffer) m_pd3dcbFogBuffer->Release();

	//�Ȱ� ���� ���ҽ�
	if (m_pd3dFogBuffer) m_pd3dFogBuffer->Release();
	if (m_pd3dSRVFog) m_pd3dSRVFog->Release();

	//��ü ���� ����Ʈ
	m_listMy.clear();
	m_listEnemy.clear();
	m_listEnemyInSight.clear();

	//��ƼŬ �ν��Ͻ� ����
	if(m_pParticleBox) m_pParticleBox->DeleteInstance();

	//���� �ν��Ͻ� ����
	if (m_pSoundManager) 
	{
		m_pSoundManager->ReleaseSound();
		m_pSoundManager->DeleteInstance();
	}

	//��Ʈ �ν��Ͻ� ����
	if (m_pNumberFont) m_pNumberFont->DeleteInstance();

	//�ǹ� ����
	if (m_pBuildCheck) m_pBuildCheck->DeleteInstance();

	//Ŀ�ǵ� â ��ġ
	m_mapCommandPosition.clear();

	//���� ���� ǥ�� ��ġ
	m_mapCrowdPosition.clear();

	//���α׷��� ��
	if (m_pProgressIconMesh) m_pProgressIconMesh->Release();
	if (m_pProgressImage) delete m_pProgressImage;

	//���콺 �̹���
	if(m_pMouseImageTexture) m_pMouseImageTexture->Release();
	if (m_pMouseImageMesh) m_pMouseImageMesh->Release();

	//���� ���� ǥ��
	if (m_pCrowdFrameTexture) m_pCrowdFrameTexture->Release();
	
	for (auto d : m_mapCrowdUnitTexture)
		d.second->Release();
	m_mapCrowdUnitTexture.clear();

	for (auto d : m_vecCrowdMeshes)
		d->Release();
	m_vecCrowdMeshes.clear();

	if (m_pd3dCrowdImageVertexShader) m_pd3dCrowdImageVertexShader->Release();
	if (m_pd3dCrowdImageVertexLayout) m_pd3dCrowdImageVertexLayout->Release();
	if (m_pd3dCrowdImagePixelShader) m_pd3dCrowdImagePixelShader->Release();

	m_mapAttachedCrowdPosition.clear();
	if (m_pCrowdAttachedFrameTexture) m_pCrowdAttachedFrameTexture->Release();
	for (auto d : m_vecAttachedCrowdMeshes)
		d->Release();
	m_vecAttachedCrowdMeshes.clear();

	ShowCursor(true);

	m_mapIsBuildingExist.clear();

	//����ó��
	if (m_pResultImageMesh) m_pResultImageMesh->Release();
	if (m_pResultImageVictory) m_pResultImageVictory->Release();
	if (m_pResultImageDefeat) m_pResultImageDefeat->Release();
}

void CScene::SetCamera(CCamera *pCamera)
{
	m_pCamera = pCamera; 
	gGameCamera = (void*)(m_pCamera); 
}


//**********************************************************************
//���� ���� �Լ��� ����
void CScene::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	//���� ���� ��ü�� ���ߴ� �ֺ������� �����Ѵ�.
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);

	//���⼺ ����
	m_pLights->m_pLights[0].m_bEnable = 1.0f;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_d3dxcAmbient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcDiffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights->m_pLights[0].m_d3dxcSpecular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 20.0f);
	m_pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(+0.707f, -0.707f, 0.0f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(LIGHTS);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pLights;
	d3dBufferData.SysMemPitch = sizeof(LIGHTS);
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights);
}

void CScene::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights)
{
	/*
	//test ���⼺ ���� �̵�
	int num = rand() % 4;
	D3DXVECTOR3 sunDirection;
	switch (num)
	{
	case 0: // ���ʿ� �¾��� �ִ�
		sunDirection = D3DXVECTOR3(-0.707f, -0.707f, 0.0f);
		break;
	case 1: // ���� to ����
		sunDirection = D3DXVECTOR3(+0.707f, -0.707f, 0.0f);
		break;
	case 2: // ���� to ����
		sunDirection = D3DXVECTOR3(0.0f, -0.707f, +0.707f);
		break;
	case 3: // ���� to ����
		sunDirection = D3DXVECTOR3(0.0f, -0.707f, -0.707f);
		break;
	}

	pLights->m_pLights[0].m_d3dxvDirection = sunDirection;
	*/

	//SUNSET_TIME
	static float fSunElapsedTime = 0.0f;
	fSunElapsedTime += m_fCurrentFrameElapsedTime;
	if (fSunElapsedTime > SUNSET_TIME)
		fSunElapsedTime = 0.0f;

	float deg = ((fSunElapsedTime / SUNSET_TIME) * 90.0f) + 45.0f;

	float xDir = -cos(deg * PI);
	float yDir = -abs(sin(deg * PI));
	
	pLights->m_pLights[0].m_d3dxvDirection = D3DXVECTOR3(xDir, yDir, 0.0f);



	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);
	pd3dDeviceContext->PSSetConstantBuffers(CB_SLOT_LIGHT, 1, &m_pd3dcbLights);
}


//**********************************************************************
//UI ���� Shader ���� ó��
void CScene::OnPrepareUIRender(ID3D11DeviceContext* pd3dDeviceContext, int uiRendingType)
{
	//pd3dDeviceContext->HSSetShader(nullptr, nullptr, 0);
	//pd3dDeviceContext->DSSetShader(nullptr, nullptr, 0);
	
	switch (uiRendingType)
	{
	case UI_RENDERING::mouserect:

		pd3dDeviceContext->IASetInputLayout(m_pd3dMouseRectVertexLayout);
		pd3dDeviceContext->VSSetShader(m_pd3dMouseRectVertexShader, NULL, 0);
		pd3dDeviceContext->GSSetShader(NULL, NULL, 0);
		pd3dDeviceContext->PSSetShader(m_pd3dMouseRectPixelShader, NULL, 0);

		break;

	case UI_RENDERING::selectedcircle:

		pd3dDeviceContext->IASetInputLayout(m_pd3dSelectedCircleVertexLayout);
		pd3dDeviceContext->VSSetShader(m_pd3dSelectedCircleVertexShader, NULL, 0);
		pd3dDeviceContext->GSSetShader(m_pd3dSelectedCircleGeometryShader, NULL, 0);
		pd3dDeviceContext->PSSetShader(m_pd3dSelectedCirclePixelShader, NULL, 0);

		break;

	case UI_RENDERING::mainframe:

		pd3dDeviceContext->IASetInputLayout(m_pd3dMainFrameVertexLayout);
		pd3dDeviceContext->VSSetShader(m_pd3dMainFrameVertexShader, NULL, 0);
		pd3dDeviceContext->GSSetShader(NULL, NULL, 0);
		pd3dDeviceContext->PSSetShader(m_pd3dMainFramePixelShader, NULL, 0);

		break;

	case UI_RENDERING::attached:

		pd3dDeviceContext->IASetInputLayout(m_pd3dAttachedMainFrameVertexLayout);
		pd3dDeviceContext->VSSetShader(m_pd3dAttachedMainFrameVertexShader, NULL, 0);
		pd3dDeviceContext->GSSetShader(NULL, NULL, 0);
		pd3dDeviceContext->PSSetShader(m_pd3dAttachedMainFramePixelShader, NULL, 0);

		break;
	
	case UI_RENDERING::minimap:

		pd3dDeviceContext->IASetInputLayout(m_pd3dMiniMapVertexLayout);
		pd3dDeviceContext->VSSetShader(m_pd3dMiniMapVertexShader, NULL, 0);
		pd3dDeviceContext->GSSetShader(NULL, NULL, 0);
		pd3dDeviceContext->PSSetShader(m_pd3dMiniMapPixelShader, NULL, 0);

		break;

	case UI_RENDERING::minimapobject:

		pd3dDeviceContext->IASetInputLayout(m_pd3dMiniMapObjectVertexLayout);
		pd3dDeviceContext->VSSetShader(m_pd3dMiniMapObjectVertexShader, NULL, 0);
		pd3dDeviceContext->GSSetShader(m_pd3dMiniMapObjectGeometryShader, NULL, 0);
		pd3dDeviceContext->PSSetShader(m_pd3dMiniMapObjectPixelShader, NULL, 0);

		break;

	case UI_RENDERING::minimaprect:

		pd3dDeviceContext->IASetInputLayout(m_pd3dMiniMapRectVertexLayout);
		pd3dDeviceContext->VSSetShader(m_pd3dMiniMapRectVertexShader, NULL, 0);
		pd3dDeviceContext->GSSetShader(nullptr, NULL, 0);
		pd3dDeviceContext->PSSetShader(m_pd3dMiniMapRectPixelShader, NULL, 0);

		break;

	case UI_RENDERING::neutralresource:

		pd3dDeviceContext->IASetInputLayout(m_pd3dNeutralResourceVertexLayout);
		pd3dDeviceContext->VSSetShader(m_pd3dNeutralResourceVertexShader, NULL, 0);
		pd3dDeviceContext->GSSetShader(m_pd3dNeutralResourceGeometryShader, NULL, 0);
		pd3dDeviceContext->PSSetShader(m_pd3dNeutralResourcePixelShader, NULL, 0);

		break;

	case UI_RENDERING::crowdimage:
		
		pd3dDeviceContext->IASetInputLayout(m_pd3dCrowdImageVertexLayout);
		pd3dDeviceContext->VSSetShader(m_pd3dCrowdImageVertexShader, NULL, 0);
		pd3dDeviceContext->GSSetShader(nullptr, NULL, 0);
		pd3dDeviceContext->PSSetShader(m_pd3dCrowdImagePixelShader, NULL, 0);

		break;

	default:
		break;
	}
}


void CScene::CreateMouseRectShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "SCREENPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSMouseRect", "vs_5_0", &m_pd3dMouseRectVertexShader, d3dInputLayout, nElements, &m_pd3dMouseRectVertexLayout);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSMouseRect", "ps_5_0", &m_pd3dMouseRectPixelShader);
}

void CScene::CreateSelectedCircleShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSSCALE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSSelectedCircle", "vs_5_0", &m_pd3dSelectedCircleVertexShader, d3dInputLayout, nElements, &m_pd3dSelectedCircleVertexLayout);
	CShader::CreateGeometryShaderFromFile(pd3dDevice, "Effect.fx", "GSSelectedCircle", "gs_5_0", &m_pd3dSelectedCircleGeometryShader);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSSelectedCircle", "ps_5_0", &m_pd3dSelectedCirclePixelShader);
}

void CScene::CreateMainFrameShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSUIFRAME", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSMainFrame", "vs_5_0", &m_pd3dMainFrameVertexShader, d3dInputLayout, nElements, &m_pd3dMainFrameVertexLayout);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSMainFrame", "ps_5_0", &m_pd3dMainFramePixelShader);
}

void CScene::CreateAttachedMainFrameShader(ID3D11Device* pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSATTACHED", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ISRENDERED", 0, DXGI_FORMAT_R32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSAttachedMainFrame", "vs_5_0", &m_pd3dAttachedMainFrameVertexShader, d3dInputLayout, nElements, &m_pd3dAttachedMainFrameVertexLayout);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSAttachedMainFrame", "ps_5_0", &m_pd3dAttachedMainFramePixelShader);
}

void CScene::CreateCrowdImageShader(ID3D11Device* pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSCROWD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSCrowdImage", "vs_5_0", &m_pd3dCrowdImageVertexShader, d3dInputLayout, nElements, &m_pd3dCrowdImageVertexLayout);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSCrowdImage", "ps_5_0", &m_pd3dCrowdImagePixelShader);
}

void CScene::CreateMiniMapShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSMINIMAP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSMiniMap", "vs_5_0", &m_pd3dMiniMapVertexShader, d3dInputLayout, nElements, &m_pd3dMiniMapVertexLayout);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSMiniMap", "ps_5_0", &m_pd3dMiniMapPixelShader);
}

void CScene::CreateMiniMapObjectShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSTYPE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSMiniMapObject", "vs_5_0", &m_pd3dMiniMapObjectVertexShader, d3dInputLayout, nElements, &m_pd3dMiniMapObjectVertexLayout);
	CShader::CreateGeometryShaderFromFile(pd3dDevice, "Effect.fx", "GSMiniMapObject", "gs_5_0", &m_pd3dMiniMapObjectGeometryShader);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSMiniMapObject", "ps_5_0", &m_pd3dMiniMapObjectPixelShader);
}

void CScene::CreateMiniMapRectShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSMiniMapRect", "vs_5_0", &m_pd3dMiniMapRectVertexShader, d3dInputLayout, nElements, &m_pd3dMiniMapRectVertexLayout);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSMiniMapRect", "ps_5_0", &m_pd3dMiniMapRectPixelShader);
}

void CScene::CreateNeutralResourceShader(ID3D11Device* pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSTYPE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSNeutralResource", "vs_5_0", &m_pd3dNeutralResourceVertexShader, d3dInputLayout, nElements, &m_pd3dNeutralResourceVertexLayout);
	CShader::CreateGeometryShaderFromFile(pd3dDevice, "Effect.fx", "GSNeutralResource", "gs_5_0", &m_pd3dNeutralResourceGeometryShader);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSNeutralResource", "ps_5_0", &m_pd3dNeutralResourcePixelShader);
}

//**********************************************************************
//��ü ����
void CScene::LoadUIImage(ID3D11Device* pd3dDevice, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState)
{
	CTexture *pTexture = nullptr;
	/////////////////////////////////////////////////////////////////
	//Ŀ�ǵ� â Attack
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/attack.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("attack", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â Stop
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/stop.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("stop", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â Move
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/move.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("move", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â Patrol
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/patrol.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("patrol", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â Hold
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/hold.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("hold", pTexture));
	pTexture->AddRef();

#ifndef DEBUG_SIMPLE_LOAD
	//Ŀ�ǵ� â mineral
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/mineral.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("mineral", pTexture));
	pTexture->AddRef();
	
	//Ŀ�ǵ� â gas
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/gas.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("gas", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â scv
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/scv.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("scv", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â rally
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/rally.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("rally", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â build
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/build.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("build", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â buildAdv
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/buildadv.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("buildadv", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â repair
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/repair.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("repair", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â grin
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/grin.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("grin", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â addattack
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/addattack.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("addattack", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â addshield
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/addshield.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("addshield", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â scope
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/scope.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("scope", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â addbuildingshield
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/addbuildingshield.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("addbuildingshield", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â bunkerup
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/bunkerup.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("bunkerup", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â sensor
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/sensor.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("sensor", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â commandcenter
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/commandcenter.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("commandcenter", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â refinery
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/refinery.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("refinery", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â supplydepot
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/supplydepot.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("supplydepot", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â barrack
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/barrack.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("barrack", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â engineeringbay
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/engineeringbay.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("engineeringbay", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â bunker
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/bunker.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("bunker", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â missileturret
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/missileturret.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("missileturret", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â sensortower
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/sensortower.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("sensortower", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â cancel
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/cancel.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("cancel", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â marineshield
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/marineshield.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("marineshield", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â steampack
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/steampack.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("steampack", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â quake
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/quake.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("quake", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â jump
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/jump.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("jump", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â clock
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/cloak.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("cloak", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â sniper
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/sniper.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("sniper", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â emp
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/emp.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("emp", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â marine
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/marine.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("marine", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â reaper
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/reaper.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("reaper", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â marauder
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/marauder.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("marauder", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â ghost
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/ghost.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("ghost", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â addmecaattack
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/addmecaattack.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("addmecaattack", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â addmecashield
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/addmecashield.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("addmecashield", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â widowmine
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/widowmine.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("widowmine", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â hellion
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/hellion.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("hellion", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â siegetank
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/siegetank.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("siegetank", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â thor
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/thor.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("thor", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â ghostacademy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/ghostacademy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("ghostacademy", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â factory
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/factory.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("factory", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â armory
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/armory.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("armory", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â starport
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/starport.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("starport", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â fusioncore
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/fusioncore.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("fusioncore", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â siegemode
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/siegemode.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("siegemode", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â yamatore
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/yamatore.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("yamatore", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â yamatoenergy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/yamatoenergy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("yamatoenergy", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â heal
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/heal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("heal", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â burnner
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/burnner.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("burnner", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â tacticaljump
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/tacticaljump.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("tacticaljump", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â banshee
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/banshee.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("banshee", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â medivac
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/medivac.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("medivac", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â raven
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/raven.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("raven", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â battlecruiser
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/battlecruiser.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("battlecruiser", pTexture));
	pTexture->AddRef();

	//Ŀ�ǵ� â viking
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/viking.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("viking", pTexture));
	pTexture->AddRef();

#endif // DEBUG_SIMPLE_LOAD

	/////////////////////////////////////////////////////////////////
	//���� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/marine_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Marine, pTexture));
	pTexture->AddRef();

	//��� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/reaper_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Reaper, pTexture));
	pTexture->AddRef();

	//�Ұ� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/marauder_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Marauder, pTexture));
	pTexture->AddRef();

	//���� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/ghost_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Ghost, pTexture));
	pTexture->AddRef();

#ifndef DEBUG_SIMPLE_LOAD
	//SCV �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/scv_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Scv, pTexture));
	pTexture->AddRef();

	//���Էκ� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/mule_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Mule, pTexture));
	pTexture->AddRef();

	//�丣 �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/thor_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Thor, pTexture));
	pTexture->AddRef();

	/*
	//���Ź����� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/widowmine_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::, pTexture));
	pTexture->AddRef();
	*/

	//��ũ �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/siegetank_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Siegetank, pTexture));
	pTexture->AddRef();

	//ȭ���� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/hellion_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Hellion, pTexture));
	pTexture->AddRef();

	//�Ƿἱ �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/medivac_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Medivac, pTexture));
	pTexture->AddRef();

	//����ŷ �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/viking_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Viking, pTexture));
	pTexture->AddRef();

	//���� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/raven_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Raven, pTexture));
	pTexture->AddRef();

	//��� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/banshee_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Banshee, pTexture));
	pTexture->AddRef();

	//���������� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/battlecruiser_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::BattleCruiser, pTexture));
	pTexture->AddRef();

	//�ǹ� �ʻ�ȭ
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/building_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Commandcenter, pTexture));
	pTexture->AddRef();

#endif // DEBUG_SIMPLE_LOAD

	/////////////////////////////////////////////////////////////////
	//���� ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/marine_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Marine, pTexture));
	pTexture->AddRef();

	//��� ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/reaper_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Reaper, pTexture));
	pTexture->AddRef();

	//�Ұ� ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/marauder_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Marauder, pTexture));
	pTexture->AddRef();

	//���� ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/ghost_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Ghost, pTexture));
	pTexture->AddRef();

#ifndef DEBUG_SIMPLE_LOAD

	//Thor ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Thor_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Thor, pTexture));
	pTexture->AddRef();

	/*
	// Widowmine ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Widowmine_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Widowmine, pTexture));
	pTexture->AddRef();
	*/

	// Scv ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Scv_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Scv, pTexture));
	pTexture->AddRef();

	// Mule ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Mule_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Mule, pTexture));
	pTexture->AddRef();

	// Siegetank ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Siegetank_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Siegetank, pTexture));
	pTexture->AddRef();

	// Hellion ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Hellion_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Hellion, pTexture));
	pTexture->AddRef();

	// Medivac ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Medivac_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Medivac, pTexture));
	pTexture->AddRef();

	// Viking ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Viking_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Viking, pTexture));
	pTexture->AddRef();

	// Raven ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Raven_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Raven, pTexture));
	pTexture->AddRef();

	// Banshee ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Banshee_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Banshee, pTexture));
	pTexture->AddRef();

	// Battlecruiser ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Battlecruiser_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::BattleCruiser, pTexture));
	pTexture->AddRef();

	// Commandcenter ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Commandcenter_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Commandcenter, pTexture));
	pTexture->AddRef();

	// Supplydepot ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Supplydepot_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Supplydepot, pTexture));
	pTexture->AddRef();

	// Engineeringbay ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Engineeringbay_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Engineeringbay, pTexture));
	pTexture->AddRef();

	// Refinery ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Refinery_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Refinery, pTexture));
	pTexture->AddRef();

	// Missileturret ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Missileturret_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Missileturret, pTexture));
	pTexture->AddRef();

	// Techlab ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Techlab_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Techlab, pTexture));
	pTexture->AddRef();

	// Ghostacademy ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Ghostacademy_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Ghostacademy, pTexture));
	pTexture->AddRef();

	// Barrack ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Barrack_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Barrack, pTexture));
	pTexture->AddRef();

	// Armory ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Armory_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Armory, pTexture));
	pTexture->AddRef();

	// Factory ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Factory_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Factory, pTexture));
	pTexture->AddRef();

	// Fusioncore ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Fusioncore_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Fusioncore, pTexture));
	pTexture->AddRef();

	// Starport ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Starport_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Starport, pTexture));
	pTexture->AddRef();

	// Bunker ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Bunker_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Bunker, pTexture));
	pTexture->AddRef();

	// Reactor ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Reactor_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Reactor, pTexture));
	pTexture->AddRef();

	// Sensortower ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Sensortower_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Sensortower, pTexture));
	pTexture->AddRef();

	// Mineral ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Mineral_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Mineral, pTexture));
	pTexture->AddRef();

	// Gas ����â
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Gas_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Gas, pTexture));
	pTexture->AddRef();


#endif //DEBUG_SIMPLE_LOAD
}

void CScene::LoadCrowdUnitImage(ID3D11Device* pd3dDevice, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState)
{
	CTexture *pTexture = nullptr;
	/////////////////////////////////////////////////////////////////
	//banshee
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/wireframe_banshee.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCrowdUnitTexture.insert(make_pair(OBJECT_TYPE::Banshee, pTexture));
	pTexture->AddRef();

	//BattleCruiser
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/wireframe_BattleCruiser.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCrowdUnitTexture.insert(make_pair(OBJECT_TYPE::BattleCruiser, pTexture));
	pTexture->AddRef();

	//Marauder
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/wireframe_Marauder.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCrowdUnitTexture.insert(make_pair(OBJECT_TYPE::Marauder, pTexture));
	pTexture->AddRef();

	//Marine
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/wireframe_Marine.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCrowdUnitTexture.insert(make_pair(OBJECT_TYPE::Marine, pTexture));
	pTexture->AddRef();

	//Medivac
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/wireframe_Medivac.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCrowdUnitTexture.insert(make_pair(OBJECT_TYPE::Medivac, pTexture));
	pTexture->AddRef();

	//Raven
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/wireframe_Raven.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCrowdUnitTexture.insert(make_pair(OBJECT_TYPE::Raven, pTexture));
	pTexture->AddRef();

	//Reaper
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/wireframe_Reaper.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCrowdUnitTexture.insert(make_pair(OBJECT_TYPE::Reaper, pTexture));
	pTexture->AddRef();

	//Scv
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/wireframe_Scv.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCrowdUnitTexture.insert(make_pair(OBJECT_TYPE::Scv, pTexture));
	pTexture->AddRef();

	//Thor
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/wireframe_Thor.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCrowdUnitTexture.insert(make_pair(OBJECT_TYPE::Thor, pTexture));
	pTexture->AddRef();

	//Viking
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/wireframe_Viking.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCrowdUnitTexture.insert(make_pair(OBJECT_TYPE::Viking, pTexture));
	pTexture->AddRef();
}

void CScene::LoadParticleImage(ID3D11Device* pd3dDevice)
{
	//����ü��
	PARTICLE_DATA explosionData;
	
	ifstream infile{ "Assets/Image/Effect/load_particle.txt" };
	string buf;
	int nParticleCount = 0;
	int bProjectile;
	float fNumSprites, fBetweenTime, fParticleSize, fPromptMaintainTime, fProjectileSpeed;
	string fileName, fileLocation;

	infile >> buf; //HEADER

	while (!infile.eof()) {
		infile >> buf;

		if (buf == string("[PROJECTILE]"))
		{
			infile >> nParticleCount;

			for (int i = 0; i < nParticleCount; ++i)
			{
				infile >> fileName;
				infile >> bProjectile >> fNumSprites >> fBetweenTime >> fParticleSize >> fPromptMaintainTime >> fProjectileSpeed;
				infile >> fileLocation;
				
				explosionData.SetParticleData(pd3dDevice, bProjectile, fNumSprites, fBetweenTime, fParticleSize, fPromptMaintainTime, fProjectileSpeed, fileLocation.data());
				m_pParticleBox->AddParticleData(fileName, explosionData);
			}
		}

		else if (buf == string("[PROMPT]"))
		{
			infile >> nParticleCount;

			for (int i = 0; i < nParticleCount; ++i)
			{
				infile >> fileName;
				infile >> bProjectile >> fNumSprites >> fBetweenTime >> fParticleSize >> fPromptMaintainTime >> fProjectileSpeed;
				infile >> fileLocation;

				explosionData.SetParticleData(pd3dDevice, bProjectile, fNumSprites, fBetweenTime, fParticleSize, fPromptMaintainTime, fProjectileSpeed, fileLocation.data());
				m_pParticleBox->AddParticleData(fileName, explosionData);
			}
		}
	}

	/*
	explosionData.SetParticleData(pd3dDevice, true, 10.0f, 0.05f, 0.7f, 0.4f, "Assets/Image/Effect/battlecruiser_fire.png");
	m_pParticleBox->AddParticleData("battlecruiser_fire", explosionData);

	explosionData.SetParticleData(pd3dDevice, true, 8.0f, 0.05f, 0.7f, 0.4f, "Assets/Image/Effect/viking_fire.png");
	m_pParticleBox->AddParticleData("viking_fire", explosionData);

	explosionData.SetParticleData(pd3dDevice, true, 12.0f, 0.05f, 0.9f, 0.4f, "Assets/Image/Effect/marauder_fire.png");
	m_pParticleBox->AddParticleData("marauder_fire", explosionData);

	//��ߵ�
	explosionData.SetParticleData(pd3dDevice, false, 18.0f, 0.1f, 0.4f, 0.4f, "Assets/Image/Effect/marine_fire.png");
	m_pParticleBox->AddParticleData("marine_fire", explosionData);

	explosionData.SetParticleData(pd3dDevice, false, 7.0f, 0.1f, 0.4f, 0.4f, "Assets/Image/Effect/reaper_fire.png");
	m_pParticleBox->AddParticleData("reaper_fire", explosionData);

	explosionData.SetParticleData(pd3dDevice, false, 14.0f, 0.05f, 1.2f, 0.4f, "Assets/Image/Effect/thor_fire.png");
	m_pParticleBox->AddParticleData("thor_fire", explosionData);

	explosionData.SetParticleData(pd3dDevice, false, 12.0f, 0.05f, 2.5f, 0.4f, "Assets/Image/Effect/explosion_1.png");
	m_pParticleBox->AddParticleData("explosion_1", explosionData);

	explosionData.SetParticleData(pd3dDevice, false, 8.0f, 0.05f, 2.5f, 0.4f, "Assets/Image/Effect/explosion_2.png");
	m_pParticleBox->AddParticleData("explosion_2", explosionData);
	*/
}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	printf("���� �̹����� �ҷ����� �ֽ��ϴ�...\n");

	//�⺻ ���÷� �ɼ� ����
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

	//�ؽ��� ���ҽ��� �����Ѵ�.
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;

	//�׼����̼� �� diffuse
	CTexture *pTSTerrainTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Terrain/mapama_160160.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	//D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Terrain/mapama_160160_detail.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTSTerrainTexture->SetTexture(0, pd3dsrvTexture);
	pTSTerrainTexture->SetSampler(0, pd3dSamplerState);

	


	//���콺 �巡�� �簢�� �̹���
	CTexture *pMouseRectTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/testMouseRect.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pMouseRectTexture->SetTexture(0, pd3dsrvTexture);
	pMouseRectTexture->SetSampler(0, pd3dSamplerState);

	//���õ� ������Ʈ���� ǥ�� �̹���
	CTexture *pSelectedCircleTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/testSelectedCircle.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pSelectedCircleTexture->SetTexture(0, pd3dsrvTexture);
	pSelectedCircleTexture->SetSampler(0, pd3dSamplerState);

	//���� ������ �̹���
	CTexture *pMainFrameTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/testMainFrame.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pMainFrameTexture->SetTexture(0, pd3dsrvTexture);
	pMainFrameTexture->SetSampler(0, pd3dSamplerState);

	//�̴ϸ� �̹���
	CTexture *pMiniMapTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/testMiniMap.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pMiniMapTexture->SetTexture(0, pd3dsrvTexture);
	pMiniMapTexture->SetSampler(0, pd3dSamplerState);

	CTexture *pMiniMapRectTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/testMinimapRect.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pMiniMapRectTexture->SetTexture(0, pd3dsrvTexture);
	pMiniMapRectTexture->SetSampler(0, pd3dSamplerState);
	
	CTexture *pNeutralResourceTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/testneutralresource.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pNeutralResourceTexture->SetTexture(0, pd3dsrvTexture);
	pNeutralResourceTexture->SetSampler(0, pd3dSamplerState);
	
	//���콺 Ŀ�� �̹���
	CTexture *pMouseImageTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/mouse_image.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pMouseImageTexture->SetTexture(0, pd3dsrvTexture);
	pMouseImageTexture->SetSampler(0, pd3dSamplerState);

	//status�� �׷��� ���� ���� �̹���
	CTexture *pCrowdFrameTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/unit_frame.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pCrowdFrameTexture->SetTexture(0, pd3dsrvTexture);
	pCrowdFrameTexture->SetSampler(0, pd3dSamplerState);
	
	LoadCrowdUnitImage(pd3dDevice, pd3dsrvTexture, pd3dSamplerState);

	//statusâ�� �ٴ� ����� ���� ���� Ȯ�� �̹���
	CTexture *pCrowdAttachedFrameTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/crowd_AttachMainFrame.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pCrowdAttachedFrameTexture->SetTexture(0, pd3dsrvTexture);
	pCrowdAttachedFrameTexture->SetSampler(0, pd3dSamplerState);
	

	printf("UI �̹����� �ҷ����� �ֽ��ϴ�...\n");
	LoadUIImage(pd3dDevice, pd3dsrvTexture, pd3dSamplerState);

	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();

	printf("������ �ҷ����� �ֽ��ϴ�...\n");
	//���� ������ �׳� ���⼭ ������ش�...
	D3DXCOLOR matDif = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXCOLOR matAmb = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	D3DXCOLOR matSpe = D3DXCOLOR(1.0f, 1.0f, 1.0f, 20.0f);
	D3DXCOLOR matEmi = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
	CMATERIAL* mat = new CMATERIAL{ matAmb, matDif, matEmi, matSpe };
	
	CTerrainShader* pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice);
	pTerrainShader->BuildObjects(pd3dDevice);
	pTerrainShader->SetTexture(pTSTerrainTexture);
	pTerrainShader->SetMaterial(mat);
	

	//���̸� ǥ���� ���� HeightMap ����
	m_pHeightMap = pTerrainShader->GetTerrain()->GetHeightMap();
	//CreateHeightMap(_T("Assets/Image/Terrain/mapama_160160_height_raw_205205.RAW"), MAP_WIDTH, MAP_HEIGHT, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	
	
	CTSTerrainShader* pTSTerrainShader = new CTSTerrainShader();
	pTSTerrainShader->CreateShader(pd3dDevice);
	pTSTerrainShader->BuildObjects(pd3dDevice);
	pTSTerrainShader->SetTexture(pTSTerrainTexture);
	pTSTerrainShader->SetMaterial(mat);
	

	printf("���� ������ �ҷ����� �ֽ��ϴ�...\n");
	m_pStaticShader = new CStaticShader();
	m_pStaticShader->CreateShader(pd3dDevice);
	m_pStaticShader->BuildObjects(pd3dDevice);

	m_pAnimationShader = new CAnimationShader();
	m_pAnimationShader->CreateShader(pd3dDevice);
	m_pAnimationShader->BuildObjects(pd3dDevice);

	
#ifdef DEBUG_NOT_CONNECT_SERVER
	sc_packet_put_object put;
	put.client_id = m_pGameFramework->m_clientID;

	/*
	sc_packet_put_object put;
	put.client_id = m_pGameFramework->m_clientID;
	
	put.x = 3.0f; put.z = 0.0f;
	put.object_type = OBJECT_TYPE::Reaper;
	Process_SC_Packet_Put_Object(&put);

	put.x = -3.0f; put.z = 0.0f;
	put.object_type = OBJECT_TYPE::Reaper_Enemy;
	Process_SC_Packet_Put_Object(&put);
	*/
	
	//�Ʊ� ���� ����
	put.x = -3.0f * 7; put.z = 5.0f;
	
	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Viking;	
	Process_SC_Packet_Put_Object(&put);
	
	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Banshee;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Reaper;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::BattleCruiser;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Marauder;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Marine;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Medivac;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Mule;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Raven;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Scv;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Thor;
	Process_SC_Packet_Put_Object(&put);

	//���� ���� ����
	put.x = -3.0f * 7; put.z = -5.0f;

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Viking_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Banshee_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Reaper_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::BattleCruiser_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Marauder_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Marine_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Medivac_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Mule_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Raven_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Scv_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Thor_Enemy;
	Process_SC_Packet_Put_Object(&put);


	//�Ʊ� �ǹ� ����
	put.x = 4.0f * 8; put.z = 2.0f;
	for (int i = 0; i < 15; ++i)
	{
		put.x -= 4.0f; put.y = 1.0f; put.z = put.z;
		put.object_type = (OBJECT_TYPE)((int)OBJECT_TYPE::Armory + i);
		Process_SC_Packet_Put_Object(&put);
	}

	//���� �ǹ� ����
	put.x = 4.0f * 8; put.z = -2.0f;
	for (int i = 0; i < 15; ++i)
	{
		put.x -= 4.0f; put.y = 1.0f; put.z = put.z;
		put.object_type = (OBJECT_TYPE)((int)OBJECT_TYPE::Armory + i + ADD_ENEMY_INDEX);
		Process_SC_Packet_Put_Object(&put);
	}

	
#endif

#ifdef DEBUG_SINGLE_FIGHT_UNITS

	sc_packet_put_object put;
	put.client_id = m_pGameFramework->m_clientID;

	//�Ʊ� ���� ����
	put.x = -3.0f * 7; put.z = 5.0f;

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Viking;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Banshee;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Reaper;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::BattleCruiser;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Marauder;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Marine;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Medivac;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Mule;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Raven;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Scv;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Thor;
	Process_SC_Packet_Put_Object(&put);

	//���� ���� ����
	//put.client_id = -2;
	put.x = -3.0f * 7; put.z = -5.0f;

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Viking_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Banshee_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Reaper_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::BattleCruiser_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Marauder_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Marine_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Medivac_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Mule_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Raven_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Scv_Enemy;
	Process_SC_Packet_Put_Object(&put);

	put.x += 3.0f; put.y = 1.0f;
	put.object_type = OBJECT_TYPE::Thor_Enemy;
	Process_SC_Packet_Put_Object(&put);

#endif

	//��ī�̹ڽ� , ����
	m_nShaders = 3;
	m_ppShaders = new CShader*[m_nShaders];
	m_ppShaders[0] = new CSkyBoxShader();
	m_ppShaders[0]->CreateShader(pd3dDevice);
	m_ppShaders[0]->BuildObjects(pd3dDevice);
	m_ppShaders[1] = pTSTerrainShader;
	m_ppShaders[2] = pTerrainShader;
	
	printf("UI ������ �ҷ����� �ֽ��ϴ�...\n");
	//MouseRect
	CreateMouseRectShader(pd3dDevice);
	CMouseRectMesh* pMouseRectMesh = new CMouseRectMesh(pd3dDevice);
	pMouseRectMesh->AddRef();
	m_pMouseRectMesh = pMouseRectMesh;
	m_pMouseRectTexture = pMouseRectTexture;
	pMouseRectTexture->AddRef();

	//SelectedCircle
	CreateSelectedCircleShader(pd3dDevice);
	CSelectedCircleMesh* pSelectedCircleMesh = new CSelectedCircleMesh(pd3dDevice);
	pSelectedCircleMesh->AddRef();
	m_pSelectedCircleMesh = pSelectedCircleMesh;
	m_pSelectedCircleTexture = pSelectedCircleTexture;
	pSelectedCircleTexture->AddRef();

	//Main Frame
	CreateMainFrameShader(pd3dDevice);
	CMainFrameMesh* pMainFrameMesh = new CMainFrameMesh(pd3dDevice, D3DXVECTOR2(-1.0f, -1.0f), D3DXVECTOR2(1.0f, -1.0f + 0.6f));
	pMainFrameMesh->AddRef();
	m_pMainFrameMesh = pMainFrameMesh;
	m_pMainFrameTexture = pMainFrameTexture;
	pMainFrameTexture->AddRef();

	//1003 MouseImage
	CMainFrameMesh* pMouseImageMesh = new CMainFrameMesh(pd3dDevice, D3DXVECTOR2(-1.0f, -1.0f), D3DXVECTOR2(-1.0f, -1.0f));
	pMouseImageMesh->AddRef();
	m_pMouseImageMesh = pMouseImageMesh;
	m_pMouseImageTexture = pMouseImageTexture;
	pMouseImageTexture->AddRef();

	//Mini Map
	CreateMiniMapShader(pd3dDevice);
	CScreenMesh* pMiniMapMesh = new CScreenMesh(pd3dDevice,
		D3DXVECTOR2(MINIMAP_POSITION_LEFT, MINIMAP_POSITION_BOTTOM), D3DXVECTOR2(MINIMAP_POSITION_RIGHT, MINIMAP_POSITION_TOP));
	pMiniMapMesh->AddRef();
	m_pMiniMapMesh = pMiniMapMesh;
	m_pMiniMapTexture = pMiniMapTexture;
	pMiniMapTexture->AddRef();

	//Minimap Rect
	CreateMiniMapRectShader(pd3dDevice);
	CMiniMapRectMesh* pMiniMapRectMesh = new CMiniMapRectMesh(pd3dDevice);
	m_pMiniMapRectMesh = pMiniMapRectMesh;
	pMiniMapRectMesh->AddRef();
	m_pMiniMapRectTexture = pMiniMapRectTexture;
	pMiniMapRectTexture->AddRef();

	//MiniMapObject
	CreateMiniMapObjectShader(pd3dDevice);
	CMiniMapObjectMesh* pMiniMapObjectMesh = new CMiniMapObjectMesh(pd3dDevice);
	pMiniMapObjectMesh->AddRef();
	m_pMiniMapObjectMesh = pMiniMapObjectMesh;
	
	//0905 �߸��ڿ� ����ֱ�
	CreateNeutralResourceShader(pd3dDevice);
	CNeutralResourceMesh* pNeutralResourceMesh = new CNeutralResourceMesh(pd3dDevice);
	m_pNeutralResourceMesh = pNeutralResourceMesh;
	pNeutralResourceMesh->AddRef();
	m_pNeutralResourceTexture = pNeutralResourceTexture;
	pNeutralResourceTexture->AddRef();


	//UI AttachedMainFrame
	CreateAttachedMainFrameShader(pd3dDevice);

	//Command					//�»�ܺ��� 1~5, �� ĭ �Ʒ� 6~10, �� �Ʒ�ĭ 11~15�� �Ѵ�.
	//0815 Command Update
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>& commandPosBox = m_mapCommandPosition;

	//1 ���� ���� ��ġ�� ���� ���� ��
	D3DXVECTOR2 vCommandLB(1.0f - (2.0f * 0.1979f), -1.0f + (0.6f * 0.0560f) - 0.015f);
	D3DXVECTOR2 vCommandRT(1.0f - (2.0f * 0.0104f), -1.0f + (0.6f * 0.7050f) - 0.015f);
	float commandWidth = 2.0f * 0.0370f;
	float commandHeight = (0.6f * 0.2124f);
	D3DXVECTOR2 lb = D3DXVECTOR2(vCommandLB.x, vCommandRT.y - commandHeight);
	D3DXVECTOR2 rt = D3DXVECTOR2(vCommandLB.x + commandWidth, vCommandRT.y);

	for (int j = 0; j < 3; ++j)
	{
		for (int i = 0; i < 5; ++i)
		{
			float leftX = lb.x + commandWidth * i;
			float rightX = leftX + commandWidth;
			float bottomY = lb.y - commandHeight * j;
			float topY = bottomY + commandHeight;
			commandPosBox.insert(make_pair( ((5 * j) + i + 1), 
				make_pair(D3DXVECTOR2(leftX, bottomY), D3DXVECTOR2(rightX, topY))));
		}
	}

	CScreenMesh* pCommandMesh = nullptr;
	
	auto comPos = commandPosBox.find(1);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("move", pCommandMesh));
	pCommandMesh->AddRef();
	
	comPos = commandPosBox.find(2);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("stop", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(3);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("hold", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(4);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("patrol", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(5);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("attack", pCommandMesh));
	pCommandMesh->AddRef();

	//0815
	comPos = commandPosBox.find(15);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("cancel", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(1);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("addattack", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(2);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("addshield", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(1);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("addmecaattack", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(2);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("addmecashield", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(11);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("build", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(12);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("buildadv", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(1);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("commandcenter", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(2);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("supplydepot", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(3);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second);
	m_mapCommandMesh.insert(make_pair("refinery", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(6);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("barrack", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(7);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("engineeringbay", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(12);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("bunker", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(10);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("missileturret", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(1);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("factory", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(2);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("starport", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(7);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("armory", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(6);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("yamatoenergy", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(7);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("yamatore", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(8);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("tacticaljump", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(11);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("steampack", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(12);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("grin", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(13);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("quake", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(11);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("cloak", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(11);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("jump", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(1);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("marineshield", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(6);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("heal", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(11);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("burnner", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(12);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("emp", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(6);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("addbuildingshield", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(7);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("bunkerup", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(10);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("repair", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(5);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("scv", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(14);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("sensor", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(1);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("marine", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(2);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("reaper", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(6);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("marauder", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(11);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("ghostacademy", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(12);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("fusioncore", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(9);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("sensortower", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(11);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("thor", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(1);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("viking", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(2);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("medivac", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(3);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("banshee", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(6);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("raven", pCommandMesh));
	pCommandMesh->AddRef();

	comPos = commandPosBox.find(7);
	pCommandMesh = new CScreenMesh(pd3dDevice, comPos->second.first, comPos->second.second, false);
	m_mapCommandMesh.insert(make_pair("battlecruiser", pCommandMesh));
	pCommandMesh->AddRef();

	//�ڿ� ��ũ�� �޽�
	pCommandMesh = new CScreenMesh(pd3dDevice, D3DXVECTOR2(SCREEN_POSITION_MINERAL_X, SCREEN_POSITION_MINERAL_Y), 
		D3DXVECTOR2(SCREEN_POSITION_MINERAL_X + 0.05f, SCREEN_POSITION_MINERAL_Y + 0.05f));
	m_mapCommandMesh.insert(make_pair("mineral", pCommandMesh));
	pCommandMesh->AddRef();

	pCommandMesh = new CScreenMesh(pd3dDevice, D3DXVECTOR2(SCREEN_POSITION_GAS_X, SCREEN_POSITION_GAS_Y),
		D3DXVECTOR2(SCREEN_POSITION_GAS_X + 0.05f, SCREEN_POSITION_GAS_Y + 0.05f));
	m_mapCommandMesh.insert(make_pair("gas", pCommandMesh));
	pCommandMesh->AddRef();
	//Ŀ�ǵ� â ���콺 �̵� �� �ʿ� �ڿ� ��ũ�� �޽�	-> �Ʒ� �ʿ� �ڿ� ��Ʈ���� �޽ø���


	//Status
	D3DXVECTOR2 vStatusLB = D3DXVECTOR2(-1.0f + (2.0f * 0.1963f), -1.0f + (0.6f * 0.0707f));
	D3DXVECTOR2 vStatusRT = D3DXVECTOR2(-1.0f + (2.0f * 0.7005f), -1.0f + (0.6f * 0.5369f));
	float vStatusWidth = vStatusRT.x - vStatusLB.x;
	float vStatusHeight = vStatusRT.y - vStatusLB.y;
	m_pStatusMesh = new CScreenMesh(pd3dDevice, vStatusLB + D3DXVECTOR2(vStatusWidth / 3.0f,0.0f)
		, vStatusLB + D3DXVECTOR2((vStatusWidth / 3.0f) * 2.0f, vStatusRT.y - vStatusLB.y ));
	m_pStatusMesh->AddRef();


	//Statusâ�� ǥ�� �� ���� ���ֵ�
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>& crowdPosBox = m_mapCrowdPosition;

	//1 ���� ���� ��ġ�� ���� ���� ��
	D3DXVECTOR2 vCrowdLB(-0.1042f - (0.0933f * 4.0f), -0.6778f - 0.0933f);
	D3DXVECTOR2 vCrowdRT(-0.1042f - (0.0933f * 4.0f) + 0.0933f, -0.6778f);
	float crowdWidth = 0.0933f;
	float crowdHeight = 0.0933f;
	D3DXVECTOR2 crowdLB = D3DXVECTOR2(vCrowdLB.x, vCrowdRT.y - crowdHeight);
	D3DXVECTOR2 crowdRT = D3DXVECTOR2(vCrowdLB.x + crowdWidth, vCrowdRT.y);

	for (int j = 0; j < 3; ++j)
	{
		for (int i = 0; i < 8; ++i)
		{
			float leftX = crowdLB.x + crowdHeight * i;
			float rightX = leftX + crowdWidth;
			float bottomY = crowdLB.y - crowdHeight * j;
			float topY = bottomY + crowdHeight;
			crowdPosBox.insert(make_pair(((8 * j) + i + 1),
				make_pair(D3DXVECTOR2(leftX, bottomY), D3DXVECTOR2(rightX, topY))));
		}
	}
	
	int nMaxCrowd = crowdPosBox.size();
	CCrowdMesh* pCrowdMesh = nullptr;
	
	m_vecCrowdMeshes.resize(nMaxCrowd);
	for (int i = 0; i < nMaxCrowd; ++i)
	{
		auto crowdPos = crowdPosBox.find(i + 1);
		pCrowdMesh = new CCrowdMesh(pd3dDevice, crowdPos->second.first, crowdPos->second.second);
		m_vecCrowdMeshes[i] = pCrowdMesh;
		pCrowdMesh->AddRef();
	}
	m_pCrowdFrameTexture = pCrowdFrameTexture;
	CreateCrowdImageShader(pd3dDevice);

	//crowd data image
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>& attachedCrowdPosBox = m_mapAttachedCrowdPosition;
	float attachedCrowdWidth = 0.0354f * 2.0f;
	float attachedCrowdHeight = 0.03520f * 2.0f;
	D3DXVECTOR2 vAttachedCrowdLB(-0.6323f, -0.6204f);
	D3DXVECTOR2 vAttachedCrowdRT(-0.6323f + attachedCrowdWidth, -0.6204f + attachedCrowdHeight);
	
	for (int i = 0; i < 8; ++i)
	{
		float leftX = vAttachedCrowdLB.x + attachedCrowdWidth * i;
		float rightX = leftX + attachedCrowdWidth;
		float bottomY = vAttachedCrowdLB.y;
		float topY = vAttachedCrowdRT.y;
		attachedCrowdPosBox.insert(make_pair((i + 1),
			make_pair(D3DXVECTOR2(leftX, bottomY), D3DXVECTOR2(rightX, topY))));
	}

	m_pCrowdAttachedFrameTexture = pCrowdAttachedFrameTexture;
	m_vecAttachedCrowdMeshes.resize(8);
	CScreenMesh* pAttachedCrowdMesh = nullptr;
	for (int i = 0; i < 8; ++i)
	{
		auto attachedCrowdPos = attachedCrowdPosBox.find(i + 1);
		pAttachedCrowdMesh = new CScreenMesh(pd3dDevice, attachedCrowdPos->second.first, attachedCrowdPos->second.second);
		m_vecAttachedCrowdMeshes[i] = pAttachedCrowdMesh;
		pAttachedCrowdMesh->AddRef();
	}


	//Portrait
	m_pPortraitMesh = new CScreenMesh(pd3dDevice, D3DXVECTOR2(-1.0f + (2.0f * 0.7220f), -1.0f + (0.6f * 0.0305f)),
		D3DXVECTOR2(-1.0f + (2.0f * 0.7854f), -1.0f + (0.6f * 0.6165f)));
	m_pPortraitMesh->AddRef();


	//UI �������� ���� ���̰��� ��,���� ���� �ʴ�
	D3D11_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = false;
	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	d3dDepthStencilDesc.StencilEnable = false;
	d3dDepthStencilDesc.StencilReadMask = 0xFF;
	d3dDepthStencilDesc.StencilWriteMask = 0xFF;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	pd3dDevice->CreateDepthStencilState(&d3dDepthStencilDesc, &m_pd3dUIDepthStencilState);

	printf("������ �ҷ����� �ֽ��ϴ�...\n");
	//���� �� ���� ������� ����
	CreateShaderVariables(pd3dDevice);

	printf("���� AI�� �ҷ����� �ֽ��ϴ�...\n");
	//��ã�� �׷����� �ʱ�ȭ
	m_pAStarComponent = AStarComponent::GetInstance();
	m_pAStarComponent->InitializeGraph(MAP_WIDTH);	//���簢�� ����

	printf("�׸��� �� �Ȱ��� �ҷ����� �ֽ��ϴ�...\n");
	//�׸��� ���̴�
	m_pStaticShader->CreateShadowShader(pd3dDevice);
	m_pAnimationShader->CreateShadowShader(pd3dDevice);

	//�Ȱ� ������� ����
	BuildFogBuffer(pd3dDevice);

	printf("����Ʈ�� �ҷ����� �ֽ��ϴ�...\n");
	//��ƼŬ �ʱ�ȭ �� �̹��� �ε�
	m_pParticleBox = CSpriteParticle::GetInstance();
	m_pParticleBox->BuildObject(pd3dDevice);
	m_pParticleBox->CreateShader(pd3dDevice);
	LoadParticleImage(pd3dDevice);

	printf("���� ���带 �ҷ����� �ֽ��ϴ�...\n");
	//���� �ʱ�ȭ �� �ε�
	m_pSoundManager = CSoundManager::GetInstance();
	string fileList = "SoundComponent/SoundList.txt";
	m_pSoundManager->CreateSound(fileList);
	srand(GetTickCount());
	m_pSoundManager->PlayBackgroundSound();
	m_pSoundManager->SetVolume(BACKGROUND_CHANNEL, 1.0f);

	printf("��Ʈ �̹����� �ҷ����� �ֽ��ϴ�...\n");
	//��Ʈ �ʱ�ȭ
	m_pNumberFont = CNumberFont::GetInstance();
	m_pNumberFont->CreateShader(pd3dDevice);
	m_pNumberFont->CreateResource(pd3dDevice);
	
	D3DXVECTOR4 posColor{ SCREEN_POSITION_MINERAL_X + 0.15f, SCREEN_POSITION_MINERAL_Y + 0.025f, 0.0f, 1.0f };
	m_pNumberFont->AddNumber(string("screen_mineral"), posColor, 0);
	posColor = D3DXVECTOR4{ SCREEN_POSITION_GAS_X + 0.15f, SCREEN_POSITION_GAS_Y + 0.025f, 0.0f, 1.0f };
	m_pNumberFont->AddNumber(string("screen_gas"), posColor, 0);

	posColor = D3DXVECTOR4{ -1.0f + (2 * 0.3516f), -1.0f + (2 * 0.0525f), 0.0f, 1.0f };
	m_pNumberFont->AddNumber(string("screen_hp"), posColor, 0);
	
	posColor = D3DXVECTOR4{ m_mapCommandPosition.find(2)->second.first.x, m_mapCommandPosition.find(2)->second.second.y + 0.15f, 0.0f, 1.0f };
	m_pNumberFont->AddNumber(string("screen_need_mineral"), posColor, 0);
	posColor.x += 0.15f;
	m_pNumberFont->AddNumber(string("screen_need_gas"), posColor, 0);

	float crowdFontXOffset = 0.075f;
	float crowdFontYOffset = 0.01f;
	for (int i = 1; i <= 8; ++i)
	{
		char buf[30];
		sprintf(buf, "crowd_save_image_%d", i);
		
		D3DXVECTOR4 crowdPos;
		crowdPos.x = (m_mapAttachedCrowdPosition.find(i)->second.first.x + m_mapAttachedCrowdPosition.find(i)->second.second.x) / 2.0f - crowdFontXOffset;
		crowdPos.y = (m_mapAttachedCrowdPosition.find(i)->second.first.y + m_mapAttachedCrowdPosition.find(i)->second.second.y) / 2.0f + crowdFontYOffset;
		crowdPos.z = 0.0f; crowdPos.w = 1.0f;
		m_pNumberFont->AddNumber(string(buf), crowdPos, i);
	}

	//Ŀ�ǵ� â ���콺 �̵� �� �ʿ� �ڿ� ��ũ�� �޽�
	posColor.x -= 0.16f; posColor.y -= 0.013f;
	pCommandMesh = new CScreenMesh(pd3dDevice, D3DXVECTOR2(posColor.x, posColor.y), D3DXVECTOR2(posColor.x + 0.03f, posColor.y + 0.03f));
	m_mapCommandMesh.insert(make_pair("need_mineral", pCommandMesh));
	pCommandMesh->AddRef();
	posColor.x += 0.16f;
	pCommandMesh = new CScreenMesh(pd3dDevice, D3DXVECTOR2(posColor.x, posColor.y), D3DXVECTOR2(posColor.x + 0.03f, posColor.y + 0.03f));
	m_mapCommandMesh.insert(make_pair("need_gas", pCommandMesh));
	pCommandMesh->AddRef();
	
	//�ǹ� ����
	m_pBuildCheck = CBuildCheck::GetInstance();
	m_pBuildCheck->CreateShader(pd3dDevice);
	m_pBuildCheck->CreateResource(pd3dDevice);
	
	m_listSCVBuild.push_back(string("commandcenter"));
	m_listSCVBuild.push_back(string("supplydepot"));
	m_listSCVBuild.push_back(string("refinery"));
	m_listSCVBuild.push_back(string("barrack"));
	m_listSCVBuild.push_back(string("engineeringbay"));
	m_listSCVBuild.push_back(string("bunker"));
	m_listSCVBuild.push_back(string("sensortower"));
	m_listSCVBuild.push_back(string("missileturret"));
	m_listSCVBuild.push_back(string("cancel"));

	m_listSCVBuildAdv.push_back(string("factory"));
	m_listSCVBuildAdv.push_back(string("starport"));
	m_listSCVBuildAdv.push_back(string("armory"));
	m_listSCVBuildAdv.push_back(string("ghostacademy"));
	m_listSCVBuildAdv.push_back(string("fusioncore"));
	m_listSCVBuildAdv.push_back(string("cancel"));

	m_listBarrackUnits.push_back(string("marine"));
	m_listBarrackUnits.push_back(string("marauder"));
	m_listBarrackUnits.push_back(string("reaper"));

	m_listFactoryUnits.push_back(string("thor"));

	m_listStarportUnits.push_back(string("viking"));
	m_listStarportUnits.push_back(string("banshee"));
	m_listStarportUnits.push_back(string("medivac"));
	m_listStarportUnits.push_back(string("raven"));
	m_listStarportUnits.push_back(string("battlecruiser"));

	// 0826 ���α׷��� ��
	D3DXVECTOR2 progressIconLB{ -1.0f + (2 * 0.4365f), -1.0f + (2 * 0.1067f) };
	D3DXVECTOR2 progressIconRT{ -1.0f + (2 * 0.4365f) + 0.05f, -1.0f + (2 * 0.1067f) + 0.05f };
	m_pProgressIconMesh = new CScreenMesh(pd3dDevice, progressIconLB, progressIconRT);

	m_pProgressImage = new CProgressImage();
	m_pProgressImage->CreateShader(pd3dDevice);
	m_pProgressImage->CreateResource(pd3dDevice);
	m_pProgressImage->m_d3dxvLeftBottom = D3DXVECTOR3(progressIconLB.x + 0.1f, progressIconLB.y, 0.0f);
	m_pProgressImage->m_d3dxvRightTop = D3DXVECTOR3(progressIconRT.x + 0.3f, progressIconRT.y, 0.0f);

#ifdef DEBUG_RENDERING_GRID
	//����� ����
	m_pDebugPrograms = new CDebugPrograms(pd3dDevice, m_pHeightMap);
#endif


	//0901 ť�� �׽�Ʈ
	m_DebugCubeShader.BuildObjects(pd3dDevice);
	m_DebugCubeShader.CreateShader(pd3dDevice);

	/*
	//0901 �߸� ���� -> �ϴ� Ŭ�� ��ü���� �ٷ� push�Ѵ�.
	for (int i = m_pStaticShader->m_vectorObjects.size() - 1; i >= 0; --i)
	{
		if (!(OBJECT_TYPE::Mineral == (OBJECT_TYPE)m_pStaticShader->m_vectorObjects[i]->m_ObjectType || OBJECT_TYPE::Gas == (OBJECT_TYPE)m_pStaticShader->m_vectorObjects[i]->m_ObjectType))
			continue;
		
		for (int j = 0; j < m_pStaticShader->m_vectorObjects[i]->m_nValidObjects; ++j)
			m_listNeutral.push_back(m_pStaticShader->m_vectorObjects[i]->m_ppObjects[j]);
	}
	*/

	//1206 ��ũƮ�� ����
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Armory, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Barrack, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Bunker, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Commandcenter, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Engineeringbay, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Factory, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Fusioncore, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Ghostacademy, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Missileturret, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Reactor, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Refinery, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Sensortower, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Starport, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Supplydepot, false));
	m_mapIsBuildingExist.insert(make_pair(OBJECT_TYPE::Techlab, false));

	//����ó�� �̹���
	D3DXVECTOR2 resultLB{ -0.5f, -0.5f };
	D3DXVECTOR2 resultRT{ +0.5f, +0.5f };
	m_pResultImageMesh = new CScreenMesh(pd3dDevice, resultLB, resultRT);
	
	ID3D11SamplerState *pd3dResultSampler = nullptr;
	ID3D11ShaderResourceView *pd3dResultTexture = nullptr;

	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dResultSampler);
	CTexture *pVictoryTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	CTexture *pDefeatTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/victory.png"), NULL, NULL, &pd3dResultTexture, NULL);
	pVictoryTexture->SetTexture(0, pd3dResultTexture);
	pVictoryTexture->SetSampler(0, pd3dResultSampler);
	m_pResultImageVictory = pVictoryTexture;

	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Additional/defeat.png"), NULL, NULL, &pd3dResultTexture, NULL);
	pDefeatTexture->SetTexture(0, pd3dResultTexture);
	pDefeatTexture->SetSampler(0, pd3dResultSampler);
	m_pResultImageDefeat = pDefeatTexture;

	pd3dResultSampler->Release();
	pd3dResultTexture->Release();
	//end ����ó��

	printf("�ε� �Ϸ�. ���ӿ� �����մϴ�...\n");
	ShowCursor(false);
	
#ifdef ENABLE_FREE_CONSOLE
	FreeConsole();
#endif

}

void CScene::ReleaseObjects()
{
	if (m_ppShaders)
	{
		for (auto i = 0; i < m_nShaders; ++i)
		{
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pStaticShader)	m_pStaticShader->Release();
	if (m_pAnimationShader)	m_pAnimationShader->Release();
}

//������ �ٿ���ڽ��� ���߿��α��� �����ش�.
bool CScene::IsPickedObject(D3DXVECTOR3& vMousePosW, CGameObject* pTarget)
{
	float x = vMousePosW.x;
	float z = vMousePosW.z;
	AABB bc = pTarget->m_bcMeshBoundingCube;
	bc.Update(&(pTarget->m_d3dxmtxWorld));
	if (pTarget->m_bAirUnit)
	{
		float zOffset = (AIR_UNIT_OFFSET - (pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f)) * 0.5946f; // sqrt(0.5) = 0.707f�� 2�� �������� sqrt�� z �������� �� ���̴�.
		bc.m_d3dxvMinimum.z += zOffset;
		bc.m_d3dxvMaximum.z += zOffset;
	}

	if (x >= bc.m_d3dxvMinimum.x && x <= bc.m_d3dxvMaximum.x && z >= bc.m_d3dxvMinimum.z && z <= bc.m_d3dxvMaximum.z)
		return true;
	else
		return false;
		
}

bool CScene::IsCrushedAABB(D3DXVECTOR3& box1Min, D3DXVECTOR3& box1Max, D3DXVECTOR3& box2Min, D3DXVECTOR3& box2Max)
{
	if (box1Min.x <= box2Max.x && box1Max.x >= box2Min.x && box1Min.z <= box2Max.z && box1Max.z >= box2Min.z)
		return true;
	else
		return false;
}

void CScene::GetMouseRectObjects()
{
	D3DXVECTOR3 leftBot, rightTop;

	if (m_d3dxvOldCursorPosition.x < m_d3dxvNextCursorPosition.x)
	{
		if (m_d3dxvOldCursorPosition.z < m_d3dxvNextCursorPosition.z)	//LT
		{
			rightTop = D3DXVECTOR3(m_d3dxvOldCursorPosition.x, m_d3dxvNextCursorPosition.y + 1.0f, m_d3dxvNextCursorPosition.z);
			leftBot = D3DXVECTOR3(m_d3dxvNextCursorPosition.x, m_d3dxvNextCursorPosition.y + 1.0f, m_d3dxvOldCursorPosition.z);
		}
		else  //LB
		{
			rightTop = D3DXVECTOR3(m_d3dxvOldCursorPosition.x, m_d3dxvNextCursorPosition.y + 1.0f, m_d3dxvOldCursorPosition.z);
			leftBot = D3DXVECTOR3(m_d3dxvNextCursorPosition.x, m_d3dxvNextCursorPosition.y + 1.0f, m_d3dxvNextCursorPosition.z);
		}
	}
	else //������
	{
		if (m_d3dxvOldCursorPosition.z < m_d3dxvNextCursorPosition.z)	//RT
		{
			rightTop = D3DXVECTOR3(m_d3dxvNextCursorPosition.x, m_d3dxvNextCursorPosition.y + 1.0f, m_d3dxvNextCursorPosition.z);
			leftBot = D3DXVECTOR3(m_d3dxvOldCursorPosition.x, m_d3dxvNextCursorPosition.y + 1.0f, m_d3dxvOldCursorPosition.z);
		}
		else //RB
		{
			rightTop = D3DXVECTOR3(m_d3dxvNextCursorPosition.x, m_d3dxvNextCursorPosition.y + 1.0f, m_d3dxvOldCursorPosition.z);
			leftBot = D3DXVECTOR3(m_d3dxvOldCursorPosition.x, m_d3dxvNextCursorPosition.y + 1.0f, m_d3dxvNextCursorPosition.z);
		}
	}

	RECT mouseRect;
	SetRect(&mouseRect, rightTop.x, rightTop.z, leftBot.x, leftBot.z);
	D3DXVECTOR3 mouseMin, mouseMax;
	mouseMin = D3DXVECTOR3(rightTop.x, 0.0f, leftBot.z);
	mouseMax = D3DXVECTOR3(leftBot.x, 0.0f, rightTop.z);

	int nValidSelectedObjects = 0;
	D3DXVECTOR3 objMin, objMax;
	AABB bc;
	float zOffset;
	//1. �Ʊ� ���� �簢����ŷ �˻�
	for (auto d : m_listMy)
	{
		if (!isAnimateObject(d->m_nObjectType)) continue;

		bc = d->m_bcMeshBoundingCube;
		bc.Update(&(d->m_d3dxmtxWorld));
		if (d->m_bAirUnit)
		{
			zOffset = (AIR_UNIT_OFFSET - (d->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f)) * 0.5946f; // sqrt(0.5) = 0.707f�� 2�� �������� sqrt�� z �������� �� ���̴�.
			bc.m_d3dxvMinimum.z += zOffset;
			bc.m_d3dxvMaximum.z += zOffset;
		}
		objMin = bc.m_d3dxvMinimum; objMax = bc.m_d3dxvMaximum;

		//mouseMin.x���� objMax.x�� ũ��, mouseMax.x���� objMin.x�� �۾ƾ��Ѵ�. �̷��� �ڽ��� x�����ȿ� �ִ�.
		//mouseMin.z���� objMax.z�� ũ��, mouseMax.z���� objMin.z�� �۾ƾ��Ѵ�. �̷��� �ڽ��� z���� �ȿ� �ִ�.
		//if (mouseMin.x <= objMax.x && mouseMax.x >= objMin.x && mouseMin.z <= objMax.z && mouseMax.z >= objMin.z)
		if(IsCrushedAABB(mouseMin, mouseMax, objMin, objMax))
		{
			m_ppSelectedObjects[nValidSelectedObjects] = d;
			nValidSelectedObjects++;

			if (nValidSelectedObjects == MAX_SELECTED_OBJECTS) break;
		}
	}

	if (nValidSelectedObjects > 0)
	{
		m_nValidSelectedObjects = nValidSelectedObjects;

		//��ǥ������ �׳� 0�� ������Ʈ�� �Ѵ�...
		int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
		if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
		else //�ǹ��� ��� what�� offset���̴�
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

		return;
	}

	//2. ���ϵ��� ���� -> ���� ���� �簢����ŷ �˻�
	for (auto d : m_listEnemyInSight)
	{
		if (!isAnimateObject(d->m_nObjectType)) continue;

		bc = d->m_bcMeshBoundingCube;
		bc.Update(&(d->m_d3dxmtxWorld));
		if (d->m_bAirUnit)
		{
			zOffset = (AIR_UNIT_OFFSET - (d->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f)) * 0.5946f; // sqrt(0.5) = 0.707f�� 2�� �������� sqrt�� z �������� �� ���̴�.
			bc.m_d3dxvMinimum.z += zOffset;
			bc.m_d3dxvMaximum.z += zOffset;
		}
		objMin = bc.m_d3dxvMinimum; objMax = bc.m_d3dxvMaximum;

		//mouseMin.x���� objMax.x�� ũ��, mouseMax.x���� objMin.x�� �۾ƾ��Ѵ�. �̷��� �ڽ��� x�����ȿ� �ִ�.
		//mouseMin.z���� objMax.z�� ũ��, mouseMax.z���� objMin.z�� �۾ƾ��Ѵ�. �̷��� �ڽ��� z���� �ȿ� �ִ�.
		//if (mouseMin.x <= objMax.x && mouseMax.x >= objMin.x && mouseMin.z <= objMax.z && mouseMax.z >= objMin.z)
		if (IsCrushedAABB(mouseMin, mouseMax, objMin, objMax))
		{
			m_ppSelectedObjects[nValidSelectedObjects] = d;
			nValidSelectedObjects++;
			if (nValidSelectedObjects == MAX_SELECTED_OBJECTS) break;
		}
	}
	if (nValidSelectedObjects > 0)
	{
		m_nValidSelectedObjects = nValidSelectedObjects;

		//��ǥ������ �׳� 0�� ������Ʈ�� �Ѵ�...
		int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
		if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
		else //�ǹ��� ��� what�� offset���̴�
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

		return;
	}


	/*
	int nValidSelectedObjects = 0;
	//1. �Ʊ� ���� ��ŷ �˻�
	//2. �Ʊ��� �ϳ��� ��ŷ���� �ʾҴٸ� ���� ���� ��ŷ
	for (auto d : m_listMy)
	{
		float x = d->GetPosition().x;
		float z = d->GetPosition().z;
		if (mouseRect.left <= x && mouseRect.right >= x	&& mouseRect.bottom <= z && mouseRect.top >= z)
		{
			m_ppSelectedObjects[nValidSelectedObjects] = d;
			nValidSelectedObjects++;
		}
	}
	if (nValidSelectedObjects > 0)
	{
		m_nValidSelectedObjects = nValidSelectedObjects;

		//��ǥ������ �׳� 0�� ������Ʈ�� �Ѵ�...
		int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
		if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
		else //�ǹ��� ��� what�� offset���̴�
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

		return;
	}
	
	//return���� �۵����� ���� -> �Ʊ� ������ ��ŷ���� �ʾҴ�.
	for (auto d : m_listEnemyInSight)
	{
		float x = d->GetPosition().x;
		float z = d->GetPosition().z;
		if (mouseRect.left <= x && mouseRect.right >= x	&& mouseRect.bottom <= z && mouseRect.top >= z)
		{
			m_ppSelectedObjects[nValidSelectedObjects] = d;
			nValidSelectedObjects++;
		}
	}
	if (nValidSelectedObjects > 0)
	{
		m_nValidSelectedObjects = nValidSelectedObjects;

		//��ǥ������ �׳� 0�� ������Ʈ�� �Ѵ�...
		int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
		if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
		else //�ǹ��� ��� what�� offset���̴�
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

		return;
	}
	*/

	/*
	//���� ���� ���̴��� ��� �Ǵ��ؾ��ұ�?
	vector<RenderObjectData*> vec;
	vec = m_pAnimationShader->m_vectorObjects;

	for (int i = vec.size() - 1; i >= 0; --i)
	{
		for (int j = 0; j < vec[i]->m_nValidObjects; ++j)
		{
			if (isEnemyObject(vec[i]->m_ppObjects[j]->m_nObjectType)) continue;

			if (true == vec[i]->m_ppObjects[j]->IsVisible(m_pCamera))
			{
				float x = vec[i]->m_ppObjects[j]->GetPosition().x;
				float z = vec[i]->m_ppObjects[j]->GetPosition().z;
				if (mouseRect.left <= x && mouseRect.right >= x
					&& mouseRect.bottom <= z && mouseRect.top >= z)
				{
					m_ppSelectedObjects[nValidSelectedObjects] = vec[i]->m_ppObjects[j];
					nValidSelectedObjects++;
				}
			}
		}
	}

	if (0 == nValidSelectedObjects) //MouseRect�� �׷����� ���� ��ü���� �ƹ��� ��ŷ�� ���� �ʾҴٸ�
	{
		//���� ��ü�鿡 ���� ��ŷ �˻�� ����
	}
	else //�ϳ��� ��ŷ�� �Ǿ���. ��ŷ ������ �����Ѵ�.
	{
		m_nValidSelectedObjects = nValidSelectedObjects;
		return;
	}

	//��ŷ�� �� �Ǿ��ٸ� ���� ���̴��� ��ü�鵵 ��ȸ�ϵ��� �Ѵ�.
	vec = m_pStaticShader->m_vectorObjects;

	for (int i = vec.size() - 1; i >= 0; --i)
	{
		for (int j = 0; j < vec[i]->m_nValidObjects; ++j)
		{
			if (isEnemyObject(vec[i]->m_ppObjects[j]->m_nObjectType)) continue;

			if (true == vec[i]->m_ppObjects[j]->IsVisible(m_pCamera))
			{
				float x = vec[i]->m_ppObjects[j]->GetPosition().x;
				float z = vec[i]->m_ppObjects[j]->GetPosition().z;
				if (mouseRect.left <= x && mouseRect.right >= x
					&& mouseRect.bottom <= z && mouseRect.top >= z)
				{
					m_ppSelectedObjects[nValidSelectedObjects] = vec[i]->m_ppObjects[j];
					nValidSelectedObjects++;
				}
			}
		}
	}


	if (0 == nValidSelectedObjects) //MouseRect�� �׷����� ���� ��ü���� �ƹ��� ��ŷ�� ���� �ʾҴٸ�
	{
		//�ƹ��͵� �� �� ����. ���� ��ŷ ������ ��� ������ �ֵ��� �Ѵ�.
	}
	else //�ϳ��� ��ŷ�� �Ǿ���. ��ŷ ������ �����Ѵ�.
	{
		m_nValidSelectedObjects = nValidSelectedObjects;

		//��ǥ������ �׳� 0�� ������Ʈ�� �Ѵ�...
		int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
		if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
		else //�ǹ��� ��� what�� offset���̴�
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);
	}
	*/
}

void CScene::GetMouseCursorObject()
{
	//nextPos�� ���ؼ� ó���ϸ� ��
	//y��, �� ���̿� ���� ó���� �ȵǴ� �����̴�. ������ �غ��� © ����
	float x = m_d3dxvNextCursorPosition.x;
	float z = m_d3dxvNextCursorPosition.z;

	//1. �� ���� ����Ʈ ��ȸ
	//2. �� ������ ��ŷ �ȵǾ����� �þ� �� �� ����Ʈ ��ȸ
	//3. �׷����� �ƴ϶�� �߸� �ڿ� ��ŷ���� �˻�
	float zOffset;
	for (auto d : m_listMy)
	{
		AABB bc = d->m_bcMeshBoundingCube;
		bc.Update(&(d->m_d3dxmtxWorld));
		if (d->m_bAirUnit)
		{
			zOffset = (AIR_UNIT_OFFSET - (d->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f)) * 0.5946f; // sqrt(0.5) = 0.707f�� 2�� �������� sqrt�� z �������� �� ���̴�.
			bc.m_d3dxvMinimum.z += zOffset;
			bc.m_d3dxvMaximum.z += zOffset;
		}
			
		//if (x >= bc.m_d3dxvMinimum.x && x <= bc.m_d3dxvMaximum.x && z >= bc.m_d3dxvMinimum.z && z <= bc.m_d3dxvMaximum.z)
		if(IsPickedObject(D3DXVECTOR3(x, 0.0f, z), d))
		{
			m_nValidSelectedObjects = 1;
			m_ppSelectedObjects[0] = d;

			if(isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			{
				int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
				CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
			}
			else
			{
				int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
				CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);
			}
			return;
		}
	}

	//return �ȵ� -> �� ���� ��ŷ���� �˻�
	for (auto d : m_listEnemyInSight)
	{
		AABB bc = d->m_bcMeshBoundingCube;
		bc.Update(&(d->m_d3dxmtxWorld));
		if (d->m_bAirUnit)
		{
			zOffset = (AIR_UNIT_OFFSET - (d->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f)) * 0.5946f; // sqrt(0.5) = 0.707f�� 2�� �������� sqrt�� z �������� �� ���̴�.
			bc.m_d3dxvMinimum.z += zOffset;
			bc.m_d3dxvMaximum.z += zOffset;
		}

		//if (x >= bc.m_d3dxvMinimum.x && x <= bc.m_d3dxvMaximum.x && z >= bc.m_d3dxvMinimum.z && z <= bc.m_d3dxvMaximum.z)
		if (IsPickedObject(D3DXVECTOR3(x, 0.0f, z), d))
		{
			m_nValidSelectedObjects = 1;
			m_ppSelectedObjects[0] = d;

			if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			{
				int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
				CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
			}
			else
			{
				int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
				CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);
			}
			return;
		}
	}

	//return �ȵ� -> �߸� �ڿ� ��ŷ���� �˻�
	for (auto d : m_listNeutral)
	{
		if (false == d->IsInSight()) continue;

		AABB bc = d->m_bcMeshBoundingCube;
		bc.Update(&(d->m_d3dxmtxWorld));
		if (x >= bc.m_d3dxvMinimum.x && x <= bc.m_d3dxvMaximum.x && z >= bc.m_d3dxvMinimum.z && z <= bc.m_d3dxvMaximum.z)
		{
			m_nValidSelectedObjects = 1;
			m_ppSelectedObjects[0] = d;
			return;
		}
	}

	//�ƹ��͵� �� �����. ���� ������ �����Ѵ�. ���� �ڵ��� �ʱ�ȭ�Ϸ��� ������ ����
	//m_nValidSelectedObjects = 0;

	/*
	//���� ���� ���̴��� ��� �Ǵ��ؾ��ұ�?
	vector<RenderObjectData*> vec;
	vec = m_pAnimationShader->m_vectorObjects;
	for (int i = vec.size() - 1; i >= 0; --i)
	{
		for (int j = 0; j < vec[i]->m_nValidObjects; ++j)
		{
			auto pObject = vec[i]->m_ppObjects[j];
			//if (isEnemyObject(pObject->m_nObjectType)) continue;

			if (true == pObject->IsVisible(m_pCamera))
			{
				AABB bc = pObject->m_bcMeshBoundingCube;
				bc.Update(&(pObject->m_d3dxmtxWorld));
				if (x >= bc.m_d3dxvMinimum.x && x <= bc.m_d3dxvMaximum.x &&
					z >= bc.m_d3dxvMinimum.z && z <= bc.m_d3dxvMaximum.z)
				{
					m_nValidSelectedObjects = 1;
					m_ppSelectedObjects[0] = pObject;

					int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
					CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
				
					return;
				}
			}
		}
	}

	//return; ���� ���������� �ʾҴٴ� ���� ���� ��ü���� ��ŷ�� ���� �ʾҴ�. ���� ��ü�� ��ȸ�Ѵ�.
	vec = m_pStaticShader->m_vectorObjects;
	for (int i = vec.size() - 1; i >= 0; --i)
	{
		for (int j = 0; j < vec[i]->m_nValidObjects; ++j)
		{
			auto pObject = vec[i]->m_ppObjects[j];
			//if (isEnemyObject(pObject->m_nObjectType)) continue;

			if (true == pObject->IsVisible(m_pCamera))
			{
				AABB bc = pObject->m_bcMeshBoundingCube;
				bc.Update(&(pObject->m_d3dxmtxWorld));
				if (x >= bc.m_d3dxvMinimum.x && x <= bc.m_d3dxvMaximum.x &&
					z >= bc.m_d3dxvMinimum.z && z <= bc.m_d3dxvMaximum.z)
				{
					m_nValidSelectedObjects = 1;
					m_ppSelectedObjects[0] = pObject;

					int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
					CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

					return;
				}
			}
		}
	}
	*/
}

void CScene::BuildFogBuffer(ID3D11Device *pd3dDevice)
{
	float xPos, zPos;

	for (int j = 0; j < (NUM_FOG_SLICE); ++j)
	{
		for (int i = 0; i < (NUM_FOG_SLICE); ++i)
		{
			//0.0f ~ 1.0f ����
			xPos = (float)i / (NUM_FOG_SLICE - 1.0f);
			zPos = 1.0f - ((float)j / (NUM_FOG_SLICE - 1.0f));

			//-102.5f ~ +102.5f
			xPos = xPos * MAP_WIDTH - (MAP_WIDTH / 2.0f);
			zPos = zPos * MAP_HEIGHT - (MAP_HEIGHT / 2.0f);

			m_d3dxvFogData[i + (int)(j*NUM_FOG_SLICE)] = D3DXVECTOR4(xPos, m_pHeightMap->GetHeight(xPos, zPos), zPos, 0.0f);

#ifdef DEBUG_DISABLE_FOG
			m_d3dxvFogData[i + (int)(j*(NUM_FOG_SLICE))].w = 2.0f;
#endif
		}
	}
	
	/*
	//������۷� �����ҵ� �� ū �뷮
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(FogData) * (NUM_FOG_SLICE) * (NUM_FOG_SLICE );	//�ִ�ũ�� 4096 * 16����Ʈ �� ����;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA pData;
	ZeroMemory(&pData, sizeof(pData));
	pData.pSysMem = m_d3dxvFogData;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &pData, &m_pd3dcbFogBuffer);
	*/
	
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.ByteWidth = sizeof(float) * NUM_FOG_SLICE * NUM_FOG_SLICE;
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = m_d3dxvFogData;
	sd.SysMemPitch = sizeof(float) * NUM_FOG_SLICE * NUM_FOG_SLICE;
	pd3dDevice->CreateBuffer(&bd, &sd, &m_pd3dFogBuffer);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.ElementWidth = sizeof(float);
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = NUM_FOG_SLICE * NUM_FOG_SLICE;
	(pd3dDevice->CreateShaderResourceView(m_pd3dFogBuffer, &srvDesc, &m_pd3dSRVFog));
}

//**********************************************************************
//������ ��ü�� ��ŷ�� ó���մϴ�.
CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, bool isLeftButton)
{
	//��ü�� ��ŷ�ϴ� �ڵ��̹Ƿ� �޽ú��� ���� ���� �ٿ�� �ڽ� ���� Ȯ���Ѵٸ� ū ȿ���� ���� �� ���� ������?
	if (!m_pCamera) return(nullptr);

	D3DXMATRIX d3dxmtxView = m_pCamera->GetViewMatrix();
	D3DXMATRIX d3dxmtxProjection = m_pCamera->GetProjectionMatrix();
	D3D11_VIEWPORT d3dViewport = m_pCamera->GetViewport();

	D3DXVECTOR3 d3dxvPickPosition;
	/*ȭ�� ��ǥ���� �� (xClient, yClient)�� ȭ�� ��ǥ ��ȯ�� ����ȯ�� ���� ��ȯ�� ����ȯ�� �Ѵ�.
	�� ����� ī�޶� ��ǥ���� ���̴�. ���� ����� ī�޶󿡼� z-������ �Ÿ��� 1�̹Ƿ� z-��ǥ�� 1�� �����Ѵ�.*/
	d3dxvPickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / d3dxmtxProjection._11;
	d3dxvPickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / d3dxmtxProjection._22;
	d3dxvPickPosition.z = 1.0f;

	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	MESHINTERSECTINFO d3dxIntersectInfo;

	CGameObject *pIntersectedObject = NULL, *pNearestObject = NULL;
	//���� ��� ���̴� ��ü�� ���Ͽ� ��ŷ�� ó���Ͽ� ī�޶�� ���� ����� ��ŷ�� ��ü�� ã�´�. ��ī�̹ڽ� ����
	int i = isLeftButton ? 2 : 1;	//LB�� ���� ����, RB�� ���� ����
	for (i; i < m_nShaders; i++)
	{
		pIntersectedObject = m_ppShaders[i]->PickObjectByRayIntersection(&d3dxvPickPosition, &d3dxmtxView, &d3dxIntersectInfo);
		if (pIntersectedObject && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pNearestObject = pIntersectedObject;
		}
	}
	return(pNearestObject);
}

D3DXVECTOR3 CScene::GetPositionPicking(int xClient, int yClient)
{
	if (!m_pCamera) return(D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX));

	D3DXMATRIX d3dxmtxView = m_pCamera->GetViewMatrix();
	D3DXMATRIX d3dxmtxProjection = m_pCamera->GetProjectionMatrix();
	D3D11_VIEWPORT d3dViewport = m_pCamera->GetViewport();

	D3DXVECTOR3 d3dxvPickPosition;
	/*ȭ�� ��ǥ���� �� (xClient, yClient)�� ȭ�� ��ǥ ��ȯ�� ����ȯ�� ���� ��ȯ�� ����ȯ�� �Ѵ�.
	�� ����� ī�޶� ��ǥ���� ���̴�. ���� ����� ī�޶󿡼� z-������ �Ÿ��� 1�̹Ƿ� z-��ǥ�� 1�� �����Ѵ�.*/
	d3dxvPickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / d3dxmtxProjection._11;
	d3dxvPickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / d3dxmtxProjection._22;
	d3dxvPickPosition.z = 1.0f;

	D3DXVECTOR3 d3dxvPickRayPosition, d3dxvPickRayDirection;
	MESHINTERSECTINFO d3dxIntersectInfo;
	d3dxIntersectInfo.m_fDistance = FLT_MAX;

	D3DXVECTOR3 nearstPosition{ FLT_MAX, FLT_MAX, FLT_MAX }, result;
	float distance = FLT_MAX;

	//0811 ����
	//���� ��ŷ�ڵ�� ���� ���� ��ġ�� ��� �ִ�. �� ��ġ�κ��� xz�� ���Ͽ� ��ü�� �����ϴ� ��� �� ��ü��
	//��ŷ�ߴٰ� ó���ϰ� �ִ�. ���� ������ ����ȭ����. �㳪 ���� ���������� �����ϸ鼭 �ణ�� ������ �ʿ��ϴ�.
	//���� ���ֵ鿡 ���ؼ��� �޽� ��ŷ�� �����ϰ�, ���� ������ ��ŷ���� ���� ��� ���� ��ŷ�� �����ϵ��� �Ѵ�.
	//�����Ѵ�... ������ �κ��� ���⿡ �ϴ� �����ϵ��� ��

	if (m_ppShaders[2])
	{
		CGameObject* pObject = m_ppShaders[2]->GetppObjects()[0];
		pObject->GenerateRayForPicking(&d3dxvPickPosition, &pObject->m_d3dxmtxWorld, &d3dxmtxView, &d3dxvPickRayPosition, &d3dxvPickRayDirection);

		//�޽ð� �������ٸ� �޽��� visable ���θ� ������ ��ȸ�ϵ��� �ϸ� �����ڴµ�?
		CMesh* pMesh = nullptr;
		for (int i = 0; i < pObject->m_nMeshes; ++i)
		{
			pMesh = pObject->GetMesh(i);
			
			bool bIsVisible = true;
			
			AABB bcBoundingCube = pMesh->GetBoundingCube();
			bcBoundingCube.Update(&pObject->m_d3dxmtxWorld);	//���� ������ǥ�� ��������̶� �������� �ʴ´�.
			bIsVisible = m_pCamera->IsInFrustum(&bcBoundingCube);
			
			if (bIsVisible)
			{
				result = pMesh->GetPositionRayIntersection(&d3dxvPickRayPosition, &d3dxvPickRayDirection, &d3dxIntersectInfo);
				if (abs(d3dxIntersectInfo.m_fDistance) < distance)
				{
					distance = abs(d3dxIntersectInfo.m_fDistance);
					nearstPosition = result;
				}
			}
		}
	}
	return(nearstPosition);
}

bool CScene::IsClickMiniMap(float xScreen, float yScreen, bool bLeftClicked, bool bAttack)
{
	//�̴ϸ��� Ŭ���Ѱ����� ���� �˻�
	float xPos = xScreen / m_fWndClientWidth;
	float yPos = (m_fWndClientHeight - yScreen) / m_fWndClientHeight;	//+y�� �������� �ٲ�
	
	float miniMapLeft = (MINIMAP_POSITION_LEFT + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float miniMapRight = (MINIMAP_POSITION_RIGHT + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float miniMapBottom = (MINIMAP_POSITION_BOTTOM + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float miniMapTop = (MINIMAP_POSITION_TOP + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float miniMapWidth = miniMapRight - miniMapLeft;
	float miniMapLength = miniMapTop - miniMapBottom;


	if (xPos > miniMapRight || xPos < miniMapLeft || yPos > miniMapTop || yPos < miniMapBottom)
		return false;

	//�̴ϸ��� Ŭ���ߴٸ� �� ��ġ�� ���� �ڷᱸ���� �����Ѵ�.
	xPos = (xPos - miniMapLeft) / miniMapWidth;
	yPos = (yPos - miniMapBottom) / miniMapLength;

	int xIdx = xPos * (NUM_FOG_SLICE - 1.0f);
	int zIdx = (1.0f - yPos) * (NUM_FOG_SLICE - 1.0f);
	D3DXVECTOR4 clickedPosition = m_d3dxvFogData[xIdx + (int)(zIdx*NUM_FOG_SLICE)];

	//�� ���� ������ �ʵ��� �����Ѵ�.
	if (clickedPosition.x <= CAMERA_MIN_LOOKAT_X)	clickedPosition.x = CAMERA_MIN_LOOKAT_X;
	if (clickedPosition.x >= CAMERA_MAX_LOOKAT_X)	clickedPosition.x = CAMERA_MAX_LOOKAT_X;
	if (clickedPosition.z <= CAMERA_MIN_LOOKAT_Z)	clickedPosition.z = CAMERA_MIN_LOOKAT_Z;
	if (clickedPosition.z >= CAMERA_MAX_LOOKAT_Z)	clickedPosition.z = CAMERA_MAX_LOOKAT_Z;

	//ī�޶��� �̵��� ī�޶� ��ġ ��ü�� �����ϴ� ���� �ƴ� LookAtWorld�� �̵���Ű�� �� ����ŭ��
	//ī�޶� ��ġ �̵��� �ؾ��Ѵ�.
	float xDifference = clickedPosition.x - m_pCamera->GetLookAtPosition().x;
	float zDifference = clickedPosition.z - m_pCamera->GetLookAtPosition().z;
	D3DXVECTOR3 vMove{ xDifference, 0.0f, zDifference };

	//����ó���� �ƴ� ���
	if (false == bAttack)
	{

		//�ϴ� ���� Ŭ������ ó������
		if (bLeftClicked)
		{
			m_pCamera->Move(vMove);
			m_pCamera->SetLookAtWorld(m_pCamera->GetLookAtPosition() + vMove);
			m_pCamera->GenerateViewMatrix();
		}
		else if (m_nValidSelectedObjects > 0)
			//������ ���콺 Ŭ��, ���� �� �Լ��� �����Ϸ��� SelectedObject�� ��� 1 �� �̻��� ����̴�.
		{
			D3DXVECTOR3 vTo = D3DXVECTOR3(clickedPosition.x, clickedPosition.y, clickedPosition.z);

			for (int i = 0; i < m_nValidSelectedObjects; ++i)
			{
				if (isEnemyObject(m_ppSelectedObjects[i]->m_nObjectType)) continue;
				if (!isAnimateObject(m_ppSelectedObjects[i]->m_nObjectType)) continue;

				if (Vec2DDistance(Vector2D(vTo.x, vTo.z), Vector2D(m_ppSelectedObjects[i]->GetPosition().x, m_ppSelectedObjects[i]->GetPosition().z)) <= 0.2f) //�ֺ� Ŭ��
					continue;

				if ((int)OBJECT_TYPE::Scv == m_ppSelectedObjects[i]->m_nObjectType)
				{
					CAnimationObject* pObject = (CAnimationObject*)m_ppSelectedObjects[i];
					if (pObject->m_bMoveForBuild)
					{
						pObject->m_bMoveForBuild = false;
						pObject->m_WillBuildObjectType = OBJECT_TYPE::Invalid;
					}
				}

				vTo.y = 0.0f;
				CS_Packet_Move_Object(m_ppSelectedObjects[i], vTo, true);

				//m_ppSelectedObjects[i]->m_listTargetPosition = m_pAStarComponent->SearchAStarPath(m_ppSelectedObjects[i]->GetPosition(), vTo);
				//m_ppSelectedObjects[i]->m_bLatestFindPath = true;
				//m_ppSelectedObjects[i]->SetTargetPosition(m_ppSelectedObjects[i]->m_listTargetPosition.front());
			}
		}
	}
	else // bAttack true
	{
		//move_with_guard �Ǵ� have_target
		D3DXVECTOR3 vec = D3DXVECTOR3(clickedPosition.x, clickedPosition.y, clickedPosition.z);
		int mouse_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(vec);
		CGameObject* pTarget = nullptr;
		for (auto d : m_listEnemyInSight)	//�Ʊ��� ������ ���� ������, �ϴ� ������ �����ϴ� ������ �Ѵ�.
		{
			/*
			AABB bc = d->m_bcMeshBoundingCube;
			bc.Update(&(d->m_d3dxmtxWorld));
			if (vec.x >= bc.m_d3dxvMinimum.x && vec.x <= bc.m_d3dxvMaximum.x && vec.z >= bc.m_d3dxvMinimum.z && vec.z <= bc.m_d3dxvMaximum.z)
			*/
			if (IsPickedObject(vec, d))
			{
				pTarget = d;
				break;
			}
		}
		//1. move_with_guard
		if (nullptr == pTarget)
		{
			for (int i = 0; i < m_nValidSelectedObjects; ++i)
			{
				int obj_type = m_ppSelectedObjects[i]->m_nObjectType;
				if (!isAnimateObject(obj_type) || isEnemyObject(obj_type)) continue;

				if (m_ppSelectedObjects[i]->m_fObjectAttackRange == 0.0f)
				{ //���ݺҰ� �����̶�� RButton �̵��� ���� ó��
					CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
					continue;
				}

				CAnimationObject* pMyObject = (CAnimationObject*)m_ppSelectedObjects[i];
				pMyObject->m_pTargetObject = nullptr;
				pMyObject->CS_Packet_State(State::move_with_guard);
				CS_Packet_Move_Object(pMyObject, vec, false);
			}
		}
		else //2. have_target
		{	//�ϴ� attack�� ������. ������ attack���� ���ݹ��� ������ Ȯ���Ѵٸ� �ٷ� have_target���� �Ѿ ���̴�.
			for (int i = 0; i < m_nValidSelectedObjects; ++i)
			{
				int obj_type = m_ppSelectedObjects[i]->m_nObjectType;
				if (!isAnimateObject(obj_type) || isEnemyObject(obj_type)) continue;

				if (m_ppSelectedObjects[i]->m_fObjectAttackRange == 0.0f)
				{ //���ݺҰ� �����̶�� RButton �̵��� ���� ó��
					CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
					continue;
				}

				if (pTarget->m_bAirUnit && !m_ppSelectedObjects[i]->m_bAirAttack)
				{	 //���� ��������, ���� ���߰��� ���ϸ�
					CAnimationObject* pMyObject = (CAnimationObject*)m_ppSelectedObjects[i];
					pMyObject->m_pTargetObject = nullptr;
					pMyObject->CS_Packet_State(State::move_with_guard);
					CS_Packet_Move_Object(pMyObject, vec, false);
					continue;
				}

				//���� ������ �����ϴٸ� attack�� ����
				CAnimationObject* pMyObject = (CAnimationObject*)m_ppSelectedObjects[i];
				pMyObject->m_pTargetObject = pTarget;

				//���� ó�� �ȵǼ� attack �ٷ� ���� �ʰ� have_target�� ��ġ�� �׽�Ʈ						
				/*
				float distance = (pMyObject->GetPosition().x - (pTarget->GetPosition().x - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (pMyObject->GetPosition().x - (pTarget->GetPosition().x - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
				+ (pMyObject->GetPosition().z - (pTarget->GetPosition().z - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (pMyObject->GetPosition().z - (pTarget->GetPosition().z - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
				if (distance <= (pMyObject->m_fObjectAttackRange * pMyObject->m_fObjectAttackRange))
				{
				pMyObject->CS_Packet_State(State::attack);
				pMyObject->m_bProcessedAttack = true;
				pMyObject->m_bPickTarget = true;
				}
				else //���� ���� �Ұ� -> ��븦 have_target���� �д�.
				*/
				{
					pMyObject->CS_Packet_State(State::have_target);
					CS_Packet_Move_Object(pMyObject, vec, false);
					pMyObject->m_bPickTarget = true;
				}
			}
		}
	}

	return true;
}

bool CScene::IsCursorInCommand(float xScreen, float yScreen, int& commandPosNum)
{
	float xPos = xScreen / m_fWndClientWidth;
	float yPos = (m_fWndClientHeight - yScreen) / m_fWndClientHeight;	//+y�� �������� �ٲ�

	D3DXVECTOR2 vLeftBottom = m_mapCommandPosition.find(11)->second.first;
	D3DXVECTOR2 vRightTop = m_mapCommandPosition.find(5)->second.second;

	float commandLeft = (vLeftBottom.x + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float commandRight = (vRightTop.x + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float commandBottom = (vLeftBottom.y + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float commandTop = (vRightTop.y + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float commandWidth = commandRight - commandLeft;
	float commandLength = commandTop - commandBottom;

	if (xPos > commandRight || xPos < commandLeft || yPos > commandTop || yPos < commandBottom)
	{
		commandPosNum = 0;
		return false;
	}
	else
	{
		//�ȿ� �����Ѵٸ�, ��� ��ġ���� �Ű������� �������ش�.
		D3DXVECTOR2 vLB, vRT;
		float left, right, bottom, top;
		for (auto& d : m_mapCommandPosition)
		{
			vLB = d.second.first; vRT = d.second.second;
			left = (vLB.x + 1.0f) / 2.0f; // 0.0f ~ 1.0f
			right = (vRT.x + 1.0f) / 2.0f; // 0.0f ~ 1.0f
			bottom = (vLB.y + 1.0f) / 2.0f; // 0.0f ~ 1.0f
			top = (vRT.y + 1.0f) / 2.0f; // 0.0f ~ 1.0f

			if (left <= xPos && xPos < right && bottom <= yPos && yPos < top)
			{
				commandPosNum = d.first;
				break;
			}
		}

		return true;
	}
}

void CScene::ProcessCursorInCommand(int commandPosNum, bool bLeftClicked, bool* bDisplayRequire)
{
	if (bLeftClicked) //�� ��ġ���� ���콺�� ��(Ŭ���ߴ� ���� ��) ���
	{
		m_bActivedBuildCheck = true;

		switch (m_nSceneState)
		{	//�ǹ����� ���� ���� �������� �ڿ� ��ȿ ���θ� �������� �ϸ� �� ���(+Ű �Է�)�� �ϰ������� ó���ȴ�.
		case SCENE_STATE::pick_scv:
			if (11 == commandPosNum)
			{	
				m_nSceneState = SCENE_STATE::scv_build;
			}
			else if (12 == commandPosNum)
			{	
				m_nSceneState = SCENE_STATE::scv_buildadv;
			}
			break;
		case SCENE_STATE::scv_build:
			if (1 == commandPosNum)
			{	//commandcenter
				m_nSceneState = SCENE_STATE::build_commandcenter;
				m_strActivedBuildCheck = "commandcenter";
			}
			else if (2 == commandPosNum)
			{	//supplydepot
				m_nSceneState = SCENE_STATE::build_supplydepot;
				m_strActivedBuildCheck = "supplydepot";
			}
			else if (3 == commandPosNum)
			{	//refinery
				m_nSceneState = SCENE_STATE::build_refinery;
				m_strActivedBuildCheck = "refinery";
			}
			else if (6 == commandPosNum)
			{	//barrack
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Commandcenter)->second)
				{ //��ũƮ�� �ǹ��� ��ɺΰ� �ִ��� Ȯ��
					m_nSceneState = SCENE_STATE::build_barrack;
					m_strActivedBuildCheck = "barrack";
				}
			}
			else if (7 == commandPosNum)
			{	//engineeringbay
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Commandcenter)->second)
				{ //��ũƮ�� �ǹ��� ��ɺΰ� �ִ��� Ȯ��
					m_nSceneState = SCENE_STATE::build_engineeringbay;
					m_strActivedBuildCheck = "engineeringbay";
				}
			}
			else if (9 == commandPosNum)
			{	//sensortower
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
				{ //��ũƮ�� �ǹ��� ������ �ִ��� Ȯ��
					m_nSceneState = SCENE_STATE::build_sensortower;
					m_strActivedBuildCheck = "sensortower";
				}
			}
			else if (10 == commandPosNum)
			{	//missileturret
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Engineeringbay)->second)
				{ //��ũƮ�� �ǹ��� ���п����Ұ� �ִ��� Ȯ��
					m_nSceneState = SCENE_STATE::build_missileturret;
					m_strActivedBuildCheck = "missileturret";
				}
			}
			else if (12 == commandPosNum)
			{	//bunker
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
				{ //��ũƮ�� �ǹ��� ������ �ִ��� Ȯ��
					m_nSceneState = SCENE_STATE::build_bunker;
					m_strActivedBuildCheck = "bunker";
				}
			}
			break;
		case SCENE_STATE::scv_buildadv:
			if (1 == commandPosNum)
			{	//factory
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
				{ //��ũƮ�� �ǹ��� ������ �ִ��� Ȯ��
					m_nSceneState = SCENE_STATE::build_factory;
					m_strActivedBuildCheck = "factory";
				}
			}
			else if (2 == commandPosNum)
			{	//starport
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Factory)->second)
				{ //��ũƮ�� �ǹ��� ���������� �ִ��� Ȯ��
					m_nSceneState = SCENE_STATE::build_starport;
					m_strActivedBuildCheck = "starport";
				}
			}
			else if (7 == commandPosNum)
			{	//armory
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Factory)->second)
				{ //��ũƮ�� �ǹ��� ���������� �ִ��� Ȯ��
					m_nSceneState = SCENE_STATE::build_armory;
					m_strActivedBuildCheck = "armory";
				}
			}
			else if (11 == commandPosNum)
			{	//ghostacademy
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Starport)->second)
				{ //��ũƮ�� �ǹ��� ���ְ����� �ִ��� Ȯ��
					m_nSceneState = SCENE_STATE::build_ghostacademy;
					m_strActivedBuildCheck = "ghostacademy";
				}
			}
			else if (12 == commandPosNum)
			{	//fusioncore
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Starport)->second)
				{ //��ũƮ�� �ǹ��� ���ְ����� �ִ��� Ȯ��
					m_nSceneState = SCENE_STATE::build_fusioncore;
					m_strActivedBuildCheck = "fusioncore";
				}
			}
			break;

			//����ǹ� Ŭ���� ���
		case SCENE_STATE::commandcenter_units:
			if (5 == commandPosNum)
			{	//scv
				CreateNewObject(OBJECT_TYPE::Scv, m_ppSelectedObjects[0]->GetPosition());
			}
			break;
		case SCENE_STATE::barrack_units:
			if (1 == commandPosNum)
			{	//marine
				CreateNewObject(OBJECT_TYPE::Marine, m_ppSelectedObjects[0]->GetPosition());
			}
			else if (2 == commandPosNum)
			{	//reaper
				CreateNewObject(OBJECT_TYPE::Reaper, m_ppSelectedObjects[0]->GetPosition());
			}
			else if (6 == commandPosNum)
			{	//marauder
				CreateNewObject(OBJECT_TYPE::Marauder, m_ppSelectedObjects[0]->GetPosition());
			}
			break;
		case SCENE_STATE::factory_units:
			if (11 == commandPosNum)
			{	//thor
				CreateNewObject(OBJECT_TYPE::Thor, m_ppSelectedObjects[0]->GetPosition());
			}
			
			break;
		case SCENE_STATE::starport_units:
			if (1 == commandPosNum)
			{	//viking
				CreateNewObject(OBJECT_TYPE::Viking, m_ppSelectedObjects[0]->GetPosition());
			}
			else if (2 == commandPosNum)
			{	//medivac
				CreateNewObject(OBJECT_TYPE::Medivac, m_ppSelectedObjects[0]->GetPosition());
			}
			else if (3 == commandPosNum)
			{	//banshee
				CreateNewObject(OBJECT_TYPE::Banshee, m_ppSelectedObjects[0]->GetPosition());
			}
			else if (6 == commandPosNum)
			{	//raven
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Fusioncore)->second)
				{ //��ũƮ�� �ǹ��� ���տ����Ұ� �ִ��� Ȯ��
					CreateNewObject(OBJECT_TYPE::Raven, m_ppSelectedObjects[0]->GetPosition());
				}
			}
			else if (7 == commandPosNum)
			{	//battlecruiser
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Fusioncore)->second)
				{ //��ũƮ�� �ǹ��� ���տ����Ұ� �ִ��� ��Ȯ
					CreateNewObject(OBJECT_TYPE::BattleCruiser, m_ppSelectedObjects[0]->GetPosition());
				}
			}	
			break;
			
		} //end switch

		if (false == (SCENE_STATE::build_armory <= m_nSceneState && m_nSceneState <= SCENE_STATE::build_techlab))
			m_bActivedBuildCheck = false;
	}

	else // Left��ư �ƴ�
	{
		//���� ��ư Ŭ���� �ƴ϶�� �� mousemove�� �ϰ� �ִٴ� ���̴�. ��Ʈ�� �����Ѵ�.
		bool isInvalid = false;
		switch (m_nSceneState)
		{
		case SCENE_STATE::scv_build:
			if (1 == commandPosNum)
			{	//commandcenter
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 400);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 0);
			}
			else if (2 == commandPosNum)
			{	//supplydepot
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 100);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 0);
			}
			else if (3 == commandPosNum)
			{	//refinery
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 75);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 0);
			}
			else if (6 == commandPosNum)
			{	//barrack
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 150);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 0);
			}
			else if (7 == commandPosNum)
			{	//engineeringbay
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 125);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 0);
			}
			else if (9 == commandPosNum)
			{	//sensortower
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 100);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 50);
			}
			else if (10 == commandPosNum)
			{	//missileturret
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 100);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 0);
			}
			else if (12 == commandPosNum)
			{	//bunker
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 100);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 0);
			}
			else
				isInvalid = true;
			break;
		case SCENE_STATE::scv_buildadv:
			if (1 == commandPosNum)
			{	//factory
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 200);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 50);
			}
			else if (2 == commandPosNum)
			{	//starport
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 150);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 100);
			}
			else if (7 == commandPosNum)
			{	//armory
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 150);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 100);
			}
			else if (11 == commandPosNum)
			{	//ghostacademy
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 150);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 50);
			}
			else if (12 == commandPosNum)
			{	//fusioncore
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 200);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 200);
			}
			else
				isInvalid = true;

			break;
		case SCENE_STATE::commandcenter_units:
			if (5 == commandPosNum)
			{	//scv
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 50);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 0);
			}
			else
				isInvalid = true;
			break;
		case SCENE_STATE::barrack_units:
			if (1 == commandPosNum)
			{	//marine
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 50);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 0);
			}
			else if (2 == commandPosNum)
			{	//reaper
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 50);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 50);
			}
			else if (6 == commandPosNum)
			{	//marauder
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 100);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 25);
			}
			else
				isInvalid = true;
			break;
		case SCENE_STATE::factory_units:
			if (11 == commandPosNum)
			{	//thor
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 300);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 200);
			}
			else
				isInvalid = true;
			break;
		case SCENE_STATE::starport_units:
			if (1 == commandPosNum)
			{	//viking
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 150);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 75);
			}
			else if (2 == commandPosNum)
			{	//medivac
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 100);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 100);
			}
			else if (3 == commandPosNum)
			{	//banshee
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 150);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 100);
			}
			else if (6 == commandPosNum)
			{	//raven
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 100);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 200);
			}
			else if (7 == commandPosNum)
			{	//battlecruiser
				m_pNumberFont->UpdateNumber(string("screen_need_mineral"), 400);
				m_pNumberFont->UpdateNumber(string("screen_need_gas"), 300);
			}
			else
				isInvalid = true;
			break;
		default:
			isInvalid = true;
		}

		if (isInvalid)
		{
			m_pNumberFont->UpdateNumber(string("screen_need_mineral"), INVALIDATE_FONT);
			m_pNumberFont->UpdateNumber(string("screen_need_gas"), INVALIDATE_FONT);
			*bDisplayRequire = false;
		}
		else
			*bDisplayRequire = true;
	}
}

bool CScene::IsCursorInCrowd(float xScreen, float yScreen, int& crowdPosNum)
{
	//��ũ����ǥ�� -1.0f ~ 1.0f���� ��ȯ
	float xPos = (xScreen / m_fWndClientWidth) * 2.0f - 1.0f;
	float yPos = ((m_fWndClientHeight - yScreen) / m_fWndClientHeight) * 2.0f - 1.0f;	//+y�� �������� �ٲ�

	int nMaxCrowd = m_nValidSelectedObjects;

	D3DXVECTOR2 vLB, vRT;
	float crowdLeft, crowdRight, crowdTop, crowdBottom;
	for (int i = 0; i < nMaxCrowd; ++i)
	{
		auto iter = m_mapCrowdPosition.find(i + 1);
		if (iter != m_mapCrowdPosition.end())
		{
			vLB = iter->second.first;
			vRT = iter->second.second;

			crowdLeft = vLB.x;
			crowdRight = vRT.x;
			crowdTop = vRT.y;
			crowdBottom = vLB.y;

			if (xPos < crowdRight && xPos >= crowdLeft && yPos <= crowdTop && yPos > crowdBottom)
			{
				crowdPosNum = i + 1;
				return true;
			}
		}
	}

	//for�� ������ return���� �ʾ����� ��ŷ �� ������ ����.
	return false;
}

void CScene::ProcessCursorInCrowd(int crowdPosNum)
{
	m_nValidSelectedObjects = 1;
	m_ppSelectedObjects[0] = m_ppSelectedObjects[crowdPosNum - 1];
	UpdateSelectedObjects();
}

//**********************************************************************
//�Է� ó��
bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	float x, y;
	D3DXVECTOR3 vec;
	bool bMiniMapClicked = false;

	switch (nMessageID)
	{
	case WM_LBUTTONUP:
	{
		m_bLeftClicked = false;
		bool isPickedObject = false;

		if (m_bActivedBuildCheck)
		{
			x = LOWORD(lParam); y = HIWORD(lParam);
			auto vec = GetPositionPicking(x, y);
			
			//0904 �ٷ� CreateObject�� �ƴ� �������� �̵��Ѵ�.
			int my_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(m_ppSelectedObjects[0]->GetPosition());
			int obj_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(vec);
			if(my_idx == obj_idx)
				CreateNewObject((OBJECT_TYPE)m_nSceneState, vec);
			else //���°����� �Ÿ��� �ִٸ�
			{
				if((int)OBJECT_TYPE::Scv == m_ppSelectedObjects[0]->m_nObjectType)
				{
					CAnimationObject* pObject = (CAnimationObject*)m_ppSelectedObjects[0];
					pObject->CS_Packet_Move(vec, true);
					pObject->m_bMoveForBuild = true;
					pObject->m_WillBuildObjectType = (OBJECT_TYPE)m_nSceneState;
					
					m_nSceneState = SCENE_STATE::pick_scv;
					m_pBuildCheck->InvalidateBuildingImage();
					m_bActivedBuildCheck = false;

					for (auto d : m_listNeutral)
					{
						if (d->m_nObjectType != (int)OBJECT_TYPE::Gas) continue;
						int gas_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(d->GetPosition());
						if (gas_idx == obj_idx)
						{
							pObject->m_pGasModel = d;
							break;
						}
					}

				}
			}
		}
		else // false == m_bAcitvedBuildCheck
		{
			if (true == m_bMouseRectDraw)
			{
				m_bMouseRectDraw = false;
				GetMouseRectObjects(); //���콺 �巡�� �簢���� ��ŷ�Ͽ� ��ü ����Ʈ�� ���
				isPickedObject = true;
			}
			else //false == m_bMouseRectDraw
			{	//�巡�״� ���� �ʾ����� Ŭ���� �ߴ�
				x = LOWORD(lParam); y = HIWORD(lParam);

				bool isCrowdClicked{ false };
				int crowdNum = 0;
				if (2 <= m_nValidSelectedObjects)
				{ //���� ���� �� �ϳ��� �����ߴ��� Ȯ���Ѵ�.
					isCrowdClicked = IsCursorInCrowd(x, y, crowdNum);
				}

				int commandNum = 0;
				bool isCommandClicked = IsCursorInCommand(x, y, commandNum);
				
				//1. Ŀ�ǵ�â Ŭ������ Ȯ���Ѵ�.
				if (isCommandClicked)
					ProcessCursorInCommand(commandNum, true, nullptr);
				//2. ���� ���� Ŭ������ Ȯ���Ѵ�.
				else if (isCrowdClicked)
					ProcessCursorInCrowd(crowdNum);
				else
				{
					//3. �� �� �ƴ϶�� ���� ��ü ��ŷ���� Ȯ���Ѵ�(�ٿ�� �ڽ��� Ȯ��).
					m_d3dxvNextCursorPosition = GetPositionPicking(x, y);
					GetMouseCursorObject();
					isPickedObject = true;
				}
			}

			if (isPickedObject && m_nValidSelectedObjects > 0)
			{
				if ((int)OBJECT_TYPE::Scv == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::pick_scv;
				else if ((int)OBJECT_TYPE::Barrack == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::barrack_units;
				else if ((int)OBJECT_TYPE::Factory == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::factory_units;
				else if ((int)OBJECT_TYPE::Starport == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::starport_units;
				else if ((int)OBJECT_TYPE::Commandcenter == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::commandcenter_units;
				else
					m_nSceneState = SCENE_STATE::nothing;
			}
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		x = LOWORD(lParam); y = HIWORD(lParam);
		
		bMiniMapClicked = IsClickMiniMap(x, y, true, false);
		
		if(!bMiniMapClicked)
		{
			m_bLeftClicked = true;
			m_bMouseRectDraw = false;

			m_d3dxvOldCursorPosition = GetPositionPicking(x, y);
		}
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (true == m_bLeftClicked)
		{
			x = LOWORD(lParam); y = HIWORD(lParam);
			m_d3dxvNextCursorPosition = GetPositionPicking(x, y);
			if (D3DXVec2Length(&(D3DXVECTOR2(m_d3dxvOldCursorPosition.x, m_d3dxvOldCursorPosition.z) -
				D3DXVECTOR2(m_d3dxvNextCursorPosition.x, m_d3dxvNextCursorPosition.z))) <= 1.0f)	//�簢���� �۴ٸ� �׳� �Ѿ��.
				break;

			m_bMouseRectDraw = true;	//draw�� Scene�� Render()����
		}
		
		x = LOWORD(lParam); y = HIWORD(lParam);
		m_ptMousePosition.x = x; m_ptMousePosition.y = y;

	}
	break;
	case WM_RBUTTONDOWN:
	{
		if (m_nValidSelectedObjects > 0)	//���� ���� ���ƴٴѴ� �ϴ�.
		{
			bMiniMapClicked = IsClickMiniMap(LOWORD(lParam), HIWORD(lParam), false, false);

			if (!bMiniMapClicked)
			{
				vec = GetPositionPicking(LOWORD(lParam), HIWORD(lParam));
				DrawMouseClickImage(vec);

				for (int i = 0; i < m_nValidSelectedObjects; ++i)
				{
					if (isEnemyObject(m_ppSelectedObjects[i]->m_nObjectType)) continue;
					if (!isAnimateObject(m_ppSelectedObjects[i]->m_nObjectType)) continue;

					if (Vec2DDistance(Vector2D(vec.x, vec.z), Vector2D(m_ppSelectedObjects[i]->GetPosition().x, m_ppSelectedObjects[i]->GetPosition().z)) <= 0.2f) //�ֺ� Ŭ��
						continue;

					if ((int)OBJECT_TYPE::Scv == m_ppSelectedObjects[i]->m_nObjectType)
					{
						CAnimationObject* pObject = (CAnimationObject*)m_ppSelectedObjects[i];
						//scv�� �̳׶� ��ŷ���� �˻��ؾ� �Ѵ�.
						CGameObject* pMineral = nullptr;
						bool bMineral = false;
						for (auto d : m_listNeutral)
						{						
							if (d->m_nObjectType == (int)OBJECT_TYPE::Gas) continue;
							if (!d->GetActive() || !d->IsInSight()) continue;

							AABB bc = d->m_bcMeshBoundingCube;
							bc.Update(&(d->m_d3dxmtxLocal * d->m_d3dxmtxWorld));
							if (vec.x >= bc.m_d3dxvMinimum.x && vec.x <= bc.m_d3dxvMaximum.x && vec.z >= bc.m_d3dxvMinimum.z && vec.z <= bc.m_d3dxvMaximum.z)
							{
								pMineral = d;
								break;
							}
						}

						if (pMineral) //��ŷ�Ǿ��ٸ�
						{	//���� ����� Ŀ�ǵ� ���͸� ã�´�.
							CGameObject* nearstCommandCenter = nullptr;
							FLOAT nearDistance = FLT_MAX;
							float distance;
							for (auto d : m_listMy)
							{
								if ((int)OBJECT_TYPE::Commandcenter != d->m_nObjectType) continue;
								D3DXVECTOR3 vDistance = pMineral->GetPosition() - d->GetPosition();
								distance = D3DXVec2Length(&D3DXVECTOR2(vDistance.x, vDistance.z));
								if (distance <= nearDistance)
								{
									nearDistance = distance;
									nearstCommandCenter = d;
								}
							}

							if (nearstCommandCenter) //ã�Ҵٸ�
							{
								CS_Packet_Dig_Resource(m_ppSelectedObjects[i], pMineral, nearstCommandCenter);
							}
							else //�� ã���� ���� Ŀ�ǵ尡 ���ٴ� ��
							{
								vec.y = 0.0f;
								CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
							}
						}
						else //nullptr == pMineral -> �̳׶���ŷ�� �ƴϴ� -> Refinery ��ŷ���� �˻��Ѵ�.
						{
							CGameObject* pRefinery = nullptr;
							for (auto d : m_listMy)
							{
								if ((int)OBJECT_TYPE::Refinery != d->m_nObjectType) continue;
								if (!d->GetActive() || !d->IsInSight()) continue;

								AABB bc = d->m_bcMeshBoundingCube;
								bc.Update(&(d->m_d3dxmtxLocal * d->m_d3dxmtxWorld));
								if (vec.x >= bc.m_d3dxvMinimum.x && vec.x <= bc.m_d3dxvMaximum.x && vec.z >= bc.m_d3dxvMinimum.z && vec.z <= bc.m_d3dxvMaximum.z)
								{
									pRefinery = d;
									break;
								}
							}

							if (pRefinery)
							{	//�����̳ʸ� ��ŷ�Ǿ��ٸ� ���� ����gas�� Ŀ�ǵ� ���͸� ã�´�.
								CGameObject* nearstCommandCenter = nullptr;
								FLOAT nearDistance = FLT_MAX;
								float distance;
								for (auto d : m_listMy)
								{
									if ((int)OBJECT_TYPE::Commandcenter != d->m_nObjectType) continue;
									D3DXVECTOR3 vDistance = pRefinery->GetPosition() - d->GetPosition();
									distance = D3DXVec2Length(&D3DXVECTOR2(vDistance.x, vDistance.z));
									if (distance <= nearDistance)
									{
										nearDistance = distance;
										nearstCommandCenter = d;
									}
								}

								if (nearstCommandCenter) //ã�Ҵٸ�
								{
									CS_Packet_Dig_Resource(m_ppSelectedObjects[i], pRefinery, nearstCommandCenter);
								}
								else //�� ã���� ���� Ŀ�ǵ尡 ���ٴ� ��
								{
									vec.y = 0.0f;
									CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
								}
							}
							else //�����̳ʸ� ��ŷ�� �ƴϴ� -> �̵� ��Ŷ�̴�.
							{
								vec.y = 0.0f;
								CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
							}
						}

						if (pObject->m_bMoveForBuild)
						{
							pObject->m_bMoveForBuild = false;
							pObject->m_WillBuildObjectType = OBJECT_TYPE::Invalid;
						}
					}
					else // not scv
					{
						vec.y = 0.0f;
						CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);

						/*
						m_ppSelectedObjects[i]->CS_Packet_State(State::move);
						//m_ppSelectedObjects[i]->m_ObjectState = State::move;
						m_ppSelectedObjects[i]->m_pTargetObject = nullptr;
						m_ppSelectedObjects[i]->m_bHasTargetPosition = true;
						*/

						/*
						if (m_ppSelectedObjects[i]->m_bAirUnit)
						{
							CAirObject* pObject = (CAirObject*)(m_ppSelectedObjects[i]);
							pObject->AirUnitFindPath(vec);
						}
						else
						{
							m_ppSelectedObjects[i]->m_listTargetPosition = m_pAStarComponent->SearchAStarPath(m_ppSelectedObjects[i]->GetPosition(), vec);
							m_ppSelectedObjects[i]->m_bLatestFindPath = true;
							m_ppSelectedObjects[i]->SetTargetPosition(m_ppSelectedObjects[i]->m_listTargetPosition.front());
						}
						*/
					}
				}

				cprintf("Clicked Position : %f , %f, %f // idx : %d\n", vec.x, vec.y, vec.z, AStarComponent::GetInstance()->GetNearstNodeIndex(vec));
			}

			//��� ���õ� ���ֵ��� ���� ����� �ƴ� ��ǥ����(0�ε���)�� ����Ѵ�.
			int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
			if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
				CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
			else //�ǹ��� ��� what�� offset���̴�
				CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

			if(m_bActivedBuildCheck)	//�ǹ� ���� ���� ���¿��� �̵��� �߻��� �Ϳ� ���� ó��
			{
				if ((int)OBJECT_TYPE::Scv == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::pick_scv;
				else
					m_nSceneState = SCENE_STATE::nothing;

				m_pBuildCheck->InvalidateBuildingImage();
				m_bActivedBuildCheck = false;
			}
		}
	}
	break;
	}	//end switch
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//Framework���� ����ϴ� Ű���� �޽���
	//case VK_NUMPAD1:
	//case VK_NUMPAD2:
	//case VK_ESCAPE:
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	{
		if (m_nValidSelectedObjects > 0)
		{
			if (wParam >= 0x31 && wParam <= 0x38) // 1~8 ���� ���
			{	//0x0000 -> ������X����X / 0x0001 -> ������O����X / 0x8000 -> ������X����O / 0x8001 -> ������O����O

				// 1. ��Ʈ�� Ű ��ۻ������� Ȯ���Ͽ� �δ����� �������� Ȯ��
				if (0x8000 & GetAsyncKeyState(VK_CONTROL)) //��Ʈ��Ű�� �����ִٸ�
				{
					if (!isEnemyObject(m_ppSelectedObjects[0]->m_nObjectType)) //������ �δ����� ���� �ʴ´�.
					{
						m_CrowdSaveData[wParam - 48 - 1].nUnits = m_nValidSelectedObjects;
						for (int i = 0; i < m_nValidSelectedObjects; ++i)
							m_CrowdSaveData[wParam - 48 - 1].ppObjects[i] = m_ppSelectedObjects[i];
					}
				}
				// 2. �ƴ� ��� ����� �δ� �ҷ������� Ȯ��
				else
				{
					if (m_CrowdSaveData[wParam - 48 - 1].nUnits == 0)
					{
						//nothing to do
					}
					else //����� ������ �ִٸ� 
					{
						//�δ� �ҷ�����
						m_nValidSelectedObjects = m_CrowdSaveData[wParam - 48 - 1].nUnits;
						for (int i = 0; i < m_CrowdSaveData[wParam - 48 - 1].nUnits; ++i)
							m_ppSelectedObjects[i] = m_CrowdSaveData[wParam - 48 - 1].ppObjects[i];

						if (m_chLastInputKey == wParam) //���� �Է� Ű�� ���� Ű�� ���� �����
						{
							if (m_fLastInputCrowdDataButtonTime <= DOUBLE_INPUT_DURATION)
							{	// ���� �Է����� Ȯ��, �´ٸ� ī�޶� �̵�
								D3DXVECTOR3 pos = m_ppSelectedObjects[0]->GetPosition() - m_pCamera->GetLookAtPosition();
								pos.y = 0.0f;
								m_pCamera->Move(pos);
								m_pCamera->SetLookAtWorld(m_pCamera->GetLookAtPosition() + pos);
								m_pCamera->GenerateViewMatrix();
							}
						}

						int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
						if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
							CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
						else //�ǹ��� ��� what�� offset���̴�
							CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);
					}

					m_chLastInputKey = wParam;
					m_fLastInputCrowdDataButtonTime = 0.0f; //�ٽ� �ʱ�ȭ
				}
			}
		}
	}
	break;

	case WM_KEYUP:
	{
		if(m_nValidSelectedObjects > 0)
		{
			if ((wParam >= 'A' && wParam <= 'Z') || wParam == 0x30 || wParam == 0x39)
			{
				if (wParam == 0x30) // 0
				{
					gGameMineral += 500;
					break;
				}
				if (wParam == 0x39) // 9
				{
					gGameGas += 500;
					break;
				}

				//TCHAR str; GetKeyNameText(lParam, (LPSTR)str, 2);
				switch (m_nSceneState)
				{
				case SCENE_STATE::nothing:  break;
				case SCENE_STATE::pick_scv:
				{
					if (wParam == 'B')
						m_nSceneState = SCENE_STATE::scv_build;
					else if (wParam == 'V')
						m_nSceneState = SCENE_STATE::scv_buildadv;			
					break;
				}
				case SCENE_STATE::scv_build:
				{
					m_bActivedBuildCheck = true;

					if (wParam == 'C')
					{
						m_nSceneState = SCENE_STATE::build_commandcenter;
						m_strActivedBuildCheck = "commandcenter";
					}
					else if (wParam == 'S')
					{	
						m_nSceneState = SCENE_STATE::build_supplydepot;
						m_strActivedBuildCheck = "supplydepot";
					}
					else if (wParam == 'R')
					{
						m_nSceneState = SCENE_STATE::build_refinery;
						m_strActivedBuildCheck = "refinery";
					}
					else if (wParam == 'B')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Commandcenter)->second)
						{ //���� ��ũƮ���� ��ɺΰ� �ִ��� Ȯ��
							m_nSceneState = SCENE_STATE::build_barrack;
							m_strActivedBuildCheck = "barrack";
						}
						else //���� ��ũ �������� ������ buildCheck �̹����� �׷����� �ȵȴ�. �ڵ� ����ȭ�Ϸ��� ����;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'E')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Commandcenter)->second)
						{ //���� ��ũƮ���� ��ɺΰ� �ִ��� Ȯ��
							m_nSceneState = SCENE_STATE::build_engineeringbay;
							m_strActivedBuildCheck = "engineeringbay";
						}
						else //���� ��ũ �������� ������ buildCheck �̹����� �׷����� �ȵȴ�. �ڵ� ����ȭ�Ϸ��� ����;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'N')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
						{ //���� ��ũƮ���� ������ �ִ��� Ȯ��
							m_nSceneState = SCENE_STATE::build_sensortower;
							m_strActivedBuildCheck = "sensortower";
						}
						else //���� ��ũ �������� ������ buildCheck �̹����� �׷����� �ȵȴ�. �ڵ� ����ȭ�Ϸ��� ����;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'T')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Engineeringbay)->second)
						{ //���� ��ũƮ���� ���п����Ұ� �ִ��� Ȯ��
							m_nSceneState = SCENE_STATE::build_missileturret;
							m_strActivedBuildCheck = "missileturret";
						}
						else //���� ��ũ �������� ������ buildCheck �̹����� �׷����� �ȵȴ�. �ڵ� ����ȭ�Ϸ��� ����;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'U')
					{ 
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
						{ //���� ��ũƮ���� ������ �ִ��� Ȯ��
							m_nSceneState = SCENE_STATE::build_bunker;
							m_strActivedBuildCheck = "bunker";
						}
						else //���� ��ũ �������� ������ buildCheck �̹����� �׷����� �ȵȴ�. �ڵ� ����ȭ�Ϸ��� ����;
							m_bActivedBuildCheck = false;
					}
					else
						m_bActivedBuildCheck = false;
					break;
				}
				case SCENE_STATE::scv_buildadv:
				{
					m_bActivedBuildCheck = true;

					if (wParam == 'F')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
						{ //���� ��ũƮ���� ������ �ִ��� Ȯ��
							m_nSceneState = SCENE_STATE::build_factory;
							m_strActivedBuildCheck = "factory";
						}
						else //���� ��ũ �������� ������ buildCheck �̹����� �׷����� �ȵȴ�. �ڵ� ����ȭ�Ϸ��� ����;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'S')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Factory)->second)
						{ //���� ��ũƮ���� ���������� �ִ��� Ȯ��
							m_nSceneState = SCENE_STATE::build_starport;
							m_strActivedBuildCheck = "starport";
						}
						else //���� ��ũ �������� ������ buildCheck �̹����� �׷����� �ȵȴ�. �ڵ� ����ȭ�Ϸ��� ����;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'A')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Factory)->second)
						{ //���� ��ũƮ���� ���������� �ִ��� Ȯ��
							m_nSceneState = SCENE_STATE::build_armory;
							m_strActivedBuildCheck = "armory";
						}
						else //���� ��ũ �������� ������ buildCheck �̹����� �׷����� �ȵȴ�. �ڵ� ����ȭ�Ϸ��� ����;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'G')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Factory)->second)
						{ //���� ��ũƮ���� ���������� �ִ��� Ȯ��
							m_nSceneState = SCENE_STATE::build_ghostacademy;
							m_strActivedBuildCheck = "ghostacademy";
						}
						else //���� ��ũ �������� ������ buildCheck �̹����� �׷����� �ȵȴ�. �ڵ� ����ȭ�Ϸ��� ����;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'C')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Starport)->second)
						{ //���� ��ũƮ���� ���ְ����� �ִ��� Ȯ��
							m_nSceneState = SCENE_STATE::build_fusioncore;
							m_strActivedBuildCheck = "fusioncore";
						}
						else //���� ��ũ �������� ������ buildCheck �̹����� �׷����� �ȵȴ�. �ڵ� ����ȭ�Ϸ��� ����;
							m_bActivedBuildCheck = false;
					}
					else
						m_bActivedBuildCheck = false;
					break;
				}
				case SCENE_STATE::commandcenter_units:
				{
					if (wParam == 'S')
					{	//scv
						CreateNewObject(OBJECT_TYPE::Scv, m_ppSelectedObjects[0]->GetPosition());
					}
					
					break;
				}
				case SCENE_STATE::barrack_units:
				{
					if (wParam == 'A')
					{	//marine
						CreateNewObject(OBJECT_TYPE::Marine, m_ppSelectedObjects[0]->GetPosition());
					}
					else if (wParam == 'R')
					{	//reaper
						CreateNewObject(OBJECT_TYPE::Reaper, m_ppSelectedObjects[0]->GetPosition());
					}
					else if (wParam == 'D')
					{	//marauder
						CreateNewObject(OBJECT_TYPE::Marauder, m_ppSelectedObjects[0]->GetPosition());
					}
					break;
				}
				case SCENE_STATE::factory_units:
				{
					if (wParam == 'T')
					{	//thor
						CreateNewObject(OBJECT_TYPE::Thor, m_ppSelectedObjects[0]->GetPosition());
					}
					break;
				}
				case SCENE_STATE::starport_units:
				{
					if (wParam == 'E')
					{	//banshee
						CreateNewObject(OBJECT_TYPE::Banshee, m_ppSelectedObjects[0]->GetPosition());
					}
					else if (wParam == 'D')
					{	//medivac
						CreateNewObject(OBJECT_TYPE::Medivac, m_ppSelectedObjects[0]->GetPosition());
					}
					else if (wParam == 'V')
					{	//viking
						CreateNewObject(OBJECT_TYPE::Viking, m_ppSelectedObjects[0]->GetPosition());
					}
					else if (wParam == 'R')
					{	//raven
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Fusioncore)->second)
						{ //�����ϱ� ���� �ǹ��� ���տ����Ұ� �ִ��� Ȯ��
							CreateNewObject(OBJECT_TYPE::Raven, m_ppSelectedObjects[0]->GetPosition());
						}
					}
					else if (wParam == 'B')
					{	//battlecruiser
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Fusioncore)->second)
						{ //�����ϱ� ���� �ǹ��� ���տ����Ұ� �ִ��� Ȯ��
							CreateNewObject(OBJECT_TYPE::BattleCruiser, m_ppSelectedObjects[0]->GetPosition());
						}
					}
					break;
				}
				} // SceneState ����ġ��

				if (SCENE_STATE::nothing == m_nSceneState || SCENE_STATE::pick_scv == m_nSceneState)
				{
					//��ǥ������ 0���� ���带 ����Ѵ�.
					if (m_ppSelectedObjects[0]->m_nObjectType != (int)OBJECT_TYPE::Mineral && m_ppSelectedObjects[0]->m_nObjectType != (int)OBJECT_TYPE::Gas)
					{
						int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
						if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
							CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
						else //�ǹ��� ��� what�� offset���̴�
							CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);
					}

					if (wParam == 'S')
					{
						for (int i = 0; i < m_nValidSelectedObjects; ++i)
							m_ppSelectedObjects[i]->CS_Packet_State(State::stop);
					}
					else if (wParam == 'A')
					{
						bool bMiniMapClicked = false;
						
						if(m_nValidSelectedObjects > 0)
							bMiniMapClicked = IsClickMiniMap(m_ptMousePosition.x, m_ptMousePosition.y, true, true);

						if (false == bMiniMapClicked)
						{
							//move_with_guard �Ǵ� have_target
							auto vec = GetPositionPicking(m_ptMousePosition.x, m_ptMousePosition.y);
							DrawMouseClickImage(vec);

							int mouse_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(vec);
							CGameObject* pTarget = nullptr;
							for (auto d : m_listEnemyInSight)	//�Ʊ��� ������ ���� ������, �ϴ� ������ �����ϴ� ������ �Ѵ�.
							{
								/*
								AABB bc = d->m_bcMeshBoundingCube;
								bc.Update(&(d->m_d3dxmtxWorld));
								if (vec.x >= bc.m_d3dxvMinimum.x && vec.x <= bc.m_d3dxvMaximum.x && vec.z >= bc.m_d3dxvMinimum.z && vec.z <= bc.m_d3dxvMaximum.z)
								*/
								if (IsPickedObject(vec, d))
								{
									pTarget = d;
									break;
								}
							}
							//1. move_with_guard
							if (nullptr == pTarget)
							{
								for (int i = 0; i < m_nValidSelectedObjects; ++i)
								{
									int obj_type = m_ppSelectedObjects[i]->m_nObjectType;
									if (!isAnimateObject(obj_type) || isEnemyObject(obj_type)) continue;

									if (m_ppSelectedObjects[i]->m_fObjectAttackRange == 0.0f)
									{ //���ݺҰ� �����̶�� RButton �̵��� ���� ó��
										CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
										continue;
									}

									CAnimationObject* pMyObject = (CAnimationObject*)m_ppSelectedObjects[i];
									pMyObject->m_pTargetObject = nullptr;
									pMyObject->CS_Packet_State(State::move_with_guard);
									CS_Packet_Move_Object(pMyObject, vec, false);
								}
							}
							else //2. have_target
							{	//�ϴ� attack�� ������. ������ attack���� ���ݹ��� ������ Ȯ���Ѵٸ� �ٷ� have_target���� �Ѿ ���̴�.
								for (int i = 0; i < m_nValidSelectedObjects; ++i)
								{
									int obj_type = m_ppSelectedObjects[i]->m_nObjectType;
									if (!isAnimateObject(obj_type) || isEnemyObject(obj_type)) continue;

									if (m_ppSelectedObjects[i]->m_fObjectAttackRange == 0.0f)
									{ //���ݺҰ� �����̶�� RButton �̵��� ���� ó��
										CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
										continue;
									}

									if (pTarget->m_bAirUnit && !m_ppSelectedObjects[i]->m_bAirAttack)
									{	 //���� ��������, ���� ���߰��� ���ϸ�
										CAnimationObject* pMyObject = (CAnimationObject*)m_ppSelectedObjects[i];
										pMyObject->m_pTargetObject = nullptr;
										pMyObject->CS_Packet_State(State::move_with_guard);
										CS_Packet_Move_Object(pMyObject, vec, false);
										continue;
									}

									//���� ������ �����ϴٸ� attack�� ����
									CAnimationObject* pMyObject = (CAnimationObject*)m_ppSelectedObjects[i];
									pMyObject->m_pTargetObject = pTarget;

									//���� ó�� �ȵǼ� attack �ٷ� ���� �ʰ� have_target�� ��ġ�� �׽�Ʈ						
									/*
									float distance = (pMyObject->GetPosition().x - (pTarget->GetPosition().x - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (pMyObject->GetPosition().x - (pTarget->GetPosition().x - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
										+ (pMyObject->GetPosition().z - (pTarget->GetPosition().z - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (pMyObject->GetPosition().z - (pTarget->GetPosition().z - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
									if (distance <= (pMyObject->m_fObjectAttackRange * pMyObject->m_fObjectAttackRange))
									{
										pMyObject->CS_Packet_State(State::attack);
										pMyObject->m_bProcessedAttack = true;
										pMyObject->m_bPickTarget = true;
									}
									else //���� ���� �Ұ� -> ��븦 have_target���� �д�.
									*/
									{
										pMyObject->CS_Packet_State(State::have_target);
										CS_Packet_Move_Object(pMyObject, vec, false);
										pMyObject->m_bPickTarget = true;
									}
								}
							}
						}
					}
				}

			} //Ű �Է� if��
		}

		switch (wParam)
		{
		case VK_SPACE:
		{
			if (m_nValidSelectedObjects > 0)
			{
				D3DXVECTOR3 pos = m_ppSelectedObjects[0]->GetPosition() - m_pCamera->GetLookAtPosition();
				pos.y = 0.0f;
				m_pCamera->Move(pos);
				m_pCamera->SetLookAtWorld(m_pCamera->GetLookAtPosition() + pos);
				m_pCamera->GenerateViewMatrix();
			}
		}
		break;
		case VK_ESCAPE:
		{
			if (m_nValidSelectedObjects > 0)
			{
				if ((int)OBJECT_TYPE::Scv == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::pick_scv;
				else if ((int)OBJECT_TYPE::Barrack == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::barrack_units;
				else if ((int)OBJECT_TYPE::Factory == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::factory_units;
				else if ((int)OBJECT_TYPE::Starport == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::starport_units;
				else if ((int)OBJECT_TYPE::Commandcenter == m_ppSelectedObjects[0]->m_nObjectType)
					m_nSceneState = SCENE_STATE::commandcenter_units;
				else
					m_nSceneState = SCENE_STATE::nothing;

				m_pBuildCheck->InvalidateBuildingImage();
				m_bActivedBuildCheck = false;
			}
		}
		break;

		default:
			break;
		}
	}
	break;
	
	default:
		break;
	}

	return(true);
}

bool CScene::ProcessInput()
{
	/*
	static UCHAR pKeysBuffer[256];
	GetKeyboardState(pKeysBuffer);
	
	//����Ű �̺�Ʈ ó��
	//0x0000 -> ������X����X / 0x0001 -> ������O����X / 0x8000 -> ������X����O / 0x8001 -> ������O����O
	bool isKeyboardArrowPressed = true;
	int inputKeyboardArrow = 0x0000;	// l r u d
	if (0x8000 & GetAsyncKeyState(VK_LEFT)) inputKeyboardArrow |= 0x1000;
	if (0x8000 & GetAsyncKeyState(VK_RIGHT)) inputKeyboardArrow |= 0x0100;
	if (0x8000 & GetAsyncKeyState(VK_UP)) inputKeyboardArrow |= 0x0010;
	if (0x8000 & GetAsyncKeyState(VK_DOWN)) inputKeyboardArrow |= 0x0001;
	*/
	return(false);
}

//���콺 �Է¿� ���� ī�޶� �� ���� ����� ���ϵ���
bool CScene::isCameraMoveOverMap()
{
	D3DXVECTOR3 vLookAtWorld = m_pCamera->GetLookAtPosition();

	float xMin = CAMERA_MIN_LOOKAT_X;
	float xMax = CAMERA_MAX_LOOKAT_X;
	float zMin = CAMERA_MIN_LOOKAT_Z;
	float zMax = CAMERA_MAX_LOOKAT_Z;

	if (vLookAtWorld.x < xMin || vLookAtWorld.x > xMax || vLookAtWorld.z < zMin || vLookAtWorld.z > zMax) return true;
	else return false;
}


//���� �� �̺�Ʈ�� ���� ���̸� ���� -> 0705 ���̸ʿ��� �����Ƿ� �ʿ� X
/*
void CScene::CreateHeightMap(LPCTSTR pFileName, int nWidth, int nHeight, D3DXVECTOR3& d3dxvScale)
{
	m_pHeightMap = new CHeightMap(pFileName, nWidth, nHeight, d3dxvScale);
}
*/

//**********************************************************************
//���� �������� ��Ŷ ó���� ���� �����Դϴ�.
CGameObject* CScene::FindObjectById(int type, int id)
{
	CGameObject* pObject = nullptr;
	if (isEnemyObject(type))
	{
		for (auto p : m_listEnemy)
		{
			if (p->m_nObjectID == id)
			{
				pObject = p;
				break;
			}
		}
	}
	else
	{
		for (auto p : m_listMy)
		{
			if (p->m_nObjectID == id)
			{
				pObject = p;
				break;
			}
		}
	}

	return pObject;
}

CGameObject* CScene::FindObjectById(int id)
{
	CGameObject* pObject = nullptr;
	
	for (auto p : m_listEnemy)
	{
		if (p->m_nObjectID == id)
		{
			pObject = p;
			break;
		}
	}

	if(nullptr == pObject) // �� ����Ʈ���� ��ã�Ҵٸ�
	{
		for (auto p : m_listMy)
		{
			if (p->m_nObjectID == id)
			{
				pObject = p;
				break;
			}
		}
	}

	return pObject;
}

void CScene::Process_SC_Packet_Put_Object(sc_packet_put_object* packet)
{
	int obj_type = (int)packet->object_type;
	bool bIsNeutral = false;
	//�Ǿ� �ĺ�
	if (packet->object_type == OBJECT_TYPE::Mineral || packet->object_type == OBJECT_TYPE::Gas)
		bIsNeutral = true;
	else if (packet->client_id != m_pGameFramework->m_clientID)
		obj_type += ADD_ENEMY_INDEX;

	//�ڿ� ��ġ�� ���⼭ ���� ó���ϵ��� �ϰڴ�.
	if (bIsNeutral)
	{
		auto vec = m_pStaticShader->m_vectorObjects;

		for (int i = vec.size() - 1; i >= 0; --i)
		{
			if (obj_type != vec[i]->m_ObjectType) continue;
			int ObjectIdx = vec[i]->m_nValidObjects;
			vec[i]->m_ppObjects[ObjectIdx]->SetActive(true);
			vec[i]->m_ppObjects[ObjectIdx]->SetPosition(packet->x, packet->y, packet->z);
			vec[i]->m_ppObjects[ObjectIdx]->m_nObjectID = packet->object_id;
			vec[i]->m_ppObjects[ObjectIdx]->m_nObjectType = obj_type;
			vec[i]->m_ppObjects[ObjectIdx]->m_nHP = packet->hp;
			
			m_listNeutral.push_back(vec[i]->m_ppObjects[ObjectIdx]);
			CStaticObject* pObject = (CStaticObject*)(vec[i]->m_ppObjects[ObjectIdx]);
			pObject->m_fCompleteBuildTime = pObject->m_fBuildTime = 1.0f;

			vec[i]->m_nValidObjects++;
			return;
		}
	}

	//type�� ���Ͽ� �����ΰ��� �ִϸ��̼����� Ȯ���ؾ��ҵ�?
	vector<RenderObjectData*> vec;
	if (isAnimateObject(obj_type))
		vec = m_pAnimationShader->m_vectorObjects;
	else
		vec = m_pStaticShader->m_vectorObjects;

	for (int i = vec.size() - 1; i >= 0; --i)
	{
		if (obj_type != vec[i]->m_ObjectType) continue;
		int ObjectIdx = vec[i]->m_nValidObjects;
		vec[i]->m_ppObjects[ObjectIdx]->SetActive(true);
		vec[i]->m_ppObjects[ObjectIdx]->SetPosition(packet->x, packet->y, packet->z);
		vec[i]->m_ppObjects[ObjectIdx]->m_nObjectID = packet->object_id;
		vec[i]->m_ppObjects[ObjectIdx]->m_nObjectType = obj_type;
		vec[i]->m_ppObjects[ObjectIdx]->m_nHP = packet->hp;
		vec[i]->m_nValidObjects++;

		//test
		if (isEnemyObject(obj_type))
		{ //�� ����
			vec[i]->m_ppObjects[ObjectIdx]->SetInSight(false); //�ϴ� false, �þ� ����Ʈ ���� �ÿ� �ٲ���̴�.

			if (vec[i]->m_ppObjects[ObjectIdx]->m_bAirUnit)
				m_listEnemy.push_front(vec[i]->m_ppObjects[ObjectIdx]);
			else //����
				m_listEnemy.push_back(vec[i]->m_ppObjects[ObjectIdx]);
		}
		else //�Ʊ� �����̴�.
		{
			vec[i]->m_ppObjects[ObjectIdx]->SetInSight(true); //�Ʊ������� ������ ���δ�.

			if (vec[i]->m_ppObjects[ObjectIdx]->m_bAirUnit)
				m_listMy.push_front(vec[i]->m_ppObjects[ObjectIdx]);
			else
				m_listMy.push_back(vec[i]->m_ppObjects[ObjectIdx]);

			int nSoundOffset = getSoundOffsetByType(vec[i]->m_ppObjects[ObjectIdx]->m_nObjectType);
			if (isAnimateObject(vec[i]->m_ppObjects[ObjectIdx]->m_nObjectType))
				CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset + UNIT_READY_OFFSET);
			else if (!isEnemyObject(vec[i]->m_ppObjects[ObjectIdx]->m_nObjectType))
			{
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::job_finish);
			}
		}

		//ó�� Ŀ�ǵ弾�Ͱ� �����Ǵ� ��Ȳ -> �� ����  �ǹ��� PutObject�� ���� ����
		if (!(isAnimateObject(obj_type)))
		{
			CStaticObject* pStaticObject = (CStaticObject*)(vec[i]->m_ppObjects[ObjectIdx]);
			pStaticObject->m_fBuildTime = pStaticObject->m_fCompleteBuildTime;
			pStaticObject->m_nHP = packet->hp;

			D3DXVECTOR3 pos = D3DXVECTOR3(packet->x, packet->y, packet->z) - m_pCamera->GetLookAtPosition();
			pos.y = 0.0f;
			m_pCamera->Move(pos);
			m_pCamera->SetLookAtWorld(m_pCamera->GetLookAtPosition() + pos);
			m_pCamera->GenerateViewMatrix();


			m_mapIsBuildingExist.find((OBJECT_TYPE)vec[i]->m_ppObjects[ObjectIdx]->m_nObjectType)->second = true;
			//printf("%d�� ���������� ��ũƮ�� true �Ǿ����ϴ�\n", vec[i]->m_ppObjects[ObjectIdx]->m_nObjectType);
			m_mapCommandMesh.find("scv")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
			m_mapCommandMesh.find("barrack")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
			m_mapCommandMesh.find("engineeringbay")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
		}
	}
}

//0803 ����ȭ��Ű�鼭 ��� ����
/*
void CScene::Process_SC_Packet_Data_Object(sc_packet_data_object* packet)
{
	//��Ŷ ������ ó���Ǵ°� �׽�Ʈ
	
	static int nPacketCount = 0;
	nPacketCount++;
	
	
	//nPacketCount++;
	//printf("PacketCount : %d\n", nPacketCount);
	
	
	auto t = chrono::high_resolution_clock::now();
	printf("position : %f, %f, %f\n", packet->x, packet->y, packet->z);
	printf("%d Client Time : %llu\n\n", m_pGameFramework->m_clientID, t.time_since_epoch().count());
	


	CGameObject* pObject = nullptr;
	if (isEnemyObject((int)packet->object_type))
	{
		for (auto p : m_listEnemy)
		{
			if (p->m_nObjectType == (int)packet->object_type)
			{
				pObject = p;
				break;
			}
		}
	}
	else
	{
		for (auto p : m_listMy)
		{
			if (p->m_nObjectType == (int)packet->object_type)
			{
				pObject = p;
				break;
			}
		}
	}
	
	if(pObject)
	{
		pObject->Process_SC_Move(D3DXVECTOR3(packet->x, packet->y, packet->z));
	}	
}
*/

void CScene::Process_SC_Packet_Data_Object_State(sc_packet_data_object_state* packet)
{
	bool bIsEnemy = false;
	int obj_type = (int)packet->object_type;
	if (packet->client_id != CSocketComponent::GetInstance()->m_clientID)
	{
		obj_type += ADD_ENEMY_INDEX;
		bIsEnemy = true;
	}
	
	CGameObject* pObject = FindObjectById(obj_type, packet->object_id);
	
	if (pObject)
	{
		pObject->Process_SC_State(packet);

		//0901 �״� ��� Scene���� �����ϴ� ObjectList�� �����ؾ� �Ѵ�.
		if (State::death == packet->state)
		{
			if (bIsEnemy)
			{
				m_listEnemy.remove(pObject);
				m_listEnemyInSight.remove(pObject);

				//victory check
				int nBuildingCount = 0;
				for (auto d = m_listEnemy.begin(); d != m_listEnemy.end(); ++d)
				{
					if (!isAnimateObject((*d)->m_nObjectType))
						++nBuildingCount;
				}
				if (0 == nBuildingCount)
				{
					//victory
					m_bGameEnd = true;
					m_bGameWin = true;
				}
			}
			else //�Ʊ� ����
			{
				m_listMy.remove(pObject);
				
				//1206 ���� ��� �ǹ��� �������� �й�ó��(������ ���༭ �ӽ÷�)
				int nBuildingCount = 0;
				for (auto d = m_listMy.begin(); d != m_listMy.end(); ++d)
				{
					if (!isAnimateObject((*d)->m_nObjectType)) 
						++nBuildingCount;
				}
				if(0 == nBuildingCount)
				{
					//defeat
					m_bGameEnd = true;
					m_bGameWin = false;
				}
			}
		}
	}	
}

void CScene::Process_SC_Packet_Data_Object_Position(sc_packet_data_object_position* packet)
{
	int obj_type = (int)packet->object_type;
	if (packet->client_id != CSocketComponent::GetInstance()->m_clientID)
		obj_type += ADD_ENEMY_INDEX;

	CGameObject* pObject = FindObjectById(obj_type, packet->object_id);

	if (pObject)
	{
		pObject->Process_SC_Move(D3DXVECTOR3(packet->x, packet->y, packet->z));
		//pObject->Process_SC_Move_Enemy(D3DXVECTOR3(packet->x, packet->y, packet->z));
	}

	/*
	static DWORD lastTime = GetTickCount();
	DWORD now = GetTickCount();
	nPacketCount++;
	if (now >= lastTime + 1000)
	{
		lastTime = now;
		printf("�ʴ� �̵���Ŷ �� : %d\n", nPacketCount);
		nPacketCount = 0;
	}
	*/
}

void CScene::Process_SC_Packet_Data_Object_Value(sc_packet_data_object_value* packet)
{
	int obj_type = (int)packet->object_type;
	
	if (OBJECT_TYPE::Mineral == packet->object_type || OBJECT_TYPE::Gas == packet->object_type)
		; //���� �̳׶��� �����Ѵ�.
	else if (packet->client_id != CSocketComponent::GetInstance()->m_clientID)
		obj_type += ADD_ENEMY_INDEX;

	CGameObject* pObject = FindObjectById(obj_type, packet->object_id);

	if (pObject)
	{
		//1206 ��ũƮ�� ǥ��
		if (packet->client_id == CSocketComponent::GetInstance()->m_clientID)
		{	//�Ʊ� ������Ʈ ��
			if (!isAnimateObject((int)packet->object_type))
			{	//�ǹ��̸� 
				if (packet->building_time == ((CStaticObject*)(pObject))->m_fCompleteBuildTime)
				{  //�ϼ��Ǵ� ������ ��� ��ũƮ�� ǥ���� ���� exist ���θ� true�� �ٲپ��ش�.

					if (true == m_mapIsBuildingExist.find((OBJECT_TYPE)pObject->m_nObjectType)->second)
					{ //�̹� true�� ��� �׳� ����������
					}
					else // ó�� �������ų� �ı��Ǿ��� ���������ν�, exist�� true�� �ٲٰ� ��ũƮ�� ������ �̹����� ������Ʈ�Ѵ�.
					{
						m_mapIsBuildingExist.find((OBJECT_TYPE)pObject->m_nObjectType)->second = true;
						printf("%s�� �Ǽ��Ϸ�Ǿ� ������ ���θ� true�� �ٲپ���.\n", getNameByType(obj_type).data());

						switch (packet->object_type)
						{
						case OBJECT_TYPE::Armory:
							break;
						
						case OBJECT_TYPE::Barrack:
							m_mapCommandMesh.find("marine")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							m_mapCommandMesh.find("reaper")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							m_mapCommandMesh.find("marauder")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);

							m_mapCommandMesh.find("factory")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);		
							m_mapCommandMesh.find("bunker")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							m_mapCommandMesh.find("sensortower")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							break;
						
						case OBJECT_TYPE::Bunker:
							break;
						
						case OBJECT_TYPE::Commandcenter:
							m_mapCommandMesh.find("scv")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);

							m_mapCommandMesh.find("barrack")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							m_mapCommandMesh.find("engineeringbay")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							break;
						
						case OBJECT_TYPE::Engineeringbay:
							m_mapCommandMesh.find("missileturret")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							break;
							
						case OBJECT_TYPE::Factory:
							m_mapCommandMesh.find("thor")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);

							m_mapCommandMesh.find("armory")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							m_mapCommandMesh.find("starport")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							break;

						case OBJECT_TYPE::Fusioncore:
							m_mapCommandMesh.find("raven")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							m_mapCommandMesh.find("battlecruiser")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							break;

						case OBJECT_TYPE::Ghostacademy:
							break;

						case OBJECT_TYPE::Missileturret:
							break;

						case OBJECT_TYPE::Reactor:
							break;
							
						case OBJECT_TYPE::Refinery:
							break;

						case OBJECT_TYPE::Sensortower:
							break;

						case OBJECT_TYPE::Starport:
							m_mapCommandMesh.find("viking")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							m_mapCommandMesh.find("medivac")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							m_mapCommandMesh.find("banshee")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);

							m_mapCommandMesh.find("ghostacademy")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							m_mapCommandMesh.find("fusioncore")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
							break;

						case OBJECT_TYPE::Supplydepot:
							break;

						case OBJECT_TYPE::Techlab:
							break;
						}
						
					}
				}
				else if (packet->hp == 0)
				{  //�ݴ�� �ı��Ǵ� ������ ��쿡�� ���� �������� ���� �ǹ��� �Ѱ��� ������ �Ѿ��, ���Եȴٸ� false�� ����
					int nBuildingCount = 0;
					for (auto d = m_listMy.begin(); d != m_listMy.end(); ++d)
					{
						if ((*d)->m_nObjectType != obj_type) continue;
						++nBuildingCount;
					}
					if (nBuildingCount <= 1) //1���� �۰ų� ���� -> �� �ı��� ���� �� �ǹ��� ���縦 �Ҿ���� -> false�� ����
					{
						m_mapIsBuildingExist.find((OBJECT_TYPE)pObject->m_nObjectType)->second = false;
						printf("%s�� ��� �ı��Ǿ� �̷����� ��ũƮ���� �ҽ��ϴ�.\n", getNameByType(obj_type).data());

						switch (packet->object_type)
						{
						case OBJECT_TYPE::Armory:
							break;

						case OBJECT_TYPE::Barrack:
							m_mapCommandMesh.find("marine")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							m_mapCommandMesh.find("reaper")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							m_mapCommandMesh.find("marauder")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);

							m_mapCommandMesh.find("factory")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							m_mapCommandMesh.find("bunker")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							m_mapCommandMesh.find("sensortower")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							break;

						case OBJECT_TYPE::Bunker:
							break;

						case OBJECT_TYPE::Commandcenter:
							m_mapCommandMesh.find("scv")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);

							m_mapCommandMesh.find("barrack")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							m_mapCommandMesh.find("engineeringbay")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							break;

						case OBJECT_TYPE::Engineeringbay:
							m_mapCommandMesh.find("missileturret")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							break;

						case OBJECT_TYPE::Factory:
							m_mapCommandMesh.find("thor")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);

							m_mapCommandMesh.find("armory")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							m_mapCommandMesh.find("starport")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							break;

						case OBJECT_TYPE::Fusioncore:
							m_mapCommandMesh.find("raven")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							m_mapCommandMesh.find("battlecruiser")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							break;

						case OBJECT_TYPE::Ghostacademy:
							break;

						case OBJECT_TYPE::Missileturret:
							break;

						case OBJECT_TYPE::Reactor:
							break;

						case OBJECT_TYPE::Refinery:
							break;

						case OBJECT_TYPE::Sensortower:
							break;

						case OBJECT_TYPE::Starport:
							m_mapCommandMesh.find("viking")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							m_mapCommandMesh.find("medivac")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							m_mapCommandMesh.find("banshee")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);

							m_mapCommandMesh.find("ghostacademy")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							m_mapCommandMesh.find("fusioncore")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, false);
							break;

						case OBJECT_TYPE::Supplydepot:
							break;

						case OBJECT_TYPE::Techlab:
							break;
						}
					}
					
				}
			}
		}
		//��ũƮ�� 

		//��ü ���¸� �����Ѵ�.
		pObject->Process_SC_Value(packet);
	}
}

void CScene::Process_SC_Packet_fail_Object_Move(sc_packet_fail_object_move* packet)
{
	int obj_type = (int)packet->object_type;
	if (packet->client_id != CSocketComponent::GetInstance()->m_clientID)
		obj_type += ADD_ENEMY_INDEX;

	CGameObject* pObject = FindObjectById(obj_type, packet->object_id);

	if (pObject)
	{
		pObject->Process_SC_Fail_Move();
	}
}

void CScene::Process_SC_Packet_Put_Building(sc_packet_put_building* packet)
{
	int obj_type = (int)packet->object_type;
	//�Ǿ� �ĺ�
	if (packet->client_id != m_pGameFramework->m_clientID)
		obj_type += ADD_ENEMY_INDEX;

	auto vec = m_pStaticShader->m_vectorObjects;
	
	for (int i = vec.size() - 1; i >= 0; --i)
	{
		if (obj_type != vec[i]->m_ObjectType) continue;
		int ObjectIdx = vec[i]->m_nValidObjects;
		CStaticObject* pObject = (CStaticObject*)(vec[i]->m_ppObjects[ObjectIdx]);
		pObject->SetActive(true);
		pObject->SetPosition(packet->x, packet->y, packet->z);
		pObject->m_nObjectID = packet->object_id;
		pObject->m_nObjectType = obj_type;
		pObject->m_fBuildTime = (float)packet->building_time;
		vec[i]->m_nValidObjects++;


		if ((int)OBJECT_TYPE::Refinery == obj_type || (int)OBJECT_TYPE::Refinery_Enemy == obj_type)
		{
			for (auto d : m_listNeutral)
			{
				int gas_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(d->GetPosition());
				int obj_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(pObject->GetPosition());
				if (gas_idx == obj_idx)
				{
					pObject->m_pBeforeGasModel = d;
					d->SetActive(false); //���� ���� �����.
					break;
				}
			}
		}

		//test
		if (isEnemyObject(obj_type))
		{ //�� ����
			vec[i]->m_ppObjects[ObjectIdx]->SetInSight(false); //�ϴ� false, �þ� ����Ʈ ���� �ÿ� �ٲ���̴�.

			if (vec[i]->m_ppObjects[ObjectIdx]->m_bAirUnit)
				m_listEnemy.push_front(vec[i]->m_ppObjects[ObjectIdx]);
			else //����
				m_listEnemy.push_back(vec[i]->m_ppObjects[ObjectIdx]);
		}
		else //�Ʊ� �����̴�.
		{
			vec[i]->m_ppObjects[ObjectIdx]->SetInSight(true); //�Ʊ������� ������ ���δ�.

			if (vec[i]->m_ppObjects[ObjectIdx]->m_bAirUnit)
				m_listMy.push_front(vec[i]->m_ppObjects[ObjectIdx]);
			else
				m_listMy.push_back(vec[i]->m_ppObjects[ObjectIdx]);

			//�Ʊ� �ǹ��� ��� -> �ڿ� ����� ���⼭ �Ѵ�.
			switch (packet->object_type)
			{
			case OBJECT_TYPE::Commandcenter:
				gGameMineral -= 400; gGameGas -= 0; break;
			case OBJECT_TYPE::Supplydepot:
				gGameMineral -= 100; gGameGas -= 0; break;
			case OBJECT_TYPE::Refinery:
				gGameMineral -= 75; gGameGas -= 0; break;
			case OBJECT_TYPE::Barrack:
				gGameMineral -= 150; gGameGas -= 0; break;
			case OBJECT_TYPE::Engineeringbay:
				gGameMineral -= 120; gGameGas -= 0; break;
			case OBJECT_TYPE::Sensortower:
				gGameMineral -= 100; gGameGas -= 50; break;
			case OBJECT_TYPE::Missileturret:
				gGameMineral -= 100; gGameGas -= 0; break;
			case OBJECT_TYPE::Bunker:
				gGameMineral -= 100; gGameGas -= 0; break;
			case OBJECT_TYPE::Factory:
				gGameMineral -= 200; gGameGas -= 50; break;
			case OBJECT_TYPE::Starport:
				gGameMineral -= 150; gGameGas -= 100; break;
			case OBJECT_TYPE::Armory:
				gGameMineral -= 150; gGameGas -= 100; break;
			case OBJECT_TYPE::Ghostacademy:
				gGameMineral -= 150; gGameGas -= 50; break;
			case OBJECT_TYPE::Fusioncore_Enemy:
				gGameMineral -= 200; gGameGas -= 200; break;
			}
		}

		break;
	}
}


void  CScene::Process_SC_Packet_Attack_Object(sc_packet_attack_object* packet)
{
	printf("SC_Packet_Attack_Object ȣ��\n");

	bool bIsMyObject = true;
	int obj_type = (int)packet->object_type;
	if (packet->client_id != CSocketComponent::GetInstance()->m_clientID)
	{
		bIsMyObject = false;
		//obj_type += ADD_ENEMY_INDEX;
	}

	CGameObject* pObject = nullptr;
	CGameObject* pTarget = nullptr;

	pObject = FindObjectById(packet->object_id);
	pTarget = FindObjectById(packet->target_id);

	if (pObject && pTarget)
	{
		pObject->Process_SC_Attack(packet, pTarget); //packet �����ʹ� ��� ����
	}
}


void CScene::Process_SC_Packet_Add_Node(sc_packet_add_node* packet)
{
	Position pos = AStarComponent::GetInstance()->GetPositionByIndex(packet->index);
	float y = m_pHeightMap->GetHeight(pos.x, pos.z);
	D3DXVECTOR3 position{pos.x, y, pos.z};

	m_DebugCubeShader.PushPosition(position);
}

void CScene::Process_SC_Packet_Remove_Node(sc_packet_remove_node* packet)
{
	Position pos = AStarComponent::GetInstance()->GetPositionByIndex(packet->index);
	float y = m_pHeightMap->GetHeight(pos.x, pos.z);
	D3DXVECTOR3 position{ pos.x, y, pos.z };

	m_DebugCubeShader.RemovePosition(position);
}

void CScene::Process_SC_Packet_Data_Resource(sc_packet_data_resource* packet)
{
	printf("Process_SC_Packet_Data_Resource�� �޾Ҵ�! mineral : %d, gas %d\n", packet->mineral, packet->gas);
	gGameMineral += packet->mineral;
	gGameGas += packet->gas;

	//�ڿ� ǥ�ø� �����Ѵ�.
	list<CGameObject*>* obj_list;
	if (CSocketComponent::GetInstance()->m_clientID != packet->client_id)
		obj_list = &m_listEnemy;
	else //�� ����
		obj_list = &m_listMy;

	for (auto d : *obj_list)
	{
		if (d->m_nObjectID != packet->obj_id) continue;
		CAnimationObject* pScv = (CAnimationObject*)d;
		pScv->m_nScvDigResource = SCV_DIG::invalid;
		return;
	}
}

void CScene::Process_SC_Packet_Get_Mineral(sc_packet_get_mineral* packet)
{
	//�ڿ� ǥ�� Ȱ��ȭ
	list<CGameObject*>* obj_list;
	if (CSocketComponent::GetInstance()->m_clientID != packet->client_id)
		obj_list = &m_listEnemy;
	else //�� ����
		obj_list = &m_listMy;

	for (auto d : *obj_list)
	{	
		if (d->m_nObjectID != packet->obj_id) continue;
		CAnimationObject* pScv = (CAnimationObject*)d;
		pScv->m_nScvDigResource = SCV_DIG::mineral;
		return;
	}
}

void CScene::Process_SC_Packet_Get_Gas(sc_packet_get_gas* packet)
{
	//�ڿ� ǥ�� Ȱ��ȭ
	list<CGameObject*>* obj_list;
	if (CSocketComponent::GetInstance()->m_clientID != packet->client_id)
		obj_list = &m_listEnemy;
	else //�� ����
		obj_list = &m_listMy;

	for (auto d : *obj_list)
	{
		if (d->m_nObjectID != packet->obj_id) continue;
		CAnimationObject* pScv = (CAnimationObject*)d;
		pScv->m_nScvDigResource = SCV_DIG::gas;
		return;
	}
}



//��Ŷ ���� �Լ���
void CScene::CS_Packet_Move_Object(CGameObject* pObject, D3DXVECTOR3& position, bool bRightClicked)
{
	cs_packet_move_object* my_packet = reinterpret_cast<cs_packet_move_object *>(CSocketComponent::GetInstance()->m_SendBuf);
	my_packet->size = sizeof(cs_packet_move_object);
	my_packet->type = CS_MOVE_OBJECT;
	my_packet->room_id = CSocketComponent::GetInstance()->m_roomID;
	my_packet->object_id = pObject->m_nObjectID;
	my_packet->object_type = (OBJECT_TYPE)pObject->m_nObjectType;
	my_packet->client_id = CSocketComponent::GetInstance()->m_clientID;
	my_packet->x = position.x;
	my_packet->y = position.y;
	my_packet->z = position.z;
	my_packet->b_click_rbutton = bRightClicked;

	CSocketComponent::GetInstance()->SendPacket(reinterpret_cast<unsigned char*>(my_packet));

	if (bRightClicked)
		printf("%d ������ Scene::Move��Ŷ�� ���´�. right : true\n", pObject->m_nObjectID);
	else
		printf("%d ������ Scene::Move��Ŷ�� ���´�. right : false\n", pObject->m_nObjectID);
}

void CScene::CS_Packet_Create_Building(CGameObject* pSCV, int nodeIndex, OBJECT_TYPE obj_type)
{
	cs_packet_create_building* my_packet = reinterpret_cast<cs_packet_create_building *>(CSocketComponent::GetInstance()->m_SendBuf);
	my_packet->size = sizeof(cs_packet_move_object);
	my_packet->type = CS_CREATE_BUILDING;
	my_packet->room_id = CSocketComponent::GetInstance()->m_roomID;
	my_packet->client_id = CSocketComponent::GetInstance()->m_clientID;
	my_packet->object_type = obj_type;
	my_packet->my_object_id = pSCV->m_nObjectID;
	my_packet->index = nodeIndex;
	
	CSocketComponent::GetInstance()->SendPacket(reinterpret_cast<unsigned char*>(my_packet));
}

void CScene::CS_Packet_Create_Unit(OBJECT_TYPE obj_type, D3DXVECTOR3 pos)
{
	cs_packet_create_unit* my_packet = reinterpret_cast<cs_packet_create_unit *>(CSocketComponent::GetInstance()->m_SendBuf);
	my_packet->size = sizeof(cs_packet_move_object);
	my_packet->type = CS_CREATE_UNIT;
	my_packet->room_id = CSocketComponent::GetInstance()->m_roomID;
	my_packet->client_id = CSocketComponent::GetInstance()->m_clientID;
	my_packet->object_type = obj_type;
	my_packet->x = pos.x;
	my_packet->y = pos.y;
	my_packet->z = pos.z;

	CSocketComponent::GetInstance()->SendPacket(reinterpret_cast<unsigned char*>(my_packet));
}

void CScene::CS_Packet_Dig_Resource(CGameObject* pSCV, CGameObject* pResource, CGameObject* pCommandcenter)
{
	printf("%d SCV�� CS_DIG�� ���´�! mineral : %d / command : %d\n", pSCV->m_nObjectID, pResource->m_nObjectID, pCommandcenter->m_nObjectID);
	cs_packet_dig_resource* my_packet = reinterpret_cast<cs_packet_dig_resource *>(CSocketComponent::GetInstance()->m_SendBuf);
	my_packet->size = sizeof(cs_packet_dig_resource);
	my_packet->type = CS_DIG_RESOURCE;
	my_packet->room_id = CSocketComponent::GetInstance()->m_roomID;
	my_packet->client_id = CSocketComponent::GetInstance()->m_clientID;
	my_packet->obj_id = pSCV->m_nObjectID;
	my_packet->resource_id = pResource->m_nObjectID;
	my_packet->command_id = pCommandcenter->m_nObjectID;
	
	CSocketComponent::GetInstance()->SendPacket(reinterpret_cast<unsigned char*>(my_packet));
}

//**********************************************************************
//���� ó��

void CScene::UpdateObject(float fTimeElapsed)
{
	//Scene�� �ٸ� �Լ����� ����ϵ��� ��������� ����ð��� �����Ѵ�.
	m_fCurrentFrameElapsedTime = fTimeElapsed;

	if (m_ppShaders && (m_nShaders >= 3))
	{
		//��ī�̹ڽ� �� �� �پ�Ѱ�
		for (auto i = 3; i < m_nShaders; ++i)
			m_ppShaders[i]->UpdateObject(fTimeElapsed, m_pHeightMap, nullptr, nullptr);	//���� enemyList�� �ʿ����� ����
	}

	//��ġ�� ���� ������ �ؾ���..
	UpdateEnemyInSight(fTimeElapsed);

	if (m_pStaticShader)		m_pStaticShader->UpdateObject(fTimeElapsed, m_pHeightMap, &m_listMy, &m_listEnemyInSight); 
	if (m_pAnimationShader)		m_pAnimationShader->UpdateObject(fTimeElapsed, m_pHeightMap, &m_listMy, &m_listEnemyInSight);

	if (m_pSoundManager) m_pSoundManager->UpdateSound();
	//UpdateCreateUnitList(fTimeElapsed);

	m_fLastInputCrowdDataButtonTime += fTimeElapsed;
}

void CScene::Animate(float fTimeElapsed)
{
	if (m_ppShaders && (m_nShaders >= 3))
	{
		//��ī�̹ڽ� �� �� �پ�Ѱ�
		for (auto i = 3; i < m_nShaders; ++i)
			m_ppShaders[i]->Animate(fTimeElapsed);
	}

	if (m_pStaticShader)		m_pStaticShader->Animate(fTimeElapsed);
	if (m_pAnimationShader)		m_pAnimationShader->Animate(fTimeElapsed);
}

//�� ����Ʈ ����
void CScene::UpdateEnemyInSight(float fElapsedTime)
{
	//�� ���� UpdateTime�� �������ִ� ���� ���ڴ�.
	static float EnemyListUpdateTimer = 0.0f;
	EnemyListUpdateTimer += m_fCurrentFrameElapsedTime; //fElapsedTime�� ���� ���̰����� �ϴ� ���ڰ����� �޾Ҵ�.

	if (EnemyListUpdateTimer < UPDATE_ENEMYLIST_TIME)
		return;
	else
		EnemyListUpdateTimer = 0.0f;

	//��� �� ��ü ����Ʈ �� �þ� ���� ���ԵǴ� ��ü�� ã�Ƴ���.
	m_listEnemyInSight.clear();
	D3DXVECTOR3 objPos;
	float x, y, z;
	int xIdx, zIdx;

	for (auto d : m_listEnemy)
	{
		objPos = d->GetPosition();
		//��ü ��ġ�� ����
		x = objPos.x; y = objPos.y; z = objPos.z;
		x += (MAP_WIDTH / 2.0f); z += (MAP_HEIGHT / 2.0f); // to 0 ~ 205
		x /= MAP_WIDTH; z /= MAP_HEIGHT;
		xIdx = (int)(x * (NUM_FOG_SLICE - 1.0f));
		zIdx = (NUM_FOG_SLICE - 1.0f) - (int)(z * (NUM_FOG_SLICE - 1.0f));

		if (m_d3dxvFogData[xIdx + (int)(zIdx * (NUM_FOG_SLICE))].w == 2.0f) //�þ� �ȿ� �����Ѵٸ�
		{
			m_listEnemyInSight.push_back(d);
			d->SetInSight(true);
		}
		else
			d->SetInSight(false);
	}	

	//0901 �߸� �ڿ� ���� �� �þ߿� ���������� �Ǵ��ؾ� �Ѵ�.
	for (auto d : m_listNeutral)
	{
		objPos = d->GetPosition();
		//��ü ��ġ�� ����
		x = objPos.x; y = objPos.y; z = objPos.z;
		x += (MAP_WIDTH / 2.0f); z += (MAP_HEIGHT / 2.0f); // to 0 ~ 205
		x /= MAP_WIDTH; z /= MAP_HEIGHT;
		xIdx = (int)(x * (NUM_FOG_SLICE - 1.0f));
		zIdx = (NUM_FOG_SLICE - 1.0f) - (int)(z * (NUM_FOG_SLICE - 1.0f));

		if (m_d3dxvFogData[xIdx + (int)(zIdx * (NUM_FOG_SLICE))].w == 2.0f) //�þ� �ȿ� �����Ѵٸ�
			d->SetInSight(true);
		else
			d->SetInSight(false);
	}
}

//�׸���
void CScene::RenderObjectShadow(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_pStaticShader)		m_pStaticShader->RenderObjectShadow(pd3dDeviceContext, m_pCamera);
	if (m_pAnimationShader)		m_pAnimationShader->RenderObjectShadow(pd3dDeviceContext, m_pCamera);
}

//�Ȱ�
void CScene::UpdateFogBuffer(ID3D11DeviceContext *pd3dDeviceContext)
{
	static float fogUpdateTimer = 0.0f;
	fogUpdateTimer += m_fCurrentFrameElapsedTime;
	
	if (fogUpdateTimer < UPDATE_FOG_TIME)
		return;
	else
		fogUpdateTimer = 0.0f;

	//���߰� ������ �����Ѵٸ�(������ push front, ������ back) 
	//���� ���� ������ �ʿ������� ���� �˻���� ȸ���� �ξ� �پ���.

	// 1. ���� list�� ���� list ���� 
	//list<CGameObject*> ObjectList;
	list<CGameObject*>& ObjectList = m_listMy;

	/*
	// 0729 ���� -> list 3���� �ڷᱸ���� �����ϸ鼭, �ذ� ���� ������ �ʿ䰡 ����.
	// 2. �þ߸� ���� ��ȿ�� ��ü���� ��ȸ, list�� push�Ѵ�. ��ü�� ���������̶�� push front!!
	vector<RenderObjectData*>* vec;
	CGameObject* pObject;
	
	//Static���� �����Ѵ�.
	vec = &(m_pStaticShader->m_vectorObjects);
	for (int i = (*vec).size() - 1; i >= 0; --i)
	{
		for (int j = 0; j < (*vec)[i]->m_nValidObjects; ++j)
		{
			pObject = (*vec)[i]->m_ppObjects[j];
			if (!pObject->GetActive()) continue;
			
			if (pObject->m_bAirUnit)
				ObjectList.push_front(pObject);
			else
				ObjectList.push_back(pObject);
		}
	}

	//Animate ��ü��
	vec = &(m_pAnimationShader->m_vectorObjects);
	for (int i = (*vec).size() - 1; i >= 0; --i)
	{
		for (int j = 0; j < (*vec)[i]->m_nValidObjects; ++j)
		{
			pObject = (*vec)[i]->m_ppObjects[j];
			if (!pObject->GetActive()) continue;

			if (pObject->m_bAirUnit)
				ObjectList.push_front(pObject);
			else
				ObjectList.push_back(pObject);
		}
	}
	*/

	// 3. ������ (NUM_FOG_SLICE)*(NUM_FOG_SLICE)�� ������ ������ ��ü�� �þ� ������ �浹 �˻縦 ����
	//    ��ü���� ��ȸ�ϸ� ���� �þ�(3)�� ����Ѵ�.
	float nodeDistance = m_d3dxvFogData[1].x - m_d3dxvFogData[0].x;
	float objRange = 0.0f;

	int nRange = 0;
	int xIdx, zIdx;

	D3DXVECTOR3 objPos;
	
	for (CGameObject* obj : ObjectList)
	{
		objPos = obj->GetPosition();
		objRange = obj->m_fObjectSightRange;

		//�þ� ������ ����
		nRange = objRange / nodeDistance;

		//��ü ��ġ�� ����
		float x, y, z;
		x = objPos.x; y = objPos.y; z = objPos.z;
		x += (MAP_WIDTH / 2.0f); z += (MAP_HEIGHT / 2.0f); // to 0 ~ 205
		x /= MAP_WIDTH; z /= MAP_HEIGHT;
		xIdx = (int)(x * (NUM_FOG_SLICE - 1.0f));
		zIdx = (NUM_FOG_SLICE - 1.0f) - (int)(z * (NUM_FOG_SLICE - 1.0f));

		//�þ߹����� ���� �ε��� ����Ʈ�� ���Ѵ�.
		for (int j = 0; j <= nRange * 2; ++j)
		{
			for (int i = 0; i <= nRange * 2; ++i)
			{
				int getX = xIdx - nRange + i;
				int getZ = zIdx - nRange + j;

				//��ȿ�� �˻�
				if (getX > int(NUM_FOG_SLICE - 1.0f) || getX < 0 || getZ > int(NUM_FOG_SLICE - 1.0f) || getZ < 0) continue;

				//�̹� ���ŵǾ����� �˻�
				if (m_d3dxvFogData[getX +  (int)(getZ * (NUM_FOG_SLICE))].w == 3.0f) continue;

				//�þ� ���� �� ������ �˻�
				int distX = xIdx - getX;
				int distZ = zIdx - getZ;
				if ((nRange*nRange) < ((distX * distX) + (distZ * distZ))) continue;

				//���������̶�� �ٷ� ������ ���ְ�, ���������� ��� ��� ���� �˻�
				if ((false == obj->m_bAirUnit) && ((m_d3dxvFogData[getX + (int)(getZ * (NUM_FOG_SLICE))].y - objPos.y) >= 1.0f)) continue;

				//���� �� �˻縦 ����� ���� �ε����� �þ߿� ���̴°��� Ȯ���Ǿ���.
				m_d3dxvFogData[getX + (int)(getZ * (NUM_FOG_SLICE))].w = 3.0f;
			}
		}
	}

	// 4. ���� ������ ��ȸ�ϸ� ���� �þ� �� ���� �þ߿����� ����� ��츦 ó�����ش�.
	for (int i = 0; i < (NUM_FOG_SLICE) * (NUM_FOG_SLICE); ++i)
	{	//0 ������ 1 ���� 2�׷�������
		if (m_d3dxvFogData[i].w <= 1.0f)	continue;

		if (m_d3dxvFogData[i].w == 2.0f) //���� �þ� ���������� ���� �����ߴ� �������� ǥ���ؾ� �ϴ�
			m_d3dxvFogData[i].w = 1.0f;
		else if (m_d3dxvFogData[i].w == 3.0f) //������ ���� ���ο� �þ� ������ ����
			m_d3dxvFogData[i].w = 2.0f;
		else
			printf("Error Occured!! Scene::UpdateFogBuffer\n");
	}

	/*
	//test
	int fog0, fog1, fog2;
	fog0 = fog1 = fog2 = 0;
	for (int i = 0; i < (NUM_FOG_SLICE) * (NUM_FOG_SLICE); ++i)
	{
		if (m_d3dxvFogData[i].w == 0.0f)
			fog0++;
		else if (m_d3dxvFogData[i].w == 1.0f)
			fog1++;
		else if (m_d3dxvFogData[i].w == 2.0f)
			fog2++;
	}
	int fogSum = fog0 + fog1 + fog2;
	*/

	/*
	// 5. ������۸� Map, PS�� ����
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbFogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	FogData *pcbFogData = (FogData *)d3dMappedResource.pData;
	memcpy(pcbFogData, &(m_d3dxvFogData[0]), sizeof(FogData) * (NUM_FOG_SLICE) * (NUM_FOG_SLICE));
	pd3dDeviceContext->Unmap(m_pd3dcbFogBuffer, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_FOG, 1, &m_pd3dcbFogBuffer);
	*/

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dFogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	float *pcbFogData = (float *)d3dMappedResource.pData;
	for (int i = 0; i < NUM_FOG_SLICE * NUM_FOG_SLICE; ++i)
	{
		pcbFogData[i] = m_d3dxvFogData[i].w;
	}
	pd3dDeviceContext->Unmap(m_pd3dFogBuffer, 0);
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_FOG, 1, &m_pd3dSRVFog);
}

void CScene::UpdateSelectedObjects()
{
	//0901 ���õ� ��ü �����ϱ�
	if (m_nValidSelectedObjects > 0)
	{
		int nValidObjects = 0;
		CGameObject* tempObjectBox[MAX_SELECTED_OBJECTS];
		for (int i = 0; i < m_nValidSelectedObjects; ++i)
		{
			if (!m_ppSelectedObjects[i]->IsInSight()) continue;
			if (!m_ppSelectedObjects[i]->GetActive()) continue;

			tempObjectBox[nValidObjects++] = m_ppSelectedObjects[i];
		}

		m_nValidSelectedObjects = nValidObjects;
		for (int i = 0; i < nValidObjects; ++i)
			m_ppSelectedObjects[i] = tempObjectBox[i];
	}
}

void CScene::UpdateCrowdSavedData()
{
	CGameObject* tempObjectBox[MAX_SELECTED_OBJECTS];
	int nValidObjects = 0;

	for (int i = 0; i < 8; ++i)
	{
		if (m_CrowdSaveData[i].nUnits == 0) continue;

		nValidObjects = 0;
		for (int j = 0; j < m_CrowdSaveData[i].nUnits; ++j)
		{	
			if (!m_CrowdSaveData[i].ppObjects[j]->IsInSight()) continue;
			if (!m_CrowdSaveData[i].ppObjects[j]->GetActive()) continue;

			tempObjectBox[nValidObjects++] = m_CrowdSaveData[i].ppObjects[j];
		}

		if (m_CrowdSaveData[i].nUnits != nValidObjects) // ���������� ���ٸ� Pass, ���������� ������ �����ؾ� �Ѵ�.
		{
			m_CrowdSaveData[i].nUnits = nValidObjects;

			for (int k = 0; k < nValidObjects; ++k)
				m_CrowdSaveData[i].ppObjects[k] = tempObjectBox[k];
		}
	}
}

void CScene::DrawMouseClickImage(D3DXVECTOR3& vScreenToWorld)
{
	//Prompt ��������Ʈ�� ���� ��ǥ�踦 �������� �������Ǳ⿡, Screen to World�� �����Ѵ�.
	CPromptParticleObject particle;
	particle.m_d3dxmtxWorld._41 = vScreenToWorld.x;
	particle.m_d3dxmtxWorld._42 = vScreenToWorld.y;
	particle.m_d3dxmtxWorld._43 = vScreenToWorld.z;

	CSpriteParticle::GetInstance()->AddPromptParticle(string("mouse_click"), particle);
}

/*
void CScene::DrawSystemErrorImage(D3DXVECTOR3& vScreenToWorld, int errorCode)
{
	//Prompt ��������Ʈ�� ���� ��ǥ�踦 �������� �������Ǳ⿡, Screen to World�� �����Ѵ�.
	CPromptParticleObject particle;
	particle.m_d3dxmtxWorld._41 = vScreenToWorld.x;
	particle.m_d3dxmtxWorld._42 = vScreenToWorld.y;
	particle.m_d3dxmtxWorld._43 = vScreenToWorld.z;

	CSpriteParticle::GetInstance()->AddPromptParticle(string("mouse_click"), particle);
}
*/

void CScene::Render(ID3D11DeviceContext* pd3dDeviceContext, CCamera* pCamera)
{
	if (m_pLights && m_pd3dcbLights) UpdateShaderVariable(pd3dDeviceContext, m_pLights);

#ifndef DEBUG_DISABLE_FOG
	//�Ȱ� ����
	UpdateFogBuffer(pd3dDeviceContext);
#else
	pd3dDeviceContext->PSSetShaderResources(PS_SLOT_FOG, 1, &m_pd3dSRVFog);
#endif

	UpdateSelectedObjects();
	UpdateCrowdSavedData();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; ++i)
		{
			//1�� �׼����̼�, 2�� ���̸�
			//if (i == 1) continue;
			if (i == 2) continue; //���̸� ������ ������ ���� ��ŷ�� �̰� �׷����� ������ ��û �������� �׼����̼� ��;
			m_ppShaders[i]->Render(pd3dDeviceContext, pCamera);
		}
	}

	//���õ� ������Ʈ�� �׸� �� ���̺񱳸� ���� �ʰ� ������ ������ �׸���.
	if (m_pd3dUIDepthStencilState) pd3dDeviceContext->OMSetDepthStencilState(m_pd3dUIDepthStencilState, 0);

	//���õ� ������Ʈ���� ���� UI ǥ��
	if (m_nValidSelectedObjects > 0)
	{
		std::vector<D3DXVECTOR4> vPosScale;
		vPosScale.reserve(m_nValidSelectedObjects);
		float fScale;
		//�Ǿ� �� �߸� �׽�Ʈ
		//������ Scale ���ڰ��� �Ʊ��̸� 0~100 / �����̸� +100 / �߸��̸� +200�� �Ѵ�.
		//���̴��ڵ忡�� 0~100�����̸� �ʷ�, 100���� ũ�� -100 �ϰ� ��������, 200���� ũ�� -200�ϰ� ������� ������
		for (auto i = 0; i < m_nValidSelectedObjects; ++i)
		{
			fScale = 2.0f * m_ppSelectedObjects[i]->m_bcMeshBoundingCube.m_d3dxvMaximum.x;
			if ((int)OBJECT_TYPE::Mineral == m_ppSelectedObjects[i]->m_nObjectType || (int)OBJECT_TYPE::Gas == m_ppSelectedObjects[i]->m_nObjectType)
				fScale += 200.0f;
			else if (isEnemyObject(m_ppSelectedObjects[i]->m_nObjectType))
				fScale += 100.0f;
			
			vPosScale.emplace_back(m_ppSelectedObjects[i]->GetPosition(), fScale);	// test ScaleType 1.0f;
		}
		OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::selectedcircle);
		m_pSelectedCircleMesh->UpdateSelectedCircle(vPosScale, pd3dDeviceContext);
		m_pSelectedCircleTexture->UpdateShaderVariable(pd3dDeviceContext);
		m_pSelectedCircleMesh->Render(pd3dDeviceContext);	//�ν��Ͻ��� �׷�����? ��εǳ�

		if ((int)OBJECT_TYPE::Mineral != m_ppSelectedObjects[0]->m_nObjectType && (int)OBJECT_TYPE::Gas != m_ppSelectedObjects[0]->m_nObjectType)
			m_pProgressImage->RenderHPImage(pd3dDeviceContext, m_ppSelectedObjects, m_nValidSelectedObjects);

	}

	pd3dDeviceContext->OMSetDepthStencilState(nullptr, 0);

	if (m_pStaticShader)		m_pStaticShader->Render(pd3dDeviceContext, pCamera);
	if (m_pAnimationShader)		m_pAnimationShader->Render(pd3dDeviceContext, pCamera);

	//UI�� Depth ����
	if (m_pd3dUIDepthStencilState) pd3dDeviceContext->OMSetDepthStencilState(m_pd3dUIDepthStencilState, 0);


	//scv�� ����ִ� �ڿ� �̹���
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::neutralresource);
	m_pNeutralResourceTexture->UpdateShaderVariable(pd3dDeviceContext);
	std::vector<D3DXVECTOR4> vResourcePosType;
	vResourcePosType.reserve(MAX_KINDS_OBJECTS);
	//1. �� scv �� ����ִ¾� ã��
	D3DXVECTOR3 vResourcePos;
	D3DXVECTOR3 vDir;
	for (auto d : m_listMy)
	{
		if ((int)OBJECT_TYPE::Scv != d->m_nObjectType) continue;

		CAnimationObject* pScv = (CAnimationObject*)d;
		if (SCV_DIG::invalid == pScv->m_nScvDigResource) continue;

		vDir = D3DXVECTOR3(pScv->m_d3dxmtxWorld._31, pScv->m_d3dxmtxWorld._32, pScv->m_d3dxmtxWorld._33);
		vResourcePos = (pScv->m_bcMeshBoundingCube.m_d3dxvMaximum.z * vDir * 2.0f) + pScv->GetPosition();
		if (SCV_DIG::mineral == pScv->m_nScvDigResource)
		{
			vResourcePosType.emplace_back(vResourcePos, 1.0f);
		}
		else //���� ��� ����
		{
			vResourcePosType.emplace_back(vResourcePos, 2.0f);
		}
	}
	//2. �� scv �� ����ִ¾� ã��
	if (m_listEnemyInSight.size() > 0)
	{
		for (auto d : m_listEnemyInSight)
		{
			if ((int)OBJECT_TYPE::Scv_Enemy != d->m_nObjectType) continue;

			CAnimationObject* pScv = (CAnimationObject*)d;
			if (SCV_DIG::invalid == pScv->m_nScvDigResource) continue;

			vDir = D3DXVECTOR3(pScv->m_d3dxmtxWorld._31, pScv->m_d3dxmtxWorld._32, pScv->m_d3dxmtxWorld._33);
			vResourcePos = (pScv->m_bcMeshBoundingCube.m_d3dxvMaximum.z * vDir * 2.0f) + pScv->GetPosition();
			if (SCV_DIG::mineral == pScv->m_nScvDigResource)
			{
				vResourcePosType.emplace_back(vResourcePos, 1.0f);
			}
			else //���� ��� ����
			{
				vResourcePosType.emplace_back(vResourcePos, 2.0f);
			}
		}
	}
	m_pNeutralResourceMesh->UpdateNeutralResource(vResourcePosType, pd3dDeviceContext);
	m_pNeutralResourceMesh->Render(pd3dDeviceContext);


	//��ƼŬ ������
	m_pParticleBox->Update(pd3dDeviceContext, m_fCurrentFrameElapsedTime);
	m_pParticleBox->Render(pd3dDeviceContext);

	//�巡�� �̺�Ʈ�� ���� UI ǥ��
	if (true == m_bMouseRectDraw)
	{
		OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::mouserect);
		//m_pMouseRectMesh->UpdateMouseRect(m_ptOldCursorPos, m_ptNextCursorPos, m_fWndClientWidth, m_fWndClientHeight, pd3dDeviceContext);
		m_pMouseRectMesh->UpdateMouseRect(m_d3dxvOldCursorPosition, m_d3dxvNextCursorPosition, pd3dDeviceContext);
		m_pMouseRectTexture->UpdateShaderVariable(pd3dDeviceContext);
		m_pMouseRectMesh->Render(pd3dDeviceContext);
	}

	//�ǹ� ���� �̹���
	if (m_pBuildCheck && m_bActivedBuildCheck)
	{
		m_pBuildCheck->SetBuildingImage(m_strActivedBuildCheck, GetPositionPicking(m_ptMousePosition.x, m_ptMousePosition.y));
		m_pBuildCheck->Render(pd3dDeviceContext);
	}

#ifdef DEBUG_RENDERING_GRID
	//����� ����
	m_pDebugPrograms->Render(pd3dDeviceContext);
#endif

	//���� ������
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::mainframe);
	m_pMainFrameTexture->UpdateShaderVariable(pd3dDeviceContext);
	m_pMainFrameMesh->Render(pd3dDeviceContext);
	
	//�̴ϸ�
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::minimap);
	m_pMiniMapTexture->UpdateShaderVariable(pd3dDeviceContext);
	m_pMiniMapMesh->Render(pd3dDeviceContext);
	
	//�̴ϸ� ��ü
	// 0 -> �Ʊ�
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::minimapobject);
	std::vector<D3DXVECTOR4> vPosType;
	vPosType.reserve(MAX_KINDS_OBJECTS * MAX_INSTANCE_OBJECTS);
	for (auto d : m_listMy)
		vPosType.emplace_back(d->GetPosition(), 0.0f);

	if (m_listEnemyInSight.size() > 0) // 1 -> ����
	{
		for (auto d : m_listEnemyInSight)
			vPosType.emplace_back(d->GetPosition(), 1.0f);
	}
	//2 -> �߸����� ex �̳׶��� ����
	if (m_listNeutral.size() > 0) // 2 -> �̳׶�
	{
		for (auto d : m_listNeutral)
			vPosType.emplace_back(d->GetPosition(), 2.0f);
	}
	m_pMiniMapObjectMesh->UpdateMiniMapObject(vPosType, pd3dDeviceContext);
	m_pMiniMapObjectMesh->Render(pd3dDeviceContext);

	//�̴ϸ� �簢��
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::minimaprect);
	m_pMiniMapRectTexture->UpdateShaderVariable(pd3dDeviceContext);
	m_pMiniMapRectMesh->UpdateMiniMapRect(m_pCamera->GetLookAtPosition(), pd3dDeviceContext);
	m_pMiniMapRectMesh->Render(pd3dDeviceContext);
	
	//���� �����ӿ� �ٴ� attached
	if (m_nValidSelectedObjects > 0)
	{
		OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::attached);

		if (m_nSceneState == SCENE_STATE::nothing || m_nSceneState == SCENE_STATE::pick_scv)
		{
			for (auto& d : m_ppSelectedObjects[0]->m_listUI)
			{
				m_mapCommandTexture.find(d)->second->UpdateShaderVariable(pd3dDeviceContext);
				m_mapCommandMesh.find(d)->second->Render(pd3dDeviceContext);
			}
		}
		else
		{
			if (SCENE_STATE::scv_build == m_nSceneState)
			{
				for (auto& d : m_listSCVBuild)
				{
					m_mapCommandTexture.find(d)->second->UpdateShaderVariable(pd3dDeviceContext);
					m_mapCommandMesh.find(d)->second->Render(pd3dDeviceContext);
				}
			}
			else if (SCENE_STATE::scv_buildadv == m_nSceneState)
			{
				for (auto& d : m_listSCVBuildAdv)
				{
					m_mapCommandTexture.find(d)->second->UpdateShaderVariable(pd3dDeviceContext);
					m_mapCommandMesh.find(d)->second->Render(pd3dDeviceContext);
				}
			}
			else if (SCENE_STATE::commandcenter_units == m_nSceneState)
			{
				m_mapCommandTexture.find(string("scv"))->second->UpdateShaderVariable(pd3dDeviceContext);
				m_mapCommandMesh.find(string("scv"))->second->Render(pd3dDeviceContext);
			}
			else if (SCENE_STATE::barrack_units == m_nSceneState)
			{
				for (auto& d : m_listBarrackUnits)
				{
					m_mapCommandTexture.find(d)->second->UpdateShaderVariable(pd3dDeviceContext);
					m_mapCommandMesh.find(d)->second->Render(pd3dDeviceContext);
				}
			}
			else if (SCENE_STATE::factory_units == m_nSceneState)
			{
				for (auto& d : m_listFactoryUnits)
				{
					m_mapCommandTexture.find(d)->second->UpdateShaderVariable(pd3dDeviceContext);
					m_mapCommandMesh.find(d)->second->Render(pd3dDeviceContext);
				}
			}
			else if (SCENE_STATE::starport_units == m_nSceneState)
			{
				for (auto& d : m_listStarportUnits)
				{
					m_mapCommandTexture.find(d)->second->UpdateShaderVariable(pd3dDeviceContext);
					m_mapCommandMesh.find(d)->second->Render(pd3dDeviceContext);
				}
			}
			else if (SCENE_STATE::build_armory <= m_nSceneState && m_nSceneState <= SCENE_STATE::build_techlab)
			{
				m_mapCommandTexture.find(string("cancel"))->second->UpdateShaderVariable(pd3dDeviceContext);
				m_mapCommandMesh.find(string("cancel"))->second->Render(pd3dDeviceContext);
			}
		}

		
		//���� �����ӿ� �ٴ� �δ����� ���� ���� ������
		m_pCrowdAttachedFrameTexture->UpdateShaderVariable(pd3dDeviceContext);
		for (int i = 0; i < 8; ++i)
		{
			if (0 == m_CrowdSaveData[i].nUnits) continue;
			
			m_vecAttachedCrowdMeshes[i]->Render(pd3dDeviceContext);
		}
		
		
		//status Ȥ�� ���� ���� ǥ�� �� �ʻ�ȭ ������
		if (0 < m_nValidSelectedObjects)
		{
			//�ϴ� �ʻ�ȭ ������, �׳� 0�� ���� ��ǥ�� �Ѵ�.
			int type = m_ppSelectedObjects[0]->m_nObjectType;
			if (isEnemyObject(type)) type -= ADD_ENEMY_INDEX;
			auto iterPortraitTexture = m_mapPortraitTexture.find(type);
			if (iterPortraitTexture != m_mapPortraitTexture.end())
			{
				iterPortraitTexture->second->UpdateShaderVariable(pd3dDeviceContext);
				m_pPortraitMesh->Render(pd3dDeviceContext);
			}
			else //�Ƹ� ��ã�Ҵٸ� �ǹ��� ���̴�.
			{
				m_mapPortraitTexture.find((int)OBJECT_TYPE::Commandcenter)->second->UpdateShaderVariable(pd3dDeviceContext);
				m_pPortraitMesh->Render(pd3dDeviceContext);
			}

			//status â
			if (1 == m_nValidSelectedObjects) //1�� ��ŷ���� statusǥ��, 2�� �̻��̶�� Crowd�� ǥ���ؾ� �Ѵ�.
			{
				int type = m_ppSelectedObjects[0]->m_nObjectType;
				bool isEnemy = false;
				if ((int)OBJECT_TYPE::Mineral == type || (int)OBJECT_TYPE::Gas == type)
				{
					;
				}
				else if (isEnemyObject(type))
				{
					type -= ADD_ENEMY_INDEX; isEnemy = true;
				}

				auto iterStatusTexture = m_mapStatusTexture.find(type);
				if (iterStatusTexture != m_mapStatusTexture.end())
				{
					iterStatusTexture->second->UpdateShaderVariable(pd3dDeviceContext);
					m_pStatusMesh->Render(pd3dDeviceContext);

					//���� �ǹ��� ��, ���� �� �������� ��� ������¸� ǥ���ؾ� �Ѵ�.
				}

				if (!isEnemy && !isAnimateObject(type))
				{
					CStaticObject* pBuilding = (CStaticObject*)(m_ppSelectedObjects[0]);
					//�Ʊ� ���� �� �ǹ��̶�� 
					if (!pBuilding->GetCompleteBuild()) //���� �� �������� ��� ������¸� ǥ���ؾ� �Ѵ�.
					{
						string unitName = getNameByType(type);
						m_mapCommandTexture.find(unitName)->second->UpdateShaderVariable(pd3dDeviceContext);
						m_pProgressIconMesh->Render(pd3dDeviceContext);
						// 2. ������� ������
						m_pProgressImage->RenderProgressImage(pd3dDeviceContext, pBuilding->m_fBuildTime / pBuilding->m_fCompleteBuildTime);
					}
					//���� �������� ������ �ִٸ� ������¸� ǥ���ؾ� �Ѵ�. �ٸ� Shader�� Set�ϹǷ� ���������� ����. 
					else if (OBJECT_TYPE::Invalid != pBuilding->m_typeGeneratedUnit)
					{	//�������� ������ �ִٸ� ���� ���ְ� ������¸� ������
						// 1. ���� ������ ������
						string unitName = getNameByType((int)pBuilding->m_typeGeneratedUnit);
						m_mapCommandTexture.find(unitName)->second->UpdateShaderVariable(pd3dDeviceContext);
						m_pProgressIconMesh->Render(pd3dDeviceContext);
						// 2. ������� ������
						m_pProgressImage->RenderProgressImage(pd3dDeviceContext, pBuilding->m_fUnitGeneratedTime / pBuilding->m_fUnitCompleteTime);
					}
				}
			} //end of 1 == m_nSelectedObjects
			else if (2 <= m_nValidSelectedObjects) //���� ���� �̹��� ������ ��
			{
				// 1. ���� ���� frame ������
				OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::crowdimage);
				m_pCrowdFrameTexture->UpdateShaderVariable(pd3dDeviceContext);
				int nSelected = m_nValidSelectedObjects > 24 ? 24 : m_nValidSelectedObjects;
				for (int i = 0; i < nSelected; ++i)
				{
					m_vecCrowdMeshes[i]->UpdateCrowdColor(pd3dDeviceContext, 0.0f);
					m_vecCrowdMeshes[i]->Render(pd3dDeviceContext);
				}

				//2. ���� �� ������
				set<OBJECT_TYPE> renderedObject;
				OBJECT_TYPE obj_type, another_obj_type;
				for (int i = 0; i < nSelected; ++i)
				{
					obj_type = (OBJECT_TYPE)m_ppSelectedObjects[i]->m_nObjectType;
					if (renderedObject.end() != renderedObject.find(obj_type)) continue; // �̹� �׷ȴ�.

					renderedObject.insert(obj_type);
					int render_obj_type = isEnemyObject((int)obj_type) ? (int)obj_type - ADD_ENEMY_INDEX : (int)obj_type;
					m_mapCrowdUnitTexture.find(OBJECT_TYPE(render_obj_type))->second->UpdateShaderVariable(pd3dDeviceContext);

					for (int j = i; j < nSelected; ++j)
					{
						another_obj_type = (OBJECT_TYPE)m_ppSelectedObjects[j]->m_nObjectType;
						if (obj_type != another_obj_type) continue; //�ٸ� �����̴�.

						float hpColor, hpRatio;
						hpRatio = (float)(m_ppSelectedObjects[j]->m_nHP) / (float)(m_ppSelectedObjects[j]->m_nMaxHP);
						if (hpRatio >= 0.5f) hpColor = 1.0f;
						else if (hpRatio >= 0.25f) hpColor = 2.0f;
						else hpColor = 3.0f;

						m_vecCrowdMeshes[j]->UpdateCrowdColor(pd3dDeviceContext, hpColor);
						m_vecCrowdMeshes[j]->Render(pd3dDeviceContext);
					}
				}
			}
		} // 0 < valid
	}

	//��Ʈ �̹���
	bool bDisplayRequire = false;
	if (m_pNumberFont)
	{
		m_pNumberFont->UpdateNumber(string("screen_mineral"), gGameMineral);
		m_pNumberFont->UpdateNumber(string("screen_gas"), gGameGas);
		
		//ü�� ǥ�� -> 1003 1 ���� ��ŷ����
		if (1 == m_nValidSelectedObjects)
		{
			if((int)OBJECT_TYPE::Mineral == m_ppSelectedObjects[0]->m_nObjectType || (int)OBJECT_TYPE::Gas == m_ppSelectedObjects[0]->m_nObjectType)
				m_pNumberFont->UpdateNumber(string("screen_hp"), INVALIDATE_FONT);
			else
				m_pNumberFont->UpdateNumber(string("screen_hp"), m_ppSelectedObjects[0]->m_nHP);
			m_pNumberFont->Render(pd3dDeviceContext, m_fWndClientWidth, m_fWndClientHeight);
		}
		else
		{
			m_pNumberFont->UpdateNumber(string("screen_hp"), INVALIDATE_FONT);
		}
		
		//���� Ȥ�� ���� ���
		int comPos = 0;
		bool isInCommand = IsCursorInCommand(m_ptMousePosition.x, m_ptMousePosition.y, comPos);
		if (isInCommand)
		{		
			ProcessCursorInCommand(comPos, false, &bDisplayRequire);
		}
		else
		{
			m_pNumberFont->UpdateNumber(string("screen_need_mineral"), INVALIDATE_FONT); 
			m_pNumberFont->UpdateNumber(string("screen_need_gas"), INVALIDATE_FONT);
		}

		//�δ� ���� ���� ǥ��
		char buf[30];
		for (int i = 0; i < 8; ++i)
		{
			sprintf(buf, "crowd_save_image_%d", i + 1);

			if (m_CrowdSaveData[i].nUnits == 0) 
			{
				m_pNumberFont->InvalidateNumber(string(buf));
				continue;
			}

			m_pNumberFont->UpdateNumber(string(buf), i + 1);
		}

		m_pNumberFont->Render(pd3dDeviceContext, m_fWndClientWidth, m_fWndClientHeight);
	}

	//�ڿ� �̹��� ǥ��
	//if (m_nValidSelectedObjects <= 0)
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::attached);
	m_mapCommandTexture.find(string("mineral"))->second->UpdateShaderVariable(pd3dDeviceContext);
	m_mapCommandMesh.find(string("mineral"))->second->Render(pd3dDeviceContext);
	if (bDisplayRequire) m_mapCommandMesh.find(string("need_mineral"))->second->Render(pd3dDeviceContext);
	m_mapCommandTexture.find(string("gas"))->second->UpdateShaderVariable(pd3dDeviceContext);
	m_mapCommandMesh.find(string("gas"))->second->Render(pd3dDeviceContext);
	if (bDisplayRequire) m_mapCommandMesh.find(string("need_gas"))->second->Render(pd3dDeviceContext);


	//���콺 Ŀ�� �̹����� �������� �׷����°� �մ��ҵ�? ���̴��� MainFrame���� ����Ѵ�.
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::mainframe);
	m_pMouseImageTexture->UpdateShaderVariable(pd3dDeviceContext);
	float xScreen, yScreen;
	xScreen = ((float)m_ptMousePosition.x / m_fWndClientWidth) * 2.0f - 1.0f;
	yScreen = ((float)(m_fWndClientHeight - m_ptMousePosition.y) / m_fWndClientHeight) * 2.0f - 1.0f;
	m_pMouseImageMesh->UpdateScreenPosition(pd3dDeviceContext, xScreen, yScreen);
	m_pMouseImageMesh->Render(pd3dDeviceContext);



	//0901 ����� ť�� �׽�Ʈ
#ifdef DEBUG_RENDERING_CUBE
	m_DebugCubeShader.Render(pd3dDeviceContext, nullptr);
#endif


	if (m_bGameEnd)
	{
		OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::attached);
		
		if (m_bGameWin)
			m_pResultImageVictory->UpdateShaderVariable(pd3dDeviceContext);
		else
			m_pResultImageDefeat->UpdateShaderVariable(pd3dDeviceContext);

		m_pResultImageMesh->Render(pd3dDeviceContext);
	}

	if (m_pd3dUIDepthStencilState) pd3dDeviceContext->OMSetDepthStencilState(nullptr, 1);
	//UI Depth ������ ����.
}



void CScene::CreateNewObject(OBJECT_TYPE obj_type, D3DXVECTOR3 vMadenPosition)
{
	if (false == isAnimateObject((int)obj_type))
	{ //�ǹ� ����
		int nodeIndex = m_pAStarComponent->GetNearstNodeIndex(vMadenPosition);

		//�ڿ� ���Ҵ� Put Building ��Ŷ���� ó���ϵ��� �Ѵ�. Fail Build�� ���� �� �ֱ� �����̴�.
		switch (obj_type)
		{
		case OBJECT_TYPE::Commandcenter:
		{
			if (gGameMineral < 400)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 0)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Commandcenter��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Commandcenter);
			}
			break;
		}
		case OBJECT_TYPE::Supplydepot:
		{
			if (gGameMineral < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 0)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Supplydepot��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Supplydepot);
			}
			break;
		}
		case OBJECT_TYPE::Refinery:
		{
			if (gGameMineral < 75)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 0)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				//0903 �����̳ʸ��� ���� ���� �������� �ϴ����� Ȯ���ؾ� �Ѵ�.
				bool bIsValid = false;
				for (auto d : m_listNeutral)
				{
					if ((int)OBJECT_TYPE::Gas != d->m_nObjectType) continue;
					if (!d->GetActive()) continue;
					
					int idx = AStarComponent::GetInstance()->GetNearstNodeIndex(d->GetPosition());
					if (idx == nodeIndex)
					{
						bIsValid = true;
						//d->SetActive(false);
						break;
					}
				}

				if(bIsValid)
				{
					printf("Refinery��(��) �Ǽ��մϴ�.\n");
					CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Refinery);
				}
				else
				{
					printf("���� �����ٸ� ���� �� �ִ�!\n");
					CSoundManager::GetInstance()->PlayEffectSound(start_offset::cannot_build);
				}
			}
			break;
		}
		case OBJECT_TYPE::Barrack:
		{
			if (gGameMineral < 150)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 0)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Barrack��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Barrack);
			}
			break;
		}
		case OBJECT_TYPE::Engineeringbay:
		{
			if (gGameMineral < 125)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 0)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Engineeringbay��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Engineeringbay);
			}
			break;
		}
		case OBJECT_TYPE::Sensortower:
		{
			if (gGameMineral < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 50)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Sensortower��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Sensortower);
			}
			break;
		}
		case OBJECT_TYPE::Missileturret:
		{
			if (gGameMineral < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 0)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Missileturret��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Missileturret);
			}
			break;
		}
		case OBJECT_TYPE::Bunker:
		{
			if (gGameMineral < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 0)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Bunker��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Bunker);
			}
			break;
		}
		case OBJECT_TYPE::Factory:
		{
			if (gGameMineral < 200)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 50)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Factory��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Factory);
			}
			break;
		}
		case OBJECT_TYPE::Starport:
		{
			if (gGameMineral < 150)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Starport��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Starport);
			}
			break;
		}
		case OBJECT_TYPE::Armory:
		{
			if (gGameMineral < 150)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Armory��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Armory);
			}
			break;
		}
		case OBJECT_TYPE::Ghostacademy:
		{
			if (gGameMineral < 150)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 50)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Ghostacademy��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Ghostacademy);
			}
			break;
		}
		case OBJECT_TYPE::Fusioncore:
		{
			if (gGameMineral < 200)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 200)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				printf("Fusioncore��(��) �Ǽ��մϴ�.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Fusioncore);
			}
			break;
		}
		}

		m_pBuildCheck->InvalidateBuildingImage();
		m_nSceneState = SCENE_STATE::pick_scv;
		m_bActivedBuildCheck = false;
	}
	else //���� ����
	{
		CStaticObject* pBuilding = (CStaticObject*)(m_ppSelectedObjects[0]);	//0���� ���� �ǹ��� ���̴�.
		
		switch(obj_type)
		{
		case OBJECT_TYPE::Scv:
		{
			if (gGameMineral < 50)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else
			{
				if(true == pBuilding->GenerateUnit(OBJECT_TYPE::Scv, 5.0f))
				{
					gGameMineral -= 50;
					printf("Scv��(��) �����մϴ�.\n");
					//PushCreateUnitList(OBJECT_TYPE::Scv, vMadenPosition + D3DXVECTOR3(-2, 0, -2), 5.0f);
				}
			}
			break;
		}
		case OBJECT_TYPE::Marine:
		{
			if (gGameMineral < 50)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else
			{
				if (true == pBuilding->GenerateUnit(OBJECT_TYPE::Marine, 5.0f))
				{
					gGameMineral -= 50;
					printf("Marine��(��) �����մϴ�.\n");
					//PushCreateUnitList(OBJECT_TYPE::Marine, vMadenPosition + D3DXVECTOR3(-2, 0, -2), 5.0f);
				}
			}
			break;
		}
		case OBJECT_TYPE::Reaper:
		{
			if (gGameMineral < 50)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if(gGameGas < 50)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				if (true == pBuilding->GenerateUnit(OBJECT_TYPE::Reaper, 5.0f))
				{
					gGameMineral -= 50; gGameGas -= 50;
					printf("Reaper��(��) �����մϴ�.\n");
					//PushCreateUnitList(OBJECT_TYPE::Reaper, vMadenPosition + D3DXVECTOR3(-2, 0, -2), 5.0f);
				}
			}
			break;
		}
		case OBJECT_TYPE::Marauder:
		{
			if (gGameMineral < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 25)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				if (true == pBuilding->GenerateUnit(OBJECT_TYPE::Marauder, 5.0f))
				{
					gGameMineral -= 100; gGameGas -= 25;
					printf("Marauder��(��) �����մϴ�.\n");
					//PushCreateUnitList(OBJECT_TYPE::Marauder, vMadenPosition + D3DXVECTOR3(-2, 0, -2), 5.0f);
				}
			}
			break;
		}
		case OBJECT_TYPE::Thor:
		{
			if (gGameMineral < 300)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 200)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				if (true == pBuilding->GenerateUnit(OBJECT_TYPE::Thor, 5.0f))
				{
					gGameMineral -= 300; gGameGas -= 200;
					printf("Thor��(��) �����մϴ�.\n");
					//PushCreateUnitList(OBJECT_TYPE::Thor, vMadenPosition + D3DXVECTOR3(-2, 0, -2), 5.0f);
				}
			}
			break;
		}
		case OBJECT_TYPE::Viking:
		{
			if (gGameMineral < 150)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 75)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				if (true == pBuilding->GenerateUnit(OBJECT_TYPE::Viking, 5.0f))
				{
					gGameMineral -= 150; gGameGas -= 75;
					printf("Viking��(��) �����մϴ�.\n");
					//PushCreateUnitList(OBJECT_TYPE::Viking, vMadenPosition + D3DXVECTOR3(-2, 0, -2), 5.0f);
				}
			}
			break;
		}
		case OBJECT_TYPE::Medivac:
		{
			if (gGameMineral < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				if (true == pBuilding->GenerateUnit(OBJECT_TYPE::Medivac, 5.0f))
				{
					gGameMineral -= 100; gGameGas -= 100;
					printf("Medivac��(��) �����մϴ�.\n");
					//PushCreateUnitList(OBJECT_TYPE::Medivac, vMadenPosition + D3DXVECTOR3(-2, 0, -2), 5.0f);
				}
			}
			break;
		}
		case OBJECT_TYPE::Banshee:
		{
			if (gGameMineral < 150)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				if (true == pBuilding->GenerateUnit(OBJECT_TYPE::Banshee, 5.0f))
				{
					gGameMineral -= 150; gGameGas -= 100;
					printf("Banshee��(��) �����մϴ�.\n");
					//PushCreateUnitList(OBJECT_TYPE::Banshee, vMadenPosition + D3DXVECTOR3(-2, 0, -2), 5.0f);
				}
			}
			break;
		}
		case OBJECT_TYPE::Raven:
		{
			if (gGameMineral < 100)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 200)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				if (true == pBuilding->GenerateUnit(OBJECT_TYPE::Raven, 5.0f))
				{
					gGameMineral -= 100; gGameGas -= 200;
					printf("Raven��(��) �����մϴ�.\n");
					//PushCreateUnitList(OBJECT_TYPE::Raven, vMadenPosition + D3DXVECTOR3(-2, 0, -2), 5.0f);
				}
			}
			break;
		}
		case OBJECT_TYPE::BattleCruiser:
		{
			if (gGameMineral < 400)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
			else if (gGameGas < 300)
				CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
			else
			{
				if (true == pBuilding->GenerateUnit(OBJECT_TYPE::BattleCruiser, 5.0f))
				{
					gGameMineral -= 400; gGameGas -= 300;
					printf("BattleCruiser��(��) �����մϴ�.\n");
					//PushCreateUnitList(OBJECT_TYPE::BattleCruiser, vMadenPosition + D3DXVECTOR3(-2, 0, -2), 5.0f);
				}
			}
			break;
		}
		}//end switch
	}
}


/*
void CScene::PushCreateUnitList(OBJECT_TYPE obj_type, D3DXVECTOR3 pos, float time)
{
	m_listCreateUnits.push_back(CREATE_UNIT_DATA{ obj_type, pos, time });
}


void CScene::UpdateCreateUnitList(float elapsedTime)
{
	for (auto&d : m_listCreateUnits)
		d.time -= elapsedTime;
	
	if (!m_listCreateUnits.empty() && m_listCreateUnits.front().time <= 0.0f)
	{
		CS_Packet_Create_Unit(m_listCreateUnits.front().obj_type, m_listCreateUnits.front().position);
		m_listCreateUnits.pop_front();
	}
}
*/