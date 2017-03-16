#include "stdafx.h"
#include "Scene.h"

//패킷 통신용
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

	//마우스 이벤트
	m_pMouseRectTexture = nullptr;
	m_pMouseRectMesh = nullptr;

	m_pd3dMouseRectVertexShader = nullptr;
	m_pd3dMouseRectVertexLayout = nullptr;
	m_pd3dMouseRectPixelShader = nullptr;

	m_bLeftClicked = false;
	m_bMouseRectDraw = false;

	//선택된 객체 선별 이벤트
	m_pSelectedCircleTexture = nullptr;
	m_pSelectedCircleMesh = nullptr;
	m_pd3dSelectedCircleVertexShader = nullptr;
	m_pd3dSelectedCircleVertexLayout = nullptr;
	m_pd3dSelectedCircleGeometryShader = nullptr;
	m_pd3dSelectedCirclePixelShader = nullptr;

	//UI 관련 이미지 map
	m_mapCommandTexture.clear();
	m_mapPortraitTexture.clear();
	m_mapStatusTexture.clear();

	m_mapCommandMesh.clear();

	m_nValidSelectedObjects = 0;
	for (auto i = 0; i < MAX_SELECTED_OBJECTS; ++i)
		m_ppSelectedObjects[i] = nullptr;

	m_pStaticShader = nullptr;
	m_pAnimationShader = nullptr;

	//길찾기
	m_pAStarComponent = nullptr;

	//디버그용
	m_pDebugPrograms = nullptr;

	//파티클
	m_pParticleBox = nullptr;

	//사운드
	m_pSoundManager = nullptr;

	//폰트
	m_pNumberFont = nullptr;

	//건물 짓기
	m_pBuildCheck = nullptr;

	//커맨드 창 위치
	m_mapCommandPosition.clear();
	
	//군중 유닛 표시 위치
	m_mapCrowdPosition.clear();

	//프로그레스 바
	m_pProgressIconMesh = nullptr;
	m_pProgressImage = nullptr;

	//마우스 이미지
	m_pMouseImageTexture = nullptr;
	m_pMouseImageMesh = nullptr;

	//군중 유닛 이미지
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

	//HeightMapTerrain 소멸자에서 진행
	//if (m_pHeightMap) delete m_pHeightMap;

	if (m_pd3dUIDepthStencilState) m_pd3dUIDepthStencilState->Release();

	//마우스 드래그
	if (m_pMouseRectMesh) m_pMouseRectMesh->Release();
	if (m_pMouseRectTexture) m_pMouseRectTexture->Release();
	if (m_pd3dMouseRectVertexShader) m_pd3dMouseRectVertexShader->Release();
	if (m_pd3dMouseRectVertexLayout) m_pd3dMouseRectVertexLayout->Release();
	if (m_pd3dMouseRectPixelShader)  m_pd3dMouseRectPixelShader->Release();

	//선택된 오브젝트들
	if (m_pSelectedCircleTexture) m_pSelectedCircleTexture->Release();
	if (m_pSelectedCircleMesh) m_pSelectedCircleMesh->Release();
	if (m_pd3dSelectedCircleVertexShader) m_pd3dSelectedCircleVertexShader->Release();
	if (m_pd3dSelectedCircleVertexLayout) m_pd3dSelectedCircleVertexLayout->Release();
	if (m_pd3dSelectedCircleGeometryShader) m_pd3dSelectedCircleGeometryShader->Release();
	if (m_pd3dSelectedCirclePixelShader) m_pd3dSelectedCirclePixelShader->Release();

	//UI 메인 프레임
	if (m_pMainFrameTexture) m_pMainFrameTexture->Release();
	if (m_pMainFrameMesh) m_pMainFrameMesh->Release();
	if (m_pd3dMainFrameVertexShader) m_pd3dMainFrameVertexShader->Release();
	if (m_pd3dMainFrameVertexLayout) m_pd3dMainFrameVertexLayout->Release();
	if (m_pd3dMainFramePixelShader) m_pd3dMainFramePixelShader->Release();

	//AttachedMainFrame
	if (m_pd3dAttachedMainFrameVertexShader) m_pd3dAttachedMainFrameVertexShader->Release();
	if (m_pd3dAttachedMainFrameVertexLayout) m_pd3dAttachedMainFrameVertexLayout->Release();
	if (m_pd3dAttachedMainFramePixelShader) m_pd3dAttachedMainFramePixelShader->Release();

	//UI 미니맵
	if (m_pMiniMapTexture) m_pMiniMapTexture->Release();
	if (m_pMiniMapMesh) m_pMiniMapMesh->Release();
	if (m_pd3dMiniMapVertexShader) m_pd3dMiniMapVertexShader->Release();
	if (m_pd3dMiniMapVertexLayout) m_pd3dMiniMapVertexLayout->Release();
	if (m_pd3dMiniMapPixelShader) m_pd3dMiniMapPixelShader->Release();

	

	//미니맵 객체
	if (m_pMiniMapObjectMesh) m_pMiniMapObjectMesh->Release();
	if (m_pd3dMiniMapObjectVertexShader) m_pd3dMiniMapObjectVertexShader->Release();
	if (m_pd3dMiniMapObjectVertexLayout) m_pd3dMiniMapObjectVertexLayout->Release();
	if (m_pd3dMiniMapObjectGeometryShader) m_pd3dMiniMapObjectGeometryShader->Release();
	if (m_pd3dMiniMapObjectPixelShader) m_pd3dMiniMapObjectPixelShader->Release();

	//미니맵 사각형
	if (m_pMiniMapRectTexture) m_pMiniMapRectTexture->Release();
	if (m_pMiniMapRectMesh) m_pMiniMapRectMesh->Release();
	if (m_pd3dMiniMapRectVertexShader) m_pd3dMiniMapRectVertexShader->Release();
	if (m_pd3dMiniMapRectPixelShader) m_pd3dMiniMapRectPixelShader->Release();
	if (m_pd3dMiniMapRectVertexLayout) m_pd3dMiniMapRectVertexLayout->Release();
	
	//scv가 채취한 중립 자원
	if (m_pNeutralResourceTexture) m_pNeutralResourceTexture->Release();
	if (m_pNeutralResourceMesh) m_pNeutralResourceMesh->Release();
	if (m_pd3dNeutralResourceVertexShader) m_pd3dNeutralResourceVertexShader->Release();
	if (m_pd3dNeutralResourcePixelShader) m_pd3dNeutralResourcePixelShader->Release();
	if (m_pd3dNeutralResourceVertexLayout) m_pd3dNeutralResourceVertexLayout->Release();

	//UI 관련 이미지
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

	//길찾기
	if (m_pAStarComponent)
		//	delete m_pAStarComponent;
		m_pAStarComponent->DeleteInstance();
	//디버그
	if(m_pDebugPrograms)
		delete m_pDebugPrograms;

	//안개 상수버퍼
	if (m_pd3dcbFogBuffer) m_pd3dcbFogBuffer->Release();

	//안개 버퍼 리소스
	if (m_pd3dFogBuffer) m_pd3dFogBuffer->Release();
	if (m_pd3dSRVFog) m_pd3dSRVFog->Release();

	//객체 관리 리스트
	m_listMy.clear();
	m_listEnemy.clear();
	m_listEnemyInSight.clear();

	//파티클 인스턴스 해제
	if(m_pParticleBox) m_pParticleBox->DeleteInstance();

	//사운드 인스턴스 해제
	if (m_pSoundManager) 
	{
		m_pSoundManager->ReleaseSound();
		m_pSoundManager->DeleteInstance();
	}

	//폰트 인스턴스 해제
	if (m_pNumberFont) m_pNumberFont->DeleteInstance();

	//건물 짓기
	if (m_pBuildCheck) m_pBuildCheck->DeleteInstance();

	//커맨드 창 위치
	m_mapCommandPosition.clear();

	//군중 유닛 표시 위치
	m_mapCrowdPosition.clear();

	//프로그레스 바
	if (m_pProgressIconMesh) m_pProgressIconMesh->Release();
	if (m_pProgressImage) delete m_pProgressImage;

	//마우스 이미지
	if(m_pMouseImageTexture) m_pMouseImageTexture->Release();
	if (m_pMouseImageMesh) m_pMouseImageMesh->Release();

	//군중 유닛 표시
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

	//승패처리
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
//조명 관련 함수의 나열
void CScene::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	//게임 월드 전체를 비추는 주변조명을 설정한다.
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);

	//방향성 조명
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
	//test 방향성 조명 이동
	int num = rand() % 4;
	D3DXVECTOR3 sunDirection;
	switch (num)
	{
	case 0: // 동쪽에 태양이 있는
		sunDirection = D3DXVECTOR3(-0.707f, -0.707f, 0.0f);
		break;
	case 1: // 서쪽 to 동쪽
		sunDirection = D3DXVECTOR3(+0.707f, -0.707f, 0.0f);
		break;
	case 2: // 남쪽 to 북쪽
		sunDirection = D3DXVECTOR3(0.0f, -0.707f, +0.707f);
		break;
	case 3: // 북쪽 to 남쪽
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
//UI 관련 Shader 세팅 처리
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
//객체 관리
void CScene::LoadUIImage(ID3D11Device* pd3dDevice, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState)
{
	CTexture *pTexture = nullptr;
	/////////////////////////////////////////////////////////////////
	//커맨드 창 Attack
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/attack.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("attack", pTexture));
	pTexture->AddRef();

	//커맨드 창 Stop
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/stop.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("stop", pTexture));
	pTexture->AddRef();

	//커맨드 창 Move
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/move.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("move", pTexture));
	pTexture->AddRef();

	//커맨드 창 Patrol
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/patrol.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("patrol", pTexture));
	pTexture->AddRef();

	//커맨드 창 Hold
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/hold.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("hold", pTexture));
	pTexture->AddRef();

#ifndef DEBUG_SIMPLE_LOAD
	//커맨드 창 mineral
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/mineral.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("mineral", pTexture));
	pTexture->AddRef();
	
	//커맨드 창 gas
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/gas.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("gas", pTexture));
	pTexture->AddRef();

	//커맨드 창 scv
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/scv.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("scv", pTexture));
	pTexture->AddRef();

	//커맨드 창 rally
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/rally.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("rally", pTexture));
	pTexture->AddRef();

	//커맨드 창 build
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/build.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("build", pTexture));
	pTexture->AddRef();

	//커맨드 창 buildAdv
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/buildadv.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("buildadv", pTexture));
	pTexture->AddRef();

	//커맨드 창 repair
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/repair.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("repair", pTexture));
	pTexture->AddRef();

	//커맨드 창 grin
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/grin.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("grin", pTexture));
	pTexture->AddRef();

	//커맨드 창 addattack
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/addattack.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("addattack", pTexture));
	pTexture->AddRef();

	//커맨드 창 addshield
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/addshield.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("addshield", pTexture));
	pTexture->AddRef();

	//커맨드 창 scope
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/scope.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("scope", pTexture));
	pTexture->AddRef();

	//커맨드 창 addbuildingshield
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/addbuildingshield.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("addbuildingshield", pTexture));
	pTexture->AddRef();

	//커맨드 창 bunkerup
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/bunkerup.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("bunkerup", pTexture));
	pTexture->AddRef();

	//커맨드 창 sensor
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/sensor.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("sensor", pTexture));
	pTexture->AddRef();

	//커맨드 창 commandcenter
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/commandcenter.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("commandcenter", pTexture));
	pTexture->AddRef();

	//커맨드 창 refinery
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/refinery.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("refinery", pTexture));
	pTexture->AddRef();

	//커맨드 창 supplydepot
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/supplydepot.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("supplydepot", pTexture));
	pTexture->AddRef();

	//커맨드 창 barrack
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/barrack.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("barrack", pTexture));
	pTexture->AddRef();

	//커맨드 창 engineeringbay
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/engineeringbay.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("engineeringbay", pTexture));
	pTexture->AddRef();

	//커맨드 창 bunker
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/bunker.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("bunker", pTexture));
	pTexture->AddRef();

	//커맨드 창 missileturret
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/missileturret.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("missileturret", pTexture));
	pTexture->AddRef();

	//커맨드 창 sensortower
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/sensortower.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("sensortower", pTexture));
	pTexture->AddRef();

	//커맨드 창 cancel
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/cancel.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("cancel", pTexture));
	pTexture->AddRef();

	//커맨드 창 marineshield
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/marineshield.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("marineshield", pTexture));
	pTexture->AddRef();

	//커맨드 창 steampack
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/steampack.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("steampack", pTexture));
	pTexture->AddRef();

	//커맨드 창 quake
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/quake.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("quake", pTexture));
	pTexture->AddRef();

	//커맨드 창 jump
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/jump.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("jump", pTexture));
	pTexture->AddRef();

	//커맨드 창 clock
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/cloak.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("cloak", pTexture));
	pTexture->AddRef();

	//커맨드 창 sniper
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/sniper.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("sniper", pTexture));
	pTexture->AddRef();

	//커맨드 창 emp
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/emp.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("emp", pTexture));
	pTexture->AddRef();

	//커맨드 창 marine
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/marine.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("marine", pTexture));
	pTexture->AddRef();

	//커맨드 창 reaper
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/reaper.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("reaper", pTexture));
	pTexture->AddRef();

	//커맨드 창 marauder
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/marauder.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("marauder", pTexture));
	pTexture->AddRef();

	//커맨드 창 ghost
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/ghost.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("ghost", pTexture));
	pTexture->AddRef();

	//커맨드 창 addmecaattack
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/addmecaattack.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("addmecaattack", pTexture));
	pTexture->AddRef();

	//커맨드 창 addmecashield
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/addmecashield.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("addmecashield", pTexture));
	pTexture->AddRef();

	//커맨드 창 widowmine
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/widowmine.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("widowmine", pTexture));
	pTexture->AddRef();

	//커맨드 창 hellion
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/hellion.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("hellion", pTexture));
	pTexture->AddRef();

	//커맨드 창 siegetank
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/siegetank.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("siegetank", pTexture));
	pTexture->AddRef();

	//커맨드 창 thor
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/thor.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("thor", pTexture));
	pTexture->AddRef();

	//커맨드 창 ghostacademy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/ghostacademy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("ghostacademy", pTexture));
	pTexture->AddRef();

	//커맨드 창 factory
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/factory.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("factory", pTexture));
	pTexture->AddRef();

	//커맨드 창 armory
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/armory.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("armory", pTexture));
	pTexture->AddRef();

	//커맨드 창 starport
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/starport.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("starport", pTexture));
	pTexture->AddRef();

	//커맨드 창 fusioncore
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/fusioncore.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("fusioncore", pTexture));
	pTexture->AddRef();

	//커맨드 창 siegemode
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/siegemode.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("siegemode", pTexture));
	pTexture->AddRef();

	//커맨드 창 yamatore
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/yamatore.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("yamatore", pTexture));
	pTexture->AddRef();

	//커맨드 창 yamatoenergy
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/yamatoenergy.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("yamatoenergy", pTexture));
	pTexture->AddRef();

	//커맨드 창 heal
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/heal.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("heal", pTexture));
	pTexture->AddRef();

	//커맨드 창 burnner
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/burnner.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("burnner", pTexture));
	pTexture->AddRef();

	//커맨드 창 tacticaljump
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/tacticaljump.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("tacticaljump", pTexture));
	pTexture->AddRef();

	//커맨드 창 banshee
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/banshee.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("banshee", pTexture));
	pTexture->AddRef();

	//커맨드 창 medivac
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/medivac.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("medivac", pTexture));
	pTexture->AddRef();

	//커맨드 창 raven
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/raven.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("raven", pTexture));
	pTexture->AddRef();

	//커맨드 창 battlecruiser
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/battlecruiser.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("battlecruiser", pTexture));
	pTexture->AddRef();

	//커맨드 창 viking
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Command/viking.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapCommandTexture.insert(make_pair("viking", pTexture));
	pTexture->AddRef();

