#include "stdafx.h"
#include "Object.h"

extern int gGameMineral;
extern int gGameGas;
extern void* gGameCamera;

//**********************************************************************
CGameObject::CGameObject(int nMeshes)
{
	D3DXMatrixIdentity(&m_d3dxmtxLocal);
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	m_bHasTargetPosition = false;

	m_nMeshes = nMeshes;
	m_ppMeshes = NULL;
	if (m_nMeshes > 0) m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = NULL;

	m_nReferences = 0;
	m_bActive = true;

	m_nObjectID = -1;
	m_nObjectType = -1;

	m_listTargetPosition.clear();
}

CGameObject::~CGameObject()
{
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Release();
			m_ppMeshes[i] = NULL;
		}
		delete[] m_ppMeshes;
	}

	m_listTargetPosition.clear();
	m_listUI.clear();
}

//**********************************************************************
//오브젝트의 피킹 처리 함수입니다.
void CGameObject::GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection)
{
	//pd3dxvPickPosition: 카메라 좌표계의 점(화면 좌표계에서 마우스를 클릭한 점을 역변환한 점)
	//pd3dxmtxWorld: 월드 변환 행렬, pd3dxmtxView: 카메라 변환 행렬
	//pd3dxvPickRayPosition: 픽킹 광선의 시작점, pd3dxvPickRayDirection: 픽킹 광선 벡터
	
	/*객체의 월드 변환 행렬이 주어지면 객체의 월드 변환 행렬과 카메라 변환 행렬을 곱하고 역행렬을 구한다. 
	이것은 카메라 변환 행렬의 역행렬과 객체의 월드 변환 행렬의 역행렬의 곱과 같다. 
	객체의 월드 변환 행렬이 주어지지 않으면 카메라 변환 행렬의 역행렬을 구한다. 
	객체의 월드 변환 행렬이 주어지면 모델 좌표계의 픽킹 광선을 구하고 
	그렇지 않으면 월드 좌표계의 픽킹 광선을 구한다.*/
	D3DXMATRIX d3dxmtxInverse;
	D3DXMATRIX d3dxmtxWorldView = *pd3dxmtxView;
	if (pd3dxmtxWorld) D3DXMatrixMultiply(&d3dxmtxWorldView, pd3dxmtxWorld, pd3dxmtxView);
	D3DXMatrixInverse(&d3dxmtxInverse, NULL, &d3dxmtxWorldView);
	D3DXVECTOR3 d3dxvCameraOrigin(0.0f, 0.0f, 0.0f);
	/*카메라 좌표계의 원점 (0, 0, 0)을 위에서 구한 역행렬로 변환한다. 변환의 결과는 
	카메라 좌표계의 원점에 대응되는 모델 좌표계의 점 또는 월드 좌표계의 점이다.*/
	D3DXVec3TransformCoord(pd3dxvPickRayPosition, &d3dxvCameraOrigin, &d3dxmtxInverse);
	/*카메라 좌표계의 점을 위에서 구한 역행렬로 변환한다. 변환의 결과는 
	마우스를 클릭한 점에 대응되는 모델 좌표계의 점 또는 월드 좌표계의 점이다.*/
	D3DXVec3TransformCoord(pd3dxvPickRayDirection, pd3dxvPickPosition, &d3dxmtxInverse);
	//픽킹 광선의 방향 벡터를 구한다.
	*pd3dxvPickRayDirection = *pd3dxvPickRayDirection - *pd3dxvPickRayPosition;
}

int CGameObject::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	//pd3dxvPickPosition: 카메라 좌표계의 점(화면 좌표계에서 마우스를 클릭한 점을 역변환한 점)
	//pd3dxmtxView: 카메라 변환 행렬
	D3DXVECTOR3 d3dxvPickRayPosition, d3dxvPickRayDirection;
	int nIntersected = 0;
	int nResultIntersected = 0; //충돌 여부만을 파악하도록 하는 변수
	//활성화된 객체에 대하여 메쉬가 있으면 픽킹 광선을 구하고 객체의 메쉬와 충돌 검사를 한다.
	
	if (m_bActive && m_ppMeshes)
	{
		//피킹 광선의 위치와 방향을 메시, 즉 모델좌표계로 변환한다.
		GenerateRayForPicking(pd3dxvPickPosition, &m_d3dxmtxWorld, pd3dxmtxView, &d3dxvPickRayPosition, &d3dxvPickRayDirection);

		for (int i = 0; i < m_nMeshes; i++)
		{
			nIntersected = m_ppMeshes[i]->CheckRayIntersection(&d3dxvPickRayPosition, &d3dxvPickRayDirection, pd3dxIntersectInfo);

			//충돌이 된 경우, 바로 끝내지 않고 모든 메시를 순회하여 정확한 근접 메시를 찾도록 한다. z-오더링 한것도 아니니까
			if (nIntersected > 0)
				nResultIntersected = 1; // 하나라도 충돌된다.
		}
	}
	return(nResultIntersected);
}

//**********************************************************************
//오브젝트의 정보 세팅
void CGameObject::SetMesh(CMesh *pMesh, int nIndex = 0)
{
	if (m_ppMeshes)
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex]->Release();
		m_ppMeshes[nIndex] = pMesh;
		if (pMesh) pMesh->AddRef();
	}
	if (pMesh)
	{
		AABB bcBoundingCube = pMesh->GetBoundingCube();
		m_bcMeshBoundingCube.Union(&bcBoundingCube);
	}
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_d3dxmtxWorld._41 = x;
	m_d3dxmtxWorld._42 = y;
	m_d3dxmtxWorld._43 = z;
}

void CGameObject::SetPosition(D3DXVECTOR3 pos)
{
	m_d3dxmtxWorld._41 = pos.x;
	m_d3dxmtxWorld._42 = pos.y;
	m_d3dxmtxWorld._43 = pos.z;
}

D3DXVECTOR3 CGameObject::GetPosition()
{
	return D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43);
}

bool CGameObject::IsVisible(CCamera *pCamera)
{
	bool bIsVisible = false;
	if (m_bActive)
	{
		AABB bcBoundingCube = m_bcMeshBoundingCube;
		bcBoundingCube.Update(&m_d3dxmtxWorld);
		if (pCamera) bIsVisible = pCamera->IsInFrustum(&bcBoundingCube);
	}
	return(bIsVisible);
}

void CGameObject::RotateToTarget(CGameObject* pTarget)
{
	RotateToTarget(pTarget->GetPosition());
}

void CGameObject::RotateToTarget(D3DXVECTOR3 vTarget)
{
	//이동 및 회전
	D3DXVECTOR3 vNow = GetPosition();
	D3DXVECTOR3 vNext = vTarget;

	if (vNow == vNext) return;

	vNow.y = vNext.y = 0.0f;
	
	D3DXMATRIX mtxWorld = m_d3dxmtxLocal * m_d3dxmtxWorld;

	D3DXVECTOR3 prevUp = D3DXVECTOR3(mtxWorld._21, mtxWorld._22, mtxWorld._23);
	D3DXVECTOR3 prevLook = D3DXVECTOR3(mtxWorld._31, mtxWorld._32, mtxWorld._33);

	D3DXVECTOR3 toTarget = vNext - vNow;
	D3DXVec3Normalize(&toTarget, &toTarget);

	m_d3dxmtxWorld._21 = 0.0f;
	m_d3dxmtxWorld._22 = 1.0f;
	m_d3dxmtxWorld._23 = 0.0f;
	m_d3dxmtxWorld._31 = toTarget.x;
	m_d3dxmtxWorld._32 = toTarget.y;
	m_d3dxmtxWorld._33 = toTarget.z;

	D3DXVECTOR3 vUp = D3DXVECTOR3(0, 1, 0);

	D3DXVECTOR3 vRight;
	D3DXVec3Cross(&vRight, &vUp, &toTarget);
	m_d3dxmtxWorld._11 = vRight.x;
	m_d3dxmtxWorld._12 = vRight.y;
	m_d3dxmtxWorld._13 = vRight.z;
}


