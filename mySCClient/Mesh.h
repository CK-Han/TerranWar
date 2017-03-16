#pragma once

#include "AnimationComponent.h"
#include "HeightMap.h"


using namespace std;

struct UINT4
{
	UINT x, y, z, w;
};


//**********************************************************************
//재질 구조체 정의
struct MATERIAL
{
	D3DXCOLOR m_d3dxcAmbient;
	D3DXCOLOR m_d3dxcDiffuse;
	D3DXCOLOR m_d3dxcEmissive;
	D3DXCOLOR m_d3dxcSpecular; //(r,g,b,a=power)
};


class CMATERIAL
{
public:
	CMATERIAL() {}
	~CMATERIAL() {}
	CMATERIAL(D3DXCOLOR& a, D3DXCOLOR& d, D3DXCOLOR& e, D3DXCOLOR& s)
	{
		m_material.m_d3dxcAmbient = a; 
		m_material.m_d3dxcDiffuse = d; 
		m_material.m_d3dxcEmissive = e; 
		m_material.m_d3dxcSpecular = s;
	}

	void AddRef() { ++m_nReference; }
	void Release() { if (--m_nReference <= 0) delete this; }

	MATERIAL m_material;

private:
	int m_nReference = 0;

};


class CMaterial
{
public:
	CMaterial(MATERIAL& mat) { m_nReferences = 0; m_pd3dcbMaterial = nullptr; m_Material = mat; }
	virtual ~CMaterial() { if (m_pd3dcbMaterial) m_pd3dcbMaterial->Release(); }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void ReleaseShaderVariables() {}
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);

private:
	int				m_nReferences;
	MATERIAL		m_Material;
	ID3D11Buffer*	m_pd3dcbMaterial;
};


//**********************************************************************
//텍스처를 관리하기 위한 텍스처 클래스
//게임 객체는 하나 이상의 텍스쳐를 가질 수 있다. CTexture는 텍스쳐를 관리하기 위한 클래스이다.
class CTexture
{
public:
	CTexture(int nTextures = 1, int nSamplers = 1, int nTextureStartSlot = 0, int nSamplerStartSlot = 0);
	virtual ~CTexture();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture);
	void SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState);

	//텍스쳐 리소스와 샘플러 상태 객체에 대한 쉐이더 변수를 변경한다.
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	//텍스쳐 리소스에 대한 쉐이더 변수를 변경한다.
	void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);
	//샘플러 상태 객체에 대한 쉐이더 변수를 변경한다.
	void UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);


private:
	int m_nTextures;
	int m_nTextureStartSlot;
	int m_nSamplers;
	int m_nSamplerStartSlot;

	ID3D11ShaderResourceView **m_ppd3dsrvTextures;
	ID3D11SamplerState **m_ppd3dSamplerStates;

	int m_nReferences;
};

//**********************************************************************
//AABB 클래스
class AABB
{
public:
	AABB() { m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX); m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX); }
	AABB(D3DXVECTOR3 d3dxvMinimum, D3DXVECTOR3 d3dxvMaximum) { m_d3dxvMinimum = d3dxvMinimum; m_d3dxvMaximum = d3dxvMaximum; }

	//두 개의 바운딩 박스를 합한다.
	void Union(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	void Union(AABB *pAABB);
	//바운딩 박스의 8개의 꼭지점을 행렬로 변환하고 최소점과 최대점을 다시 계산한다.
	void Update(D3DXMATRIX *pd3dxmtxTransform);

public:
	D3DXVECTOR3 m_d3dxvMinimum;
	D3DXVECTOR3 m_d3dxvMaximum;
};

//**********************************************************************
//피킹 클래스의 정의
struct MESHINTERSECTINFO {
	DWORD m_dwFaceIndex;
	float m_fU;
	float m_fV;
	float m_fDistance;
};

//**********************************************************************
//Mesh 클래스들의 나열
class CMesh
{
public:
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

	void AddRef() { m_nReferences++; }			//메시의 참조카운팅 처리
	void Release() { if (--m_nReferences <= 0) delete this; }

