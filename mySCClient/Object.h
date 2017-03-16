#pragma once

#include "Mesh.h"
#include "Camera.h"

//���� ���
#include "SocketComponent.h"

//���� ó�� �� Ÿ�ٿ� ���� ���� ã�ƾ� �ϴ� ���
#include "AstarComponent.h"

//��ƼŬ ó��
#include "Particle\SpriteParticle.h"

//���� ó��
#include "SoundComponent\SoundManager.h"

//1214 ������Ʈ
#include "GlobalFunctions.h"


enum SCV_DIG { invalid = 0, mineral, gas};

class CHeightMap;

//**********************************************************************
//�⺻ ���� ������Ʈ�� ����
class CGameObject
{
public:
	CGameObject(int nMeshes = 0);
	virtual ~CGameObject();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	CMesh *GetMesh(int i = 0) { return(m_ppMeshes[i]); }

	virtual void SetMesh(CMesh *pMesh, int nIndex);

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(D3DXVECTOR3 d3dxvPosition);

	D3DXVECTOR3 GetPosition();

	bool IsVisible(CCamera *pCamera = nullptr);

	//���� ��ǥ���� ��ŷ ������ �����Ѵ�.
	void GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection);
	//���� ��ǥ���� ��ŷ ������ �����Ѵ�.
	int PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);

	//��ǻ� �Ⱦ��� �ڵ�
	void MoveForward(float fDistance);
	void MoveDirection(D3DXVECTOR3 normalizedDir, float fDistance);

	//��ü�� ������ �޽� ��ü�� ���� �ٿ�� �ڽ��̴�.
	AABB m_bcMeshBoundingCube;

private:
	int m_nReferences;
	bool m_bActive;

	//���� �����ӿ�ũ�� �Լ�
	////////////////////////////////////////////////////////////////////

public:
	//�þ� ó��
	bool IsInSight() { return m_bInSight; }
	void SetInSight(bool bInSight = false) { m_bInSight = bInSight; }

	//������ ����, �״� ó�� ��
	void SetActive(bool bActive = false) { m_bActive = bActive; }
	bool GetActive() { return m_bActive; }

	//��ǥ ���� �̵�
	virtual void SetTargetPosition(D3DXVECTOR3 target) {}
	virtual void FreeTargetPosition() {}

	//���� �� �̵��� ���� ���� ���뿡 ���� ó��
	virtual void UpdateMoveEvent(CHeightMap* pHeightMap) {}
	virtual void UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList) {}
	virtual void Animate(float fTimeElapsed) {}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera);	// Shader���� �ν��Ͻ��ϹǷ� ��� X

	//���� �� ȸ��
	void RotateToTarget(D3DXVECTOR3 vTarget);
	void RotateToTarget(CGameObject* pTarget);

	D3DXMATRIX m_d3dxmtxLocal;	//�޽��� ������ �������� �� ���̴�.
	D3DXMATRIX m_d3dxmtxWorld;	//���庯ȯ ���

	bool							 m_bHasTargetPosition{ false };
	D3DXVECTOR3						 m_d3dxvTargetPosition{ D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX) };
	std::list<D3DXVECTOR3>			 m_listTargetPosition;

public:
	//���� ���
	virtual void CS_Packet_State(State state);
	virtual void Process_SC_State(sc_packet_data_object_state* packet);

	virtual void CS_Packet_Move(D3DXVECTOR3 vTo, bool bRightClicked) {}
	virtual void Process_SC_Move(D3DXVECTOR3 vPos) {}
	virtual void Process_SC_Move_Enemy(D3DXVECTOR3 vPos) {}
	
	virtual void CS_Packet_Attack(CGameObject* pTarget) {}
	virtual void Process_SC_Attack(sc_packet_attack_object* packet, CGameObject* pTarget) {}
	virtual void BeginParticleImage(CGameObject* pTarget) {}

	virtual void Process_SC_Value(sc_packet_data_object_value* packet) {}
	virtual void Process_SC_Fail_Move() {}

	float m_fLastSendPacketTime{ 0.0f };