//**********************************************************************
//이동 관련
void CGameObject::MoveForward(float fDistance)
{
	//게임 객체를 로컬 z-축 방향으로 이동한다.
	D3DXVECTOR3 d3dxvPosition = GetPosition();
	D3DXVECTOR3 d3dxvLookAt(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33);
	D3DXVec3Normalize(&d3dxvLookAt, &d3dxvLookAt);

	d3dxvPosition += fDistance * d3dxvLookAt;
	CGameObject::SetPosition(d3dxvPosition);
}

void CGameObject::MoveDirection(D3DXVECTOR3 normalizedDir, float fDistance)
{
	m_d3dxmtxWorld._41 += normalizedDir.x * fDistance;
	m_d3dxmtxWorld._42 += normalizedDir.y * fDistance;
	m_d3dxmtxWorld._43 += normalizedDir.z * fDistance;
}


//**********************************************************************
//패킷 통신 관련 처리
void CGameObject::CS_Packet_State(State state)
{
#ifdef DEBUG_NOT_CONNECT_SERVER
	sc_packet_data_object_state packet;
	packet.state = state;
	Process_SC_State(&packet);
	return;
#endif
	
	//m_bLastPacketProcessed = false;

	m_bLastStatePacketProcessed = false;

	cs_packet_data_object_state* my_packet = reinterpret_cast<cs_packet_data_object_state *>(CSocketComponent::GetInstance()->m_SendBuf);
	my_packet->size = sizeof(cs_packet_data_object_state);
	my_packet->type = CS_DATA_OBJECT_STATE;
	my_packet->room_id = CSocketComponent::GetInstance()->m_roomID;
	my_packet->object_id = m_nObjectID;
	my_packet->object_type = (OBJECT_TYPE)m_nObjectType;
	my_packet->state = state;
	if (m_pTargetObject)
		my_packet->target_id = m_pTargetObject->m_nObjectID;

	CSocketComponent::GetInstance()->SendPacket(reinterpret_cast<unsigned char*>(my_packet));

	printf("%d 유닛이 CS_Packet_State를 보냄, State : %d\n", m_nObjectID, (int)state);

	//test
	//m_ObjectState = state;
}

void CGameObject::Process_SC_State(sc_packet_data_object_state* packet)
{
	m_bLastStatePacketProcessed = true;

	//m_bLastPacketProcessed = true;
	//State state = packet->state;
	printf("%d는 State변경을 받았다. 상태 : %d\n", m_nObjectID, (int)packet->state);
	//m_ObjectState = state;
}

//**********************************************************************
//프레임 갱신 처리
void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera)
{
	//연결된 메쉬를 렌더링한다.
	//월드 좌표와 재질 값과 텍스처는 셰이더에서 하도록 구성함.
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i])
			{
				bool bIsVisible = true;
				if (pCamera)
				{
					AABB bcBoundingCube = m_ppMeshes[i]->GetBoundingCube();
					bcBoundingCube.Update(&m_d3dxmtxWorld);
					bIsVisible = pCamera->IsInFrustum(&bcBoundingCube);
				}
				if (bIsVisible) m_ppMeshes[i]->Render(pd3dDeviceContext);
			}
		}
	}
}



//**********************************************************************
//높이맵 지형
CHeightMapTerrain::CHeightMapTerrain(ID3D11Device *pd3dDevice, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, D3DXVECTOR3 d3dxvScale) : CGameObject(0)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	int cxQuadsPerBlock = nBlockWidth - 1;
	int czQuadsPerBlock = nBlockLength - 1;

	m_d3dxvScale = d3dxvScale;

	m_pHeightMap = new CHeightMap(pFileName, nWidth, nLength, m_d3dxvScale);

	//long cxBlocks = (m_nWidth - 1) / cxQuadsPerBlock;
	//long czBlocks = (m_nLength - 1) / czQuadsPerBlock;
	long cxBlocks = (m_nWidth ) / cxQuadsPerBlock;		// 205 / 5의 몫 -> 41
	long czBlocks = (m_nLength ) / czQuadsPerBlock;

	m_nMeshes = cxBlocks * czBlocks;
	m_ppMeshes = new CMesh*[m_nMeshes];
	for (int i = 0; i < m_nMeshes; i++)m_ppMeshes[i] = NULL;

	CHeightMapGridMesh *pHeightMapGridMesh = NULL;
	int xStart, zStart;
	for (int z = 0; z < czBlocks; z++)
	{
		for (int x = 0; x < cxBlocks; x++)
		{
			xStart = x * (nBlockWidth - 1) -int(MAP_WIDTH / 2.0f);
			zStart = z * (nBlockLength - 1) -int(MAP_HEIGHT / 2.0f);
			pHeightMapGridMesh = new CHeightMapGridMesh(pd3dDevice, xStart, zStart, nBlockWidth, nBlockLength, d3dxvScale, m_pHeightMap);
			SetMesh(pHeightMapGridMesh, x + (z*cxBlocks));
		}
	}

	//test 
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	pd3dDevice->CreateRasterizerState(&rd, &m_pd3dRasterizerState);
}

CHeightMapTerrain::~CHeightMapTerrain()
{
	if (m_pHeightMap) delete m_pHeightMap;
	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();
}


void CHeightMapTerrain::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CGameObject::Render(pd3dDeviceContext, pCamera);
}





//**********************************************************************
//정적 오브젝트 설정 -> 건물 및 자원
CStaticObject::CStaticObject() : CGameObject(1)
{
}

CStaticObject::~CStaticObject()
{
}


void CStaticObject::UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList)
{
	float y = pHeightMap->GetHeight(m_d3dxmtxWorld._41, m_d3dxmtxWorld._43, false);
	
	//m_fBuildTime += fTimeElapsed; //-> 서버에서 값을 준다. 굳이 줄 필요는 없나?
		
	float ratio = (m_fBuildTime / m_fCompleteBuildTime);
	ratio = ratio > 1.0f ? 1.0f : ratio;
	
	m_d3dxmtxWorld._42 = y * ratio;

	if (m_fBuildTime < m_fCompleteBuildTime)
	{	//건물이 덜 지어진 상태라면 
		return;
	}

	if (m_typeGeneratedUnit != OBJECT_TYPE::Invalid)
	{
		m_fUnitGeneratedTime += fTimeElapsed;
		if (m_fUnitCompleteTime <= m_fUnitGeneratedTime)
		{	//유닛 생성 요청 패킷 전송
			
			//0907 유닛 위치 자리잡기
			//생성되어지는 유효한 위치를 얻는다.
			D3DXVECTOR3 vOffset = GetPosition() + D3DXVECTOR3(-2, 0, -2);
			bool bStand = false;
			int iOffset = AStarComponent::GetInstance()->GetNearstNodeIndex(vOffset);
			int obj_idx;
			for (auto d : *allyList)
			{
				obj_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(d->GetPosition());
				//공중vs지상 혹은 지상vs공중인 경우 충돌이 아니다
				if (!(d->m_bAirUnit && isAirObject(((int)m_typeGeneratedUnit))))
				{
					;
				}
				if (obj_idx == iOffset)
				{
					iOffset += 1;
				}
			}
			for (auto d : *enemyList)
			{
				obj_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(d->GetPosition());
				//공중vs지상 혹은 지상vs공중인 경우 충돌이 아니다
				if (!(d->m_bAirUnit && isAirObject(((int)m_typeGeneratedUnit))))
				{
					;
				}
				if (obj_idx == iOffset)
				{
					iOffset += 1;
				}
			}
			Position pos = AStarComponent::GetInstance()->GetPositionByIndex(iOffset);
			float yPos = pHeightMap->GetHeight(pos.x, pos.z);
			vOffset = D3DXVECTOR3(pos.x, yPos, pos.z);
			//수정코드 끝부분

			//CS_Packet_Create_Unit(m_typeGeneratedUnit, GetPosition() + D3DXVECTOR3(-2, 0,-2));	//offset은 유동적으로 다시 해야함.
			CS_Packet_Create_Unit(m_typeGeneratedUnit, vOffset);	//offset은 유동적으로 다시 해야함.
			CancelGenerateUnit();
		}
	}
}

void CStaticObject::Process_SC_Value(sc_packet_data_object_value* packet)
{
	if(m_fBuildTime != m_fCompleteBuildTime)	//짓는 경우가 아닌 체력 변경 시에 대한 처리
		m_fBuildTime = packet->building_time;
	
	m_nHP = packet->hp;

	if(packet->building_time == m_fCompleteBuildTime && CSocketComponent::GetInstance()->m_clientID == packet->client_id)
		CSoundManager::GetInstance()->PlayEffectSound(start_offset::job_finish);
}

