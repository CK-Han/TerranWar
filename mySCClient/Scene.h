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
	CGameObject*	ppObjects[MAX_SELECTED_OBJECTS]; //�̸� �������°� �� �� ���ҵ�
};

//**********************************************************************
//Scene ���� ó�� ����
#define MAX_LIGHTS		1
#define POINT_LIGHT		1.0f
#define SPOT_LIGHT		2.0f
#define DIRECTIONAL_LIGHT	3.0f

//1���� ������ ǥ���ϴ� ����ü�̴�. 
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

//��� ���۴� ũ�Ⱑ �ݵ�� 16 ����Ʈ�� ����� �Ǿ�� �Ѵ�. 
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
//Scene Ŭ����
class CScene
{
public:
	CScene(void* pGameFramework);
	~CScene();

	//��ũƮ�� ǥ����, Map ���� ���̱� ���� �ǹ��� �������ų� �ı��Ǵ� ��Ȳ���� Map�� ȣ���ؾ� �ϴµ� �̸� ���� DC�� ������ ��Ȳ�� �ƴ� ������ �ʿ��ϴ�.
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
	void ReleaseShaderVariables() { /*�Ҹ��ڿ��� ����*/ }

	//���� �� �̺�Ʈ�� ���� ���̸� ���� 0705 -> ���̸� �̿����� ����
	//void CreateHeightMap(LPCTSTR pFileName, int nWidth, int nHeight, D3DXVECTOR3& d3dxvScale);

	//ī�޶� �̵��� �� ���� ����� ���ϵ��� �Ѵ�.
	bool isCameraMoveOverMap();

	/*
	//type�� ���� �о� ���� ������Ʈ���� �ִϸ��̼� ������Ʈ���� �����Ѵ�.
	bool isAnimateObject(int type);
	//type�� ���� �о� ������ �Ʊ� ������Ʈ���� Ȯ���Ѵ�.
	bool isEnemyObject(int type);
	*/


	//UI���� �������� ���̴� ����
	void OnPrepareUIRender(ID3D11DeviceContext* pd3dDeviceContext, int uiRendingType);
	
	//���콺 �巡�� ��ŷ �̺�Ʈ
	void CreateMouseRectShader(ID3D11Device *pd3dDevice);
	void GetMouseRectObjects();
	void GetMouseCursorObject();
	void CreateSelectedCircleShader(ID3D11Device *pd3dDevice);

	//0907 ���յ� ��ŷ �浹 �Լ�
	//�ٿ���ڽ��� �������ֿ��α��� �����ش�.
	bool IsPickedObject(D3DXVECTOR3& vMousePosW, CGameObject* pTarget);
	//�̰� ������ AABB
	bool IsCrushedAABB(D3DXVECTOR3& box1Min, D3DXVECTOR3& box1Max, D3DXVECTOR3& box2Min, D3DXVECTOR3& box2Max);

	//�̴ϸ��� Ŭ���ϴ� ���, ��ȯ�ϸ� ī�޶� Ȥ�� ���� �̵�ó���� ���� ��
	bool IsClickMiniMap(float xScreen, float yScreen, bool bLeftClicked, bool bAttack);
	
	//���콺�� Ŀ�ǵ�â �ȿ� �����ϴ� �� ���θ� Ȯ��, ���� �ȿ� �ִٸ� ��� ��ġ�� �ִ��� ��ȯ
	bool IsCursorInCommand(float xScreen, float yScreen, int& commandPosNum);
	//Ŀ�ǵ� â ��ġ�� ����, Scene�� ���¿� ���� ���콺 �̵� �� Ŭ���� ó��
	void ProcessCursorInCommand(int commandPosNum, bool bLeftClicked, bool* bDisplayRequire);
	
	//���콺�� ���� ���� ��ŷ�� �ϴ��� �˻�, ���� Ŭ�� �� ��� ��ġ���� ��ȯ
	bool IsCursorInCrowd(float xScreen, float yScreen, int& crowdPosNum);
	//���� ���� ��ŷ�� ���� ó��
	void ProcessCursorInCrowd(int crowdPosNum);



	//UI ����
	void CreateMainFrameShader(ID3D11Device* pd3dDevice);
	void CreateAttachedMainFrameShader(ID3D11Device* pd3dDevice);
	void CreateCrowdImageShader(ID3D11Device* pd3dDevice);