public:
	CMesh**			m_ppMeshes;
	int				m_nMeshes;
	
	//������ �ʿ��� ��ü �������� ����
	int		m_nObjectID;
	int		m_nObjectType;

	float m_fObjectSpeed{ 3.0f };
	float m_fObjectSightRange{ 20.0f };
	
	bool m_bInSight{ false };	//�Ʊ��� �׻� true, ������ ������ -> ������ ���θ� �Ǵ��ϱ� ���ؼ�
	bool m_bAirUnit{ false };

	//0731 test ��Ŷ ó��
	bool			m_bLastPacketProcessed{ true };

	//0731 ����ó���� ���� ��ü ����ó��
	float			m_fObjectAttackRange{ 12.0f };
	CGameObject*	m_pTargetObject{ nullptr };
	State			m_ObjectState{ State::stop };
	bool			m_bGroundAttack{ true };
	bool			m_bAirAttack{ false };

	//0805 ��ã�⸦ �� �������� ���� Ȯ���ϱ�����
	bool			m_bLatestFindPath{ false };

	
	//0811 ��ƼŬ ǥ��
	//D3DXVECTOR3						m_d3dxvAttackParticlePosition;
	string							m_strParticle;
	CProjectileParticleObject		m_ProjectileParticle;
	CPromptParticleObject			m_PromptParticle;
	bool							m_bProjectileParticle;

	//0812 ����
	int								m_nSoundOffset{ 7 };

	//0814 UI list
	int								m_nNeedMineral{ 0 };
	int								m_nNeedGas{ 0 };
	list<string>					m_listUI;
	
	int								m_nHP{ 40 };
	int								m_nMaxHP{ 40 };
	bool							m_bDeath{ false };
	
	bool							m_bLastStatePacketProcessed{ true };
};


//**********************************************************************
//���̸� ���� ������Ʈ Ŭ����
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D11Device *pd3dDevice, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, D3DXVECTOR3 d3dxvScale);
	virtual ~CHeightMapTerrain();

public:
	//������ ���� ���̸� ��ȯ�Ѵ�. ���� ���� ���̿� �������� ���� ���̴�.
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMap->GetHeight(x, z, bReverseQuad) * m_d3dxvScale.y); }
	D3DXVECTOR3 GetNormal(float x, float z) { return(m_pHeightMap->GetHeightMapNormal(int(x / m_d3dxvScale.x), int(z / m_d3dxvScale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMap->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMap->GetHeightMapLength()); }

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	//������ ���� ũ��(����/����)�� ��ȯ�Ѵ�. ���� ���� ũ�⿡ �������� ���� ���̴�.
	float GetWidth() { return(m_nWidth * m_d3dxvScale.x); }
	float GetLength() { return(m_nLength * m_d3dxvScale.z); }
	CHeightMap*	GetHeightMap() { return m_pHeightMap; }

	float GetPeakHeight() { return(m_bcMeshBoundingCube.m_d3dxvMaximum.y); }

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);


private:
	//������ ���� ������ ����� �̹����̴�.
	CHeightMap *m_pHeightMap;

	//������ ���ο� ���� ũ���̴�.
	int m_nWidth;
	int m_nLength;

	//������ ������ �� �� Ȯ���� ���ΰ��� ��Ÿ���� ������ �����̴�.
	D3DXVECTOR3 m_d3dxvScale;

	//���̾������� �׽�Ʈ��
	ID3D11RasterizerState		*m_pd3dRasterizerState{ nullptr };
};




//**********************************************************************
//���� ������Ʈ Ŭ���� -> �ǹ� �� �ڿ�
class CStaticObject : public CGameObject
{
public:
	CStaticObject();
	virtual ~CStaticObject();

public:
	//�����Լ� ����
	virtual void UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList);

	virtual void Animate(float fTimeElapsed) {}
	virtual void Process_SC_Value(sc_packet_data_object_value* packet);
	virtual void Process_SC_State(sc_packet_data_object_state* packet); //�ǹ��� State �޴� ��찡 Die�� ���ۿ� �������̴�.
	
	void AddParticleImage(); //���� ��ƼŬ ����

	void	CancelGenerateUnit() { m_typeGeneratedUnit = OBJECT_TYPE::Invalid; m_fUnitGeneratedTime = 0.0f; }
	void	CS_Packet_Create_Unit(OBJECT_TYPE obj_type, D3DXVECTOR3 pos);
	bool	GetCompleteBuild() { return m_fBuildTime / m_fCompleteBuildTime >= 1.0f; }
	bool	GenerateUnit(OBJECT_TYPE obj_type, float fCompleteTime);