void CStaticObject::Process_SC_State(sc_packet_data_object_state* packet)
{
	m_bLastStatePacketProcessed = true;

	if (State::death != packet->state)
		return;

	//폭발 애니메이션 진행
	int nCreatedParticle = rand() % 10 + 2;
	for (int i = 0; i < nCreatedParticle; ++i)
		AddParticleImage();
	
	if ((int)OBJECT_TYPE::Refinery == m_nObjectType || (int)OBJECT_TYPE::Refinery_Enemy == m_nObjectType)
	{
		if(m_pBeforeGasModel)
			m_pBeforeGasModel->SetActive(true);
	}

	m_bDeath = true;
	SetActive(false);

	//1214 사운드 수정 -> 카메라와 멀리 떨어져 있다면 사운드 재생 안하도록
	CCamera* pCamera = (CCamera*)gGameCamera;
	D3DXVECTOR3 camLookPosition = pCamera->GetLookAtPosition();
	D3DXVECTOR3 vSub = camLookPosition - D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43);
	if (D3DXVec3Length(&vSub) <= CAMERA_UNIT_SOUND_DISTANCE)
	{
		int obj_type = (int)packet->object_type;
		int nSoundOffset = getSoundOffsetByType(obj_type);
		CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset + UNIT_DEATH_OFFSET);
	}
}


void CStaticObject::CS_Packet_Create_Unit(OBJECT_TYPE obj_type, D3DXVECTOR3 pos)
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

bool CStaticObject::GenerateUnit(OBJECT_TYPE obj_type, float fCompleteTime)
{
	if (!GetCompleteBuild() || (m_typeGeneratedUnit != OBJECT_TYPE::Invalid))
		return false;		//아직 덜 지어졌거나 현재 생산중인 유닛이 있다면 생성되지 않는다.

	m_typeGeneratedUnit = obj_type;
	m_fUnitCompleteTime = fCompleteTime;
	m_fUnitGeneratedTime = 0.0f;
	return true;
}

void CStaticObject::AddParticleImage()
{
	CPromptParticleObject promptParticle;

	promptParticle.m_d3dxmtxWorld = m_d3dxmtxLocal * m_d3dxmtxWorld;
	promptParticle.m_pTargetObject = this;
	promptParticle.m_pMyObject = m_PromptParticle.m_pMyObject;

	//m_PromptParticle.m_d3dxmtxWorld = m_d3dxmtxLocal * m_d3dxmtxWorld;
	//m_PromptParticle.m_fMaxParticleTime = 0.9f;
	//m_PromptParticle.m_pTargetObject = this;

	//임의의 즉발 위치를 만들어낸다.
	D3DXVECTOR3 vMin, vMax;
	vMin = this->m_bcMeshBoundingCube.m_d3dxvMinimum;
	vMax = this->m_bcMeshBoundingCube.m_d3dxvMaximum;

	float xRatio = (float)rand() / (float)RAND_MAX;
	float yRatio = (float)rand() / (float)RAND_MAX;
	float zRatio = (float)rand() / (float)RAND_MAX;

	//m_PromptParticle.m_fRandXOffset = vMin.x * xRatio + vMax.x * (1.0f - xRatio);
	//m_PromptParticle.m_fRandYOffset = vMin.y * yRatio + vMax.y * (1.0f - yRatio);
	//m_PromptParticle.m_fRandZOffset = vMin.z * zRatio + vMax.z * (1.0f - zRatio);
	promptParticle.m_fRandXOffset = vMin.x * xRatio + vMax.x * (1.0f - xRatio);
	promptParticle.m_fRandYOffset = vMin.y * yRatio + vMax.y * (1.0f - yRatio);
	promptParticle.m_fRandZOffset = vMin.z * zRatio + vMax.z * (1.0f - zRatio);

	int explosion_seed = rand() % 2;
	if (0 == explosion_seed)
	{
		auto pParticle = CSpriteParticle::GetInstance()->m_mapParticleData.find(string("explosion_1"));
		if (pParticle != CSpriteParticle::GetInstance()->m_mapParticleData.end())
		{
			promptParticle.m_fMaxParticleTime = pParticle->second.m_fPromptMaintainTime;
			promptParticle.m_fParticleElapsedTime = zRatio * promptParticle.m_fMaxParticleTime;
			CSpriteParticle::GetInstance()->AddPromptParticle(string("explosion_1"), promptParticle);
		}
	}
	else
	{
		auto pParticle = CSpriteParticle::GetInstance()->m_mapParticleData.find(string("explosion_2"));
		if (pParticle != CSpriteParticle::GetInstance()->m_mapParticleData.end())
		{
			promptParticle.m_fMaxParticleTime = pParticle->second.m_fPromptMaintainTime;
			promptParticle.m_fParticleElapsedTime = zRatio * promptParticle.m_fMaxParticleTime;
			CSpriteParticle::GetInstance()->AddPromptParticle(string("explosion_2"), promptParticle);
		}
	}
}

//**********************************************************************
//애니메이션 오브젝트 설정
CAnimationObject::CAnimationObject() : CGameObject(1)
{
	m_fTotalAnimationElapsedTime = 0.0f;
	
	m_mapAnimationSet.clear();
	m_vecFinalTransforms.resize(MAX_BONE_COUNT);

	m_strAnimationClip = "Take_001";
	m_strCurrentAnimation = "Idle";
}

CAnimationObject::~CAnimationObject()
{
	m_vecFinalTransforms.clear();
}

void CAnimationObject::SetMesh(CMesh* pMesh)
{
	CGameObject::SetMesh(pMesh);
}

void CAnimationObject::SetAnimation(string& animName)
{
	auto iter = m_mapAnimationSet.find(animName);

	if (iter == m_mapAnimationSet.end())
	{
		printf("잘못된 애니메이션 이름입니다.\n");
	}
	else if (animName == m_strCurrentAnimation)
	{
		//printf("같은 애니메이션을 호출했으니 그대로 진행합니다.");
	}
	else //애니메이션을 찾았다면
	{
		m_strCurrentAnimation = animName;
		CAnimationMesh* pMesh = (CAnimationMesh*)m_ppMeshes[0];
		int startKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.first;
		m_fTotalAnimationElapsedTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, startKeyFrameNum);
	}
}

void CAnimationObject::SetTargetPosition(D3DXVECTOR3 target)
{
	m_d3dxvTargetPosition = D3DXVECTOR3(target.x, 0.0f, target.z);
	m_bHasTargetPosition = true;

	//string strAnim = "Move";
	//SetAnimation(strAnim);
	
	//State를 move로 바꾸는 행동은 RBUTTON 클릭밖에 없다.
	//m_ObjectState = State::move;
}

void CAnimationObject::FreeTargetPosition()
{
	m_bHasTargetPosition = false;
	m_listTargetPosition.clear();
	m_d3dxvTargetPosition = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);

	//string strAnim = "Idle";
	//SetAnimation(strAnim);
	
	CS_Packet_State(State::stop);
}

void CAnimationObject::Animate(float fTimeElapsed)
{
	//기존 애니메이션 코드
	/*
	CAnimationMesh* pMesh = (CAnimationMesh*)m_ppMeshes[0];

	m_fAnimationElapsedTime += fTimeElapsed;

	pMesh->m_pSkinnedData->GetFinalTransforms(m_strAnimationClip, m_fAnimationElapsedTime, m_vecFinalTransforms);

	int startKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.first;
	int endKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.second;

	float endTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, endKeyFrameNum);
	if (m_fAnimationElapsedTime >= endTime)
	m_fAnimationElapsedTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, startKeyFrameNum);
	*/

	//UpdateObject가 30프레임으로 진행되다보니 불일치가 발생한다.
	//-> 애니메이션은 이미 완료이나 UpdateObject는 0.0333이 되지 않아 갱신되지 않는 현상이라 생각하면 됨
	
	if (m_fLastSendPacketTime < 0.033333f)
		return;

	CAnimationMesh* pMesh = (CAnimationMesh*)m_ppMeshes[0];

	//if(State::attack == m_ObjectState)
	//	m_fAnimationElapsedTime += m_fLastSendPacketTime * m_fAttackSpeed;
	//else //평상시
		//m_fTotalAnimationElapsedTime += m_fLastSendPacketTime;

	m_fTotalAnimationElapsedTime += m_fAnimationElapsedTime;

	pMesh->m_pSkinnedData->GetFinalTransforms(m_strAnimationClip, m_fTotalAnimationElapsedTime, m_vecFinalTransforms);

	int startKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.first;
	int endKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.second;

	float endTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, endKeyFrameNum);
	if (m_fTotalAnimationElapsedTime >= endTime)
	{
		if (m_strCurrentAnimation == string("Die"))
			SetActive(false);
		else
			m_fTotalAnimationElapsedTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, startKeyFrameNum);
	}
	//애니메이션에서 초기화
	m_fLastSendPacketTime = 0.0f;
}


