#pragma once

#include "Mesh.h"
#include "Camera.h"

//소켓 통신
#include "SocketComponent.h"

//공격 처리 시 타겟에 대한 길을 찾아야 하는 경우
#include "AstarComponent.h"

//파티클 처리
#include "Particle\SpriteParticle.h"

//사운드 처리
#include "SoundComponent\SoundManager.h"

//1214 업데이트
#include "GlobalFunctions.h"


enum SCV_DIG { invalid = 0, mineral, gas};

class CHeightMap;

//**********************************************************************
//기본 게임 오브젝트의 정의
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

	//월드 좌표계의 픽킹 광선을 생성한다.
	void GenerateRayForPicking(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxWorld, D3DXMATRIX *pd3dxmtxView, D3DXVECTOR3 *pd3dxvPickRayPosition, D3DXVECTOR3 *pd3dxvPickRayDirection);
	//월드 좌표계의 픽킹 광선을 생성한다.
	int PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);

	//사실상 안쓰는 코드
	void MoveForward(float fDistance);
	void MoveDirection(D3DXVECTOR3 normalizedDir, float fDistance);

	//객체가 가지는 메쉬 전체에 대한 바운딩 박스이다.
	AABB m_bcMeshBoundingCube;

private:
	int m_nReferences;
	bool m_bActive;

	//기존 프레임워크의 함수
	////////////////////////////////////////////////////////////////////

public:
	//시야 처리
	bool IsInSight() { return m_bInSight; }
	void SetInSight(bool bInSight = false) { m_bInSight = bInSight; }

	//렌더링 여부, 죽는 처리 등
	void SetActive(bool bActive = false) { m_bActive = bActive; }
	bool GetActive() { return m_bActive; }

	//목표 지점 이동
	virtual void SetTargetPosition(D3DXVECTOR3 target) {}
	virtual void FreeTargetPosition() {}

	//지형 위 이동과 같은 갱신 내용에 대한 처리
	virtual void UpdateMoveEvent(CHeightMap* pHeightMap) {}
	virtual void UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList) {}
	virtual void Animate(float fTimeElapsed) {}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera);	// Shader에서 인스턴싱하므로 사용 X

	//공격 시 회전
	void RotateToTarget(D3DXVECTOR3 vTarget);
	void RotateToTarget(CGameObject* pTarget);

	D3DXMATRIX m_d3dxmtxLocal;	//메시의 로컬이 곱해지게 될 것이다.
	D3DXMATRIX m_d3dxmtxWorld;	//월드변환 행렬

	bool							 m_bHasTargetPosition{ false };
	D3DXVECTOR3						 m_d3dxvTargetPosition{ D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX) };
	std::list<D3DXVECTOR3>			 m_listTargetPosition;

public:
	//소켓 통신
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
	
	//로직에 필요한 객체 변수들의 나열
	int		m_nObjectID;
	int		m_nObjectType;

	float m_fObjectSpeed{ 3.0f };
	float m_fObjectSightRange{ 20.0f };
	
	bool m_bInSight{ false };	//아군은 항상 true, 적군은 유동적 -> 렌더링 여부를 판단하기 위해서
	bool m_bAirUnit{ false };

	//0731 test 패킷 처리
	bool			m_bLastPacketProcessed{ true };

	//0731 공격처리를 위한 객체 상태처리
	float			m_fObjectAttackRange{ 12.0f };
	CGameObject*	m_pTargetObject{ nullptr };
	State			m_ObjectState{ State::stop };
	bool			m_bGroundAttack{ true };
	bool			m_bAirAttack{ false };

	//0805 길찾기를 갓 실행했을 때를 확인하기위해
	bool			m_bLatestFindPath{ false };

	
	//0811 파티클 표현
	//D3DXVECTOR3						m_d3dxvAttackParticlePosition;
	string							m_strParticle;
	CProjectileParticleObject		m_ProjectileParticle;
	CPromptParticleObject			m_PromptParticle;
	bool							m_bProjectileParticle;

	//0812 사운드
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
//높이맵 지형 오브젝트 클래스
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D11Device *pd3dDevice, LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, D3DXVECTOR3 d3dxvScale);
	virtual ~CHeightMapTerrain();