#endif // DEBUG_SIMPLE_LOAD

	/////////////////////////////////////////////////////////////////
	//마린 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/marine_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Marine, pTexture));
	pTexture->AddRef();

	//사신 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/reaper_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Reaper, pTexture));
	pTexture->AddRef();

	//불곰 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/marauder_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Marauder, pTexture));
	pTexture->AddRef();

	//유령 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/ghost_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Ghost, pTexture));
	pTexture->AddRef();

#ifndef DEBUG_SIMPLE_LOAD
	//SCV 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/scv_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Scv, pTexture));
	pTexture->AddRef();

	//지게로봇 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/mule_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Mule, pTexture));
	pTexture->AddRef();

	//토르 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/thor_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Thor, pTexture));
	pTexture->AddRef();

	/*
	//땅거미지뢰 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/widowmine_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::, pTexture));
	pTexture->AddRef();
	*/

	//탱크 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/siegetank_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Siegetank, pTexture));
	pTexture->AddRef();

	//화염차 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/hellion_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Hellion, pTexture));
	pTexture->AddRef();

	//의료선 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/medivac_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Medivac, pTexture));
	pTexture->AddRef();

	//바이킹 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/viking_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Viking, pTexture));
	pTexture->AddRef();

	//밤까마귀 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/raven_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Raven, pTexture));
	pTexture->AddRef();

	//밴시 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/banshee_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Banshee, pTexture));
	pTexture->AddRef();

	//전투순양함 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/battlecruiser_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::BattleCruiser, pTexture));
	pTexture->AddRef();

	//건물 초상화
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Portrait/building_portrait.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapPortraitTexture.insert(make_pair((int)OBJECT_TYPE::Commandcenter, pTexture));
	pTexture->AddRef();

#endif // DEBUG_SIMPLE_LOAD

	/////////////////////////////////////////////////////////////////
	//마린 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/marine_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Marine, pTexture));
	pTexture->AddRef();

	//사신 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/reaper_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Reaper, pTexture));
	pTexture->AddRef();

	//불곰 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/marauder_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Marauder, pTexture));
	pTexture->AddRef();

	//유령 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/ghost_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Ghost, pTexture));
	pTexture->AddRef();

#ifndef DEBUG_SIMPLE_LOAD

	//Thor 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Thor_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Thor, pTexture));
	pTexture->AddRef();

	/*
	// Widowmine 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Widowmine_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Widowmine, pTexture));
	pTexture->AddRef();
	*/

	// Scv 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Scv_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Scv, pTexture));
	pTexture->AddRef();

	// Mule 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Mule_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Mule, pTexture));
	pTexture->AddRef();

	// Siegetank 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Siegetank_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Siegetank, pTexture));
	pTexture->AddRef();

	// Hellion 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Hellion_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Hellion, pTexture));
	pTexture->AddRef();

	// Medivac 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Medivac_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Medivac, pTexture));
	pTexture->AddRef();

	// Viking 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Viking_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Viking, pTexture));
	pTexture->AddRef();

	// Raven 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Raven_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Raven, pTexture));
	pTexture->AddRef();

	// Banshee 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Banshee_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Banshee, pTexture));
	pTexture->AddRef();

	// Battlecruiser 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Battlecruiser_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::BattleCruiser, pTexture));
	pTexture->AddRef();

	// Commandcenter 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Commandcenter_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Commandcenter, pTexture));
	pTexture->AddRef();

	// Supplydepot 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Supplydepot_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Supplydepot, pTexture));
	pTexture->AddRef();

	// Engineeringbay 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Engineeringbay_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Engineeringbay, pTexture));
	pTexture->AddRef();

	// Refinery 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Refinery_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Refinery, pTexture));
	pTexture->AddRef();

	// Missileturret 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Missileturret_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Missileturret, pTexture));
	pTexture->AddRef();

	// Techlab 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Techlab_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Techlab, pTexture));
	pTexture->AddRef();

	// Ghostacademy 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Ghostacademy_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Ghostacademy, pTexture));
	pTexture->AddRef();

	// Barrack 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Barrack_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Barrack, pTexture));
	pTexture->AddRef();

	// Armory 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Armory_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Armory, pTexture));
	pTexture->AddRef();

	// Factory 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Factory_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Factory, pTexture));
	pTexture->AddRef();

	// Fusioncore 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Fusioncore_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Fusioncore, pTexture));
	pTexture->AddRef();

	// Starport 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Starport_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Starport, pTexture));
	pTexture->AddRef();

	// Bunker 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Bunker_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Bunker, pTexture));
	pTexture->AddRef();

	// Reactor 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Reactor_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Reactor, pTexture));
	pTexture->AddRef();

	// Sensortower 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Sensortower_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Sensortower, pTexture));
	pTexture->AddRef();

	// Mineral 상태창
	pTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Status/Mineral_status.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTexture->SetTexture(0, pd3dsrvTexture);
	pTexture->SetSampler(0, pd3dSamplerState);
	m_mapStatusTexture.insert(make_pair((int)OBJECT_TYPE::Mineral, pTexture));
	pTexture->AddRef();

	// Gas 상태창
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
	//투사체들
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

	//즉발들
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
	printf("게임 이미지를 불러오고 있습니다...\n");

	//기본 샘플러 옵션 설정
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

	//텍스쳐 리소스를 생성한다.
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;

	//테셀레이션 맵 diffuse
	CTexture *pTSTerrainTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Terrain/mapama_160160.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	//D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Terrain/mapama_160160_detail.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pTSTerrainTexture->SetTexture(0, pd3dsrvTexture);
	pTSTerrainTexture->SetSampler(0, pd3dSamplerState);

	


	//마우스 드래그 사각형 이미지
	CTexture *pMouseRectTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/testMouseRect.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pMouseRectTexture->SetTexture(0, pd3dsrvTexture);
	pMouseRectTexture->SetSampler(0, pd3dSamplerState);

	//선택된 오브젝트들의 표시 이미지
	CTexture *pSelectedCircleTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/testSelectedCircle.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pSelectedCircleTexture->SetTexture(0, pd3dsrvTexture);
	pSelectedCircleTexture->SetSampler(0, pd3dSamplerState);

	//메인 프레임 이미지
	CTexture *pMainFrameTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/testMainFrame.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pMainFrameTexture->SetTexture(0, pd3dsrvTexture);
	pMainFrameTexture->SetSampler(0, pd3dSamplerState);

	//미니맵 이미지
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
	
	//마우스 커서 이미지
	CTexture *pMouseImageTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/mouse_image.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pMouseImageTexture->SetTexture(0, pd3dsrvTexture);
	pMouseImageTexture->SetSampler(0, pd3dSamplerState);

	//status에 그려질 군중 유닛 이미지
	CTexture *pCrowdFrameTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/unit_frame.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pCrowdFrameTexture->SetTexture(0, pd3dsrvTexture);
	pCrowdFrameTexture->SetSampler(0, pd3dSamplerState);
	
	LoadCrowdUnitImage(pd3dDevice, pd3dsrvTexture, pd3dSamplerState);

	//status창에 붙는 저장된 군중 유닛 확인 이미지
	CTexture *pCrowdAttachedFrameTexture = new CTexture(1, 1, PS_SLOT_TEXTURE, PS_SLOT_SAMPLER);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/UI/Crowd/crowd_AttachMainFrame.png"), NULL, NULL, &pd3dsrvTexture, NULL);
	pCrowdAttachedFrameTexture->SetTexture(0, pd3dsrvTexture);
	pCrowdAttachedFrameTexture->SetSampler(0, pd3dSamplerState);
	

	printf("UI 이미지를 불러오고 있습니다...\n");
	LoadUIImage(pd3dDevice, pd3dsrvTexture, pd3dSamplerState);

	pd3dsrvTexture->Release();
	pd3dSamplerState->Release();

	printf("지형을 불러오고 있습니다...\n");
	//지형 재질은 그냥 여기서 만들어준다...
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
	

	//높이맵 표현을 위한 HeightMap 생성
	m_pHeightMap = pTerrainShader->GetTerrain()->GetHeightMap();
	//CreateHeightMap(_T("Assets/Image/Terrain/mapama_160160_height_raw_205205.RAW"), MAP_WIDTH, MAP_HEIGHT, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	
	
	CTSTerrainShader* pTSTerrainShader = new CTSTerrainShader();
	pTSTerrainShader->CreateShader(pd3dDevice);
	pTSTerrainShader->BuildObjects(pd3dDevice);
	pTSTerrainShader->SetTexture(pTSTerrainTexture);
	pTSTerrainShader->SetMaterial(mat);
	

	printf("유닛 정보를 불러오고 있습니다...\n");
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
	
	//아군 유닛 샘플
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

	//적군 유닛 샘플
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


	//아군 건물 샘플
	put.x = 4.0f * 8; put.z = 2.0f;
	for (int i = 0; i < 15; ++i)
	{
		put.x -= 4.0f; put.y = 1.0f; put.z = put.z;
		put.object_type = (OBJECT_TYPE)((int)OBJECT_TYPE::Armory + i);
		Process_SC_Packet_Put_Object(&put);
	}

	//적군 건물 샘플
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

	//아군 유닛 샘플
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

	//적군 유닛 샘플
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

	//스카이박스 , 지형
	m_nShaders = 3;
	m_ppShaders = new CShader*[m_nShaders];
	m_ppShaders[0] = new CSkyBoxShader();
	m_ppShaders[0]->CreateShader(pd3dDevice);
	m_ppShaders[0]->BuildObjects(pd3dDevice);
	m_ppShaders[1] = pTSTerrainShader;
	m_ppShaders[2] = pTerrainShader;
	
	printf("UI 정보를 불러오고 있습니다...\n");
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
	
	//0905 중립자원 들고있기
	CreateNeutralResourceShader(pd3dDevice);
	CNeutralResourceMesh* pNeutralResourceMesh = new CNeutralResourceMesh(pd3dDevice);
	m_pNeutralResourceMesh = pNeutralResourceMesh;
	pNeutralResourceMesh->AddRef();
	m_pNeutralResourceTexture = pNeutralResourceTexture;
	pNeutralResourceTexture->AddRef();


	//UI AttachedMainFrame
	CreateAttachedMainFrameShader(pd3dDevice);

	//Command					//좌상단부터 1~5, 한 칸 아래 6~10, 맨 아래칸 11~15로 한다.
	//0815 Command Update
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>& commandPosBox = m_mapCommandPosition;

	//1 번의 시작 위치와 가로 세로 폭
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

	//자원 스크린 메시
	pCommandMesh = new CScreenMesh(pd3dDevice, D3DXVECTOR2(SCREEN_POSITION_MINERAL_X, SCREEN_POSITION_MINERAL_Y), 
		D3DXVECTOR2(SCREEN_POSITION_MINERAL_X + 0.05f, SCREEN_POSITION_MINERAL_Y + 0.05f));
	m_mapCommandMesh.insert(make_pair("mineral", pCommandMesh));
	pCommandMesh->AddRef();

	pCommandMesh = new CScreenMesh(pd3dDevice, D3DXVECTOR2(SCREEN_POSITION_GAS_X, SCREEN_POSITION_GAS_Y),
		D3DXVECTOR2(SCREEN_POSITION_GAS_X + 0.05f, SCREEN_POSITION_GAS_Y + 0.05f));
	m_mapCommandMesh.insert(make_pair("gas", pCommandMesh));
	pCommandMesh->AddRef();
	//커맨드 창 마우스 이동 시 필요 자원 스크린 메시	-> 아래 필요 자원 폰트에서 메시만듬


	//Status
	D3DXVECTOR2 vStatusLB = D3DXVECTOR2(-1.0f + (2.0f * 0.1963f), -1.0f + (0.6f * 0.0707f));
	D3DXVECTOR2 vStatusRT = D3DXVECTOR2(-1.0f + (2.0f * 0.7005f), -1.0f + (0.6f * 0.5369f));
	float vStatusWidth = vStatusRT.x - vStatusLB.x;
	float vStatusHeight = vStatusRT.y - vStatusLB.y;
	m_pStatusMesh = new CScreenMesh(pd3dDevice, vStatusLB + D3DXVECTOR2(vStatusWidth / 3.0f,0.0f)
		, vStatusLB + D3DXVECTOR2((vStatusWidth / 3.0f) * 2.0f, vStatusRT.y - vStatusLB.y ));
	m_pStatusMesh->AddRef();


	//Status창에 표시 할 군중 유닛들
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>& crowdPosBox = m_mapCrowdPosition;

	//1 번의 시작 위치와 가로 세로 폭
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


	//UI 렌더링을 위한 깊이값을 비교,쓰기 하지 않는
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

	printf("조명을 불러오고 있습니다...\n");
	//조명 및 조명 상수버퍼 생성
	CreateShaderVariables(pd3dDevice);

	printf("게임 AI를 불러오고 있습니다...\n");
	//길찾기 그래프의 초기화
	m_pAStarComponent = AStarComponent::GetInstance();
	m_pAStarComponent->InitializeGraph(MAP_WIDTH);	//정사각형 맵임

	printf("그림자 및 안개를 불러오고 있습니다...\n");
	//그림자 셰이더
	m_pStaticShader->CreateShadowShader(pd3dDevice);
	m_pAnimationShader->CreateShadowShader(pd3dDevice);

	//안개 상수버퍼 생성
	BuildFogBuffer(pd3dDevice);

	printf("이펙트를 불러오고 있습니다...\n");
	//파티클 초기화 및 이미지 로딩
	m_pParticleBox = CSpriteParticle::GetInstance();
	m_pParticleBox->BuildObject(pd3dDevice);
	m_pParticleBox->CreateShader(pd3dDevice);
	LoadParticleImage(pd3dDevice);

	printf("게임 사운드를 불러오고 있습니다...\n");
	//사운드 초기화 및 로드
	m_pSoundManager = CSoundManager::GetInstance();
	string fileList = "SoundComponent/SoundList.txt";
	m_pSoundManager->CreateSound(fileList);
	srand(GetTickCount());
	m_pSoundManager->PlayBackgroundSound();
	m_pSoundManager->SetVolume(BACKGROUND_CHANNEL, 1.0f);

	printf("폰트 이미지를 불러오고 있습니다...\n");
	//폰트 초기화
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

	//커맨드 창 마우스 이동 시 필요 자원 스크린 메시
	posColor.x -= 0.16f; posColor.y -= 0.013f;
	pCommandMesh = new CScreenMesh(pd3dDevice, D3DXVECTOR2(posColor.x, posColor.y), D3DXVECTOR2(posColor.x + 0.03f, posColor.y + 0.03f));
	m_mapCommandMesh.insert(make_pair("need_mineral", pCommandMesh));
	pCommandMesh->AddRef();
	posColor.x += 0.16f;
	pCommandMesh = new CScreenMesh(pd3dDevice, D3DXVECTOR2(posColor.x, posColor.y), D3DXVECTOR2(posColor.x + 0.03f, posColor.y + 0.03f));
	m_mapCommandMesh.insert(make_pair("need_gas", pCommandMesh));
	pCommandMesh->AddRef();
	
	//건물 짓기
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

	// 0826 프로그레스 바
	D3DXVECTOR2 progressIconLB{ -1.0f + (2 * 0.4365f), -1.0f + (2 * 0.1067f) };
	D3DXVECTOR2 progressIconRT{ -1.0f + (2 * 0.4365f) + 0.05f, -1.0f + (2 * 0.1067f) + 0.05f };
	m_pProgressIconMesh = new CScreenMesh(pd3dDevice, progressIconLB, progressIconRT);

	m_pProgressImage = new CProgressImage();
	m_pProgressImage->CreateShader(pd3dDevice);
	m_pProgressImage->CreateResource(pd3dDevice);
	m_pProgressImage->m_d3dxvLeftBottom = D3DXVECTOR3(progressIconLB.x + 0.1f, progressIconLB.y, 0.0f);
	m_pProgressImage->m_d3dxvRightTop = D3DXVECTOR3(progressIconRT.x + 0.3f, progressIconRT.y, 0.0f);

