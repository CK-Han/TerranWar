#pragma once

#include "Shader.h"
#include "AstarComponent.h"
#include "DebugPrograms.h"
#include "Particle\SpriteParticle.h"
#include "SoundComponent\SoundManager.h"
#include "NumberFont\NumberFont.h"
#include "BuildCheck\BuildCheck.h"
#include "ProgressBar\ProgressImage.h"

class CGameFramework;

enum UI_RENDERING { mouserect = 1, selectedcircle, mainframe, attached, minimap, minimapobject, minimaprect, neutralresource, crowdimage };
enum SCENE_STATE {
	nothing = 0, pick_scv, scv_build, scv_buildadv,
	build_armory = 256,
	build_barrack = 257,
	build_bunker = 258,
	build_commandcenter = 259,
	build_engineeringbay = 260,
	build_factory = 261,
	build_fusioncore = 262,
	build_ghostacademy = 263,
	build_missileturret = 264,
	build_reactor = 265,
	build_refinery = 266,
	build_sensortower = 267,
	build_starport = 268,
	build_supplydepot = 269,
	build_techlab = 270,

	commandcenter_units,
	barrack_units,
	factory_units,
	starport_units
};

struct CREATE_UNIT_DATA
{
	OBJECT_TYPE obj_type;
	D3DXVECTOR3 position;
	float	time;
};

struct CROWD_SAVE_DATA
{
	unsigned int				nUnits{ 0 };
	CGameObject*	ppObjects[MAX_SELECTED_OBJECTS]; //미리 만들어놓는게 좀 더 편할듯
};

//**********************************************************************
//Scene 조명 처리 내용
#define MAX_LIGHTS		1
#define POINT_LIGHT		1.0f
#define SPOT_LIGHT		2.0f
#define DIRECTIONAL_LIGHT	3.0f

//1개의 조명을 표현하는 구조체이다. 
struct LIGHT
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcSpecular;
	D3DXVECTOR3 m_d3dxvPosition;
	float m_fRange;
	D3DXVECTOR3 m_d3dxvDirection;
	float m_nType;
	D3DXVECTOR3 m_d3dxvAttenuation;
	float m_fFalloff;
	float m_fTheta; //cos(m_fTheta)
	float m_fPhi; //cos(m_fPhi)
	float m_bEnable;
	float padding;
};

//상수 버퍼는 크기가 반드시 16 바이트의 배수가 되어야 한다. 
struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	D3DXCOLOR		 m_d3dxcGlobalAmbient;
	D3DXVECTOR4		 m_d3dxvCameraPosition;
};

struct FogData
{
	D3DXVECTOR4 posValue;
	//16 packing
};

class CCamera;
//**********************************************************************
//Scene 클래스
class CScene
{
public:
	CScene(void* pGameFramework);
	~CScene();

	//테크트리 표현시, Map 수를 줄이기 위해 건물이 지어지거나 파괴되는 상황에서 Map을 호출해야 하는데 이를 위해 DC가 렌더링 상황이 아닐 때에도 필요하다.
	ID3D11DeviceContext*	m_pd3dDeviceContext{ nullptr };

	void BuildObjects(ID3D11Device *pd3dDevice);
	void ReleaseObjects();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CGameObject *PickObjectPointedByCursor(int xClient, int yClient, bool isLeftButton);
	D3DXVECTOR3 GetPositionPicking(int xClient, int yClient);

	
	void SetWindowSize(float width, float height) { m_fWndClientWidth = width; m_fWndClientHeight = height; }
	void SetCamera(CCamera *pCamera);
	
	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseShaderVariables() { /*소멸자에서 진행*/ }

	//지형 위 이벤트를 위한 높이맵 생성 0705 -> 높이맵 이용으로 제거
	//void CreateHeightMap(LPCTSTR pFileName, int nWidth, int nHeight, D3DXVECTOR3& d3dxvScale);

	//카메라 이동이 맵 밖을 벗어나지 못하도록 한다.
	bool isCameraMoveOverMap();

	/*
	//type의 값을 읽어 정적 오브젝트인지 애니메이션 오브젝트인지 구분한다.
	bool isAnimateObject(int type);
	//type의 값을 읽어 적인지 아군 오브젝트인지 확인한다.
	bool isEnemyObject(int type);
	*/