	//MiniMap ����
	void CreateMiniMapShader(ID3D11Device *pd3dDevice);
	void CreateMiniMapObjectShader(ID3D11Device *pd3dDevice);
	void CreateMiniMapRectShader(ID3D11Device* pd3dDevice);

	//Scv�� ����ִ� �ڿ� ǥ��
	void CreateNeutralResourceShader(ID3D11Device* pd3dDevice);
	
	//UI ���� �����ӿ� ���� �̹������� �ҷ��´�. ����â, �ʻ�ȭ, ��ɾ� â
	void LoadUIImage(ID3D11Device* pd3dDevice, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState);

	//���� ���� �̹��� �ҷ�����
	void LoadCrowdUnitImage(ID3D11Device* pd3dDevice, ID3D11ShaderResourceView *pd3dsrvTexture, ID3D11SamplerState *pd3dSamplerState);

	//������ ��Ŷ���� ������ �� ����ϴ� Owner Framework
	CGameFramework* m_pGameFramework;

	//********************************************************************
	CGameObject* FindObjectById(int type, int id);
	CGameObject* FindObjectById(int id);
	//�������� Ŭ���̾�Ʈ�� ���� ��Ŷ�� ���� ó��
	void Process_SC_Packet_Put_Object(sc_packet_put_object* packet);
	//0803 ����ȭ�ϸ鼭 ����
	//void Process_SC_Packet_Data_Object(sc_packet_data_object* packet);
	void Process_SC_Packet_Data_Object_State(sc_packet_data_object_state* packet);
	void Process_SC_Packet_Data_Object_Position(sc_packet_data_object_position* packet);
	void Process_SC_Packet_Data_Object_Value(sc_packet_data_object_value* packet);
	void Process_SC_Packet_fail_Object_Move(sc_packet_fail_object_move* packet);
	void Process_SC_Packet_Put_Building(sc_packet_put_building* packet);
	void Process_SC_Packet_Attack_Object(sc_packet_attack_object* packet);

	//0901 ��� �׽�Ʈ
	void Process_SC_Packet_Add_Node(sc_packet_add_node* packet);
	void Process_SC_Packet_Remove_Node(sc_packet_remove_node* packet);

	//0905
	void Process_SC_Packet_Data_Resource(sc_packet_data_resource* packet);
	void Process_SC_Packet_Get_Mineral(sc_packet_get_mineral* packet);
	void Process_SC_Packet_Get_Gas(sc_packet_get_gas* packet);

	//0815 �̵� ����
	void CS_Packet_Move_Object(CGameObject* pObject, D3DXVECTOR3& position, bool bRightCliecked);
	void CS_Packet_Create_Building(CGameObject* pSCV, int nodeIndex, OBJECT_TYPE obj_type);
	void CS_Packet_Create_Unit(OBJECT_TYPE obj_type, D3DXVECTOR3 pos);
	void CS_Packet_Dig_Resource(CGameObject* pSCV, CGameObject* pMineral, CGameObject* pCommandcenter);