void CAnimationObject::UpdateMoveEvent(CHeightMap* pHeightMap)
{
	//이동 관련 처리
	//if (m_bLastPacketProcessed && (!m_listTargetPosition.empty() || m_bHasTargetPosition))
	if ((!m_listTargetPosition.empty() || m_bHasTargetPosition))
	{
		D3DXVECTOR3 vNext = m_d3dxvTargetPosition;
		D3DXVECTOR3 nowPosition = GetPosition();
		vNext.y = 0.0f; nowPosition.y = 0.0f;	//x-z 평면으로만 이동 계산을 한다.

		D3DXVECTOR3 toTarget = vNext - nowPosition;
		D3DXVec3Normalize(&toTarget, &toTarget);

		//이동한 다음 위치를 얻는다.
		nowPosition.x += toTarget.x * m_fLastSendPacketTime * m_fObjectSpeed;
		nowPosition.z += toTarget.z * m_fLastSendPacketTime * m_fObjectSpeed;

		//지터링 처리 및 다음 목표 위치 갱신
		if (D3DXVec3Length(&(vNext - nowPosition)) <= (m_fObjectSpeed * m_fLastSendPacketTime) / 2.0f)
		{
			nowPosition = vNext;
			m_listTargetPosition.pop_front();

			if (!m_listTargetPosition.empty())
			{
				m_d3dxvTargetPosition = D3DXVECTOR3(m_listTargetPosition.front().x, 0.0f, m_listTargetPosition.front().z);
				m_bHasTargetPosition = true;
			}
			else
			{
				if(!isEnemyObject(m_nObjectType))
					FreeTargetPosition();
				else
				{
					m_bHasTargetPosition = false;
					m_listTargetPosition.clear();
					m_d3dxvTargetPosition = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);

					//string strAnim = "Idle";
					//SetAnimation(strAnim);
					//not CS_Packet_State
				}
			}
		}
		nowPosition.y = pHeightMap->GetHeight(nowPosition.x, nowPosition.z, false);// + (m_bcMeshBoundingCube.m_d3dxvMaximum.y / 2.0f);
		
		if (!isEnemyObject(m_nObjectType))
			CS_Packet_Move(nowPosition, false);
		else
			Process_SC_Move(nowPosition); 

		//printf("%f, %f, %f\n", nowPosition.x, nowPosition.y, nowPosition.z);
	}
}

void CAnimationObject::CS_Packet_Move(D3DXVECTOR3 vTo, bool bRightClicked)
{
#ifdef DEBUG_NOT_CONNECT_SERVER
	Process_SC_Move(vTo);
	return;
#endif

	m_bLastPacketProcessed = false;

	cs_packet_move_object* my_packet = reinterpret_cast<cs_packet_move_object *>(CSocketComponent::GetInstance()->m_SendBuf);
	my_packet->size = sizeof(cs_packet_move_object);
	my_packet->type = CS_MOVE_OBJECT;
	my_packet->room_id = CSocketComponent::GetInstance()->m_roomID;
	my_packet->object_id = m_nObjectID;
	my_packet->object_type = (OBJECT_TYPE)m_nObjectType;
	my_packet->client_id = CSocketComponent::GetInstance()->m_clientID;
	my_packet->x = vTo.x;
	my_packet->y = vTo.y;
	my_packet->z = vTo.z;
	my_packet->b_click_rbutton = bRightClicked;

	CSocketComponent::GetInstance()->SendPacket(reinterpret_cast<unsigned char*>(my_packet));

	if (bRightClicked)
		printf("%d 유닛은 Scene::Move패킷을 보냈다. right : true\n", m_nObjectID);
	else
		printf("%d 유닛은 Scene::Move패킷을 보냈다. right : false\n", m_nObjectID);

	/*
	my_packet->move_reset = m_bLatestFindPath;
	if (true == m_bLatestFindPath) m_bLatestFindPath = false;

	//test
	if (m_listTargetPosition.empty())
		my_packet->nodeindex = -1;
	else
		my_packet->nodeindex = AStarComponent::GetInstance()->GetNearstNodeIndex(m_listTargetPosition.front());
	*/

	
	//0812 본인은 바로 움직인다. 잘못된 길이라면 서버가 알려줄것이다.
	//Process_SC_Move(vTo);
}

void CAnimationObject::Process_SC_Move(D3DXVECTOR3 vPos)
{
	//printf("%d 유닛은 Process_SC_Move를 받았다.\n", m_nObjectID);

	//SetAnimation(string("Move"));
	//m_ObjectState = State::move;

	m_bLastPacketProcessed = true;

	//이동 및 회전
	D3DXVECTOR3 vNow = GetPosition();
	D3DXVECTOR3 vNext = vPos;

	if (vNow == vNext)		return;
	
	vNow.y = vNext.y = 0.0f;
	D3DXMATRIX mtxWorld = m_d3dxmtxLocal * m_d3dxmtxWorld;

	D3DXVECTOR3 prevUp = D3DXVECTOR3(mtxWorld._21, mtxWorld._22, mtxWorld._23);
	D3DXVECTOR3 prevLook = D3DXVECTOR3(mtxWorld._31, mtxWorld._32, mtxWorld._33);

	D3DXVECTOR3 toTarget = vNext - vNow;
	D3DXVec3Normalize(&toTarget, &toTarget);

	m_d3dxmtxWorld._21 = 0.0f;
	m_d3dxmtxWorld._22 = 1.0f;
	m_d3dxmtxWorld._23 = 0.0f;
	m_d3dxmtxWorld._31 = toTarget.x;
	m_d3dxmtxWorld._32 = toTarget.y;
	m_d3dxmtxWorld._33 = toTarget.z;

	D3DXVECTOR3 vUp = D3DXVECTOR3(0, 1, 0);

	D3DXVECTOR3 vRight;
	D3DXVec3Cross(&vRight, &vUp, &toTarget);
	m_d3dxmtxWorld._11 = vRight.x;
	m_d3dxmtxWorld._12 = vRight.y;
	m_d3dxmtxWorld._13 = vRight.z;

	SetPosition(vPos);
	//printf("갱신 위치 %f, %f, %f // idx : %d\n", vPos.x, vPos.y, vPos.z, AStarComponent::GetInstance()->GetNearstNodeIndex(vPos));


	if(State::have_target == m_ObjectState)
		++m_nElapsedMoveCount;
}

//0812 Enemy Update
void CAnimationObject::Process_SC_Move_Enemy(D3DXVECTOR3 vPos)
{
	m_bLastPacketProcessed = true;

	RotateToTarget(vPos);
	m_listTargetPosition.push_back(vPos);
	SetTargetPosition(m_listTargetPosition.front());
}