	//UI관련 렌더링의 셰이더 설정
	void OnPrepareUIRender(ID3D11DeviceContext* pd3dDeviceContext, int uiRendingType);
	
	//마우스 드래그 피킹 이벤트
	void CreateMouseRectShader(ID3D11Device *pd3dDevice);
	void GetMouseRectObjects();
	void GetMouseCursorObject();
	void CreateSelectedCircleShader(ID3D11Device *pd3dDevice);

	//0907 통합된 피킹 충돌 함수
	//바운딩박스와 공중유닛여부까지 따져준다.
	bool IsPickedObject(D3DXVECTOR3& vMousePosW, CGameObject* pTarget);
	//이건 순수한 AABB
	bool IsCrushedAABB(D3DXVECTOR3& box1Min, D3DXVECTOR3& box1Max, D3DXVECTOR3& box2Min, D3DXVECTOR3& box2Max);

	//미니맵을 클릭하는 경우, 반환하며 카메라 혹은 유닛 이동처리도 같이 함
	bool IsClickMiniMap(float xScreen, float yScreen, bool bLeftClicked, bool bAttack);
	
	//마우스가 커맨드창 안에 존재하는 지 여부를 확인, 만일 안에 있다면 어느 위치에 있는지 반환
	bool IsCursorInCommand(float xScreen, float yScreen, int& commandPosNum);
	//커맨드 창 위치에 따라, Scene의 상태에 따라 마우스 이동 및 클릭을 처리
	void ProcessCursorInCommand(int commandPosNum, bool bLeftClicked, bool* bDisplayRequire);
	
	//마우스가 군중 유닛 피킹을 하는지 검사, 만일 클릭 시 어느 위치인지 반환
	bool IsCursorInCrowd(float xScreen, float yScreen, int& crowdPosNum);
	//군중 유닛 피킹에 따른 처리
	void ProcessCursorInCrowd(int crowdPosNum);



	//UI 관련
	void CreateMainFrameShader(ID3D11Device* pd3dDevice);
	void CreateAttachedMainFrameShader(ID3D11Device* pd3dDevice);
	void CreateCrowdImageShader(ID3D11Device* pd3dDevice);

	//MiniMap 관련
	void CreateMiniMapShader(ID3D11Device *pd3dDevice);
	void CreateMiniMapObjectShader(ID3D11Device *pd3dDevice);
	void CreateMiniMapRectShader(ID3D11Device* pd3dDevice);

	//Scv가 들고있는 자원 표현
	void CreateNeutralResourceShader(ID3D11Device* pd3dDevice);
	
	//UI 메인 프레임에 들어가는 이미지들을 불러온다. 상태창, 초상화, 명령어 창
	void LoadUIImage(ID3D11Device* pd3dDevice, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState);

	//군중 유닛 이미지 불러오기
	void LoadCrowdUnitImage(ID3D11Device* pd3dDevice, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState);

	//정보를 패킷으로 전송할 때 사용하는 Owner Framework
	CGameFramework* m_pGameFramework;

	//********************************************************************
	CGameObject* FindObjectById(int type, int id);
	CGameObject* FindObjectById(int id);
	//서버에서 클라이언트로 오는 패킷에 대한 처리
	void Process_SC_Packet_Put_Object(sc_packet_put_object* packet);
	//0803 세분화하면서 삭제
	//void Process_SC_Packet_Data_Object(sc_packet_data_object* packet);
	void Process_SC_Packet_Data_Object_State(sc_packet_data_object_state* packet);
	void Process_SC_Packet_Data_Object_Position(sc_packet_data_object_position* packet);
	void Process_SC_Packet_Data_Object_Value(sc_packet_data_object_value* packet);
	void Process_SC_Packet_fail_Object_Move(sc_packet_fail_object_move* packet);
	void Process_SC_Packet_Put_Building(sc_packet_put_building* packet);
	void Process_SC_Packet_Attack_Object(sc_packet_attack_object* packet);

	//0901 노드 테스트
	void Process_SC_Packet_Add_Node(sc_packet_add_node* packet);
	void Process_SC_Packet_Remove_Node(sc_packet_remove_node* packet);

	//0905
	void Process_SC_Packet_Data_Resource(sc_packet_data_resource* packet);
	void Process_SC_Packet_Get_Mineral(sc_packet_get_mineral* packet);
	void Process_SC_Packet_Get_Gas(sc_packet_get_gas* packet);

