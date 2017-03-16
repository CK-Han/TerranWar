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
//������Ʈ�� ��ŷ ó�� �Լ��Դϴ�.
void CGameObject::GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection)
{
	//pd3dxvPickPosition: ī�޶� ��ǥ���� ��(ȭ�� ��ǥ�迡�� ���콺�� Ŭ���� ���� ����ȯ�� ��)
	//pd3dxmtxWorld: ���� ��ȯ ���, pd3dxmtxView: ī�޶� ��ȯ ���
	//pd3dxvPickRayPosition: ��ŷ ������ ������, pd3dxvPickRayDirection: ��ŷ ���� ����
	
	/*��ü�� ���� ��ȯ ����� �־����� ��ü�� ���� ��ȯ ��İ� ī�޶� ��ȯ ����� ���ϰ� ������� ���Ѵ�. 
	�̰��� ī�޶� ��ȯ ����� ����İ� ��ü�� ���� ��ȯ ����� ������� ���� ����. 
	��ü�� ���� ��ȯ ����� �־����� ������ ī�޶� ��ȯ ����� ������� ���Ѵ�. 
	��ü�� ���� ��ȯ ����� �־����� �� ��ǥ���� ��ŷ ������ ���ϰ� 
	�׷��� ������ ���� ��ǥ���� ��ŷ ������ ���Ѵ�.*/
	D3DXMATRIX d3dxmtxInverse;
	D3DXMATRIX d3dxmtxWorldView = *pd3dxmtxView;
	if (pd3dxmtxWorld) D3DXMatrixMultiply(&d3dxmtxWorldView, pd3dxmtxWorld, pd3dxmtxView);
	D3DXMatrixInverse(&d3dxmtxInverse, NULL, &d3dxmtxWorldView);
	D3DXVECTOR3 d3dxvCameraOrigin(0.0f, 0.0f, 0.0f);
	/*ī�޶� ��ǥ���� ���� (0, 0, 0)�� ������ ���� ����ķ� ��ȯ�Ѵ�. ��ȯ�� ����� 
	ī�޶� ��ǥ���� ������ �����Ǵ� �� ��ǥ���� �� �Ǵ� ���� ��ǥ���� ���̴�.*/
	D3DXVec3TransformCoord(pd3dxvPickRayPosition, &d3dxvCameraOrigin, &d3dxmtxInverse);
	/*ī�޶� ��ǥ���� ���� ������ ���� ����ķ� ��ȯ�Ѵ�. ��ȯ�� ����� 
	���콺�� Ŭ���� ���� �����Ǵ� �� ��ǥ���� �� �Ǵ� ���� ��ǥ���� ���̴�.*/
	D3DXVec3TransformCoord(pd3dxvPickRayDirection, pd3dxvPickPosition, &d3dxmtxInverse);
	//��ŷ ������ ���� ���͸� ���Ѵ�.
	*pd3dxvPickRayDirection = *pd3dxvPickRayDirection - *pd3dxvPickRayPosition;
}

int CGameObject::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	//pd3dxvPickPosition: ī�޶� ��ǥ���� ��(ȭ�� ��ǥ�迡�� ���콺�� Ŭ���� ���� ����ȯ�� ��)
	//pd3dxmtxView: ī�޶� ��ȯ ���
	D3DXVECTOR3 d3dxvPickRayPosition, d3dxvPickRayDirection;
	int nIntersected = 0;
	int nResultIntersected = 0; //�浹 ���θ��� �ľ��ϵ��� �ϴ� ����
	//Ȱ��ȭ�� ��ü�� ���Ͽ� �޽��� ������ ��ŷ ������ ���ϰ� ��ü�� �޽��� �浹 �˻縦 �Ѵ�.
	
	if (m_bActive && m_ppMeshes)
	{
		//��ŷ ������ ��ġ�� ������ �޽�, �� ����ǥ��� ��ȯ�Ѵ�.
		GenerateRayForPicking(pd3dxvPickPosition, &m_d3dxmtxWorld, pd3dxmtxView, &d3dxvPickRayPosition, &d3dxvPickRayDirection);

		for (int i = 0; i < m_nMeshes; i++)
		{
			nIntersected = m_ppMeshes[i]->CheckRayIntersection(&d3dxvPickRayPosition, &d3dxvPickRayDirection, pd3dxIntersectInfo);

			//�浹�� �� ���, �ٷ� ������ �ʰ� ��� �޽ø� ��ȸ�Ͽ� ��Ȯ�� ���� �޽ø� ã���� �Ѵ�. z-������ �Ѱ͵� �ƴϴϱ�
			if (nIntersected > 0)
				nResultIntersected = 1; // �ϳ��� �浹�ȴ�.
		}
	}
	return(nResultIntersected);
}