//임시
void CAnimationObject::BeginParticleImage(CGameObject* pTarget)
{
	//파티클은 전체적으로 y축을 기준으로 +90도 돌아가있다. 고로 90도 돌려줘야 방향 제대로 맞을듯
	//투사체는 투사체가 닿았을 때 CS_Packet_Attack을 호출할것이다.
	D3DXMATRIX mtxParticleDirection, mtxFinalTransform;
	D3DXMatrixIdentity(&mtxParticleDirection);
	D3DXMatrixRotationY(&mtxParticleDirection, D3DXToRadian(+90.0f));
	mtxFinalTransform = mtxParticleDirection * m_d3dxmtxLocal * m_d3dxmtxWorld;

	D3DXVECTOR3 vOffset;
	D3DXVec3TransformCoord(&vOffset, &D3DXVECTOR3(0, 0, 0), &m_d3dxmtxWorld);
	if (m_bAttackOffset)
	{ //AttackOffset을 사용하여 정확히 표현해야 하는 경우
		D3DXMATRIX mtxOffset = m_vecFinalTransforms[m_nAttackOffsetIndex] * mtxFinalTransform;
		
		vOffset = D3DXVECTOR3(mtxOffset._41, mtxOffset._42, mtxOffset._43) + (D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33) * m_bcMeshBoundingCube.m_d3dxvMaximum.z * 2.5f);
	}
	else //그냥 z방향으로 조금 떼 주면 되는 경우
	{
		if((int)OBJECT_TYPE::Thor == m_nObjectType || (int)OBJECT_TYPE::Thor_Enemy == m_nObjectType)
			vOffset += D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33) * m_bcMeshBoundingCube.m_d3dxvMaximum.z / 3.0f;
		else
			vOffset += D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33) * m_bcMeshBoundingCube.m_d3dxvMaximum.z;
	}
	
	if (m_bProjectileParticle)
	{
		m_ProjectileParticle.m_d3dxmtxWorld = mtxFinalTransform;
		
		//바이킹은 모델 스케일링 오류로 mtx가 매우 작아 파티클도 작게 그려져서... 다시금 스케일링을 한다. 0.008f
		if ((int)OBJECT_TYPE::Viking == m_nObjectType || (int)OBJECT_TYPE::Viking_Enemy == m_nObjectType)
		{
			D3DXMATRIX mtxScale;
			D3DXMatrixScaling(&mtxScale, 1.0f / 0.008f * 1.5f, 1.0f / 0.008f * 1.5f, 1.0f / 0.008f * 1.5f);
			m_ProjectileParticle.m_d3dxmtxWorld = mtxScale * m_ProjectileParticle.m_d3dxmtxWorld;
		}

		m_ProjectileParticle.m_d3dxmtxWorld._41 = vOffset.x;
		m_ProjectileParticle.m_d3dxmtxWorld._42 = vOffset.y;
		m_ProjectileParticle.m_d3dxmtxWorld._43 = vOffset.z;
		
		m_ProjectileParticle.m_pTargetObject = pTarget;
		CSpriteParticle::GetInstance()->AddProjectileParticle(m_strParticle, m_ProjectileParticle);
	}
	else
	{
		m_PromptParticle.m_d3dxmtxWorld = mtxFinalTransform;
		
		m_PromptParticle.m_d3dxmtxWorld._41 = vOffset.x;
		m_PromptParticle.m_d3dxmtxWorld._42 = vOffset.y;
		m_PromptParticle.m_d3dxmtxWorld._43 = vOffset.z;

		m_PromptParticle.m_fMaxParticleTime = 0.6f;
		m_PromptParticle.m_pTargetObject = pTarget;

		//임의의 즉발 위치를 만들어낸다.
		D3DXVECTOR3 vMin, vMax;
		vMin = pTarget->m_bcMeshBoundingCube.m_d3dxvMinimum;
		vMax = pTarget->m_bcMeshBoundingCube.m_d3dxvMaximum;

		float xRatio = (float)rand() / (float)RAND_MAX;
		float yRatio = (float)rand() / (float)RAND_MAX;
		float zRatio = (float)rand() / (float)RAND_MAX;

		m_PromptParticle.m_fRandXOffset = vMin.x * xRatio + vMax.x * (1.0f - xRatio);
		m_PromptParticle.m_fRandYOffset = vMin.y * yRatio + vMax.y * (1.0f - yRatio);
		m_PromptParticle.m_fRandZOffset = vMin.z * zRatio + vMax.z * (1.0f - zRatio);

		CSpriteParticle::GetInstance()->AddPromptParticle(m_strParticle, m_PromptParticle);
	}
}

void CAnimationObject::CS_Packet_Attack(CGameObject* pTarget)
{
	m_bLastPacketProcessed = false;

	printf("CS_Patcket_Attack 호출!\n");
	//데이터도 보내고 해야함
	cs_packet_attack_object* packet = reinterpret_cast<cs_packet_attack_object *>(CSocketComponent::GetInstance()->m_SendBuf);
	packet->type = CS_ATTACK_OBJECT;
	packet->size = sizeof(cs_packet_attack_object);
	packet->client_id = CSocketComponent::GetInstance()->m_clientID;
	packet->object_id = m_nObjectID;
	packet->object_type = (OBJECT_TYPE)m_nObjectType;
	packet->room_id = CSocketComponent::GetInstance()->m_roomID;
	packet->target_id = pTarget->m_nObjectID;

	CSocketComponent::GetInstance()->SendPacket(reinterpret_cast<unsigned char*>(packet));


	//1214 사운드 수정 -> 카메라와 멀리 떨어져 있다면 사운드 재생 안하도록
	CCamera* pCamera = (CCamera*)gGameCamera;
	D3DXVECTOR3 camLookPosition = pCamera->GetLookAtPosition();
	D3DXVECTOR3 vSub = camLookPosition - D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43);
	if (D3DXVec3Length(&vSub) <= CAMERA_UNIT_SOUND_DISTANCE)
	{
		int nSoundOffset = getSoundOffsetByType(m_nObjectType);
		CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset + UNIT_ATTACK_OFFSET);
	}

#ifdef DEBUG_NOT_CONNECT_SERVER
	//Process_SC_Attack();
	return;
#endif
	
}

//공격 처리에 대해서... 모든 정보를 Object_Data 메시지로 처리하는건 한계가 있음. 동작도 구분이 필요하다고 봄. 일단 임시
void CAnimationObject::Process_SC_Attack(sc_packet_attack_object* packet, CGameObject* pTarget)
{
	m_bLastPacketProcessed = true;
	//Scene에서 적 유닛 구분은 끝
	BeginParticleImage(pTarget);
	
}

void CAnimationObject::Process_SC_Value(sc_packet_data_object_value* packet)
{
	m_bLastPacketProcessed = true;
	//이후 캐릭터 정보 갱신
	m_nHP = packet->hp;
}

void CAnimationObject::Process_SC_Fail_Move()
{
	m_bLastPacketProcessed = true;
	printf("%d는 Fail Move를 받았다!\n", m_nObjectID);
	/*
	// 1. 그래프에서 내 위치의 인덱스를 얻고, 그 노드를 제외시킨다. -> GameFramework에서 ADD,REMOVE node 진행함.
	// 2. 다시 길을 찾는다.
	D3DXVECTOR3 dest = *(m_listTargetPosition.rbegin());
	m_listTargetPosition = AStarComponent::GetInstance()->SearchAStarPath(GetPosition(), dest);
	m_bLatestFindPath = true;
	SetTargetPosition(m_listTargetPosition.front());
	*/
}