public:
	float			m_fCompleteBuildTime{ 10.0f };
	float			m_fBuildTime{ 0.0f };

	//0826
	float				m_fUnitGeneratedTime{ 0.0f };
	float				m_fUnitCompleteTime{ 10.0f };
	OBJECT_TYPE			m_typeGeneratedUnit{ OBJECT_TYPE::Invalid}; //Invalid�� ���� ��� �����ϴ� ������ ���ٰ� �Ǵ�
	CGameObject*		m_pBeforeGasModel{ nullptr };	//�����̳ʸ��� ���, �������� �� ������ ����Ʈ�� ����, �ǹ��� ���� �� �ٽ� Ȱ��ȭ��Ű����
};

//**********************************************************************
//�ִϸ��̼� ������Ʈ Ŭ����
class CAnimationObject : public CGameObject
{
public:
	CAnimationObject();
	virtual ~CAnimationObject();

	//�����Լ� ����
	virtual void SetTargetPosition(D3DXVECTOR3 target);
	virtual void FreeTargetPosition();
	
	virtual void UpdateMoveEvent(CHeightMap* pHeightMap);
	virtual void UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList);
	virtual void Animate(float fTimeElapsed);

	virtual void CS_Packet_Move(D3DXVECTOR3 vTo, bool bRightClicked);
	virtual void Process_SC_Move(D3DXVECTOR3 vPos);
	virtual void Process_SC_Move_Enemy(D3DXVECTOR3 vPos);

	virtual void CS_Packet_Attack(CGameObject* pTarget);
	virtual void Process_SC_Attack(sc_packet_attack_object* packet, CGameObject* pTarget);
	virtual void BeginParticleImage(CGameObject* pTarget);

	virtual void Process_SC_Value(sc_packet_data_object_value* packet);
	virtual void Process_SC_Fail_Move();

	virtual void Process_SC_State(sc_packet_data_object_state* packet);

	//�ִϸ��̼� ����
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetAnimation(string& animName);

	std::string						m_strAnimationClip;		//�ִϸ��̼� Ŭ�� �̸�
	std::string						m_strCurrentAnimation;	//Ŭ���� �����ϴ� ���� ����
	map<string, pair<int, int>>		m_mapAnimationSet;
	float							m_fTotalAnimationElapsedTime{ 0.0f };
	//0907 ������ ����
	float							m_fAnimationElapsedTime{ 0.0f };
	std::vector<D3DXMATRIX>			m_vecFinalTransforms;

	bool							m_bProcessedAttack{ true };
	//0812 attack state
	int								m_nTargetObjectID{ -1 };
	//0901 have_target state
	int								m_nElapsedMoveCount{ UPDATE_MOVE_COUNT + 1 };

	//0904 scv;;
	bool							m_bMoveForBuild{ false };
	OBJECT_TYPE						m_WillBuildObjectType{ OBJECT_TYPE::Invalid };
	CGameObject*					m_pGasModel{ nullptr };		//�������� ��, �̵��ؼ� ����� �Ѵٸ� ������ �����͸� ���� �ִ´�.

	//0904 have_target ��ȸ�� ����
	int								m_nCannotFindTargetPosition{ 0 };

	//0905 ���� ���� ����
	bool							m_bPickTarget{ false };

	//0905 �ڿ� ����ֱ�
	int								m_nScvDigResource{ SCV_DIG::invalid };

	//0907 ���ݼӵ�
	float							m_fAttackSpeed{ 1.0f };

	//1001 �ѱ� ������
	bool							m_bAttackOffset{ false };
	int								m_nAttackOffsetIndex{ -1 };
};

//**********************************************************************
//���� ���� ������Ʈ Ŭ����
class CGroundObject : public CAnimationObject
{
public:
	CGroundObject();
	virtual ~CGroundObject();
};

//**********************************************************************
//���� ���� ������Ʈ Ŭ����
class CAirObject : public CAnimationObject
{
public:
	CAirObject();
	virtual ~CAirObject();

	//�����Լ� ���� -> �ʿ���� �κ��� ��� X
	virtual void UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList);
	virtual void CS_Packet_Move(D3DXVECTOR3 vTo, bool bRightClicked);

	//������ ���� �Լ�
	void	AirUnitFindPath(D3DXVECTOR3 target);

public:
	float				m_fAirOffset{ AIR_UNIT_OFFSET };

	
};