//**********************************************************************
//������Ʈ�� ���� ����
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
	//�̵� �� ȸ��
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
//�̵� ����
void CGameObject::MoveForward(float fDistance)
{
	//���� ��ü�� ���� z-�� �������� �̵��Ѵ�.
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
//��Ŷ ��� ���� ó��
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

	printf("%d ������ CS_Packet_State�� ����, State : %d\n", m_nObjectID, (int)state);

	//test
	//m_ObjectState = state;
}

void CGameObject::Process_SC_State(sc_packet_data_object_state* packet)
{
	m_bLastStatePacketProcessed = true;

	//m_bLastPacketProcessed = true;
	//State state = packet->state;
	printf("%d�� State������ �޾Ҵ�. ���� : %d\n", m_nObjectID, (int)packet->state);
	//m_ObjectState = state;
}

//**********************************************************************
//������ ���� ó��
void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera)
{
	//����� �޽��� �������Ѵ�.
	//���� ��ǥ�� ���� ���� �ؽ�ó�� ���̴����� �ϵ��� ������.
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
//���̸� ����
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
	long cxBlocks = (m_nWidth ) / cxQuadsPerBlock;		// 205 / 5�� �� -> 41
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
//���� ������Ʈ ���� -> �ǹ� �� �ڿ�
CStaticObject::CStaticObject() : CGameObject(1)
{
}

CStaticObject::~CStaticObject()
{
}


void CStaticObject::UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList)
{
	float y = pHeightMap->GetHeight(m_d3dxmtxWorld._41, m_d3dxmtxWorld._43, false);
	
	//m_fBuildTime += fTimeElapsed; //-> �������� ���� �ش�. ���� �� �ʿ�� ����?
		
	float ratio = (m_fBuildTime / m_fCompleteBuildTime);
	ratio = ratio > 1.0f ? 1.0f : ratio;
	
	m_d3dxmtxWorld._42 = y * ratio;

	if (m_fBuildTime < m_fCompleteBuildTime)
	{	//�ǹ��� �� ������ ���¶�� 
		return;
	}

	if (m_typeGeneratedUnit != OBJECT_TYPE::Invalid)
	{
		m_fUnitGeneratedTime += fTimeElapsed;
		if (m_fUnitCompleteTime <= m_fUnitGeneratedTime)
		{	//���� ���� ��û ��Ŷ ����
			
			//0907 ���� ��ġ �ڸ����
			//�����Ǿ����� ��ȿ�� ��ġ�� ��´�.
			D3DXVECTOR3 vOffset = GetPosition() + D3DXVECTOR3(-2, 0, -2);
			bool bStand = false;
			int iOffset = AStarComponent::GetInstance()->GetNearstNodeIndex(vOffset);
			int obj_idx;
			for (auto d : *allyList)
			{
				obj_idx = AStarComponent::GetInstance()->GetNearstNodeIndex(d->GetPosition());
				//����vs���� Ȥ�� ����vs������ ��� �浹�� �ƴϴ�
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
				//����vs���� Ȥ�� ����vs������ ��� �浹�� �ƴϴ�
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
			//�����ڵ� ���κ�

			//CS_Packet_Create_Unit(m_typeGeneratedUnit, GetPosition() + D3DXVECTOR3(-2, 0,-2));	//offset�� ���������� �ٽ� �ؾ���.
			CS_Packet_Create_Unit(m_typeGeneratedUnit, vOffset);	//offset�� ���������� �ٽ� �ؾ���.
			CancelGenerateUnit();
		}
	}
}

void CStaticObject::Process_SC_Value(sc_packet_data_object_value* packet)
{
	if(m_fBuildTime != m_fCompleteBuildTime)	//���� ��찡 �ƴ� ü�� ���� �ÿ� ���� ó��
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

	//���� �ִϸ��̼� ����
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

	//1214 ���� ���� -> ī�޶�� �ָ� ������ �ִٸ� ���� ��� ���ϵ���
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
		return false;		//���� �� �������ų� ���� �������� ������ �ִٸ� �������� �ʴ´�.

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

	//������ ��� ��ġ�� ������.
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
//�ִϸ��̼� ������Ʈ ����
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
		printf("�߸��� �ִϸ��̼� �̸��Դϴ�.\n");
	}
	else if (animName == m_strCurrentAnimation)
	{
		//printf("���� �ִϸ��̼��� ȣ�������� �״�� �����մϴ�.");
	}
	else //�ִϸ��̼��� ã�Ҵٸ�
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
	
	//State�� move�� �ٲٴ� �ൿ�� RBUTTON Ŭ���ۿ� ����.
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
	//���� �ִϸ��̼� �ڵ�
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

	//UpdateObject�� 30���������� ����Ǵٺ��� ����ġ�� �߻��Ѵ�.
	//-> �ִϸ��̼��� �̹� �Ϸ��̳� UpdateObject�� 0.0333�� ���� �ʾ� ���ŵ��� �ʴ� �����̶� �����ϸ� ��
	
	if (m_fLastSendPacketTime < 0.033333f)
		return;

	CAnimationMesh* pMesh = (CAnimationMesh*)m_ppMeshes[0];

	//if(State::attack == m_ObjectState)
	//	m_fAnimationElapsedTime += m_fLastSendPacketTime * m_fAttackSpeed;
	//else //����
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
	//�ִϸ��̼ǿ��� �ʱ�ȭ
	m_fLastSendPacketTime = 0.0f;
}