	//0815 이동 변경
	void CS_Packet_Move_Object(CGameObject* pObject, D3DXVECTOR3& position, bool bRightCliecked);
	void CS_Packet_Create_Building(CGameObject* pSCV, int nodeIndex, OBJECT_TYPE obj_type);
	void CS_Packet_Create_Unit(OBJECT_TYPE obj_type, D3DXVECTOR3 pos);
	void CS_Packet_Dig_Resource(CGameObject* pSCV, CGameObject* pMineral, CGameObject* pCommandcenter);

	bool ProcessInput();
	void UpdateEnemyObject(float fTimeElpased); //
	void UpdateObject(float fTimeElapsed);	//지형 위 움직임과 같은 갱신해야할 이벤트에 대한
	void Animate(float fTimeElapsed);
	void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera);	

	//그림자
	LIGHT*			GetLight(int idx) { return &(m_pLights->m_pLights[idx]); }
	CHeightMap*		GetHeightMap() { return m_pHeightMap; }
	void			CreateShadowShader(ID3D11Device* pd3dDevice);
	void			RenderObjectShadow(ID3D11DeviceContext *pd3dDeviceContext);

	//안개처리 함수
	void	BuildFogBuffer(ID3D11Device *pd3dDevice);
	void	UpdateFogBuffer(ID3D11DeviceContext *pd3dDeviceContext);

	//시야 내 적 리스트 갱신
	void	UpdateEnemyInSight(float fElapsedTime);

	//파티클 이미지 불러오기
	void	LoadParticleImage(ID3D11Device* pd3dDevice);

	//void    PushCreateUnitList(OBJECT_TYPE obj_type, D3DXVECTOR3 pos, float time);
	//void    UpdateCreateUnitList(float elapsedTime);

	void	CreateNewObject(OBJECT_TYPE obj_type, D3DXVECTOR3 vMadenPosition);

	void	UpdateSelectedObjects();
	void	UpdateCrowdSavedData();

	//1001 마우스 클릭 이미지
	void	DrawMouseClickImage(D3DXVECTOR3& vScreenToWorld);
	
	//void	DrawSystemErrorImage(D3DXVECTOR3& vScreenToWorld, int errorCode);

public:
	CShader**		 m_ppShaders;
	int				 m_nShaders;
	
	CStaticShader*		m_pStaticShader;
	CAnimationShader*	m_pAnimationShader;

	CCamera*		 m_pCamera;

	float			m_fCurrentFrameElapsedTime{ 0.0f };