void CAnimationObject::Process_SC_State(sc_packet_data_object_state* packet)
{
	m_bLastStatePacketProcessed = true;

	switch (packet->state)
	{
	case State::stop:
		if (m_bMoveForBuild)
		{
			m_bMoveForBuild = false;
			int mineral, gas;
			getNeedResourceForCreate((int)m_WillBuildObjectType, mineral, gas);

			if(mineral > gGameMineral || gas > gGameGas)
			{
				if (mineral > gGameMineral)
					CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_mineral);
				else if (gas > gGameGas)
					CSoundManager::GetInstance()->PlayEffectSound(start_offset::not_enough_gas);
				
				m_WillBuildObjectType = OBJECT_TYPE::Invalid;
				SetAnimation(string("Idle"));
				break;
			}

			int build_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(GetPosition());

			if(OBJECT_TYPE::Refinery == m_WillBuildObjectType)
			{
				if (nullptr == m_pGasModel || !m_pGasModel->GetActive() || build_idx != AStarComponent::GetInstance()->GetNearstNodeIndex(m_pGasModel->GetPosition()))
				{	//누군가 이미 지었거나, 위치가 잘못되었다면
					m_pGasModel = nullptr;
					printf("가스 위에다만 지을 수 있다!\n");
					CSoundManager::GetInstance()->PlayEffectSound(start_offset::cannot_build);
					m_WillBuildObjectType = OBJECT_TYPE::Invalid;
					break;
				}
			}

			cs_packet_create_building* my_packet = reinterpret_cast<cs_packet_create_building *>(CSocketComponent::GetInstance()->m_SendBuf);
			my_packet->size = sizeof(cs_packet_move_object);
			my_packet->type = CS_CREATE_BUILDING;
			my_packet->room_id = CSocketComponent::GetInstance()->m_roomID;
			my_packet->client_id = CSocketComponent::GetInstance()->m_clientID;
			my_packet->object_type = m_WillBuildObjectType; //지으려는 건물 타입
			my_packet->my_object_id = m_nObjectID;
			my_packet->index = build_idx;
			CSocketComponent::GetInstance()->SendPacket(reinterpret_cast<unsigned char*>(my_packet));
			
			m_WillBuildObjectType = OBJECT_TYPE::Invalid;
		}
		else
			SetAnimation(string("Idle"));

		m_bPickTarget = false;
		break;

	case State::move:
		SetAnimation(string("Move"));
		break;
		
	case State::move_with_guard:
		SetAnimation(string("Move"));
		m_bPickTarget = false;
		break;

	case State::attack:
		SetAnimation(string("Attack"));
		m_nTargetObjectID = packet->target_id;
		break;

	case State::have_target:
		SetAnimation(string("Move"));
		break;

	case State::death:
		SetAnimation(string("Die"));
		m_bMoveForBuild = false;
		m_WillBuildObjectType = OBJECT_TYPE::Invalid;
		m_bDeath = true;

		//1214 사운드 수정 -> 카메라와 멀리 떨어져 있다면 사운드 재생 안하도록
		CCamera* pCamera = (CCamera*)gGameCamera;
		D3DXVECTOR3 camLookPosition = pCamera->GetLookAtPosition();
		D3DXVECTOR3 vSub = camLookPosition - D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43);
		if (D3DXVec3Length(&vSub) <= CAMERA_UNIT_SOUND_DISTANCE)
		{
			int obj_type = (int)packet->object_type;
			int nSoundOffset = getSoundOffsetByType(obj_type);
			CSoundManager::GetInstance()->PlayEffectSound(nSoundOffset + UNIT_DEATH_OFFSET);
		}
		break;
	}

	m_ObjectState = packet->state;
	if (packet->client_id != CSocketComponent::GetInstance()->m_clientID)
		printf("적군유닛 ");
	else
		printf("아군유닛 ");

	printf("%d는 state : %d를 받았다\n", m_nObjectID, (int)packet->state);

	/*
	if (packet->state == State::stop)
		SetAnimation(string("Idle"));

	m_bLastPacketProcessed = true;

	if (State::attack == packet->state)
	{
		m_nTargetObjectID = packet->target_id;
	}
	*/
	
	/*
	//적군 유닛의 애니메이션은 State에서 바로 갱신, 아군은 UpdateObject에서 진행할 것이다.
	if(isEnemyObject(m_nObjectType))
	{
	switch (packet->state)
	{
	case State::stop:
		SetAnimation(string("Idle"));
		m_bHasTargetPosition = false;
		m_listTargetPosition.clear();
		m_d3dxvTargetPosition = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
		break;
	case State::have_target:
		SetAnimation(string("Move"));
		break;
	case State::move:
		SetAnimation(string("Move"));
		break;
	case State::move_with_guard:
		SetAnimation(string("Move"));
		break;
	case State::attack:
		m_nTargetObjectID = packet->target_id;
		SetAnimation(string("Attack"));
		m_bHasTargetPosition = false;
		m_listTargetPosition.clear();
		m_d3dxvTargetPosition = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
		break;
	}

	m_ObjectState = packet->state;

	}
	else
		CGameObject::Process_SC_State(packet);
		*/
}