	ID3D11Buffer* CreateBuffer(ID3D11Device *pd3dDevice, UINT nStride, int nElements, void *pBufferData, UINT nBindFlags, D3D11_USAGE d3dUsage, UINT nCPUAccessFlags);
	void AssembleToVertexBuffer(int nBuffers = 0, ID3D11Buffer **m_pd3dBuffers = NULL, UINT *pnBufferStrides = NULL, UINT *pnBufferOffsets = NULL);

	AABB GetBoundingCube() { return(m_bcBoundingCube); }
	MATERIAL&	GetMaterial() { return m_meshMaterial; }
	void		SetMaterial(MATERIAL& mat) { m_meshMaterial = mat; }
	D3DXMATRIX	GetMeshLocalMatrix() { return m_d3dxmtxLocal; }

	//Picking
	int CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo);
	D3DXVECTOR3 GetPositionRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo);

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice) {}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances = 0, int nStartInstance = 0);
	virtual void RenderInstancedShadow(ID3D11DeviceContext *pd3dDeviceContext, ID3D11Buffer* pd3dInstanceBuffer, int nInstances = 0, int nStartInstance = 0) {}

protected:
	ID3D11Buffer**					m_ppd3dVertexBuffers;
	ID3D11Buffer*					m_pd3dPositionBuffer;
	ID3D11Buffer*					m_pd3dIndexBuffer;

	ID3D11RasterizerState *			m_pd3dRasterizerState;

	UINT						    m_nVertices;
	UINT							m_nBuffers;
	UINT							m_nSlot;				//버퍼를 IA에 연결할 때 시작 슬롯 번호
	UINT							m_nStartVertex;

	UINT							m_nIndices;
	UINT							m_nStartIndex;
	int								m_nBaseVertex;
	DXGI_FORMAT						m_dxgiIndexFormat;
	UINT							m_nIndexOffset;

	UINT*							m_pnVertexStrides;
	UINT*							m_pnVertexOffsets;
	
	UINT*							m_pnIndices;
	
	D3DXVECTOR3*					m_pd3dxvPositions;		//피킹을 위해 저장됨
	D3DXMATRIX						m_d3dxmtxLocal;
	
	D3D11_PRIMITIVE_TOPOLOGY		 m_d3dPrimitiveTopology;

	AABB							 m_bcBoundingCube;
	MATERIAL						 m_meshMaterial;


private:
	int m_nReferences;			//메시의 참조카운팅

};

//**********************************************************************
//테스트용 큐브 메시
class CCubeMesh : public CMesh
{
public:
	CCubeMesh(ID3D11Device* pd3dDevice);
	virtual ~CCubeMesh();
};



//**********************************************************************
//MouseRect를 위한 사각형 메시
class CMouseRectMesh : public CMesh
{
	struct MouseRectVertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 tex;
	};

public:
	CMouseRectMesh(ID3D11Device* pd3dDevice);
	virtual ~CMouseRectMesh();

	//원래 스크린 좌표로만 했던 코드인데, 그러다보니 마우스로 이동할 경우에 대한 처리를 하지 못했다.
	//void UpdateMouseRect(D3DXVECTOR2&, D3DXVECTOR2&, float, float, ID3D11DeviceContext*);

	//return값은 nextPos의 위치 -> LT가 1번, RT가 0번, LB가 3번, RB가 2번이다.
	void UpdateMouseRect(D3DXVECTOR3&, D3DXVECTOR3&, ID3D11DeviceContext*);
};

//**********************************************************************
//선택된 객체를 표시하는 UI를 위한 메시
class CSelectedCircleMesh : public CMesh
{
public:
	CSelectedCircleMesh(ID3D11Device* pd3dDevice);
	virtual ~CSelectedCircleMesh();

	void UpdateSelectedCircle(std::vector<D3DXVECTOR4>& vPosScale, ID3D11DeviceContext* pd3dDeviceContext);
};

//**********************************************************************
//미니맵 위에 표시되는 객체들을 위한 메시
class CMiniMapObjectMesh : public CMesh
{
public:
	CMiniMapObjectMesh(ID3D11Device* pd3dDevice);
	virtual ~CMiniMapObjectMesh();

	void UpdateMiniMapObject(std::vector<D3DXVECTOR4>& vPosType, ID3D11DeviceContext* pd3dDeviceContext);
};

