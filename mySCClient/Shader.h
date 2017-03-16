#pragma once

#include "Object.h"
#include "BoxIncluder.h"

class CCamera;

struct VS_CB_WORLD_MATRIX
{
	D3DXMATRIX	m_d3dxmtxWorld;
	D3DXMATRIX	m_d3dxmtxWorldInvTranspose;
};

struct VS_INSTANCE_WITH_SLOT
{
	D3DXMATRIX	m_d3dxmtxWorld;
	UINT		nSlot;
};

//**********************************************************************
class CShader
{
public:
	CShader();
	virtual ~CShader();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetTexture(CTexture* pTexture);
	void SetMaterial(CMATERIAL* pMaterial);
	//void SetMaterial(CMaterial *pMaterial);

	static void CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, LPCSTR pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout);
	static void CreateGeometryShaderFromFile(ID3D11Device *pd3dDevice, LPCSTR pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11GeometryShader **ppd3dGeometryShader);
	static void CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, LPCSTR pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader);

	void CreateHullShaderFromFile(ID3D11Device *pd3dDevice, LPCSTR pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11HullShader **ppd3dHullShader);
	void CreateDomainShaderFromFile(ID3D11Device *pd3dDevice, LPCSTR pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11DomainShader **ppd3dDomainShader);


	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device* pd3dDevice);
	virtual void ReleaseObjects();

	virtual void CreateShaderVariables(ID3D11Device *pd3dDevice);
	virtual void CreateMaterialConstantBuffer(ID3D11Device* pd3dDevice);

	virtual void ReleaseShaderVariables();

	static void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld);
	static void UpdateMaterial(ID3D11DeviceContext* pd3dDeviceContext, MATERIAL* pMaterial);

	CGameObject** GetppObjects() { return m_ppObjects; }
	
	//Picking
	virtual CGameObject* PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo);

	
	virtual void UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera);



public:
	ID3D11VertexShader*		m_pd3dVertexShader;
	ID3D11InputLayout*		m_pd3dVertexLayout;
	ID3D11GeometryShader*	m_pd3dGeometryShader;
	ID3D11PixelShader*		m_pd3dPixelShader;

	ID3D11HullShader*		 m_pd3dHullShader;
	ID3D11DomainShader*		 m_pd3dDomainShader;

	static ID3D11Buffer*			m_pd3dcbWorldMatrix;
	static ID3D11Buffer*			m_pd3dcbMaterial;

	CTexture*	 m_pTexture;
	CMATERIAL*	 m_pMaterial;
	//CMaterial*   m_pMaterial;

	CGameObject** m_ppObjects;
	int m_nObjects;

protected:
	int m_nReferences;
};


//**********************************************************************
//스카이 박스 셰이더
class CSkyBoxShader : public CShader
{
public:
	CSkyBoxShader();
	virtual ~CSkyBoxShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

private:
	CSkyBoxMesh*	m_pSkyBoxMesh;
	D3DXMATRIX		m_d3dxmtxWorld;
};


//**********************************************************************
//지형 셰이더
class CTerrainShader : public CShader
{
public:
	CTerrainShader();
	virtual ~CTerrainShader();

	CHeightMapTerrain *GetTerrain();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};


//**********************************************************************
//테셀레이션 지형 셰이더
class CTSTerrainShader : public CTerrainShader
{
private:
	ID3D11ShaderResourceView *m_pd3dHeightResource;
	ID3D11SamplerState *m_pd3dHeightSampler;

public:
	CTSTerrainShader();
	virtual ~CTSTerrainShader();

	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

//**********************************************************************
//테스트용 큐브 셰이더
class CCubeShader : public CShader
{
	struct CB_DEBUGCUBE_OPTION
	{
		D3DXMATRIX	mtxWorld;
		D3DXVECTOR4 fColor;
	};

public:
	CCubeShader();
	virtual ~CCubeShader();