void CAnimationObject::UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList)
{
	//지형 위에 배치
	float y = pHeightMap->GetHeight(m_d3dxmtxWorld._41, m_d3dxmtxWorld._43, false);
	m_d3dxmtxWorld._42 = y;// +(m_bcMeshBoundingCube.m_d3dxvMaximum.y / 2.0f);

	//패킷 전송률 30프레임 테스트	-> 0831 사실 패킷 전송률이 아니라 Update 카운터다.
	m_fLastSendPacketTime += fTimeElapsed;
	
	//if (!m_bLastPacketProcessed || m_fLastSendPacketTime < 0.033333f)
	if(m_fLastSendPacketTime < 0.033333f || !m_bLastStatePacketProcessed)
		return;

	if (State::attack == m_ObjectState)
		m_fAnimationElapsedTime = m_fLastSendPacketTime * m_fAttackSpeed;
	else
		m_fAnimationElapsedTime = m_fLastSendPacketTime;


	if (isEnemyObject(m_nObjectType)) //적군 유닛인 경우 무조건 패킷 내용으로 처리해야하나...
	{
		if (m_ObjectState == State::attack)
		{
			SetAnimation(string("Attack"));

			CGameObject* pObject = nullptr;
			for (auto d : *allyList)
			{
				if (d->m_nObjectID == m_nTargetObjectID)
				{
					pObject = d;
					break;
				}
			}

			if(pObject) 
				RotateToTarget(pObject);
		}
		
		return;
	}

	//아군 유닛이라면
	switch (m_ObjectState)
	{
	case State::stop:
	{
		//SetAnimation(string("Idle"));
		
		CGameObject* pNearstObject = nullptr;
		float fNearstDistance = FLT_MAX;

		for (auto obj : *enemyList) //가장 가까운 적 중 공격이 가능한 적을 찾는다.
		{
			if (!m_bAirAttack && obj->m_bAirUnit) continue; //공중 공격 못하는데 공중 유닛이라면
			if (obj->m_bDeath) continue;	//죽은 유닛, 즉 Die 애니메이션 중이라면

			float distance = (GetPosition().x - (obj->GetPosition().x - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (GetPosition().x - (obj->GetPosition().x - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
				+ (GetPosition().z - (obj->GetPosition().z - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (GetPosition().z - (obj->GetPosition().z - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
			if (fNearstDistance > distance)
			{
				fNearstDistance = distance;
				pNearstObject = obj;
			}
		}

		if(pNearstObject)
		{
			//서버 처리 안되서 attack 바로 가지 않고 have_target을 거치는 테스트						
			/*
			if (fNearstDistance <= (m_fObjectAttackRange * m_fObjectAttackRange)) //사정거리 안에 존재한다면
			{
				m_pTargetObject = pNearstObject;
				CS_Packet_State(State::attack);
				m_bProcessedAttack = true;
				//SetAnimation(string("Attack"));
			}
			else*/ if (fNearstDistance <= (m_fObjectAttackRange * 1.5f) * (m_fObjectAttackRange * 1.5f))
			{	//사정거리 밖이라면 일정 경계범위 안인지 확인한다. 일단 임시
				 //본래는 시야와 공격범위의 중간정도의 범위에 적이 있으면 쫓아가서 공격하지만 처리하지 않음

				m_pTargetObject = pNearstObject;
				CS_Packet_State(State::have_target);
				m_bPickTarget = false;
				CS_Packet_Move(m_pTargetObject->GetPosition(), false);
			}
		}
		break;
	}

	case State::move:
	{
		/*
		SetAnimation(string("Move"));
		if(m_bHasTargetPosition)
			UpdateMoveEvent(pHeightMap);
		m_pTargetObject = nullptr;
		*/
		break;
	}

	case State::move_with_guard:
	{
		CGameObject* pNearstObject = nullptr;
		float fNearstDistance = FLT_MAX;
		for (auto obj : *enemyList) //가장 가까운 적 중 공격이 가능한 적을 찾는다.
		{
			if (!m_bAirAttack && obj->m_bAirUnit) continue; //공중 공격 못하는데 공중 유닛이라면

			float distance = (GetPosition().x - (obj->GetPosition().x - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (GetPosition().x - (obj->GetPosition().x - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
				+ (GetPosition().z - (obj->GetPosition().z - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (GetPosition().z - (obj->GetPosition().z - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
			if (fNearstDistance > distance)
			{
				if (distance <= ( (m_fObjectAttackRange * 1.5f) * (m_fObjectAttackRange * 1.5f))) //공격탐지거리 안에 존재한다면
				{
					fNearstDistance = distance;
					pNearstObject = obj;
				}
			}
		}

		if (nullptr != pNearstObject)
		{
			if (fNearstDistance <= (m_fObjectAttackRange * m_fObjectAttackRange)) //공격범위 안에도 존재하는지 확인해본다.
			{
				m_pTargetObject = pNearstObject;
				CS_Packet_State(State::attack);
				m_bProcessedAttack = true;
				//SetAnimation(string("Attack"));
				RotateToTarget(m_pTargetObject);
			}
			
			else //공격탐지거리엔 존재하지만 공격범위 안에는 없는 경우
			{
				m_pTargetObject = pNearstObject;
				m_bProcessedAttack = true;
				m_bPickTarget = false;
				//SetAnimation(string("Move"));
				CS_Packet_State(State::have_target);
				//m_nElapsedMoveCount = UPDATE_MOVE_COUNT + 1;
			}
			
		}
		else //근처에 적이 없다면
		{
			
		}


		break;
	}

	case State::attack: //attack state는 실제로 공격 범위 안에 존재하는 것과, 타겟이 지정된 상태인 두 가지로 세분화된다.
	{
		//m_pTargetObject가 nullptr일때도 생각해야함

		if (nullptr == m_pTargetObject || true == m_pTargetObject->m_bDeath || 
			!m_pTargetObject->GetActive() || !m_pTargetObject->IsInSight()) //타겟이 죽거나 다른 이유로 비활성화 되었다면
		{
			//FreeTargetPosition(); //Idle 처리 및 TargetPosition 없앰 State도 바꿈
			CS_Packet_State(State::stop);
			m_pTargetObject = nullptr;
		}
		else //TargetObject is Actived
		{
			float distance = (GetPosition().x - (m_pTargetObject->GetPosition().x - m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (GetPosition().x - (m_pTargetObject->GetPosition().x - m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
				+ (GetPosition().z - (m_pTargetObject->GetPosition().z - m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (GetPosition().z - (m_pTargetObject->GetPosition().z - m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
			if (distance <= (m_fObjectAttackRange * m_fObjectAttackRange))
			{
				SetAnimation(string("Attack"));
				RotateToTarget(m_pTargetObject);
							
				//다시 길 찾는 경우를 위해 이동 관련 정보를 초기화한다. 허나 애니메이션 및 상태를 바꾸면 안된다. 
				//m_bHasTargetPosition = false;
				//m_listTargetPosition.clear();
				//m_d3dxvTargetPosition = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
				
				CAnimationMesh* pMesh = (CAnimationMesh*)m_ppMeshes[0];
				int startKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.first;
				int endKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.second;

				float animStartTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, startKeyFrameNum);
				float animEndTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, endKeyFrameNum);

				float attackTime = (animStartTime + animEndTime) / 2.0f; //공격 메시지는 애니메이션의 중간이 지나갔다면 보낸다.
				
				//아직 Animate에서 경과시간을 더하지 않은 상태이다. 고로 더한 값으로 비교한다.
				if (m_bProcessedAttack && ((m_fTotalAnimationElapsedTime + m_fAnimationElapsedTime) >= attackTime))
				{
					m_bProcessedAttack = false;

					CS_Packet_Attack(m_pTargetObject);
					
					/*
					static DWORD lastTime = GetTickCount();
					DWORD now = GetTickCount();
					DWORD time = now - lastTime;
					lastTime = now;
					printf("공격메시지를 날렸다! 경과시간 %d\n", time);
					*/
				}					

				
				if ((m_fTotalAnimationElapsedTime + m_fAnimationElapsedTime) >= animEndTime) //공격 메시지가 전송되고 애니메이션이 다음 루프가 될 때 까지
				{
					m_bProcessedAttack = true;
				}
			}
			else //공격 범위 밖이라면
			{
				m_bProcessedAttack = true;
				//SetAnimation(string("Move"));
				CS_Packet_State(State::have_target);
				m_nElapsedMoveCount = UPDATE_MOVE_COUNT + 1;
				//_sleep(300);
				CS_Packet_Move(m_pTargetObject->GetPosition(), false);
			}
		}
		break;
	}

	case State::have_target:
	{
		if (nullptr == m_pTargetObject || true == m_pTargetObject->m_bDeath)
		{
			//FreeTargetPosition();
			CS_Packet_State(State::stop);
			m_pTargetObject = nullptr;
			m_bPickTarget = false;
			break;
		}

		float distance = (GetPosition().x - (m_pTargetObject->GetPosition().x - m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (GetPosition().x - (m_pTargetObject->GetPosition().x - m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
			+ (GetPosition().z - (m_pTargetObject->GetPosition().z - m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (GetPosition().z - (m_pTargetObject->GetPosition().z - m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
		if (distance <= (m_fObjectAttackRange * m_fObjectAttackRange))
		{
			m_bProcessedAttack = true;
			CS_Packet_State(State::attack);
		}
		else //아직도 사거리 바깥이라면
		{
			if (m_bPickTarget) // 타겟을 a버튼을 눌러 공격처리한 경우
			{
				//다시 타겟을 찾는 작업하지 않는다.
			}
			else //경계를 통해 타겟이 정해진 경우 -> 상대가 멀어졌다면 다시 타겟을 찾는다.
			{
				CGameObject* pNearstObject = nullptr;
				float fNearstDistance = FLT_MAX;

				for (auto obj : *enemyList) //가장 가까운 적 중 공격이 가능한 적을 찾는다.
				{
					if (!m_bAirAttack && obj->m_bAirUnit) continue; //공중 공격 못하는데 공중 유닛이라면

					float newDistance = (GetPosition().x - (obj->GetPosition().x - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (GetPosition().x - (obj->GetPosition().x - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
						+ (GetPosition().z - (obj->GetPosition().z - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (GetPosition().z - (obj->GetPosition().z - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
					if (fNearstDistance > newDistance)
					{
						if (newDistance <= (m_fObjectAttackRange * m_fObjectAttackRange)) //시야거리 안에 존재한다면
						{
							fNearstDistance = newDistance;
							pNearstObject = obj;
						}
					}
				}
				if (pNearstObject)
					m_pTargetObject = pNearstObject;
			}
			/*
			++m_nCannotFindTargetPosition;
			if (m_nCannotFindTargetPosition >= 10)
			{
				m_pTargetObject = nullptr;
				m_nCannotFindTargetPosition = 0;
			}
			*/
			/*
			if (m_nElapsedMoveCount >= UPDATE_MOVE_COUNT)
			{
				CS_Packet_Move(m_pTargetObject->GetPosition(), false);
				m_nElapsedMoveCount = 0;
			}
			*/
		}

		/*
		if (m_listTargetPosition.empty())
		{
			m_listTargetPosition = AStarComponent::GetInstance()->SearchAStarPath(GetPosition(), m_pTargetObject->GetPosition());
			m_bLatestFindPath = true;

			m_d3dxvTargetPosition = D3DXVECTOR3(m_listTargetPosition.front().x, 0.0f, m_listTargetPosition.front().z);
			m_bHasTargetPosition = true;
		}
		//UpdateMoveEvent(pHeightMap);
		*/
		break;
	}
	} //end switch
	
	//0.033333이 넘은 상태이므로 -> 애니메이션에서 이 시간을 사용하므로 그곳에서 초기화
	//m_fLastSendPacketTime = 0.0f;
}




//**********************************************************************
//지상 유닛 오브젝트 설정
CGroundObject::CGroundObject() : CAnimationObject()
{
}

CGroundObject::~CGroundObject()
{
}



//**********************************************************************
//공중 유닛 오브젝트 설정
CAirObject::CAirObject() : CAnimationObject()
{
	m_bAirUnit = true;
}

CAirObject::~CAirObject()
{
}

void CAirObject::AirUnitFindPath(D3DXVECTOR3 target)
{
	list<D3DXVECTOR3> next;
	next.push_back(target);
	
	m_listTargetPosition = next;
	m_bLatestFindPath = true;

	m_d3dxvTargetPosition = D3DXVECTOR3(m_listTargetPosition.front().x, 0.0f, m_listTargetPosition.front().z);
	m_bHasTargetPosition = true;
}

void CAirObject::CS_Packet_Move(D3DXVECTOR3 vTo, bool bRightClicked)
{
	D3DXVECTOR3 vToAddOffset = D3DXVECTOR3(vTo.x, vTo.y + m_fAirOffset, vTo.z);
	CAnimationObject::CS_Packet_Move(vToAddOffset, bRightClicked);
}


void CAirObject::UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList)
{
	CAnimationObject::UpdateObject(fTimeElapsed, pHeightMap, allyList, enemyList);
	m_d3dxmtxWorld._42 = m_fAirOffset;	 //+=이 아닌 이유 -> 항상 같은 높이에 있어야함 언덕에 상관없이
	return;







	/*
	//지형 위에 배치
	float y = pHeightMap->GetHeight(m_d3dxmtxWorld._41, m_d3dxmtxWorld._43, false);
	m_d3dxmtxWorld._42 = y + m_fAirOffset;

	//패킷 전송률 30프레임 테스트
	m_fLastSendPacketTime += fTimeElapsed;

	//if (!m_bLastPacketProcessed || m_fLastSendPacketTime < 0.033333f)
	if(m_fLastSendPacketTime < 0.033333f)
		return;

	if (isEnemyObject(m_nObjectType)) //적군 유닛인 경우 무조건 패킷 내용으로 처리해야하나...
	{
		if (m_bHasTargetPosition)
			//UpdateMoveEvent(pHeightMap);

		return;
	}

	switch (m_ObjectState)
	{
	case State::stop:
	{
		SetAnimation(string("Idle"));
		CGameObject* pNearstObject = nullptr;
		float fNearstDistance = FLT_MAX;
		for (auto obj : *enemyList) //가장 가까운 적 중 공격이 가능한 적을 찾는다.
		{
			if (!m_bAirAttack && obj->m_bAirUnit) continue; //공중 공격 못하는데 공중 유닛이라면

			float distance = (GetPosition().x - obj->GetPosition().x) * (GetPosition().x - obj->GetPosition().x)
				+ (GetPosition().z - obj->GetPosition().z) * (GetPosition().z - obj->GetPosition().z);
			if (fNearstDistance > distance)
			{
				fNearstDistance = distance;
				pNearstObject = obj;
			}
		}

		if (fNearstDistance <= (m_fObjectAttackRange * m_fObjectAttackRange)) //사정거리 안에 존재한다면
		{
			m_pTargetObject = pNearstObject;
			CS_Packet_State(State::attack);
			m_bProcessedAttack = true;
			SetAnimation(string("Attack"));
		}
		else //사정거리 밖이라면 일정 경계범위 안인지 확인한다. 일단 임시
		{	 //본래는 시야와 공격범위의 중간정도의 범위에 적이 있으면 쫓아가서 공격하지만 처리하지 않음
			m_pTargetObject = nullptr;
		}

		break;
	}

	case State::move:
	{
		SetAnimation(string("Move"));
		m_pTargetObject = nullptr;
		//UpdateMoveEvent(pHeightMap);
		break;
	}

	case State::move_with_guard:
	{
		CGameObject* pNearstObject = nullptr;
		float fNearstDistance = FLT_MAX;
		for (auto obj : *enemyList) //가장 가까운 적 중 공격이 가능한 적을 찾는다.
		{
			if (!m_bAirAttack && obj->m_bAirUnit) continue; //공중 공격 못하는데 공중 유닛이라면

			float distance = (GetPosition().x - obj->GetPosition().x) * (GetPosition().x - obj->GetPosition().x)
				+ (GetPosition().z - obj->GetPosition().z) * (GetPosition().z - obj->GetPosition().z);
			if (fNearstDistance > distance)
			{
				fNearstDistance = distance;
				pNearstObject = obj;
			}
		}

		if (fNearstDistance <= (m_fObjectSightRange * m_fObjectSightRange)) //시야거리 안에 존재한다면
		{
			if (fNearstDistance <= (m_fObjectAttackRange * m_fObjectAttackRange)) //공격범위 안에도 존재하는지 확인해본다.
			{
				m_pTargetObject = pNearstObject;
				CS_Packet_State(State::attack);
				m_bProcessedAttack = true;
				SetAnimation(string("Attack"));
				RotateToTarget(m_pTargetObject);
			}
			else //시야엔 존재하지만 공격범위 안에는 없는 경우
			{
				m_pTargetObject = pNearstObject;
				m_bProcessedAttack = true;
				SetAnimation(string("Move"));
				CS_Packet_State(State::have_target);
			}
		}
		else //시야거리 안에도 존재하지 않는다 -> 이동 이벤트를 계속 진행한다.
		{
			//UpdateMoveEvent(pHeightMap);
		}

		break;
	}


	case State::attack: //attack state는 실제로 공격 범위 안에 존재하는 것과, 타겟이 지정된 상태인 두 가지로 세분화된다.
	{
		if (!m_pTargetObject->GetActive() || !m_pTargetObject->IsInSight()) //타겟이 죽거나 다른 이유로 비활성화 되었다면
		{
			FreeTargetPosition(); //Idle 처리 및 TargetPosition 없앰 State도 바꿈
			m_pTargetObject = nullptr;
		}
		else //TargetObject is Actived
		{
			float distance = (GetPosition().x - m_pTargetObject->GetPosition().x) * (GetPosition().x - m_pTargetObject->GetPosition().x)
				+ (GetPosition().z - m_pTargetObject->GetPosition().z) * (GetPosition().z - m_pTargetObject->GetPosition().z);
			if (distance <= (m_fObjectAttackRange * m_fObjectAttackRange))
			{
				SetAnimation(string("Attack"));
				RotateToTarget(m_pTargetObject);

				//다시 길 찾는 경우를 위해 이동 관련 정보를 초기화한다. 허나 애니메이션 및 상태를 바꾸면 안된다. 
				m_bHasTargetPosition = false;
				m_listTargetPosition.clear();
				m_d3dxvTargetPosition = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);

				CAnimationMesh* pMesh = (CAnimationMesh*)m_ppMeshes[0];
				int startKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.first;
				int endKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.second;

				float animStartTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, startKeyFrameNum);
				float animEndTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, endKeyFrameNum);

				float attackTime = (animStartTime + animEndTime) / 2.0f; //공격 메시지는 애니메이션의 중간이 지나갔다면 보낸다.

																		 //아직 Animate에서 경과시간을 더하지 않은 상태이다. 고로 더한 값으로 비교한다.
				if (m_bProcessedAttack && ((m_fAnimationElapsedTime + m_fLastSendPacketTime) >= attackTime))
				{
					m_bProcessedAttack = false;

					CS_Packet_Attack(m_pTargetObject);

					
					static DWORD lastTime = GetTickCount();
					DWORD now = GetTickCount();
					DWORD time = now - lastTime;
					lastTime = now;
					printf("공격메시지를 날렸다! 경과시간 %d\n", time);
					
				}

				if ((m_fAnimationElapsedTime + m_fLastSendPacketTime) >= animEndTime) //공격 메시지가 전송되고 애니메이션이 다음 루프가 될 때 까지
				{
					m_bProcessedAttack = true;
				}
			}
			else //공격 범위 밖이라면
			{
				m_bProcessedAttack = true;
				SetAnimation(string("Move"));
				CS_Packet_State(State::have_target);
			}
		}
		break;
	}
	case State::have_target:
	{
		float distance = (GetPosition().x - m_pTargetObject->GetPosition().x) * (GetPosition().x - m_pTargetObject->GetPosition().x)
			+ (GetPosition().z - m_pTargetObject->GetPosition().z) * (GetPosition().z - m_pTargetObject->GetPosition().z);
		if (distance <= (m_fObjectAttackRange * m_fObjectAttackRange))
		{
			m_bProcessedAttack = true;
			CS_Packet_State(State::attack);
			break;
		}

		if (m_listTargetPosition.empty())
		{
			AirUnitFindPath(m_pTargetObject->GetPosition());
		}
		//UpdateMoveEvent(pHeightMap);
		break;
	}
	} //end switch

	  //0.033333이 넘은 상태이므로 -> 애니메이션에서 이 시간을 사용하므로 그곳에서 초기화
	  //m_fLastSendPacketTime = 0.0f;
	  */
}