private:
	//조명
	LIGHTS*			 m_pLights;
	ID3D11Buffer*	 m_pd3dcbLights;

	float m_fWndClientWidth;
	float m_fWndClientHeight;

	//테셀레이션에서 높이를 표현하기 위한 raw 높이맵이다.
	CHeightMap*		m_pHeightMap = nullptr;

	//UI는 깊이버퍼를 바꾸지 않으며 비교하지도 않는다. 순서대로 그릴거니까
	ID3D11DepthStencilState*	m_pd3dUIDepthStencilState = nullptr;

	//마우스 이벤트 처리, 블렌딩 상태가 필요할 줄 알았으나 색이 섞일 필요 없이 위에 그려지므로 clip만 수행하니 깔끔한데?
	CTexture*				m_pMouseRectTexture;
	CMouseRectMesh*			m_pMouseRectMesh;
	ID3D11VertexShader*		m_pd3dMouseRectVertexShader;
	ID3D11InputLayout*		m_pd3dMouseRectVertexLayout;
	ID3D11PixelShader*		m_pd3dMouseRectPixelShader;

	bool			m_bLeftClicked{false};
	bool			m_bMouseRectDraw{false};
	D3DXVECTOR3		m_d3dxvOldCursorPosition;
	D3DXVECTOR3		m_d3dxvNextCursorPosition;
	
	//선택된 객체에 대한 처리, 객체들의 위치정보 및 크기정보, 혹시 적군인지 등을 구분하여 상수버퍼에 갱신, 일괄적으로 기하셰이더 렌더링
	CTexture*				m_pSelectedCircleTexture;
	CSelectedCircleMesh*	m_pSelectedCircleMesh;
	ID3D11VertexShader*		m_pd3dSelectedCircleVertexShader;
	ID3D11InputLayout*		m_pd3dSelectedCircleVertexLayout;
	ID3D11GeometryShader*	m_pd3dSelectedCircleGeometryShader;
	ID3D11PixelShader*		m_pd3dSelectedCirclePixelShader;

	//UI 메인 프레임 창
	CTexture*				m_pMainFrameTexture = nullptr;
	CMainFrameMesh*			m_pMainFrameMesh = nullptr;
	ID3D11VertexShader*		m_pd3dMainFrameVertexShader = nullptr;
	ID3D11InputLayout*		m_pd3dMainFrameVertexLayout = nullptr;
	ID3D11PixelShader*		m_pd3dMainFramePixelShader = nullptr;

	//1003 마우스 이미지는 메인 프레임과 같은 셰이더 함수를 사용하면 된다.
	CTexture*				m_pMouseImageTexture{ nullptr };
	CMainFrameMesh*			m_pMouseImageMesh{ nullptr };

	//메인 프레임의 스크린 위치
	//LB = -1.0f, -1.0f  // RT = 1.0f, -0.4f
	
	//0731 ScreenMesh의 Shader로는 미니맵 관련 다른 처리를 못하므로 다른 셰이더함수를 사용하도록 새 Shader를 갖는다.
	CTexture*				m_pMiniMapTexture = nullptr;
	CScreenMesh*			m_pMiniMapMesh = nullptr;
	ID3D11VertexShader*		m_pd3dMiniMapVertexShader = nullptr;
	ID3D11InputLayout*		m_pd3dMiniMapVertexLayout = nullptr;
	ID3D11PixelShader*		m_pd3dMiniMapPixelShader = nullptr;
	
	//0731 미니맵에 표시하는 객체들의 대한 처리 -> 텍스쳐 리소스는 따로 필요 없다.
	CMiniMapObjectMesh*		m_pMiniMapObjectMesh = nullptr;
	ID3D11VertexShader*		m_pd3dMiniMapObjectVertexShader= nullptr;
	ID3D11InputLayout*		m_pd3dMiniMapObjectVertexLayout= nullptr;
	ID3D11GeometryShader*	m_pd3dMiniMapObjectGeometryShader= nullptr;
	ID3D11PixelShader*		m_pd3dMiniMapObjectPixelShader= nullptr;

	//0905 MinimapRect
	CTexture*					m_pMiniMapRectTexture = nullptr;
	CMiniMapRectMesh*			m_pMiniMapRectMesh = nullptr;
	ID3D11VertexShader*			m_pd3dMiniMapRectVertexShader = nullptr;
	ID3D11InputLayout*			m_pd3dMiniMapRectVertexLayout = nullptr;
	ID3D11PixelShader*			m_pd3dMiniMapRectPixelShader = nullptr;

	//0905 NeutralResource
	CTexture*					m_pNeutralResourceTexture = nullptr;
	CNeutralResourceMesh*		m_pNeutralResourceMesh = nullptr;
	ID3D11VertexShader*			m_pd3dNeutralResourceVertexShader = nullptr;
	ID3D11InputLayout*			m_pd3dNeutralResourceVertexLayout = nullptr;
	ID3D11GeometryShader*		m_pd3dNeutralResourceGeometryShader = nullptr;
	ID3D11PixelShader*			m_pd3dNeutralResourcePixelShader = nullptr;

	
	//UI 메인 프레임 위에 붙는 ScreenMesh들에 대한 Shader
	ID3D11VertexShader*		m_pd3dAttachedMainFrameVertexShader = nullptr;
	ID3D11InputLayout*		m_pd3dAttachedMainFrameVertexLayout = nullptr;
	ID3D11PixelShader*		m_pd3dAttachedMainFramePixelShader = nullptr;

	//UI 커맨드
	map<string, CTexture*>		m_mapCommandTexture;
	map<string, CScreenMesh*>	m_mapCommandMesh;
	
	//UI 상태창
	map<int, CTexture*>		m_mapStatusTexture;
	CScreenMesh*			m_pStatusMesh = nullptr;


	//1003 상태창에 그려지는 군중 유닛 표시
	CTexture*						m_pCrowdFrameTexture{ nullptr };
	map<OBJECT_TYPE, CTexture*>		m_mapCrowdUnitTexture;
	
	vector<CCrowdMesh*>		m_vecCrowdMeshes;
	ID3D11VertexShader*		m_pd3dCrowdImageVertexShader{ nullptr };
	ID3D11InputLayout*		m_pd3dCrowdImageVertexLayout{ nullptr };
	ID3D11PixelShader*		m_pd3dCrowdImagePixelShader{ nullptr };

	//부대지정 데이터 여부 표시
	CTexture*				m_pCrowdAttachedFrameTexture{ nullptr };
	vector<CScreenMesh*>	m_vecAttachedCrowdMeshes;


	//UI 초상화
	map<int, CTexture*>		m_mapPortraitTexture;
	CScreenMesh*			m_pPortraitMesh = nullptr;
	
	//길찾기를 위한 Astar 컴포넌트이다
	AStarComponent*			m_pAStarComponent;

	//디버그 그리드를 그리기 위한 
	CDebugPrograms*			m_pDebugPrograms;

	//안개처리용 상수버퍼
	ID3D11Buffer*		m_pd3dcbFogBuffer{ nullptr };

	ID3D11Buffer*				m_pd3dFogBuffer{ nullptr };
	ID3D11ShaderResourceView*	m_pd3dSRVFog{ nullptr };
	D3DXVECTOR4					m_d3dxvFogData[(int)(NUM_FOG_SLICE * NUM_FOG_SLICE)];

	//현재 내 시야(뷰리스트)에 포함되는 모든 적 객체들의 집합, 이 리스트는 내 객체 갱신 전에 갱신되어야 한다.
	list<CGameObject*>		m_listMy;			//렌더링되는 내 객체들의 리스트
	list<CGameObject*>		m_listEnemy;		//현재 게임에 존재하는 상대방 객체들의 리스트
	list<CGameObject*>		m_listEnemyInSight;	//적 객체로써 렌더링되어야 하는, 내 시야 안에 존재하는 적 리스트
	list<CGameObject*>		m_listNeutral;		//미네랄과 광물의 정보들

	//파티클 이미지
	CSpriteParticle*		m_pParticleBox{ nullptr };

	//0812 sound
	CSoundManager*			m_pSoundManager{ nullptr };

	CNumberFont*			m_pNumberFont{ nullptr };
	CBuildCheck*			m_pBuildCheck{ nullptr };

	//0826 프로그레스 바
	CScreenMesh*			m_pProgressIconMesh{ nullptr };
	CProgressImage*			m_pProgressImage{ nullptr };

	//1206 승패처리
	CScreenMesh*			m_pResultImageMesh{ nullptr };
	CTexture*				m_pResultImageVictory{ nullptr };
	CTexture*				m_pResultImageDefeat{ nullptr };

