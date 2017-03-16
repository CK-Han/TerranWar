#include "../Object.h"
#include "ParticleObject.h"


//**************************************************************************
//����ü
CProjectileParticleObject::CProjectileParticleObject() : CParticleObject()
{

}

CProjectileParticleObject::~CProjectileParticleObject()
{

}


bool CProjectileParticleObject::UpdateAndGetValid(float fElapsedTime)
{
	m_fParticleElapsedTime += fElapsedTime;
	
	D3DXVECTOR3 vDir = m_pTargetObject->GetPosition() - D3DXVECTOR3(m_d3dxmtxWorld._41, m_d3dxmtxWorld._42, m_d3dxmtxWorld._43);
	D3DXVec3Normalize(&vDir, &vDir);
	
	m_d3dxmtxWorld._41 += vDir.x * m_fProjectileSpeed * fElapsedTime;
	m_d3dxmtxWorld._42 += vDir.y * m_fProjectileSpeed * fElapsedTime;
	m_d3dxmtxWorld._43 += vDir.z * m_fProjectileSpeed * fElapsedTime;

	//ȸ���� ���� �׻� vMax�� x+z+������ �ƴ� �� �ִٴ� �� ����
	D3DXVECTOR3 vMin, vMax;
	//D3DXVec3TransformCoord(&vMin, &(m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMinimum), &(m_pTargetObject->m_d3dxmtxWorld));
	//D3DXVec3TransformCoord(&vMax, &(m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMaximum), &(m_pTargetObject->m_d3dxmtxWorld));
	vMin = m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMinimum;
	vMax = m_pTargetObject->m_bcMeshBoundingCube.m_d3dxvMaximum;
	vMin.x += m_pTargetObject->m_d3dxmtxWorld._41; vMax.x += m_pTargetObject->m_d3dxmtxWorld._41;
	vMin.y += m_pTargetObject->m_d3dxmtxWorld._42; vMax.y += m_pTargetObject->m_d3dxmtxWorld._42;
	vMin.z += m_pTargetObject->m_d3dxmtxWorld._43; vMax.z += m_pTargetObject->m_d3dxmtxWorld._43;

	if (vMin.x <= m_d3dxmtxWorld._41 && vMax.x >= m_d3dxmtxWorld._41 &&
		vMin.y <= m_d3dxmtxWorld._42 && vMax.y >= m_d3dxmtxWorld._42 &&
		vMin.z <= m_d3dxmtxWorld._43 && vMax.z >= m_d3dxmtxWorld._43)
		return false; //�浹�� �Ǿ���.
	else
		return true;
}

//**************************************************************************
//���
CPromptParticleObject::CPromptParticleObject() : CParticleObject()
{

}

CPromptParticleObject::~CPromptParticleObject()
{

}

bool CPromptParticleObject::UpdateAndGetValid(float fElapsedTime)
{
	m_fParticleElapsedTime += fElapsedTime;

	if (m_fMaxParticleTime < m_fParticleElapsedTime)
		return false;
	else 
		return true;
}