	list<D3DXVECTOR4>		m_listPositions;
	CCubeMesh*				m_pCubeMesh;

	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void CreateShader(ID3D11Device *pd3dDevice); //-> CShader의 셰이더 사용
	
	void	PushPosition(D3DXVECTOR3 pPos);
	void	RemovePosition(D3DXVECTOR3 pPos);

	ID3D11Buffer*		m_pd3dcbDebugCube{ nullptr };
	
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
};

//**********************************************************************
//인스턴싱 셰이더

//셰이더로 객체를 그려내기 위해 필요한 정보들을 담는 구조체
struct RenderObjectData
{
	int					m_ObjectType{ 0 };
	CMesh*				m_pMesh{ nullptr };
	int					m_nValidObjects{0};
	ID3D11Buffer*		m_pd3dInstanceBuffer{ nullptr };
	CGameObject**		m_ppObjects{ nullptr };
	CTexture*			m_pTexture{ nullptr };
	CMATERIAL*			m_pMaterial{ nullptr };

	RenderObjectData(int type, CMesh* pMesh, int nValidObjects, ID3D11Buffer* pd3dInstanceBuffer, CGameObject** ppObjects, CTexture* pTexTure, CMATERIAL* pMaterial)
	{
		m_ObjectType = type;
		m_pMesh = pMesh;
		m_nValidObjects = nValidObjects;
		m_pd3dInstanceBuffer = pd3dInstanceBuffer;
		m_ppObjects = ppObjects;
		m_pTexture = pTexTure;
		m_pMaterial = pMaterial;
	}
};

class CInstancingShader : public CShader
{
public:
	CInstancingShader();
	~CInstancingShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShadowShader(ID3D11Device *pd3dDevice) {}

	ID3D11Buffer *CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData);

	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	virtual void RenderObjectShadow(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

	virtual void UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList);
	virtual void Animate(float fTimeElapsed);

	virtual void ReleaseAllObjects(CGameObject** ppObjects);

protected:
	UINT m_nInstanceBufferStride;
	UINT m_nInstanceBufferOffset;

	//ID3D11Buffer *m_pd3dInstanceBuffer;


public:
	//객체를 구분짓는 타입, ppObjects
	vector<RenderObjectData*>	m_vectorObjects;

	//Static 및 Animate 객체들의 그림자를 그리기 위한 Shader
	ID3D11VertexShader*		m_pd3dShadowVertexShader = nullptr;
	ID3D11InputLayout*		m_pd3dShadowVertexLayout = nullptr;
	ID3D11PixelShader*		m_pd3dShadowPixelShader = nullptr;

};

//**********************************************************************
//제작할 셰이더를 나열

//Pos, Tex, Nor의 정보만 있는 정적인 객체를 관리하는 StaticShader
class CStaticShader : public CInstancingShader
{
public:
	CStaticShader();
	virtual ~CStaticShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShadowShader(ID3D11Device* pd3dDevice);
	virtual void BuildObjects(ID3D11Device *pd3dDevice);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera);
	virtual void RenderObjectShadow(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

public:
	CStaticMeshBox*				m_pModelBox;
};



//**********************************************************************
//Bone 정보가 담겨있는 AnimationShader

class CAnimationShader : public CInstancingShader
{
public:
	CAnimationShader();
	virtual ~CAnimationShader();

	virtual void CreateShader(ID3D11Device *pd3dDevice);
	virtual void CreateShadowShader(ID3D11Device* pd3dDevice);
	void CreateAnimationShaderResourceBuffer(ID3D11Device* pd3dDevice);
	
	virtual void BuildObjects(ID3D11Device *pd3dDevice);

	void UpdateAnimationBuffer(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxAnimation, D3DXMATRIX *pd3dxmtxLocal, int nBone);
	
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera);
	virtual void RenderObjectShadow(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
		
public :
	CAnimationMeshBox*			m_pModelBox;

private:
	ID3D11Buffer					*m_pd3dSRVAnimationBuffer{ nullptr };
	ID3D11ShaderResourceView		*m_pd3dSRVAnimation{ nullptr };
};