public:
	int				 m_nValidSelectedObjects;
	CGameObject*	 m_ppSelectedObjects[MAX_SELECTED_OBJECTS];	

	int						m_nSceneState{ 0 };
	list<string>			m_listSCVBuild;
	list<string>			m_listSCVBuildAdv;
	list<string>			m_listBarrackUnits;
	list<string>			m_listFactoryUnits;
	list<string>			m_listStarportUnits;

	bool					m_bActivedBuildCheck{ false };
	string					m_strActivedBuildCheck;
	POINT					m_ptMousePosition;

	list<CREATE_UNIT_DATA>	m_listCreateUnits;

	//0825 커맨드 창 마우스 매핑, 좌상단부터 1~5, 한 칸 아래 6~10, 맨 아래칸 11~15로 한다. pair는 LB과 RT이다.
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>		m_mapCommandPosition;

	//1003 군중 유닛 status 표시, 1~8, 9~16, 17~24이고 pair는 LB, RT로 한다.
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>		m_mapCrowdPosition;

	CROWD_SAVE_DATA			m_CrowdSaveData[8]; //1 부터 8 까지만 쓰기로 했다.
	float					m_fLastInputCrowdDataButtonTime{ 1000.0f };
	char					m_chLastInputKey{ 0 };

	//부대지정 몇 번이 되어있는지 확인하기 위한 crowd_AttachMainFrame
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>		m_mapAttachedCrowdPosition;
	
	CCubeShader				m_DebugCubeShader;

	//1206 테크트리 적용 및 이미지 표현
	map<OBJECT_TYPE, bool>		m_mapIsBuildingExist; //건물의 이름으로 검색, bool로 현재 지어져있는지 여부를 확인한다.

	bool						m_bGameEnd{ false };
	bool						m_bGameWin{ false };
};