public:
	//지형의 실제 높이를 반환한다. 높이 맵의 높이에 스케일을 곱한 값이다.
	float GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMap->GetHeight(x, z, bReverseQuad) * m_d3dxvScale.y); }
	D3DXVECTOR3 GetNormal(float x, float z) { return(m_pHeightMap->GetHeightMapNormal(int(x / m_d3dxvScale.x), int(z / m_d3dxvScale.z))); }

	int GetHeightMapWidth() { return(m_pHeightMap->GetHeightMapWidth()); }
	int GetHeightMapLength() { return(m_pHeightMap->GetHeightMapLength()); }

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	//지형의 실제 크기(가로/세로)를 반환한다. 높이 맵의 크기에 스케일을 곱한 값이다.
	float GetWidth() { return(m_nWidth * m_d3dxvScale.x); }
	float GetLength() { return(m_nLength * m_d3dxvScale.z); }
	CHeightMap*	GetHeightMap() { return m_pHeightMap; }

	float GetPeakHeight() { return(m_bcMeshBoundingCube.m_d3dxvMaximum.y); }

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);


private:
	//지형의 높이 맵으로 사용할 이미지이다.
	CHeightMap *m_pHeightMap;

	//지형의 가로와 세로 크기이다.
	int m_nWidth;
	int m_nLength;

	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다.
	D3DXVECTOR3 m_d3dxvScale;

	//와이어프레임 테스트용
	ID3D11RasterizerState		*m_pd3dRasterizerState{ nullptr };
};




//**********************************************************************
//정적 오브젝트 클래스 -> 건물 및 자원
class CStaticObject : public CGameObject
{
public:
	CStaticObject();
	virtual ~CStaticObject();

public:
	//가상함수 나열
	virtual void UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList);

	virtual void Animate(float fTimeElapsed) {}
	virtual void Process_SC_Value(sc_packet_data_object_value* packet);
	virtual void Process_SC_State(sc_packet_data_object_state* packet); //건물은 State 받는 경우가 Die인 경우밖에 없을것이다.
	
	void AddParticleImage(); //폭발 파티클 진행

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
	OBJECT_TYPE			m_typeGeneratedUnit{ OBJECT_TYPE::Invalid}; //Invalid을 갖는 경우 생성하는 유닛이 없다고 판단
	CGameObject*		m_pBeforeGasModel{ nullptr };	//리파이너리인 경우, 지어지기 전 가스의 포인트를 저장, 건물이 터질 때 다시 활성화시키도록
};

//**********************************************************************
//애니메이션 오브젝트 클래스
class CAnimationObject : public CGameObject
{
public:
	CAnimationObject();
	virtual ~CAnimationObject();

	//가상함수 나열
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

	//애니메이션 관련
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetAnimation(string& animName);

	std::string						m_strAnimationClip;		//애니메이션 클립 이름
	std::string						m_strCurrentAnimation;	//클립에 존재하는 동작 구분
	map<string, pair<int, int>>		m_mapAnimationSet;
	float							m_fTotalAnimationElapsedTime{ 0.0f };
	//0907 공속을 위해
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
	CGameObject*					m_pGasModel{ nullptr };		//가스지을 때, 이동해서 지어야 한다면 가스의 포인터를 갖고 있는다.

	//0904 have_target 공회전 제한
	int								m_nCannotFindTargetPosition{ 0 };

	//0905 공격 로직 수정
	bool							m_bPickTarget{ false };

	//0905 자원 들고있기
	int								m_nScvDigResource{ SCV_DIG::invalid };

	//0907 공격속도
	float							m_fAttackSpeed{ 1.0f };

	//1001 총구 오프셋
	bool							m_bAttackOffset{ false };
	int								m_nAttackOffsetIndex{ -1 };
};

//**********************************************************************
//지상 유닛 오브젝트 클래스
class CGroundObject : public CAnimationObject
{
public:
	CGroundObject();
	virtual ~CGroundObject();
};

//**********************************************************************
//공중 유닛 오브젝트 클래스
class CAirObject : public CAnimationObject
{
public:
	CAirObject();
	virtual ~CAirObject();

	//가상함수 나열 -> 필요없는 부분은 상속 X
	virtual void UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList);
	virtual void CS_Packet_Move(D3DXVECTOR3 vTo, bool bRightClicked);

	//새로이 만든 함수
	void	AirUnitFindPath(D3DXVECTOR3 target);

public:
	float				m_fAirOffset{ AIR_UNIT_OFFSET };

	
};