//**********************************************************************
//미니맵에 현재 보고있는 위치를 표현하는 미니맵 사각형
class CMiniMapRectMesh : public CMesh
{
	struct VERTEX_MINIMAP_RECT
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texCoord;
	};

public:
	CMiniMapRectMesh(ID3D11Device* pd3dDevice);
	virtual ~CMiniMapRectMesh();

	void UpdateMiniMapRect(D3DXVECTOR3 cameraLookPos, ID3D11DeviceContext* pd3dDeviceContext);
};

//**********************************************************************
//Scv들이 자원을 들고있는 경우 그려내는 것
class CNeutralResourceMesh : public CMesh
{
public:
	CNeutralResourceMesh(ID3D11Device* pd3dDevice);
	virtual ~CNeutralResourceMesh();

	void UpdateNeutralResource(std::vector<D3DXVECTOR4>& vPosType, ID3D11DeviceContext* pd3dDeviceContext);
};


//**********************************************************************
//메인 프레임 UI 메시
struct MainFrameVertex
{
	D3DXVECTOR2 pos;
	D3DXVECTOR2 tex;
};

class CMainFrameMesh : public CMesh
{
private:
	ID3D11BlendState* m_pd3dBlendState = nullptr;

public:
	CMainFrameMesh(ID3D11Device* pd3dDevice, D3DXVECTOR2 vScreenLB, D3DXVECTOR2 vScreenRT);
	virtual ~CMainFrameMesh();

	void UpdateScreenPosition(ID3D11DeviceContext* pd3dDeviceContext, float xPosition, float yPosition);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

//**********************************************************************
//메인 프레임 위에 그려질 UI 이미지에 대한 Mesh
struct ScreenVertex
{
	D3DXVECTOR2 pos;
	D3DXVECTOR2 tex;
	float fRendered; //0.0f -> 회색, 1.0f 정상적으로
};

class CScreenMesh : public CMesh
{
public:
	CScreenMesh(ID3D11Device* pd3dDevice, D3DXVECTOR2& leftBottom, D3DXVECTOR2& rightTop, bool beRendered = true);
	virtual ~CScreenMesh();

	//테크트리 표현
	ScreenVertex	m_VertexData[4];
	void UpdateScreenMeshRendered(ID3D11DeviceContext* pd3dDeviceContext, bool beRendered);
};

//**********************************************************************
//1003 군중 유닛 표시를 위한 Mesh
struct CrowdVertex
{
	D3DXVECTOR2 pos;
	D3DXVECTOR2 tex;
	float		color; // 0 green, 1 yellow, 2 red
};

class CCrowdMesh : public CMesh
{
public:
	CCrowdMesh(ID3D11Device* pd3dDevice, D3DXVECTOR2& leftBottom, D3DXVECTOR2& rightTop);
	virtual ~CCrowdMesh();

	void UpdateCrowdColor(ID3D11DeviceContext* pd3dDeviceContext, float fColor);

private:
	D3DXVECTOR2 vLB, vRT;
};

//**********************************************************************
//normal, texture
class CDefaultMesh : public CMesh
{
public:
	CDefaultMesh(ID3D11Device* pd3dDevice);
	virtual ~CDefaultMesh();

protected:
	ID3D11Buffer*					m_pd3dNormalBuffer;
	ID3D11Buffer*					m_pd3dTexCoordBuffer;
};

//**********************************************************************
//SkyBoxMesh 클래스의 정의
class CSkyBoxMesh : public CMesh
{
public:
	CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f);
	virtual ~CSkyBoxMesh();

	void OnChangeSkyBoxTextures(ID3D11Device *pd3dDevice, int nIndex = 0);

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);

protected:
	ID3D11Buffer*			 m_pd3dTexCoordBuffer;
	ID3D11DepthStencilState* m_pd3dDepthStencilState;

	CTexture *m_pSkyboxTexture;
};