#ifdef DEBUG_RENDERING_GRID
	//디버그 직선
	m_pDebugPrograms = new CDebugPrograms(pd3dDevice, m_pHeightMap);
#endif


	//0901 큐브 테스트
	m_DebugCubeShader.BuildObjects(pd3dDevice);
	m_DebugCubeShader.CreateShader(pd3dDevice);

	/*
	//0901 중립 유닛 -> 일단 클라 자체에서 바로 push한다.
	for (int i = m_pStaticShader->m_vectorObjects.size() - 1; i >= 0; --i)
	{
		if (!(OBJECT_TYPE::Mineral == (OBJECT_TYPE)m_pStaticShader->m_vectorObjects[i]->m_ObjectType || OBJECT_TYPE::Gas == (OBJECT_TYPE)m_pStaticShader->m_vectorObjects[i]->m_ObjectType))
			continue;
		
		for (int j = 0; j < m_pStaticShader->m_vectorObjects[i]->m_nValidObjects; ++j)
			m_listNeutral.push_back(m_pStaticShader->m_vectorObjects[i]->m_ppObjects[j]);
	}
	*/

	//1206 테크트리 적용
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

	//승패처리 이미지
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
	//end 승패처리

	printf("로딩 완료. 게임에 진입합니다...\n");
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