void CAnimationObject::UpdateMoveEvent(CHeightMap* pHeightMap)
{
	//�̵� ���� ó��
	//if (m_bLastPacketProcessed && (!m_listTargetPosition.empty() || m_bHasTargetPosition))
	if ((!m_listTargetPosition.empty() || m_bHasTargetPosition))
	{
		D3DXVECTOR3 vNext = m_d3dxvTargetPosition;
		D3DXVECTOR3 nowPosition = GetPosition();
		vNext.y = 0.0f; nowPosition.y = 0.0f;	//x-z ������θ� �̵� ����� �Ѵ�.

		D3DXVECTOR3 toTarget = vNext - nowPosition;
		D3DXVec3Normalize(&toTarget, &toTarget);

		//�̵��� ���� ��ġ�� ��´�.
		nowPosition.x += toTarget.x * m_fLastSendPacketTime * m_fObjectSpeed;
		nowPosition.z += toTarget.z * m_fLastSendPacketTime * m_fObjectSpeed;

		//���͸� ó�� �� ���� ��ǥ ��ġ ����
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
		printf("%d ������ Scene::Move��Ŷ�� ���´�. right : true\n", m_nObjectID);
	else
		printf("%d ������ Scene::Move��Ŷ�� ���´�. right : false\n", m_nObjectID);

	/*
	my_packet->move_reset = m_bLatestFindPath;
	if (true == m_bLatestFindPath) m_bLatestFindPath = false;

	//test
	if (m_listTargetPosition.empty())
		my_packet->nodeindex = -1;
	else
		my_packet->nodeindex = AStarComponent::GetInstance()->GetNearstNodeIndex(m_listTargetPosition.front());
	*/

	
	//0812 ������ �ٷ� �����δ�. �߸��� ���̶�� ������ �˷��ٰ��̴�.
	//Process_SC_Move(vTo);
}

void CAnimationObject::Process_SC_Move(D3DXVECTOR3 vPos)
{
	//printf("%d ������ Process_SC_Move�� �޾Ҵ�.\n", m_nObjectID);

	//SetAnimation(string("Move"));
	//m_ObjectState = State::move;

	m_bLastPacketProcessed = true;

	//�̵� �� ȸ��
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
	//printf("���� ��ġ %f, %f, %f // idx : %d\n", vPos.x, vPos.y, vPos.z, AStarComponent::GetInstance()->GetNearstNodeIndex(vPos));


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

//�ӽ�
void CAnimationObject::BeginParticleImage(CGameObject* pTarget)
{
	//��ƼŬ�� ��ü������ y���� �������� +90�� ���ư��ִ�. ��� 90�� ������� ���� ����� ������
	//����ü�� ����ü�� ����� �� CS_Packet_Attack�� ȣ���Ұ��̴�.
	D3DXMATRIX mtxParticleDirection, mtxFinalTransform;
	D3DXMatrixIdentity(&mtxParticleDirection);
	D3DXMatrixRotationY(&mtxParticleDirection, D3DXToRadian(+90.0f));
	mtxFinalTransform = mtxParticleDirection * m_d3dxmtxLocal * m_d3dxmtxWorld;

	D3DXVECTOR3 vOffset;
	D3DXVec3TransformCoord(&vOffset, &D3DXVECTOR3(0, 0, 0), &m_d3dxmtxWorld);
	if (m_bAttackOffset)
	{ //AttackOffset�� ����Ͽ� ��Ȯ�� ǥ���ؾ� �ϴ� ���
		D3DXMATRIX mtxOffset = m_vecFinalTransforms[m_nAttackOffsetIndex] * mtxFinalTransform;
		
		vOffset = D3DXVECTOR3(mtxOffset._41, mtxOffset._42, mtxOffset._43) + (D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33) * m_bcMeshBoundingCube.m_d3dxvMaximum.z * 2.5f);
	}
	else //�׳� z�������� ���� �� �ָ� �Ǵ� ���
	{
		if((int)OBJECT_TYPE::Thor == m_nObjectType || (int)OBJECT_TYPE::Thor_Enemy == m_nObjectType)
			vOffset += D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33) * m_bcMeshBoundingCube.m_d3dxvMaximum.z / 3.0f;
		else
			vOffset += D3DXVECTOR3(m_d3dxmtxWorld._31, m_d3dxmtxWorld._32, m_d3dxmtxWorld._33) * m_bcMeshBoundingCube.m_d3dxvMaximum.z;
	}
	
	if (m_bProjectileParticle)
	{
		m_ProjectileParticle.m_d3dxmtxWorld = mtxFinalTransform;
		
		//����ŷ�� �� �����ϸ� ������ mtx�� �ſ� �۾� ��ƼŬ�� �۰� �׷�����... �ٽñ� �����ϸ��� �Ѵ�. 0.008f
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

		//������ ��� ��ġ�� ������.
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

	printf("CS_Patcket_Attack ȣ��!\n");
	//�����͵� ������ �ؾ���
	cs_packet_attack_object* packet = reinterpret_cast<cs_packet_attack_object *>(CSocketComponent::GetInstance()->m_SendBuf);
	packet->type = CS_ATTACK_OBJECT;
	packet->size = sizeof(cs_packet_attack_object);
	packet->client_id = CSocketComponent::GetInstance()->m_clientID;
	packet->object_id = m_nObjectID;
	packet->object_type = (OBJECT_TYPE)m_nObjectType;
	packet->room_id = CSocketComponent::GetInstance()->m_roomID;
	packet->target_id = pTarget->m_nObjectID;

	CSocketComponent::GetInstance()->SendPacket(reinterpret_cast<unsigned char*>(packet));


	//1214 ���� ���� -> ī�޶�� �ָ� ������ �ִٸ� ���� ��� ���ϵ���
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

//���� ó���� ���ؼ�... ��� ������ Object_Data �޽����� ó���ϴ°� �Ѱ谡 ����. ���۵� ������ �ʿ��ϴٰ� ��. �ϴ� �ӽ�
void CAnimationObject::Process_SC_Attack(sc_packet_attack_object* packet, CGameObject* pTarget)
{
	m_bLastPacketProcessed = true;
	//Scene���� �� ���� ������ ��
	BeginParticleImage(pTarget);
	
}

void CAnimationObject::Process_SC_Value(sc_packet_data_object_value* packet)
{
	m_bLastPacketProcessed = true;
	//���� ĳ���� ���� ����
	m_nHP = packet->hp;
}

void CAnimationObject::Process_SC_Fail_Move()
{
	m_bLastPacketProcessed = true;
	printf("%d�� Fail Move�� �޾Ҵ�!\n", m_nObjectID);
	/*
	// 1. �׷������� �� ��ġ�� �ε����� ���, �� ��带 ���ܽ�Ų��. -> GameFramework���� ADD,REMOVE node ������.
	// 2. �ٽ� ���� ã�´�.
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
				{	//������ �̹� �����ų�, ��ġ�� �߸��Ǿ��ٸ�
					m_pGasModel = nullptr;
					printf("���� �����ٸ� ���� �� �ִ�!\n");
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
			my_packet->object_type = m_WillBuildObjectType; //�������� �ǹ� Ÿ��
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

		//1214 ���� ���� -> ī�޶�� �ָ� ������ �ִٸ� ���� ��� ���ϵ���
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
		printf("�������� ");
	else
		printf("�Ʊ����� ");

	printf("%d�� state : %d�� �޾Ҵ�\n", m_nObjectID, (int)packet->state);

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
	//���� ������ �ִϸ��̼��� State���� �ٷ� ����, �Ʊ��� UpdateObject���� ������ ���̴�.
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
	//���� ���� ��ġ
	float y = pHeightMap->GetHeight(m_d3dxmtxWorld._41, m_d3dxmtxWorld._43, false);
	m_d3dxmtxWorld._42 = y;// +(m_bcMeshBoundingCube.m_d3dxvMaximum.y / 2.0f);

	//��Ŷ ���۷� 30������ �׽�Ʈ	-> 0831 ��� ��Ŷ ���۷��� �ƴ϶� Update ī���ʹ�.
	m_fLastSendPacketTime += fTimeElapsed;
	
	//if (!m_bLastPacketProcessed || m_fLastSendPacketTime < 0.033333f)
	if(m_fLastSendPacketTime < 0.033333f || !m_bLastStatePacketProcessed)
		return;

	if (State::attack == m_ObjectState)
		m_fAnimationElapsedTime = m_fLastSendPacketTime * m_fAttackSpeed;
	else
		m_fAnimationElapsedTime = m_fLastSendPacketTime;


	if (isEnemyObject(m_nObjectType)) //���� ������ ��� ������ ��Ŷ �������� ó���ؾ��ϳ�...
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

	//�Ʊ� �����̶��
	switch (m_ObjectState)
	{
	case State::stop:
	{
		//SetAnimation(string("Idle"));
		
		CGameObject* pNearstObject = nullptr;
		float fNearstDistance = FLT_MAX;

		for (auto obj : *enemyList) //���� ����� �� �� ������ ������ ���� ã�´�.
		{
			if (!m_bAirAttack && obj->m_bAirUnit) continue; //���� ���� ���ϴµ� ���� �����̶��
			if (obj->m_bDeath) continue;	//���� ����, �� Die �ִϸ��̼� ���̶��

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
			//���� ó�� �ȵǼ� attack �ٷ� ���� �ʰ� have_target�� ��ġ�� �׽�Ʈ						
			/*
			if (fNearstDistance <= (m_fObjectAttackRange * m_fObjectAttackRange)) //�����Ÿ� �ȿ� �����Ѵٸ�
			{
				m_pTargetObject = pNearstObject;
				CS_Packet_State(State::attack);
				m_bProcessedAttack = true;
				//SetAnimation(string("Attack"));
			}
			else*/ if (fNearstDistance <= (m_fObjectAttackRange * 1.5f) * (m_fObjectAttackRange * 1.5f))
			{	//�����Ÿ� ���̶�� ���� ������ ������ Ȯ���Ѵ�. �ϴ� �ӽ�
				 //������ �þ߿� ���ݹ����� �߰������� ������ ���� ������ �Ѿư��� ���������� ó������ ����

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
		for (auto obj : *enemyList) //���� ����� �� �� ������ ������ ���� ã�´�.
		{
			if (!m_bAirAttack && obj->m_bAirUnit) continue; //���� ���� ���ϴµ� ���� �����̶��

			float distance = (GetPosition().x - (obj->GetPosition().x - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (GetPosition().x - (obj->GetPosition().x - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
				+ (GetPosition().z - (obj->GetPosition().z - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (GetPosition().z - (obj->GetPosition().z - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
			if (fNearstDistance > distance)
			{
				if (distance <= ( (m_fObjectAttackRange * 1.5f) * (m_fObjectAttackRange * 1.5f))) //����Ž���Ÿ� �ȿ� �����Ѵٸ�
				{
					fNearstDistance = distance;
					pNearstObject = obj;
				}
			}
		}

		if (nullptr != pNearstObject)
		{
			if (fNearstDistance <= (m_fObjectAttackRange * m_fObjectAttackRange)) //���ݹ��� �ȿ��� �����ϴ��� Ȯ���غ���.
			{
				m_pTargetObject = pNearstObject;
				CS_Packet_State(State::attack);
				m_bProcessedAttack = true;
				//SetAnimation(string("Attack"));
				RotateToTarget(m_pTargetObject);
			}
			
			else //����Ž���Ÿ��� ���������� ���ݹ��� �ȿ��� ���� ���
			{
				m_pTargetObject = pNearstObject;
				m_bProcessedAttack = true;
				m_bPickTarget = false;
				//SetAnimation(string("Move"));
				CS_Packet_State(State::have_target);
				//m_nElapsedMoveCount = UPDATE_MOVE_COUNT + 1;
			}
			
		}
		else //��ó�� ���� ���ٸ�
		{
			
		}


		break;
	}

	case State::attack: //attack state�� ������ ���� ���� �ȿ� �����ϴ� �Ͱ�, Ÿ���� ������ ������ �� ������ ����ȭ�ȴ�.
	{
		//m_pTargetObject�� nullptr�϶��� �����ؾ���

		if (nullptr == m_pTargetObject || true == m_pTargetObject->m_bDeath || 
			!m_pTargetObject->GetActive() || !m_pTargetObject->IsInSight()) //Ÿ���� �װų� �ٸ� ������ ��Ȱ��ȭ �Ǿ��ٸ�
		{
			//FreeTargetPosition(); //Idle ó�� �� TargetPosition ���� State�� �ٲ�
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
							
				//�ٽ� �� ã�� ��츦 ���� �̵� ���� ������ �ʱ�ȭ�Ѵ�. �㳪 �ִϸ��̼� �� ���¸� �ٲٸ� �ȵȴ�. 
				//m_bHasTargetPosition = false;
				//m_listTargetPosition.clear();
				//m_d3dxvTargetPosition = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
				
				CAnimationMesh* pMesh = (CAnimationMesh*)m_ppMeshes[0];
				int startKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.first;
				int endKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.second;

				float animStartTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, startKeyFrameNum);
				float animEndTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, endKeyFrameNum);

				float attackTime = (animStartTime + animEndTime) / 2.0f; //���� �޽����� �ִϸ��̼��� �߰��� �������ٸ� ������.
				
				//���� Animate���� ����ð��� ������ ���� �����̴�. ��� ���� ������ ���Ѵ�.
				if (m_bProcessedAttack && ((m_fTotalAnimationElapsedTime + m_fAnimationElapsedTime) >= attackTime))
				{
					m_bProcessedAttack = false;

					CS_Packet_Attack(m_pTargetObject);
					
					/*
					static DWORD lastTime = GetTickCount();
					DWORD now = GetTickCount();
					DWORD time = now - lastTime;
					lastTime = now;
					printf("���ݸ޽����� ���ȴ�! ����ð� %d\n", time);
					*/
				}					

				
				if ((m_fTotalAnimationElapsedTime + m_fAnimationElapsedTime) >= animEndTime) //���� �޽����� ���۵ǰ� �ִϸ��̼��� ���� ������ �� �� ����
				{
					m_bProcessedAttack = true;
				}
			}
			else //���� ���� ���̶��
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
		else //������ ��Ÿ� �ٱ��̶��
		{
			if (m_bPickTarget) // Ÿ���� a��ư�� ���� ����ó���� ���
			{
				//�ٽ� Ÿ���� ã�� �۾����� �ʴ´�.
			}
			else //��踦 ���� Ÿ���� ������ ��� -> ��밡 �־����ٸ� �ٽ� Ÿ���� ã�´�.
			{
				CGameObject* pNearstObject = nullptr;
				float fNearstDistance = FLT_MAX;

				for (auto obj : *enemyList) //���� ����� �� �� ������ ������ ���� ã�´�.
				{
					if (!m_bAirAttack && obj->m_bAirUnit) continue; //���� ���� ���ϴµ� ���� �����̶��

					float newDistance = (GetPosition().x - (obj->GetPosition().x - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f)) * (GetPosition().x - (obj->GetPosition().x - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.x / 2.0f))
						+ (GetPosition().z - (obj->GetPosition().z - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f)) * (GetPosition().z - (obj->GetPosition().z - obj->m_bcMeshBoundingCube.m_d3dxvMaximum.z / 2.0f));
					if (fNearstDistance > newDistance)
					{
						if (newDistance <= (m_fObjectAttackRange * m_fObjectAttackRange)) //�þ߰Ÿ� �ȿ� �����Ѵٸ�
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
	
	//0.033333�� ���� �����̹Ƿ� -> �ִϸ��̼ǿ��� �� �ð��� ����ϹǷ� �װ����� �ʱ�ȭ
	//m_fLastSendPacketTime = 0.0f;
}




//**********************************************************************
//���� ���� ������Ʈ ����
CGroundObject::CGroundObject() : CAnimationObject()
{
}

CGroundObject::~CGroundObject()
{
}



//**********************************************************************
//���� ���� ������Ʈ ����
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
	m_d3dxmtxWorld._42 = m_fAirOffset;	 //+=�� �ƴ� ���� -> �׻� ���� ���̿� �־���� ����� �������
	return;







	/*
	//���� ���� ��ġ
	float y = pHeightMap->GetHeight(m_d3dxmtxWorld._41, m_d3dxmtxWorld._43, false);
	m_d3dxmtxWorld._42 = y + m_fAirOffset;

	//��Ŷ ���۷� 30������ �׽�Ʈ
	m_fLastSendPacketTime += fTimeElapsed;

	//if (!m_bLastPacketProcessed || m_fLastSendPacketTime < 0.033333f)
	if(m_fLastSendPacketTime < 0.033333f)
		return;

	if (isEnemyObject(m_nObjectType)) //���� ������ ��� ������ ��Ŷ �������� ó���ؾ��ϳ�...
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
		for (auto obj : *enemyList) //���� ����� �� �� ������ ������ ���� ã�´�.
		{
			if (!m_bAirAttack && obj->m_bAirUnit) continue; //���� ���� ���ϴµ� ���� �����̶��

			float distance = (GetPosition().x - obj->GetPosition().x) * (GetPosition().x - obj->GetPosition().x)
				+ (GetPosition().z - obj->GetPosition().z) * (GetPosition().z - obj->GetPosition().z);
			if (fNearstDistance > distance)
			{
				fNearstDistance = distance;
				pNearstObject = obj;
			}
		}

		if (fNearstDistance <= (m_fObjectAttackRange * m_fObjectAttackRange)) //�����Ÿ� �ȿ� �����Ѵٸ�
		{
			m_pTargetObject = pNearstObject;
			CS_Packet_State(State::attack);
			m_bProcessedAttack = true;
			SetAnimation(string("Attack"));
		}
		else //�����Ÿ� ���̶�� ���� ������ ������ Ȯ���Ѵ�. �ϴ� �ӽ�
		{	 //������ �þ߿� ���ݹ����� �߰������� ������ ���� ������ �Ѿư��� ���������� ó������ ����
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
		for (auto obj : *enemyList) //���� ����� �� �� ������ ������ ���� ã�´�.
		{
			if (!m_bAirAttack && obj->m_bAirUnit) continue; //���� ���� ���ϴµ� ���� �����̶��

			float distance = (GetPosition().x - obj->GetPosition().x) * (GetPosition().x - obj->GetPosition().x)
				+ (GetPosition().z - obj->GetPosition().z) * (GetPosition().z - obj->GetPosition().z);
			if (fNearstDistance > distance)
			{
				fNearstDistance = distance;
				pNearstObject = obj;
			}
		}

		if (fNearstDistance <= (m_fObjectSightRange * m_fObjectSightRange)) //�þ߰Ÿ� �ȿ� �����Ѵٸ�
		{
			if (fNearstDistance <= (m_fObjectAttackRange * m_fObjectAttackRange)) //���ݹ��� �ȿ��� �����ϴ��� Ȯ���غ���.
			{
				m_pTargetObject = pNearstObject;
				CS_Packet_State(State::attack);
				m_bProcessedAttack = true;
				SetAnimation(string("Attack"));
				RotateToTarget(m_pTargetObject);
			}
			else //�þ߿� ���������� ���ݹ��� �ȿ��� ���� ���
			{
				m_pTargetObject = pNearstObject;
				m_bProcessedAttack = true;
				SetAnimation(string("Move"));
				CS_Packet_State(State::have_target);
			}
		}
		else //�þ߰Ÿ� �ȿ��� �������� �ʴ´� -> �̵� �̺�Ʈ�� ��� �����Ѵ�.
		{
			//UpdateMoveEvent(pHeightMap);
		}

		break;
	}


	case State::attack: //attack state�� ������ ���� ���� �ȿ� �����ϴ� �Ͱ�, Ÿ���� ������ ������ �� ������ ����ȭ�ȴ�.
	{
		if (!m_pTargetObject->GetActive() || !m_pTargetObject->IsInSight()) //Ÿ���� �װų� �ٸ� ������ ��Ȱ��ȭ �Ǿ��ٸ�
		{
			FreeTargetPosition(); //Idle ó�� �� TargetPosition ���� State�� �ٲ�
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

				//�ٽ� �� ã�� ��츦 ���� �̵� ���� ������ �ʱ�ȭ�Ѵ�. �㳪 �ִϸ��̼� �� ���¸� �ٲٸ� �ȵȴ�. 
				m_bHasTargetPosition = false;
				m_listTargetPosition.clear();
				m_d3dxvTargetPosition = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);

				CAnimationMesh* pMesh = (CAnimationMesh*)m_ppMeshes[0];
				int startKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.first;
				int endKeyFrameNum = m_mapAnimationSet.find(m_strCurrentAnimation)->second.second;

				float animStartTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, startKeyFrameNum);
				float animEndTime = pMesh->m_pSkinnedData->GetKeyFrameTime(m_strAnimationClip, endKeyFrameNum);

				float attackTime = (animStartTime + animEndTime) / 2.0f; //���� �޽����� �ִϸ��̼��� �߰��� �������ٸ� ������.

																		 //���� Animate���� ����ð��� ������ ���� �����̴�. ��� ���� ������ ���Ѵ�.
				if (m_bProcessedAttack && ((m_fAnimationElapsedTime + m_fLastSendPacketTime) >= attackTime))
				{
					m_bProcessedAttack = false;

					CS_Packet_Attack(m_pTargetObject);

					
					static DWORD lastTime = GetTickCount();
					DWORD now = GetTickCount();
					DWORD time = now - lastTime;
					lastTime = now;
					printf("���ݸ޽����� ���ȴ�! ����ð� %d\n", time);
					
				}

				if ((m_fAnimationElapsedTime + m_fLastSendPacketTime) >= animEndTime) //���� �޽����� ���۵ǰ� �ִϸ��̼��� ���� ������ �� �� ����
				{
					m_bProcessedAttack = true;
				}
			}
			else //���� ���� ���̶��
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

	  //0.033333�� ���� �����̹Ƿ� -> �ִϸ��̼ǿ��� �� �ð��� ����ϹǷ� �װ����� �ʱ�ȭ
	  //m_fLastSendPacketTime = 0.0f;
	  */
}