//**********************************************************************
//높이맵 표현을 위한 CHeightMapGridMesh 클래스의 정의
class CHeightMapGridMesh : public CDefaultMesh
{
protected:
	//격자의 크기(가로: x-방향, 세로: z-방향)이다.
	int m_nWidth;
	int m_nLength;
	/*격자의 스케일(가로: x-방향, 세로: z-방향, 높이: y-방향) 벡터이다. 실제 격자 메쉬의 각 정점의 x-좌표, y-좌표, z-좌표는 스케일 벡터의 x-좌표, y-좌표, z-좌표로 곱한 값을 갖는다. 즉, 실제 격자의 x-축 방향의 간격은 1이 아니라 스케일 벡터의 x-좌표가 된다. 이렇게 하면 작은 격자를 사용하더라도 큰 격자를 생성할 수 있다.*/
	D3DXVECTOR3 m_d3dxvScale;

	//ID3D11Buffer *m_pd3dFogBuffer;

public:
	CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale = D3DXVECTOR3(1.0f, 1.0f, 1.0f), void *pContext = NULL);
	virtual ~CHeightMapGridMesh();

	bool IsSearched;

	D3DXVECTOR3 GetScale() { return(m_d3dxvScale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	//void BuildFogBuffer(ID3D11Device *pd3dDevice);
	//void SetVisited(ID3D11DeviceContext *pd3dDeviceContext);

	//격자의 교점(정점)의 높이를 설정한다.
	virtual float OnGetHeight(int x, int z, void *pContext);
};

//**********************************************************************
//TerrainMesh 클래스의 정의
class CTerrainMesh : public CDefaultMesh
{
public:
	CTerrainMesh(ID3D11Device *pd3dDevice, float fWidth = 100.0f, float fHeight = 100.0f);
	virtual ~CTerrainMesh();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

//**********************************************************************
//테셀레이션 지형... 클래스의 정의
class CTSTerrainMesh : public CMesh
{
private:
	//normal은 테셀레이션을 통해 얻을 수 있다.
	ID3D11Buffer* m_pd3dTexCoordBuffer;

public:
	//0 왼쪽위 1 오른쪽 위 2왼쪽아래 3오른쪽아래
	CTSTerrainMesh(ID3D11Device* pd3dDevice);
	virtual ~CTSTerrainMesh();
};

//**********************************************************************
//건물 및 자원과 같은 정적 오브젝트들에 대한 Mesh
class CStaticMesh : public CDefaultMesh
{
public:
	CStaticMesh(ID3D11Device* pd3dDevice);
	virtual ~CStaticMesh();

	//Static object... pos, tex, nor
	void LoadStaticObjectFromFBX(const string& fileName, D3DXVECTOR3 vScale, ID3D11Device* pd3dDevice, D3DXVECTOR3 offset, float aabbSize);

	//다른 텍스쳐를 입혀 렌더링하기 위해 기존 메시정보들(버퍼)을 새 메시에 복사하는 함수
	CStaticMesh* CreateAndCopyMeshData();
	
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext);
	virtual void RenderInstancedShadow(ID3D11DeviceContext *pd3dDeviceContext, ID3D11Buffer* pd3dInstanceBuffer, int nInstances = 0, int nStartInstance = 0);

protected:
	//1214 노말맵
	ID3D11Buffer*				m_pd3dTangentBuffer{ nullptr };

};

//**********************************************************************
//BoneID, BoneWeight가 포함된 애니메이션 메시 클래스 정의
class CAnimationMesh : public CDefaultMesh
{
public:
	CAnimationMesh(ID3D11Device* pd3dDevice);
	virtual ~CAnimationMesh();

	//Only Animative
	void LoadAnimateObjectFromFBX(const string& fileName, D3DXVECTOR3 vScale, ID3D11Device* pd3dDevice, D3DXVECTOR3 offset, float aabbSize);
	CAnimationMesh* CreateAndCopyMeshData();

	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext);
	virtual void RenderInstancedShadow(ID3D11DeviceContext *pd3dDeviceContext, ID3D11Buffer* pd3dInstanceBuffer, int nInstances = 0, int nStartInstance = 0);

protected:
	ID3D11Buffer*				m_pd3dBoneIDBuffer;
	ID3D11Buffer*				m_pd3dWeightBuffer;
	//1214 노말맵
	ID3D11Buffer*				m_pd3dTangentBuffer{ nullptr };

public:
	SkinnedData*				m_pSkinnedData{ nullptr };
};