	bool ProcessInput();
	void UpdateEnemyObject(float fTimeElpased); //
	void UpdateObject(float fTimeElapsed);	//���� �� �����Ӱ� ���� �����ؾ��� �̺�Ʈ�� ����
	void Animate(float fTimeElapsed);
	void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera);	

	//�׸���
	LIGHT*			GetLight(int idx) { return &(m_pLights->m_pLights[idx]); }
	CHeightMap*		GetHeightMap() { return m_pHeightMap; }
	void			CreateShadowShader(ID3D11Device* pd3dDevice);
	void			RenderObjectShadow(ID3D11DeviceContext *pd3dDeviceContext);

	//�Ȱ�ó�� �Լ�
	void	BuildFogBuffer(ID3D11Device *pd3dDevice);
	void	UpdateFogBuffer(ID3D11DeviceContext *pd3dDeviceContext);

	//�þ� �� �� ����Ʈ ����
	void	UpdateEnemyInSight(float fElapsedTime);

	//��ƼŬ �̹��� �ҷ�����
	void	LoadParticleImage(ID3D11Device* pd3dDevice);

	//void    PushCreateUnitList(OBJECT_TYPE obj_type, D3DXVECTOR3 pos, float time);
	//void    UpdateCreateUnitList(float elapsedTime);

	void	CreateNewObject(OBJECT_TYPE obj_type, D3DXVECTOR3 vMadenPosition);

	void	UpdateSelectedObjects();
	void	UpdateCrowdSavedData();

	//1001 ���콺 Ŭ�� �̹���
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
	//����
	LIGHTS*			 m_pLights;
	ID3D11Buffer*	 m_pd3dcbLights;

	float m_fWndClientWidth;
	float m_fWndClientHeight;

	//�׼����̼ǿ��� ���̸� ǥ���ϱ� ���� raw ���̸��̴�.
	CHeightMap*		m_pHeightMap = nullptr;

	//UI�� ���̹��۸� �ٲ��� ������ �������� �ʴ´�. ������� �׸��Ŵϱ�
	ID3D11DepthStencilState*	m_pd3dUIDepthStencilState = nullptr;

	//���콺 �̺�Ʈ ó��, ���� ���°� �ʿ��� �� �˾����� ���� ���� �ʿ� ���� ���� �׷����Ƿ� clip�� �����ϴ� ����ѵ�?
	CTexture*				m_pMouseRectTexture;
	CMouseRectMesh*			m_pMouseRectMesh;
	ID3D11VertexShader*		m_pd3dMouseRectVertexShader;
	ID3D11InputLayout*		m_pd3dMouseRectVertexLayout;
	ID3D11PixelShader*		m_pd3dMouseRectPixelShader;

	bool			m_bLeftClicked{false};
	bool			m_bMouseRectDraw{false};
	D3DXVECTOR3		m_d3dxvOldCursorPosition;
	D3DXVECTOR3		m_d3dxvNextCursorPosition;
	
	//���õ� ��ü�� ���� ó��, ��ü���� ��ġ���� �� ũ������, Ȥ�� �������� ���� �����Ͽ� ������ۿ� ����, �ϰ������� ���ϼ��̴� ������
	CTexture*				m_pSelectedCircleTexture;
	CSelectedCircleMesh*	m_pSelectedCircleMesh;
	ID3D11VertexShader*		m_pd3dSelectedCircleVertexShader;
	ID3D11InputLayout*		m_pd3dSelectedCircleVertexLayout;
	ID3D11GeometryShader*	m_pd3dSelectedCircleGeometryShader;
	ID3D11PixelShader*		m_pd3dSelectedCirclePixelShader;

	//UI ���� ������ â
	CTexture*				m_pMainFrameTexture = nullptr;
	CMainFrameMesh*			m_pMainFrameMesh = nullptr;
	ID3D11VertexShader*		m_pd3dMainFrameVertexShader = nullptr;
	ID3D11InputLayout*		m_pd3dMainFrameVertexLayout = nullptr;
	ID3D11PixelShader*		m_pd3dMainFramePixelShader = nullptr;

	//1003 ���콺 �̹����� ���� �����Ӱ� ���� ���̴� �Լ��� ����ϸ� �ȴ�.
	CTexture*				m_pMouseImageTexture{ nullptr };
	CMainFrameMesh*			m_pMouseImageMesh{ nullptr };

	//���� �������� ��ũ�� ��ġ
	//LB = -1.0f, -1.0f  // RT = 1.0f, -0.4f
	
	//0731 ScreenMesh�� Shader�δ� �̴ϸ� ���� �ٸ� ó���� ���ϹǷ� �ٸ� ���̴��Լ��� ����ϵ��� �� Shader�� ���´�.
	CTexture*				m_pMiniMapTexture = nullptr;
	CScreenMesh*			m_pMiniMapMesh = nullptr;
	ID3D11VertexShader*		m_pd3dMiniMapVertexShader = nullptr;
	ID3D11InputLayout*		m_pd3dMiniMapVertexLayout = nullptr;
	ID3D11PixelShader*		m_pd3dMiniMapPixelShader = nullptr;
	
	//0731 �̴ϸʿ� ǥ���ϴ� ��ü���� ���� ó�� -> �ؽ��� ���ҽ��� ���� �ʿ� ����.
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

	
	//UI ���� ������ ���� �ٴ� ScreenMesh�鿡 ���� Shader
	ID3D11VertexShader*		m_pd3dAttachedMainFrameVertexShader = nullptr;
	ID3D11InputLayout*		m_pd3dAttachedMainFrameVertexLayout = nullptr;
	ID3D11PixelShader*		m_pd3dAttachedMainFramePixelShader = nullptr;

	//UI Ŀ�ǵ�
	map<string, CTexture*>		m_mapCommandTexture;
	map<string, CScreenMesh*>	m_mapCommandMesh;
	
	//UI ����â
	map<int, CTexture*>		m_mapStatusTexture;
	CScreenMesh*			m_pStatusMesh = nullptr;


	//1003 ����â�� �׷����� ���� ���� ǥ��
	CTexture*						m_pCrowdFrameTexture{ nullptr };
	map<OBJECT_TYPE, CTexture*>		m_mapCrowdUnitTexture;
	
	vector<CCrowdMesh*>		m_vecCrowdMeshes;
	ID3D11VertexShader*		m_pd3dCrowdImageVertexShader{ nullptr };
	ID3D11InputLayout*		m_pd3dCrowdImageVertexLayout{ nullptr };
	ID3D11PixelShader*		m_pd3dCrowdImagePixelShader{ nullptr };

	//�δ����� ������ ���� ǥ��
	CTexture*				m_pCrowdAttachedFrameTexture{ nullptr };
	vector<CScreenMesh*>	m_vecAttachedCrowdMeshes;


	//UI �ʻ�ȭ
	map<int, CTexture*>		m_mapPortraitTexture;
	CScreenMesh*			m_pPortraitMesh = nullptr;
	
	//��ã�⸦ ���� Astar ������Ʈ�̴�
	AStarComponent*			m_pAStarComponent;

	//����� �׸��带 �׸��� ���� 
	CDebugPrograms*			m_pDebugPrograms;

	//�Ȱ�ó���� �������
	ID3D11Buffer*		m_pd3dcbFogBuffer{ nullptr };

	ID3D11Buffer*				m_pd3dFogBuffer{ nullptr };
	ID3D11ShaderResourceView*	m_pd3dSRVFog{ nullptr };
	D3DXVECTOR4					m_d3dxvFogData[(int)(NUM_FOG_SLICE * NUM_FOG_SLICE)];

	//���� �� �þ�(�丮��Ʈ)�� ���ԵǴ� ��� �� ��ü���� ����, �� ����Ʈ�� �� ��ü ���� ���� ���ŵǾ�� �Ѵ�.
	list<CGameObject*>		m_listMy;			//�������Ǵ� �� ��ü���� ����Ʈ
	list<CGameObject*>		m_listEnemy;		//���� ���ӿ� �����ϴ� ���� ��ü���� ����Ʈ
	list<CGameObject*>		m_listEnemyInSight;	//�� ��ü�ν� �������Ǿ�� �ϴ�, �� �þ� �ȿ� �����ϴ� �� ����Ʈ
	list<CGameObject*>		m_listNeutral;		//�̳׶��� ������ ������

	//��ƼŬ �̹���
	CSpriteParticle*		m_pParticleBox{ nullptr };

	//0812 sound
	CSoundManager*			m_pSoundManager{ nullptr };

	CNumberFont*			m_pNumberFont{ nullptr };
	CBuildCheck*			m_pBuildCheck{ nullptr };

	//0826 ���α׷��� ��
	CScreenMesh*			m_pProgressIconMesh{ nullptr };
	CProgressImage*			m_pProgressImage{ nullptr };

	//1206 ����ó��
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

	//0825 Ŀ�ǵ� â ���콺 ����, �»�ܺ��� 1~5, �� ĭ �Ʒ� 6~10, �� �Ʒ�ĭ 11~15�� �Ѵ�. pair�� LB�� RT�̴�.
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>		m_mapCommandPosition;

	//1003 ���� ���� status ǥ��, 1~8, 9~16, 17~24�̰� pair�� LB, RT�� �Ѵ�.
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>		m_mapCrowdPosition;

	CROWD_SAVE_DATA			m_CrowdSaveData[8]; //1 ���� 8 ������ ����� �ߴ�.
	float					m_fLastInputCrowdDataButtonTime{ 1000.0f };
	char					m_chLastInputKey{ 0 };

	//�δ����� �� ���� �Ǿ��ִ��� Ȯ���ϱ� ���� crowd_AttachMainFrame
	map<int, pair<D3DXVECTOR2, D3DXVECTOR2>>		m_mapAttachedCrowdPosition;
	
	CCubeShader				m_DebugCubeShader;

	//1206 ��ũƮ�� ���� �� �̹��� ǥ��
	map<OBJECT_TYPE, bool>		m_mapIsBuildingExist; //�ǹ��� �̸����� �˻�, bool�� ���� �������ִ��� ���θ� Ȯ���Ѵ�.

	bool						m_bGameEnd{ false };
	bool						m_bGameWin{ false };
};