//유닛의 바운딩박스와 공중여부까지 따져준다.
bool CScene::IsPickedObject(D3DXVECTOR3& vMousePosW, CGameObject* pTarget)
{
	float x = vMousePosW.x;
	float z = vMousePosW.z;
	AABB bc = pTarget->m_bcMeshBoundingCube;
	bc.Update(&(pTarget->m_d3dxmtxWorld));
	if (pTarget->m_bAirUnit)
	{
		float zOffset = (AIR_UNIT_OFFSET - (pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f)) * 0.5946f; // sqrt(0.5) = 0.707f를 2로 나눈것의 sqrt가 z 오프셋이 될 것이다.
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
	else //오른쪽
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
	//1. 아군 유닛 사각형피킹 검사
	for (auto d : m_listMy)
	{
		if (!isAnimateObject(d->m_nObjectType)) continue;

		bc = d->m_bcMeshBoundingCube;
		bc.Update(&(d->m_d3dxmtxWorld));
		if (d->m_bAirUnit)
		{
			zOffset = (AIR_UNIT_OFFSET - (d->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f)) * 0.5946f; // sqrt(0.5) = 0.707f를 2로 나눈것의 sqrt가 z 오프셋이 될 것이다.
			bc.m_d3dxvMinimum.z += zOffset;
			bc.m_d3dxvMaximum.z += zOffset;
		}
		objMin = bc.m_d3dxvMinimum; objMax = bc.m_d3dxvMaximum;

		//mouseMin.x보다 objMax.x가 크고, mouseMax.x보다 objMin.x가 작아야한다. 이러면 박스는 x범위안에 있다.
		//mouseMin.z보다 objMax.z가 크고, mouseMax.z보다 objMin.z가 작아야한다. 이러면 박스는 z범위 안에 있다.
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

		//대표유닛을 그냥 0번 오브젝트로 한다...
		int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
		if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
		else //건물인 경우 what이 offset값이다
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

		return;
	}

	//2. 리턴되지 않음 -> 적군 유닛 사각형피킹 검사
	for (auto d : m_listEnemyInSight)
	{
		if (!isAnimateObject(d->m_nObjectType)) continue;

		bc = d->m_bcMeshBoundingCube;
		bc.Update(&(d->m_d3dxmtxWorld));
		if (d->m_bAirUnit)
		{
			zOffset = (AIR_UNIT_OFFSET - (d->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f)) * 0.5946f; // sqrt(0.5) = 0.707f를 2로 나눈것의 sqrt가 z 오프셋이 될 것이다.
			bc.m_d3dxvMinimum.z += zOffset;
			bc.m_d3dxvMaximum.z += zOffset;
		}
		objMin = bc.m_d3dxvMinimum; objMax = bc.m_d3dxvMaximum;

		//mouseMin.x보다 objMax.x가 크고, mouseMax.x보다 objMin.x가 작아야한다. 이러면 박스는 x범위안에 있다.
		//mouseMin.z보다 objMax.z가 크고, mouseMax.z보다 objMin.z가 작아야한다. 이러면 박스는 z범위 안에 있다.
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

		//대표유닛을 그냥 0번 오브젝트로 한다...
		int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
		if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
		else //건물인 경우 what이 offset값이다
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

		return;
	}


	/*
	int nValidSelectedObjects = 0;
	//1. 아군 유닛 피킹 검사
	//2. 아군이 하나도 피킹되지 않았다면 적군 유닛 피킹
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

		//대표유닛을 그냥 0번 오브젝트로 한다...
		int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
		if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
		else //건물인 경우 what이 offset값이다
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

		return;
	}
	
	//return문이 작동하지 않음 -> 아군 유닛이 피킹되지 않았다.
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

		//대표유닛을 그냥 0번 오브젝트로 한다...
		int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
		if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
		else //건물인 경우 what이 offset값이다
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

		return;
	}
	*/

	/*
	//정적 동적 셰이더를 어떻게 판단해야할까?
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

	if (0 == nValidSelectedObjects) //MouseRect를 그렸으나 동적 객체들이 아무런 피킹이 되지 않았다면
	{
		//정적 객체들에 대한 피킹 검사로 진행
	}
	else //하나라도 피킹이 되었다. 피킹 정보를 갱신한다.
	{
		m_nValidSelectedObjects = nValidSelectedObjects;
		return;
	}

	//피킹이 안 되었다면 정적 셰이더의 객체들도 순회하도록 한다.
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


	if (0 == nValidSelectedObjects) //MouseRect를 그렸으나 정적 객체들이 아무런 피킹이 되지 않았다면
	{
		//아무것도 할 게 없다. 이전 피킹 정보를 계속 가지고 있도록 한다.
	}
	else //하나라도 피킹이 되었다. 피킹 정보를 갱신한다.
	{
		m_nValidSelectedObjects = nValidSelectedObjects;

		//대표유닛을 그냥 0번 오브젝트로 한다...
		int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
		if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
			CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
		else //건물인 경우 what이 offset값이다
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);
	}
	*/
}

void CScene::GetMouseCursorObject()
{
	//nextPos에 대해서 처리하면 됨
	//y축, 즉 높이에 대해 처리가 안되는 상태이다. 생각좀 해보고 짤 예정
	float x = m_d3dxvNextCursorPosition.x;
	float z = m_d3dxvNextCursorPosition.z;

	//1. 내 유닛 리스트 순회
	//2. 내 유닛이 피킹 안되었으면 시야 내 적 리스트 순회
	//3. 그럼에도 아니라면 중립 자원 피킹인지 검사
	float zOffset;
	for (auto d : m_listMy)
	{
		AABB bc = d->m_bcMeshBoundingCube;
		bc.Update(&(d->m_d3dxmtxWorld));
		if (d->m_bAirUnit)
		{
			zOffset = (AIR_UNIT_OFFSET - (d->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f)) * 0.5946f; // sqrt(0.5) = 0.707f를 2로 나눈것의 sqrt가 z 오프셋이 될 것이다.
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

	//return 안됨 -> 적 유닛 피킹인지 검사
	for (auto d : m_listEnemyInSight)
	{
		AABB bc = d->m_bcMeshBoundingCube;
		bc.Update(&(d->m_d3dxmtxWorld));
		if (d->m_bAirUnit)
		{
			zOffset = (AIR_UNIT_OFFSET - (d->m_bcMeshBoundingCube.m_d3dxvMaximum.y * 2.0f)) * 0.5946f; // sqrt(0.5) = 0.707f를 2로 나눈것의 sqrt가 z 오프셋이 될 것이다.
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

	//return 안됨 -> 중립 자원 피킹인지 검사
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

	//아무것도 못 찍었다. 기존 정보를 유지한다. 원래 코드인 초기화하려면 밑줄을 진행
	//m_nValidSelectedObjects = 0;

	/*
	//정적 동적 셰이더를 어떻게 판단해야할까?
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

	//return; 으로 빠져나오지 않았다는 것은 동적 객체들이 피킹이 되지 않았다. 정적 객체를 순회한다.
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
			//0.0f ~ 1.0f 보간
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
	//상수버퍼로 부족할듯 더 큰 용량
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(FogData) * (NUM_FOG_SLICE) * (NUM_FOG_SLICE );	//최대크기 4096 * 16바이트 딱 맞춤;
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
//씬에서 객체의 피킹을 처리합니다.
CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, bool isLeftButton)
{
	//객체를 피킹하는 코드이므로 메시별로 돌지 말고 바운딩 박스 별로 확인한다면 큰 효율을 가질 수 있지 않을까?
	if (!m_pCamera) return(nullptr);

	D3DXMATRIX d3dxmtxView = m_pCamera->GetViewMatrix();
	D3DXMATRIX d3dxmtxProjection = m_pCamera->GetProjectionMatrix();
	D3D11_VIEWPORT d3dViewport = m_pCamera->GetViewport();

	D3DXVECTOR3 d3dxvPickPosition;
	/*화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다.
	그 결과는 카메라 좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.*/
	d3dxvPickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / d3dxmtxProjection._11;
	d3dxvPickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / d3dxmtxProjection._22;
	d3dxvPickPosition.z = 1.0f;

	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	MESHINTERSECTINFO d3dxIntersectInfo;

	CGameObject *pIntersectedObject = NULL, *pNearestObject = NULL;
	//씬의 모든 쉐이더 객체에 대하여 픽킹을 처리하여 카메라와 가장 가까운 픽킹된 객체를 찾는다. 스카이박스 제외
	int i = isLeftButton ? 2 : 1;	//LB면 지형 제외, RB면 지형 포함
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
	/*화면 좌표계의 점 (xClient, yClient)를 화면 좌표 변환의 역변환과 투영 변환의 역변환을 한다.
	그 결과는 카메라 좌표계의 점이다. 투영 평면이 카메라에서 z-축으로 거리가 1이므로 z-좌표는 1로 설정한다.*/
	d3dxvPickPosition.x = (((2.0f * xClient) / d3dViewport.Width) - 1) / d3dxmtxProjection._11;
	d3dxvPickPosition.y = -(((2.0f * yClient) / d3dViewport.Height) - 1) / d3dxmtxProjection._22;
	d3dxvPickPosition.z = 1.0f;

	D3DXVECTOR3 d3dxvPickRayPosition, d3dxvPickRayDirection;
	MESHINTERSECTINFO d3dxIntersectInfo;
	d3dxIntersectInfo.m_fDistance = FLT_MAX;

	D3DXVECTOR3 nearstPosition{ FLT_MAX, FLT_MAX, FLT_MAX }, result;
	float distance = FLT_MAX;

	//0811 수정
	//현재 피킹코드는 지형 위의 위치를 얻고 있다. 그 위치로부터 xz를 비교하여 객체가 존재하는 경우 그 객체를
	//피킹했다고 처리하고 있다. 나름 생각한 최적화였다. 허나 이제 공중유닛이 존재하면서 약간의 변경이 필요하다.
	//공중 유닛들에 대해서는 메시 피킹을 수행하고, 공중 유닛이 피킹되지 않은 경우 지형 피킹을 실행하도록 한다.
	//무시한다... 수정할 부분이 많기에 일단 진행하도록 함

	if (m_ppShaders[2])
	{
		CGameObject* pObject = m_ppShaders[2]->GetppObjects()[0];
		pObject->GenerateRayForPicking(&d3dxvPickPosition, &pObject->m_d3dxmtxWorld, &d3dxmtxView, &d3dxvPickRayPosition, &d3dxvPickRayDirection);

		//메시가 많아진다면 메시의 visable 여부를 따져서 순회하도록 하면 괜찮겠는데?
		CMesh* pMesh = nullptr;
		for (int i = 0; i < pObject->m_nMeshes; ++i)
		{
			pMesh = pObject->GetMesh(i);
			
			bool bIsVisible = true;
			
			AABB bcBoundingCube = pMesh->GetBoundingCube();
			bcBoundingCube.Update(&pObject->m_d3dxmtxWorld);	//지형 로컬좌표는 단위행렬이라 곱해주지 않는다.
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
	//미니맵을 클릭한건지에 대한 검사
	float xPos = xScreen / m_fWndClientWidth;
	float yPos = (m_fWndClientHeight - yScreen) / m_fWndClientHeight;	//+y가 위쪽으로 바꿈
	
	float miniMapLeft = (MINIMAP_POSITION_LEFT + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float miniMapRight = (MINIMAP_POSITION_RIGHT + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float miniMapBottom = (MINIMAP_POSITION_BOTTOM + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float miniMapTop = (MINIMAP_POSITION_TOP + 1.0f) / 2.0f; // 0.0f ~ 1.0f
	float miniMapWidth = miniMapRight - miniMapLeft;
	float miniMapLength = miniMapTop - miniMapBottom;


	if (xPos > miniMapRight || xPos < miniMapLeft || yPos > miniMapTop || yPos < miniMapBottom)
		return false;

	//미니맵을 클릭했다면 그 위치를 지형 자료구조로 매핑한다.
	xPos = (xPos - miniMapLeft) / miniMapWidth;
	yPos = (yPos - miniMapBottom) / miniMapLength;

	int xIdx = xPos * (NUM_FOG_SLICE - 1.0f);
	int zIdx = (1.0f - yPos) * (NUM_FOG_SLICE - 1.0f);
	D3DXVECTOR4 clickedPosition = m_d3dxvFogData[xIdx + (int)(zIdx*NUM_FOG_SLICE)];

	//맵 밖이 보이지 않도록 보간한다.
	if (clickedPosition.x <= CAMERA_MIN_LOOKAT_X)	clickedPosition.x = CAMERA_MIN_LOOKAT_X;
	if (clickedPosition.x >= CAMERA_MAX_LOOKAT_X)	clickedPosition.x = CAMERA_MAX_LOOKAT_X;
	if (clickedPosition.z <= CAMERA_MIN_LOOKAT_Z)	clickedPosition.z = CAMERA_MIN_LOOKAT_Z;
	if (clickedPosition.z >= CAMERA_MAX_LOOKAT_Z)	clickedPosition.z = CAMERA_MAX_LOOKAT_Z;

	//카메라의 이동은 카메라 위치 자체를 변경하는 것이 아닌 LookAtWorld를 이동시키고 그 값만큼을
	//카메라 위치 이동을 해야한다.
	float xDifference = clickedPosition.x - m_pCamera->GetLookAtPosition().x;
	float zDifference = clickedPosition.z - m_pCamera->GetLookAtPosition().z;
	D3DXVECTOR3 vMove{ xDifference, 0.0f, zDifference };

	//어택처리가 아닌 경우
	if (false == bAttack)
	{

		//일단 왼쪽 클릭부터 처리하자
		if (bLeftClicked)
		{
			m_pCamera->Move(vMove);
			m_pCamera->SetLookAtWorld(m_pCamera->GetLookAtPosition() + vMove);
			m_pCamera->GenerateViewMatrix();
		}
		else if (m_nValidSelectedObjects > 0)
			//오른쪽 마우스 클릭, 또한 이 함수에 접근하려면 SelectedObject가 적어도 1 개 이상인 경우이다.
		{
			D3DXVECTOR3 vTo = D3DXVECTOR3(clickedPosition.x, clickedPosition.y, clickedPosition.z);

			for (int i = 0; i < m_nValidSelectedObjects; ++i)
			{
				if (isEnemyObject(m_ppSelectedObjects[i]->m_nObjectType)) continue;
				if (!isAnimateObject(m_ppSelectedObjects[i]->m_nObjectType)) continue;

				if (Vec2DDistance(Vector2D(vTo.x, vTo.z), Vector2D(m_ppSelectedObjects[i]->GetPosition().x, m_ppSelectedObjects[i]->GetPosition().z)) <= 0.2f) //주변 클릭
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
		//move_with_guard 또는 have_target
		D3DXVECTOR3 vec = D3DXVECTOR3(clickedPosition.x, clickedPosition.y, clickedPosition.z);
		int mouse_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(vec);
		CGameObject* pTarget = nullptr;
		for (auto d : m_listEnemyInSight)	//아군을 공격할 수도 있지만, 일단 적군만 공격하는 것으로 한다.
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
				{ //공격불가 유닛이라면 RButton 이동과 같게 처리
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
		{	//일단 attack을 보낸다. 어차피 attack에서 공격범위 밖임을 확인한다면 바로 have_target으로 넘어갈 것이다.
			for (int i = 0; i < m_nValidSelectedObjects; ++i)
			{
				int obj_type = m_ppSelectedObjects[i]->m_nObjectType;
				if (!isAnimateObject(obj_type) || isEnemyObject(obj_type)) continue;

				if (m_ppSelectedObjects[i]->m_fObjectAttackRange == 0.0f)
				{ //공격불가 유닛이라면 RButton 이동과 같게 처리
					CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
					continue;
				}

				if (pTarget->m_bAirUnit && !m_ppSelectedObjects[i]->m_bAirAttack)
				{	 //상대는 공중유닛, 나는 공중공격 못하면
					CAnimationObject* pMyObject = (CAnimationObject*)m_ppSelectedObjects[i];
					pMyObject->m_pTargetObject = nullptr;
					pMyObject->CS_Packet_State(State::move_with_guard);
					CS_Packet_Move_Object(pMyObject, vec, false);
					continue;
				}

				//당장 공격이 가능하다면 attack을 송출
				CAnimationObject* pMyObject = (CAnimationObject*)m_ppSelectedObjects[i];
				pMyObject->m_pTargetObject = pTarget;

				//서버 처리 안되서 attack 바로 가지 않고 have_target을 거치는 테스트						
				/*
				float distance = (pMyObject->GetPosition().x - (pTarget->GetPosition().x - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (pMyObject->GetPosition().x - (pTarget->GetPosition().x - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
				+ (pMyObject->GetPosition().z - (pTarget->GetPosition().z - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (pMyObject->GetPosition().z - (pTarget->GetPosition().z - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
				if (distance <= (pMyObject->m_fObjectAttackRange * pMyObject->m_fObjectAttackRange))
				{
				pMyObject->CS_Packet_State(State::attack);
				pMyObject->m_bProcessedAttack = true;
				pMyObject->m_bPickTarget = true;
				}
				else //당장 공격 불가 -> 상대를 have_target으로 둔다.
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
	float yPos = (m_fWndClientHeight - yScreen) / m_fWndClientHeight;	//+y가 위쪽으로 바꿈

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
		//안에 존재한다면, 어느 위치인지 매개변수에 대입해준다.
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
	if (bLeftClicked) //이 위치에서 마우스를 뗀(클릭했다 보면 됨) 경우
	{
		m_bActivedBuildCheck = true;

		switch (m_nSceneState)
		{	//건물같은 경우는 짓는 시점에서 자원 유효 여부를 따지도록 하면 두 경우(+키 입력)가 일괄적으로 처리된다.
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
				{ //테크트리 건물인 사령부가 있는지 확인
					m_nSceneState = SCENE_STATE::build_barrack;
					m_strActivedBuildCheck = "barrack";
				}
			}
			else if (7 == commandPosNum)
			{	//engineeringbay
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Commandcenter)->second)
				{ //테크트리 건물인 사령부가 있는지 확인
					m_nSceneState = SCENE_STATE::build_engineeringbay;
					m_strActivedBuildCheck = "engineeringbay";
				}
			}
			else if (9 == commandPosNum)
			{	//sensortower
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
				{ //테크트리 건물인 병영이 있는지 확인
					m_nSceneState = SCENE_STATE::build_sensortower;
					m_strActivedBuildCheck = "sensortower";
				}
			}
			else if (10 == commandPosNum)
			{	//missileturret
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Engineeringbay)->second)
				{ //테크트리 건물인 공학연구소가 있는지 확인
					m_nSceneState = SCENE_STATE::build_missileturret;
					m_strActivedBuildCheck = "missileturret";
				}
			}
			else if (12 == commandPosNum)
			{	//bunker
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
				{ //테크트리 건물인 병영이 있는지 확인
					m_nSceneState = SCENE_STATE::build_bunker;
					m_strActivedBuildCheck = "bunker";
				}
			}
			break;
		case SCENE_STATE::scv_buildadv:
			if (1 == commandPosNum)
			{	//factory
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
				{ //테크트리 건물인 병영이 있는지 확인
					m_nSceneState = SCENE_STATE::build_factory;
					m_strActivedBuildCheck = "factory";
				}
			}
			else if (2 == commandPosNum)
			{	//starport
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Factory)->second)
				{ //테크트리 건물인 군수공장이 있는지 확인
					m_nSceneState = SCENE_STATE::build_starport;
					m_strActivedBuildCheck = "starport";
				}
			}
			else if (7 == commandPosNum)
			{	//armory
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Factory)->second)
				{ //테크트리 건물인 군수공장이 있는지 확인
					m_nSceneState = SCENE_STATE::build_armory;
					m_strActivedBuildCheck = "armory";
				}
			}
			else if (11 == commandPosNum)
			{	//ghostacademy
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Starport)->second)
				{ //테크트리 건물인 우주공항이 있는지 확인
					m_nSceneState = SCENE_STATE::build_ghostacademy;
					m_strActivedBuildCheck = "ghostacademy";
				}
			}
			else if (12 == commandPosNum)
			{	//fusioncore
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Starport)->second)
				{ //테크트리 건물인 우주공항이 있는지 확인
					m_nSceneState = SCENE_STATE::build_fusioncore;
					m_strActivedBuildCheck = "fusioncore";
				}
			}
			break;

			//생산건물 클릭인 경우
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
				{ //테크트리 건물인 융합연구소가 있는지 확인
					CreateNewObject(OBJECT_TYPE::Raven, m_ppSelectedObjects[0]->GetPosition());
				}
			}
			else if (7 == commandPosNum)
			{	//battlecruiser
				if (m_mapIsBuildingExist.find(OBJECT_TYPE::Fusioncore)->second)
				{ //테크트리 건물인 융합연구소가 있는지 인확
					CreateNewObject(OBJECT_TYPE::BattleCruiser, m_ppSelectedObjects[0]->GetPosition());
				}
			}	
			break;
			
		} //end switch

		if (false == (SCENE_STATE::build_armory <= m_nSceneState && m_nSceneState <= SCENE_STATE::build_techlab))
			m_bActivedBuildCheck = false;
	}

	else // Left버튼 아님
	{
		//왼쪽 버튼 클릭이 아니라는 건 mousemove를 하고 있다는 것이다. 폰트를 갱신한다.
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
	//스크린좌표를 -1.0f ~ 1.0f으로 변환
	float xPos = (xScreen / m_fWndClientWidth) * 2.0f - 1.0f;
	float yPos = ((m_fWndClientHeight - yScreen) / m_fWndClientHeight) * 2.0f - 1.0f;	//+y가 위쪽으로 바꿈

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

	//for문 내에서 return하지 않았으면 피킹 된 정보가 없다.
	return false;
}

void CScene::ProcessCursorInCrowd(int crowdPosNum)
{
	m_nValidSelectedObjects = 1;
	m_ppSelectedObjects[0] = m_ppSelectedObjects[crowdPosNum - 1];
	UpdateSelectedObjects();
}

//**********************************************************************
//입력 처리
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
			
			//0904 바로 CreateObject가 아닌 짓기위해 이동한다.
			int my_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(m_ppSelectedObjects[0]->GetPosition());
			int obj_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(vec);
			if(my_idx == obj_idx)
				CreateNewObject((OBJECT_TYPE)m_nSceneState, vec);
			else //짓는곳까지 거리가 있다면
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
				GetMouseRectObjects(); //마우스 드래그 사각형을 피킹하여 객체 리스트를 얻기
				isPickedObject = true;
			}
			else //false == m_bMouseRectDraw
			{	//드래그는 하지 않았지만 클릭을 했다
				x = LOWORD(lParam); y = HIWORD(lParam);

				bool isCrowdClicked{ false };
				int crowdNum = 0;
				if (2 <= m_nValidSelectedObjects)
				{ //군중 유닛 중 하나를 선택했는지 확인한다.
					isCrowdClicked = IsCursorInCrowd(x, y, crowdNum);
				}

				int commandNum = 0;
				bool isCommandClicked = IsCursorInCommand(x, y, commandNum);
				
				//1. 커맨드창 클릭인지 확인한다.
				if (isCommandClicked)
					ProcessCursorInCommand(commandNum, true, nullptr);
				//2. 군중 유닛 클릭인지 확인한다.
				else if (isCrowdClicked)
					ProcessCursorInCrowd(crowdNum);
				else
				{
					//3. 둘 다 아니라면 단일 객체 피킹인지 확인한다(바운딩 박스로 확인).
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
				D3DXVECTOR2(m_d3dxvNextCursorPosition.x, m_d3dxvNextCursorPosition.z))) <= 1.0f)	//사각형이 작다면 그냥 넘어간다.
				break;

			m_bMouseRectDraw = true;	//draw는 Scene의 Render()에서
		}
		
		x = LOWORD(lParam); y = HIWORD(lParam);
		m_ptMousePosition.x = x; m_ptMousePosition.y = y;

	}
	break;
	case WM_RBUTTONDOWN:
	{
		if (m_nValidSelectedObjects > 0)	//지형 위만 돌아다닌다 일단.
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

					if (Vec2DDistance(Vector2D(vec.x, vec.z), Vector2D(m_ppSelectedObjects[i]->GetPosition().x, m_ppSelectedObjects[i]->GetPosition().z)) <= 0.2f) //주변 클릭
						continue;

					if ((int)OBJECT_TYPE::Scv == m_ppSelectedObjects[i]->m_nObjectType)
					{
						CAnimationObject* pObject = (CAnimationObject*)m_ppSelectedObjects[i];
						//scv면 미네랄 피킹인지 검사해야 한다.
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

						if (pMineral) //피킹되었다면
						{	//가장 가까운 커맨드 센터를 찾는다.
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

							if (nearstCommandCenter) //찾았다면
							{
								CS_Packet_Dig_Resource(m_ppSelectedObjects[i], pMineral, nearstCommandCenter);
							}
							else //못 찾았을 경우는 커맨드가 없다는 뜻
							{
								vec.y = 0.0f;
								CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
							}
						}
						else //nullptr == pMineral -> 미네랄피킹이 아니다 -> Refinery 피킹인지 검사한다.
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
							{	//리파이너리 피킹되었다면 가장 가까gas운 커맨드 센터를 찾는다.
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

								if (nearstCommandCenter) //찾았다면
								{
									CS_Packet_Dig_Resource(m_ppSelectedObjects[i], pRefinery, nearstCommandCenter);
								}
								else //못 찾았을 경우는 커맨드가 없다는 뜻
								{
									vec.y = 0.0f;
									CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
								}
							}
							else //리파이너리 피킹도 아니다 -> 이동 패킷이다.
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

			//모든 선택된 유닛들의 사운드 재생이 아닌 대표유닛(0인덱스)만 재생한다.
			int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
			if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
				CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
			else //건물인 경우 what이 offset값이다
				CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);

			if(m_bActivedBuildCheck)	//건물 짓기 직전 상태에서 이동이 발생한 것에 대한 처리
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
	//Framework에서 사용하는 키보드 메시지
	//case VK_NUMPAD1:
	//case VK_NUMPAD2:
	//case VK_ESCAPE:
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	{
		if (m_nValidSelectedObjects > 0)
		{
			if (wParam >= 0x31 && wParam <= 0x38) // 1~8 누른 경우
			{	//0x0000 -> 누른적X현재X / 0x0001 -> 누른적O현재X / 0x8000 -> 누른적X현재O / 0x8001 -> 누른적O현재O

				// 1. 컨트롤 키 토글상태인지 확인하여 부대지정 저장인지 확인
				if (0x8000 & GetAsyncKeyState(VK_CONTROL)) //컨트롤키가 눌려있다면
				{
					if (!isEnemyObject(m_ppSelectedObjects[0]->m_nObjectType)) //적군은 부대지정 하지 않는다.
					{
						m_CrowdSaveData[wParam - 48 - 1].nUnits = m_nValidSelectedObjects;
						for (int i = 0; i < m_nValidSelectedObjects; ++i)
							m_CrowdSaveData[wParam - 48 - 1].ppObjects[i] = m_ppSelectedObjects[i];
					}
				}
				// 2. 아닌 경우 저장된 부대 불러오는지 확인
				else
				{
					if (m_CrowdSaveData[wParam - 48 - 1].nUnits == 0)
					{
						//nothing to do
					}
					else //저장된 정보가 있다면 
					{
						//부대 불러오기
						m_nValidSelectedObjects = m_CrowdSaveData[wParam - 48 - 1].nUnits;
						for (int i = 0; i < m_CrowdSaveData[wParam - 48 - 1].nUnits; ++i)
							m_ppSelectedObjects[i] = m_CrowdSaveData[wParam - 48 - 1].ppObjects[i];

						if (m_chLastInputKey == wParam) //예전 입력 키와 같은 키가 눌린 경우라면
						{
							if (m_fLastInputCrowdDataButtonTime <= DOUBLE_INPUT_DURATION)
							{	// 더블 입력인지 확인, 맞다면 카메라 이동
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
						else //건물인 경우 what이 offset값이다
							CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset);
					}

					m_chLastInputKey = wParam;
					m_fLastInputCrowdDataButtonTime = 0.0f; //다시 초기화
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
						{ //상위 테크트리인 사령부가 있는지 확인
							m_nSceneState = SCENE_STATE::build_barrack;
							m_strActivedBuildCheck = "barrack";
						}
						else //상위 테크 존재하지 않으면 buildCheck 이미지가 그려지면 안된다. 코드 간결화하려다 꼬임;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'E')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Commandcenter)->second)
						{ //상위 테크트리인 사령부가 있는지 확인
							m_nSceneState = SCENE_STATE::build_engineeringbay;
							m_strActivedBuildCheck = "engineeringbay";
						}
						else //상위 테크 존재하지 않으면 buildCheck 이미지가 그려지면 안된다. 코드 간결화하려다 꼬임;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'N')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
						{ //상위 테크트리인 병영이 있는지 확인
							m_nSceneState = SCENE_STATE::build_sensortower;
							m_strActivedBuildCheck = "sensortower";
						}
						else //상위 테크 존재하지 않으면 buildCheck 이미지가 그려지면 안된다. 코드 간결화하려다 꼬임;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'T')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Engineeringbay)->second)
						{ //상위 테크트리인 공학연구소가 있는지 확인
							m_nSceneState = SCENE_STATE::build_missileturret;
							m_strActivedBuildCheck = "missileturret";
						}
						else //상위 테크 존재하지 않으면 buildCheck 이미지가 그려지면 안된다. 코드 간결화하려다 꼬임;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'U')
					{ 
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Barrack)->second)
						{ //상위 테크트리인 병영이 있는지 확인
							m_nSceneState = SCENE_STATE::build_bunker;
							m_strActivedBuildCheck = "bunker";
						}
						else //상위 테크 존재하지 않으면 buildCheck 이미지가 그려지면 안된다. 코드 간결화하려다 꼬임;
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
						{ //상위 테크트리인 병영이 있는지 확인
							m_nSceneState = SCENE_STATE::build_factory;
							m_strActivedBuildCheck = "factory";
						}
						else //상위 테크 존재하지 않으면 buildCheck 이미지가 그려지면 안된다. 코드 간결화하려다 꼬임;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'S')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Factory)->second)
						{ //상위 테크트리인 군수공장이 있는지 확인
							m_nSceneState = SCENE_STATE::build_starport;
							m_strActivedBuildCheck = "starport";
						}
						else //상위 테크 존재하지 않으면 buildCheck 이미지가 그려지면 안된다. 코드 간결화하려다 꼬임;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'A')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Factory)->second)
						{ //상위 테크트리인 군수공장이 있는지 확인
							m_nSceneState = SCENE_STATE::build_armory;
							m_strActivedBuildCheck = "armory";
						}
						else //상위 테크 존재하지 않으면 buildCheck 이미지가 그려지면 안된다. 코드 간결화하려다 꼬임;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'G')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Factory)->second)
						{ //상위 테크트리인 군수공장이 있는지 확인
							m_nSceneState = SCENE_STATE::build_ghostacademy;
							m_strActivedBuildCheck = "ghostacademy";
						}
						else //상위 테크 존재하지 않으면 buildCheck 이미지가 그려지면 안된다. 코드 간결화하려다 꼬임;
							m_bActivedBuildCheck = false;
					}
					else if (wParam == 'C')
					{
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Starport)->second)
						{ //상위 테크트리인 우주공항이 있는지 확인
							m_nSceneState = SCENE_STATE::build_fusioncore;
							m_strActivedBuildCheck = "fusioncore";
						}
						else //상위 테크 존재하지 않으면 buildCheck 이미지가 그려지면 안된다. 코드 간결화하려다 꼬임;
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
						{ //생산하기 위한 건물인 융합연구소가 있는지 확인
							CreateNewObject(OBJECT_TYPE::Raven, m_ppSelectedObjects[0]->GetPosition());
						}
					}
					else if (wParam == 'B')
					{	//battlecruiser
						if (m_mapIsBuildingExist.find(OBJECT_TYPE::Fusioncore)->second)
						{ //생산하기 위한 건물인 융합연구소가 있는지 확인
							CreateNewObject(OBJECT_TYPE::BattleCruiser, m_ppSelectedObjects[0]->GetPosition());
						}
					}
					break;
				}
				} // SceneState 스위치문

				if (SCENE_STATE::nothing == m_nSceneState || SCENE_STATE::pick_scv == m_nSceneState)
				{
					//대표유닛인 0번의 사운드를 출력한다.
					if (m_ppSelectedObjects[0]->m_nObjectType != (int)OBJECT_TYPE::Mineral && m_ppSelectedObjects[0]->m_nObjectType != (int)OBJECT_TYPE::Gas)
					{
						int nSoundOffset = getSoundOffsetByType(m_ppSelectedObjects[0]->m_nObjectType);
						if (isAnimateObject(m_ppSelectedObjects[0]->m_nObjectType))
							CSoundManager::GetInstance()->PlayUnitWhatSound(nSoundOffset + UNIT_WHAT_OFFSET);
						else //건물인 경우 what이 offset값이다
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
							//move_with_guard 또는 have_target
							auto vec = GetPositionPicking(m_ptMousePosition.x, m_ptMousePosition.y);
							DrawMouseClickImage(vec);

							int mouse_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(vec);
							CGameObject* pTarget = nullptr;
							for (auto d : m_listEnemyInSight)	//아군을 공격할 수도 있지만, 일단 적군만 공격하는 것으로 한다.
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
									{ //공격불가 유닛이라면 RButton 이동과 같게 처리
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
							{	//일단 attack을 보낸다. 어차피 attack에서 공격범위 밖임을 확인한다면 바로 have_target으로 넘어갈 것이다.
								for (int i = 0; i < m_nValidSelectedObjects; ++i)
								{
									int obj_type = m_ppSelectedObjects[i]->m_nObjectType;
									if (!isAnimateObject(obj_type) || isEnemyObject(obj_type)) continue;

									if (m_ppSelectedObjects[i]->m_fObjectAttackRange == 0.0f)
									{ //공격불가 유닛이라면 RButton 이동과 같게 처리
										CS_Packet_Move_Object(m_ppSelectedObjects[i], vec, true);
										continue;
									}

									if (pTarget->m_bAirUnit && !m_ppSelectedObjects[i]->m_bAirAttack)
									{	 //상대는 공중유닛, 나는 공중공격 못하면
										CAnimationObject* pMyObject = (CAnimationObject*)m_ppSelectedObjects[i];
										pMyObject->m_pTargetObject = nullptr;
										pMyObject->CS_Packet_State(State::move_with_guard);
										CS_Packet_Move_Object(pMyObject, vec, false);
										continue;
									}

									//당장 공격이 가능하다면 attack을 송출
									CAnimationObject* pMyObject = (CAnimationObject*)m_ppSelectedObjects[i];
									pMyObject->m_pTargetObject = pTarget;

									//서버 처리 안되서 attack 바로 가지 않고 have_target을 거치는 테스트						
									/*
									float distance = (pMyObject->GetPosition().x - (pTarget->GetPosition().x - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (pMyObject->GetPosition().x - (pTarget->GetPosition().x - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
										+ (pMyObject->GetPosition().z - (pTarget->GetPosition().z - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (pMyObject->GetPosition().z - (pTarget->GetPosition().z - pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
									if (distance <= (pMyObject->m_fObjectAttackRange * pMyObject->m_fObjectAttackRange))
									{
										pMyObject->CS_Packet_State(State::attack);
										pMyObject->m_bProcessedAttack = true;
										pMyObject->m_bPickTarget = true;
									}
									else //당장 공격 불가 -> 상대를 have_target으로 둔다.
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

			} //키 입력 if문
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
	
	//방향키 이벤트 처리
	//0x0000 -> 누른적X현재X / 0x0001 -> 누른적O현재X / 0x8000 -> 누른적X현재O / 0x8001 -> 누른적O현재O
	bool isKeyboardArrowPressed = true;
	int inputKeyboardArrow = 0x0000;	// l r u d
	if (0x8000 & GetAsyncKeyState(VK_LEFT)) inputKeyboardArrow |= 0x1000;
	if (0x8000 & GetAsyncKeyState(VK_RIGHT)) inputKeyboardArrow |= 0x0100;
	if (0x8000 & GetAsyncKeyState(VK_UP)) inputKeyboardArrow |= 0x0010;
	if (0x8000 & GetAsyncKeyState(VK_DOWN)) inputKeyboardArrow |= 0x0001;
	*/
	return(false);
}

//마우스 입력에 따른 카메라가 맵 밖을 벗어나지 못하도록
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


//지형 위 이벤트를 위한 높이맵 생성 -> 0705 높이맵에서 얻어오므로 필요 X
/*
void CScene::CreateHeightMap(LPCTSTR pFileName, int nWidth, int nHeight, D3DXVECTOR3& d3dxvScale)
{
	m_pHeightMap = new CHeightMap(pFileName, nWidth, nHeight, d3dxvScale);
}
*/

//**********************************************************************
//게임 씬에서의 패킷 처리에 대한 나열입니다.
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

	if(nullptr == pObject) // 적 리스트에서 못찾았다면
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
	//피아 식별
	if (packet->object_type == OBJECT_TYPE::Mineral || packet->object_type == OBJECT_TYPE::Gas)
		bIsNeutral = true;
	else if (packet->client_id != m_pGameFramework->m_clientID)
		obj_type += ADD_ENEMY_INDEX;

	//자원 배치는 여기서 따로 처리하도록 하겠다.
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

	//type을 비교하여 정적인건지 애니메이션인지 확인해야할듯?
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
		{ //적 유닛
			vec[i]->m_ppObjects[ObjectIdx]->SetInSight(false); //일단 false, 시야 리스트 갱신 시에 바뀔것이다.

			if (vec[i]->m_ppObjects[ObjectIdx]->m_bAirUnit)
				m_listEnemy.push_front(vec[i]->m_ppObjects[ObjectIdx]);
			else //지상
				m_listEnemy.push_back(vec[i]->m_ppObjects[ObjectIdx]);
		}
		else //아군 유닛이다.
		{
			vec[i]->m_ppObjects[ObjectIdx]->SetInSight(true); //아군유닛은 언제나 보인다.

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

		//처음 커맨드센터가 제공되는 상황 -> 갓 시작  건물은 PutObject로 오지 않음
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
			//printf("%d가 정상적으로 테크트리 true 되었습니다\n", vec[i]->m_ppObjects[ObjectIdx]->m_nObjectType);
			m_mapCommandMesh.find("scv")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
			m_mapCommandMesh.find("barrack")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
			m_mapCommandMesh.find("engineeringbay")->second->UpdateScreenMeshRendered(m_pd3dDeviceContext, true);
		}
	}
}

//0803 세분화시키면서 사용 안함
/*
void CScene::Process_SC_Packet_Data_Object(sc_packet_data_object* packet)
{
	//패킷 느리게 처리되는거 테스트
	
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

		//0901 죽는 경우 Scene에서 관리하는 ObjectList를 갱신해야 한다.
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
			else //아군 유닛
			{
				m_listMy.remove(pObject);
				
				//1206 만일 모든 건물이 터졌으면 패배처리(서버가 안줘서 임시로)
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
		printf("초당 이동패킷 수 : %d\n", nPacketCount);
		nPacketCount = 0;
	}
	*/
}

void CScene::Process_SC_Packet_Data_Object_Value(sc_packet_data_object_value* packet)
{
	int obj_type = (int)packet->object_type;
	
	if (OBJECT_TYPE::Mineral == packet->object_type || OBJECT_TYPE::Gas == packet->object_type)
		; //가스 미네랄은 무시한다.
	else if (packet->client_id != CSocketComponent::GetInstance()->m_clientID)
		obj_type += ADD_ENEMY_INDEX;

	CGameObject* pObject = FindObjectById(obj_type, packet->object_id);

	if (pObject)
	{
		//1206 테크트리 표현
		if (packet->client_id == CSocketComponent::GetInstance()->m_clientID)
		{	//아군 오브젝트 중
			if (!isAnimateObject((int)packet->object_type))
			{	//건물이며 
				if (packet->building_time == ((CStaticObject*)(pObject))->m_fCompleteBuildTime)
				{  //완성되는 시점인 경우 테크트리 표현을 위해 exist 여부를 true로 바꾸어준다.

					if (true == m_mapIsBuildingExist.find((OBJECT_TYPE)pObject->m_nObjectType)->second)
					{ //이미 true인 경우 그냥 지나가지만
					}
					else // 처음 지어지거나 파괴되었다 지어짐으로써, exist를 true로 바꾸고 테크트리 렌더링 이미지를 업데이트한다.
					{
						m_mapIsBuildingExist.find((OBJECT_TYPE)pObject->m_nObjectType)->second = true;
						printf("%s가 건설완료되어 지어진 여부를 true로 바꾸었다.\n", getNameByType(obj_type).data());

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
				{  //반대로 파괴되는 시점인 경우에는 현재 보유중인 같은 건물이 한개라도 있으면 넘어가고, 없게된다면 false로 변경
					int nBuildingCount = 0;
					for (auto d = m_listMy.begin(); d != m_listMy.end(); ++d)
					{
						if ((*d)->m_nObjectType != obj_type) continue;
						++nBuildingCount;
					}
					if (nBuildingCount <= 1) //1보다 작거나 같다 -> 이 파괴를 통해 그 건물의 존재를 잃어버림 -> false로 변경
					{
						m_mapIsBuildingExist.find((OBJECT_TYPE)pObject->m_nObjectType)->second = false;
						printf("%s가 모두 파괴되어 이로인한 테크트리를 잃습니다.\n", getNameByType(obj_type).data());

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
		//테크트리 

		//객체 상태를 갱신한다.
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
	//피아 식별
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
					d->SetActive(false); //가스 모델을 숨긴다.
					break;
				}
			}
		}

		//test
		if (isEnemyObject(obj_type))
		{ //적 유닛
			vec[i]->m_ppObjects[ObjectIdx]->SetInSight(false); //일단 false, 시야 리스트 갱신 시에 바뀔것이다.

			if (vec[i]->m_ppObjects[ObjectIdx]->m_bAirUnit)
				m_listEnemy.push_front(vec[i]->m_ppObjects[ObjectIdx]);
			else //지상
				m_listEnemy.push_back(vec[i]->m_ppObjects[ObjectIdx]);
		}
		else //아군 유닛이다.
		{
			vec[i]->m_ppObjects[ObjectIdx]->SetInSight(true); //아군유닛은 언제나 보인다.

			if (vec[i]->m_ppObjects[ObjectIdx]->m_bAirUnit)
				m_listMy.push_front(vec[i]->m_ppObjects[ObjectIdx]);
			else
				m_listMy.push_back(vec[i]->m_ppObjects[ObjectIdx]);

			//아군 건물인 경우 -> 자원 사용을 여기서 한다.
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
	printf("SC_Packet_Attack_Object 호출\n");

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
		pObject->Process_SC_Attack(packet, pTarget); //packet 포인터는 사용 안함
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
	printf("Process_SC_Packet_Data_Resource를 받았다! mineral : %d, gas %d\n", packet->mineral, packet->gas);
	gGameMineral += packet->mineral;
	gGameGas += packet->gas;

	//자원 표시를 꺼야한다.
	list<CGameObject*>* obj_list;
	if (CSocketComponent::GetInstance()->m_clientID != packet->client_id)
		obj_list = &m_listEnemy;
	else //내 유닛
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
	//자원 표시 활성화
	list<CGameObject*>* obj_list;
	if (CSocketComponent::GetInstance()->m_clientID != packet->client_id)
		obj_list = &m_listEnemy;
	else //내 유닛
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
	//자원 표시 활성화
	list<CGameObject*>* obj_list;
	if (CSocketComponent::GetInstance()->m_clientID != packet->client_id)
		obj_list = &m_listEnemy;
	else //내 유닛
		obj_list = &m_listMy;

	for (auto d : *obj_list)
	{
		if (d->m_nObjectID != packet->obj_id) continue;
		CAnimationObject* pScv = (CAnimationObject*)d;
		pScv->m_nScvDigResource = SCV_DIG::gas;
		return;
	}
}



//패킷 전송 함수들
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
		printf("%d 유닛은 Scene::Move패킷을 보냈다. right : true\n", pObject->m_nObjectID);
	else
		printf("%d 유닛은 Scene::Move패킷을 보냈다. right : false\n", pObject->m_nObjectID);
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
	printf("%d SCV는 CS_DIG를 보냈다! mineral : %d / command : %d\n", pSCV->m_nObjectID, pResource->m_nObjectID, pCommandcenter->m_nObjectID);
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
//갱신 처리

void CScene::UpdateObject(float fTimeElapsed)
{
	//Scene의 다른 함수에서 사용하도록 멤버변수에 경과시간을 저장한다.
	m_fCurrentFrameElapsedTime = fTimeElapsed;

	if (m_ppShaders && (m_nShaders >= 3))
	{
		//스카이박스 및 맵 뛰어넘고
		for (auto i = 3; i < m_nShaders; ++i)
			m_ppShaders[i]->UpdateObject(fTimeElapsed, m_pHeightMap, nullptr, nullptr);	//딱히 enemyList가 필요하지 않음
	}

	//위치에 대한 생각좀 해야함..
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
		//스카이박스 및 맵 뛰어넘고
		for (auto i = 3; i < m_nShaders; ++i)
			m_ppShaders[i]->Animate(fTimeElapsed);
	}

	if (m_pStaticShader)		m_pStaticShader->Animate(fTimeElapsed);
	if (m_pAnimationShader)		m_pAnimationShader->Animate(fTimeElapsed);
}

//적 리스트 갱신
void CScene::UpdateEnemyInSight(float fElapsedTime)
{
	//이 역시 UpdateTime을 관리해주는 것이 좋겠다.
	static float EnemyListUpdateTimer = 0.0f;
	EnemyListUpdateTimer += m_fCurrentFrameElapsedTime; //fElapsedTime과 같은 값이겠지만 일단 인자값으로 받았다.

	if (EnemyListUpdateTimer < UPDATE_ENEMYLIST_TIME)
		return;
	else
		EnemyListUpdateTimer = 0.0f;

	//모든 적 객체 리스트 중 시야 내에 포함되는 객체를 찾아낸다.
	m_listEnemyInSight.clear();
	D3DXVECTOR3 objPos;
	float x, y, z;
	int xIdx, zIdx;

	for (auto d : m_listEnemy)
	{
		objPos = d->GetPosition();
		//객체 위치를 보간
		x = objPos.x; y = objPos.y; z = objPos.z;
		x += (MAP_WIDTH / 2.0f); z += (MAP_HEIGHT / 2.0f); // to 0 ~ 205
		x /= MAP_WIDTH; z /= MAP_HEIGHT;
		xIdx = (int)(x * (NUM_FOG_SLICE - 1.0f));
		zIdx = (NUM_FOG_SLICE - 1.0f) - (int)(z * (NUM_FOG_SLICE - 1.0f));

		if (m_d3dxvFogData[xIdx + (int)(zIdx * (NUM_FOG_SLICE))].w == 2.0f) //시야 안에 존재한다면
		{
			m_listEnemyInSight.push_back(d);
			d->SetInSight(true);
		}
		else
			d->SetInSight(false);
	}	

	//0901 중립 자원 역시 내 시야에 들어오는지를 판단해야 한다.
	for (auto d : m_listNeutral)
	{
		objPos = d->GetPosition();
		//객체 위치를 보간
		x = objPos.x; y = objPos.y; z = objPos.z;
		x += (MAP_WIDTH / 2.0f); z += (MAP_HEIGHT / 2.0f); // to 0 ~ 205
		x /= MAP_WIDTH; z /= MAP_HEIGHT;
		xIdx = (int)(x * (NUM_FOG_SLICE - 1.0f));
		zIdx = (NUM_FOG_SLICE - 1.0f) - (int)(z * (NUM_FOG_SLICE - 1.0f));

		if (m_d3dxvFogData[xIdx + (int)(zIdx * (NUM_FOG_SLICE))].w == 2.0f) //시야 안에 존재한다면
			d->SetInSight(true);
		else
			d->SetInSight(false);
	}
}

//그림자
void CScene::RenderObjectShadow(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_pStaticShader)		m_pStaticShader->RenderObjectShadow(pd3dDeviceContext, m_pCamera);
	if (m_pAnimationShader)		m_pAnimationShader->RenderObjectShadow(pd3dDeviceContext, m_pCamera);
}

//안개
void CScene::UpdateFogBuffer(ID3D11DeviceContext *pd3dDeviceContext)
{
	static float fogUpdateTimer = 0.0f;
	fogUpdateTimer += m_fCurrentFrameElapsedTime;
	
	if (fogUpdateTimer < UPDATE_FOG_TIME)
		return;
	else
		fogUpdateTimer = 0.0f;

	//공중과 지상을 구분한다면(공중을 push front, 지상은 back) 
	//구분 짓는 연산은 필요하지만 이후 검사들의 회수가 훨씬 줄어든다.

	// 1. 공중 list와 지상 list 생성 
	//list<CGameObject*> ObjectList;
	list<CGameObject*>& ObjectList = m_listMy;

	/*
	// 0729 수정 -> list 3개의 자료구조를 관리하면서, 밑과 같은 연산이 필요가 없다.
	// 2. 시야를 갖는 유효한 객체들을 순회, list에 push한다. 객체가 공중유닛이라면 push front!!
	vector<RenderObjectData*>* vec;
	CGameObject* pObject;
	
	//Static부터 진행한다.
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

	//Animate 객체들
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

	// 3. 지형을 (NUM_FOG_SLICE)*(NUM_FOG_SLICE)로 매핑한 정점과 객체의 시야 원과의 충돌 검사를 통해
	//    객체들을 순회하며 갱신 시야(3)를 기록한다.
	float nodeDistance = m_d3dxvFogData[1].x - m_d3dxvFogData[0].x;
	float objRange = 0.0f;

	int nRange = 0;
	int xIdx, zIdx;

	D3DXVECTOR3 objPos;
	
	for (CGameObject* obj : ObjectList)
	{
		objPos = obj->GetPosition();
		objRange = obj->m_fObjectSightRange;

		//시야 범위를 보간
		nRange = objRange / nodeDistance;

		//객체 위치를 보간
		float x, y, z;
		x = objPos.x; y = objPos.y; z = objPos.z;
		x += (MAP_WIDTH / 2.0f); z += (MAP_HEIGHT / 2.0f); // to 0 ~ 205
		x /= MAP_WIDTH; z /= MAP_HEIGHT;
		xIdx = (int)(x * (NUM_FOG_SLICE - 1.0f));
		zIdx = (NUM_FOG_SLICE - 1.0f) - (int)(z * (NUM_FOG_SLICE - 1.0f));

		//시야범위를 통한 인덱스 리스트를 구한다.
		for (int j = 0; j <= nRange * 2; ++j)
		{
			for (int i = 0; i <= nRange * 2; ++i)
			{
				int getX = xIdx - nRange + i;
				int getZ = zIdx - nRange + j;

				//유효성 검사
				if (getX > int(NUM_FOG_SLICE - 1.0f) || getX < 0 || getZ > int(NUM_FOG_SLICE - 1.0f) || getZ < 0) continue;

				//이미 갱신되었는지 검사
				if (m_d3dxvFogData[getX +  (int)(getZ * (NUM_FOG_SLICE))].w == 3.0f) continue;

				//시야 범위 내 값인지 검사
				int distX = xIdx - getX;
				int distZ = zIdx - getZ;
				if ((nRange*nRange) < ((distX * distX) + (distZ * distZ))) continue;

				//공중유닛이라면 바로 갱신을 해주고, 지상유닛인 경우 언덕 여부 검사
				if ((false == obj->m_bAirUnit) && ((m_d3dxvFogData[getX + (int)(getZ * (NUM_FOG_SLICE))].y - objPos.y) >= 1.0f)) continue;

				//이제 이 검사를 통과한 지형 인덱스는 시야에 보이는것이 확정되었다.
				m_d3dxvFogData[getX + (int)(getZ * (NUM_FOG_SLICE))].w = 3.0f;
			}
		}
	}

	// 4. 지형 정점을 순회하며 갱신 시야 및 기존 시야였지만 변경된 경우를 처리해준다.
	for (int i = 0; i < (NUM_FOG_SLICE) * (NUM_FOG_SLICE); ++i)
	{	//0 미정찰 1 정찰 2그려져야할
		if (m_d3dxvFogData[i].w <= 1.0f)	continue;

		if (m_d3dxvFogData[i].w == 2.0f) //기존 시야 범위였지만 이젠 정찰했던 지역으로 표시해야 하는
			m_d3dxvFogData[i].w = 1.0f;
		else if (m_d3dxvFogData[i].w == 3.0f) //갱신을 통해 새로운 시야 범위의 값들
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
	// 5. 상수버퍼를 Map, PS에 연결
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
	//0901 선택된 객체 갱신하기
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

		if (m_CrowdSaveData[i].nUnits != nValidObjects) // 변동사항이 없다면 Pass, 변동사항이 있으면 갱신해야 한다.
		{
			m_CrowdSaveData[i].nUnits = nValidObjects;

			for (int k = 0; k < nValidObjects; ++k)
				m_CrowdSaveData[i].ppObjects[k] = tempObjectBox[k];
		}
	}
}

void CScene::DrawMouseClickImage(D3DXVECTOR3& vScreenToWorld)
{
	//Prompt 스프라이트는 월드 좌표계를 기준으로 렌더링되기에, Screen to World를 진행한다.
	CPromptParticleObject particle;
	particle.m_d3dxmtxWorld._41 = vScreenToWorld.x;
	particle.m_d3dxmtxWorld._42 = vScreenToWorld.y;
	particle.m_d3dxmtxWorld._43 = vScreenToWorld.z;

	CSpriteParticle::GetInstance()->AddPromptParticle(string("mouse_click"), particle);
}

/*
void CScene::DrawSystemErrorImage(D3DXVECTOR3& vScreenToWorld, int errorCode)
{
	//Prompt 스프라이트는 월드 좌표계를 기준으로 렌더링되기에, Screen to World를 진행한다.
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
	//안개 갱신
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
			//1이 테셀레이션, 2가 높이맵
			//if (i == 1) continue;
			if (i == 2) continue; //높이맵 지형은 렌더링 안함 피킹용 이거 그려보니 프레임 엄청 떨어지네 테셀레이션 굿;
			m_ppShaders[i]->Render(pd3dDeviceContext, pCamera);
		}
	}

	//선택된 오브젝트를 그릴 때 깊이비교를 하지 않고 쓰지도 않으며 그린다.
	if (m_pd3dUIDepthStencilState) pd3dDeviceContext->OMSetDepthStencilState(m_pd3dUIDepthStencilState, 0);

	//선택된 오브젝트들의 대한 UI 표현
	if (m_nValidSelectedObjects > 0)
	{
		std::vector<D3DXVECTOR4> vPosScale;
		vPosScale.reserve(m_nValidSelectedObjects);
		float fScale;
		//피아 및 중립 테스트
		//마지막 Scale 인자값에 아군이면 0~100 / 적군이면 +100 / 중립이면 +200을 한다.
		//셰이더코드에서 0~100사이이면 초록, 100보다 크면 -100 하고 빨강으로, 200보다 크면 -200하고 노랑으로 렌더링
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
		m_pSelectedCircleMesh->Render(pd3dDeviceContext);	//인스턴스로 그려볼까? 고민되네

		if ((int)OBJECT_TYPE::Mineral != m_ppSelectedObjects[0]->m_nObjectType && (int)OBJECT_TYPE::Gas != m_ppSelectedObjects[0]->m_nObjectType)
			m_pProgressImage->RenderHPImage(pd3dDeviceContext, m_ppSelectedObjects, m_nValidSelectedObjects);

	}

	pd3dDeviceContext->OMSetDepthStencilState(nullptr, 0);

	if (m_pStaticShader)		m_pStaticShader->Render(pd3dDeviceContext, pCamera);
	if (m_pAnimationShader)		m_pAnimationShader->Render(pd3dDeviceContext, pCamera);

	//UI용 Depth 설정
	if (m_pd3dUIDepthStencilState) pd3dDeviceContext->OMSetDepthStencilState(m_pd3dUIDepthStencilState, 0);


	//scv가 들고있는 자원 이미지
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::neutralresource);
	m_pNeutralResourceTexture->UpdateShaderVariable(pd3dDeviceContext);
	std::vector<D3DXVECTOR4> vResourcePosType;
	vResourcePosType.reserve(MAX_KINDS_OBJECTS);
	//1. 내 scv 중 들고있는애 찾기
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
		else //가스 들고 있음
		{
			vResourcePosType.emplace_back(vResourcePos, 2.0f);
		}
	}
	//2. 적 scv 중 들고있는애 찾기
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
			else //가스 들고 있음
			{
				vResourcePosType.emplace_back(vResourcePos, 2.0f);
			}
		}
	}
	m_pNeutralResourceMesh->UpdateNeutralResource(vResourcePosType, pd3dDeviceContext);
	m_pNeutralResourceMesh->Render(pd3dDeviceContext);


	//파티클 렌더링
	m_pParticleBox->Update(pd3dDeviceContext, m_fCurrentFrameElapsedTime);
	m_pParticleBox->Render(pd3dDeviceContext);

	//드래그 이벤트에 대한 UI 표현
	if (true == m_bMouseRectDraw)
	{
		OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::mouserect);
		//m_pMouseRectMesh->UpdateMouseRect(m_ptOldCursorPos, m_ptNextCursorPos, m_fWndClientWidth, m_fWndClientHeight, pd3dDeviceContext);
		m_pMouseRectMesh->UpdateMouseRect(m_d3dxvOldCursorPosition, m_d3dxvNextCursorPosition, pd3dDeviceContext);
		m_pMouseRectTexture->UpdateShaderVariable(pd3dDeviceContext);
		m_pMouseRectMesh->Render(pd3dDeviceContext);
	}

	//건물 짓기 이미지
	if (m_pBuildCheck && m_bActivedBuildCheck)
	{
		m_pBuildCheck->SetBuildingImage(m_strActivedBuildCheck, GetPositionPicking(m_ptMousePosition.x, m_ptMousePosition.y));
		m_pBuildCheck->Render(pd3dDeviceContext);
	}

#ifdef DEBUG_RENDERING_GRID
	//디버그 직선
	m_pDebugPrograms->Render(pd3dDeviceContext);
#endif

	//메인 프레임
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::mainframe);
	m_pMainFrameTexture->UpdateShaderVariable(pd3dDeviceContext);
	m_pMainFrameMesh->Render(pd3dDeviceContext);
	
	//미니맵
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::minimap);
	m_pMiniMapTexture->UpdateShaderVariable(pd3dDeviceContext);
	m_pMiniMapMesh->Render(pd3dDeviceContext);
	
	//미니맵 객체
	// 0 -> 아군
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::minimapobject);
	std::vector<D3DXVECTOR4> vPosType;
	vPosType.reserve(MAX_KINDS_OBJECTS * MAX_INSTANCE_OBJECTS);
	for (auto d : m_listMy)
		vPosType.emplace_back(d->GetPosition(), 0.0f);

	if (m_listEnemyInSight.size() > 0) // 1 -> 적군
	{
		for (auto d : m_listEnemyInSight)
			vPosType.emplace_back(d->GetPosition(), 1.0f);
	}
	//2 -> 중립유닛 ex 미네랄과 가스
	if (m_listNeutral.size() > 0) // 2 -> 미네랄
	{
		for (auto d : m_listNeutral)
			vPosType.emplace_back(d->GetPosition(), 2.0f);
	}
	m_pMiniMapObjectMesh->UpdateMiniMapObject(vPosType, pd3dDeviceContext);
	m_pMiniMapObjectMesh->Render(pd3dDeviceContext);

	//미니맵 사각형
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::minimaprect);
	m_pMiniMapRectTexture->UpdateShaderVariable(pd3dDeviceContext);
	m_pMiniMapRectMesh->UpdateMiniMapRect(m_pCamera->GetLookAtPosition(), pd3dDeviceContext);
	m_pMiniMapRectMesh->Render(pd3dDeviceContext);
	
	//메인 프레임에 붙는 attached
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

		
		//메인 프레임에 붙는 부대지정 저장 정보 렌더링
		m_pCrowdAttachedFrameTexture->UpdateShaderVariable(pd3dDeviceContext);
		for (int i = 0; i < 8; ++i)
		{
			if (0 == m_CrowdSaveData[i].nUnits) continue;
			
			m_vecAttachedCrowdMeshes[i]->Render(pd3dDeviceContext);
		}
		
		
		//status 혹은 군중 유닛 표시 및 초상화 렌더링
		if (0 < m_nValidSelectedObjects)
		{
			//일단 초상화 렌더링, 그냥 0번 놈을 대표로 한다.
			int type = m_ppSelectedObjects[0]->m_nObjectType;
			if (isEnemyObject(type)) type -= ADD_ENEMY_INDEX;
			auto iterPortraitTexture = m_mapPortraitTexture.find(type);
			if (iterPortraitTexture != m_mapPortraitTexture.end())
			{
				iterPortraitTexture->second->UpdateShaderVariable(pd3dDeviceContext);
				m_pPortraitMesh->Render(pd3dDeviceContext);
			}
			else //아마 못찾았다면 건물일 것이다.
			{
				m_mapPortraitTexture.find((int)OBJECT_TYPE::Commandcenter)->second->UpdateShaderVariable(pd3dDeviceContext);
				m_pPortraitMesh->Render(pd3dDeviceContext);
			}

			//status 창
			if (1 == m_nValidSelectedObjects) //1개 피킹때만 status표시, 2개 이상이라면 Crowd를 표시해야 한다.
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

					//만일 건물일 때, 아직 덜 지어졌을 경우 진행상태를 표시해야 한다.
				}

				if (!isEnemy && !isAnimateObject(type))
				{
					CStaticObject* pBuilding = (CStaticObject*)(m_ppSelectedObjects[0]);
					//아군 유닛 중 건물이라면 
					if (!pBuilding->GetCompleteBuild()) //아직 덜 지어졌을 경우 진행상태를 표기해야 한다.
					{
						string unitName = getNameByType(type);
						m_mapCommandTexture.find(unitName)->second->UpdateShaderVariable(pd3dDeviceContext);
						m_pProgressIconMesh->Render(pd3dDeviceContext);
						// 2. 진행상태 렌더링
						m_pProgressImage->RenderProgressImage(pd3dDeviceContext, pBuilding->m_fBuildTime / pBuilding->m_fCompleteBuildTime);
					}
					//현재 생산중인 유닛이 있다면 진행상태를 표시해야 한다. 다른 Shader를 Set하므로 마지막으로 뺐다. 
					else if (OBJECT_TYPE::Invalid != pBuilding->m_typeGeneratedUnit)
					{	//생산중인 유닛이 있다면 생산 유닛과 진행상태를 렌더링
						// 1. 유닛 아이콘 렌더링
						string unitName = getNameByType((int)pBuilding->m_typeGeneratedUnit);
						m_mapCommandTexture.find(unitName)->second->UpdateShaderVariable(pd3dDeviceContext);
						m_pProgressIconMesh->Render(pd3dDeviceContext);
						// 2. 진행상태 렌더링
						m_pProgressImage->RenderProgressImage(pd3dDeviceContext, pBuilding->m_fUnitGeneratedTime / pBuilding->m_fUnitCompleteTime);
					}
				}
			} //end of 1 == m_nSelectedObjects
			else if (2 <= m_nValidSelectedObjects) //군중 유닛 이미지 렌더링 시
			{
				// 1. 군중 유닛 frame 렌더링
				OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::crowdimage);
				m_pCrowdFrameTexture->UpdateShaderVariable(pd3dDeviceContext);
				int nSelected = m_nValidSelectedObjects > 24 ? 24 : m_nValidSelectedObjects;
				for (int i = 0; i < nSelected; ++i)
				{
					m_vecCrowdMeshes[i]->UpdateCrowdColor(pd3dDeviceContext, 0.0f);
					m_vecCrowdMeshes[i]->Render(pd3dDeviceContext);
				}

				//2. 유닛 별 렌더링
				set<OBJECT_TYPE> renderedObject;
				OBJECT_TYPE obj_type, another_obj_type;
				for (int i = 0; i < nSelected; ++i)
				{
					obj_type = (OBJECT_TYPE)m_ppSelectedObjects[i]->m_nObjectType;
					if (renderedObject.end() != renderedObject.find(obj_type)) continue; // 이미 그렸다.

					renderedObject.insert(obj_type);
					int render_obj_type = isEnemyObject((int)obj_type) ? (int)obj_type - ADD_ENEMY_INDEX : (int)obj_type;
					m_mapCrowdUnitTexture.find(OBJECT_TYPE(render_obj_type))->second->UpdateShaderVariable(pd3dDeviceContext);

					for (int j = i; j < nSelected; ++j)
					{
						another_obj_type = (OBJECT_TYPE)m_ppSelectedObjects[j]->m_nObjectType;
						if (obj_type != another_obj_type) continue; //다른 유닛이다.

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

	//폰트 이미지
	bool bDisplayRequire = false;
	if (m_pNumberFont)
	{
		m_pNumberFont->UpdateNumber(string("screen_mineral"), gGameMineral);
		m_pNumberFont->UpdateNumber(string("screen_gas"), gGameGas);
		
		//체력 표시 -> 1003 1 마리 피킹때만
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
		
		//생산 혹은 짓는 경우
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

		//부대 지정 저장 표시
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

	//자원 이미지 표시
	//if (m_nValidSelectedObjects <= 0)
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::attached);
	m_mapCommandTexture.find(string("mineral"))->second->UpdateShaderVariable(pd3dDeviceContext);
	m_mapCommandMesh.find(string("mineral"))->second->Render(pd3dDeviceContext);
	if (bDisplayRequire) m_mapCommandMesh.find(string("need_mineral"))->second->Render(pd3dDeviceContext);
	m_mapCommandTexture.find(string("gas"))->second->UpdateShaderVariable(pd3dDeviceContext);
	m_mapCommandMesh.find(string("gas"))->second->Render(pd3dDeviceContext);
	if (bDisplayRequire) m_mapCommandMesh.find(string("need_gas"))->second->Render(pd3dDeviceContext);


	//마우스 커서 이미지는 마지막에 그려지는게 합당할듯? 셰이더는 MainFrame것을 사용한다.
	OnPrepareUIRender(pd3dDeviceContext, UI_RENDERING::mainframe);
	m_pMouseImageTexture->UpdateShaderVariable(pd3dDeviceContext);
	float xScreen, yScreen;
	xScreen = ((float)m_ptMousePosition.x / m_fWndClientWidth) * 2.0f - 1.0f;
	yScreen = ((float)(m_fWndClientHeight - m_ptMousePosition.y) / m_fWndClientHeight) * 2.0f - 1.0f;
	m_pMouseImageMesh->UpdateScreenPosition(pd3dDeviceContext, xScreen, yScreen);
	m_pMouseImageMesh->Render(pd3dDeviceContext);



	//0901 디버그 큐브 테스트
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
	//UI Depth 설정을 끈다.
}



void CScene::CreateNewObject(OBJECT_TYPE obj_type, D3DXVECTOR3 vMadenPosition)
{
	if (false == isAnimateObject((int)obj_type))
	{ //건물 짓기
		int nodeIndex = m_pAStarComponent->GetNearstNodeIndex(vMadenPosition);

		//자원 감소는 Put Building 패킷에서 처리하도록 한다. Fail Build가 생길 수 있기 때문이다.
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
				printf("Commandcenter을(를) 건설합니다.\n");
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
				printf("Supplydepot을(를) 건설합니다.\n");
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
				//0903 리파이너리는 가스 위에 지으려고 하는지를 확인해야 한다.
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
					printf("Refinery을(를) 건설합니다.\n");
					CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Refinery);
				}
				else
				{
					printf("가스 위에다만 지을 수 있다!\n");
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
				printf("Barrack을(를) 건설합니다.\n");
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
				printf("Engineeringbay을(를) 건설합니다.\n");
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
				printf("Sensortower을(를) 건설합니다.\n");
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
				printf("Missileturret을(를) 건설합니다.\n");
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
				printf("Bunker을(를) 건설합니다.\n");
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
				printf("Factory을(를) 건설합니다.\n");
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
				printf("Starport을(를) 건설합니다.\n");
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
				printf("Armory을(를) 건설합니다.\n");
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
				printf("Ghostacademy을(를) 건설합니다.\n");
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
				printf("Fusioncore을(를) 건설합니다.\n");
				CS_Packet_Create_Building(m_ppSelectedObjects[0], nodeIndex, OBJECT_TYPE::Fusioncore);
			}
			break;
		}
		}

		m_pBuildCheck->InvalidateBuildingImage();
		m_nSceneState = SCENE_STATE::pick_scv;
		m_bActivedBuildCheck = false;
	}
	else //유닛 생산
	{
		CStaticObject* pBuilding = (CStaticObject*)(m_ppSelectedObjects[0]);	//0번이 현재 건물일 것이다.
		
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
					printf("Scv을(를) 생산합니다.\n");
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
					printf("Marine을(를) 생산합니다.\n");
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
					printf("Reaper을(를) 생산합니다.\n");
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
					printf("Marauder을(를) 생산합니다.\n");
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
					printf("Thor을(를) 생산합니다.\n");
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
					printf("Viking을(를) 생산합니다.\n");
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
					printf("Medivac을(를) 생산합니다.\n");
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
					printf("Banshee을(를) 생산합니다.\n");
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
					printf("Raven을(를) 생산합니다.\n");
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
					printf("BattleCruiser을(를) 생산합니다.\n");
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