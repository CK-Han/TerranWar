#pragma once

#include "AnimationComponent.h"
#include "HeightMap.h"


using namespace std;

struct UINT4
{
	UINT x, y, z, w;
};


//**********************************************************************
//���� ����ü ����
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
//�ؽ�ó�� �����ϱ� ���� �ؽ�ó Ŭ����
//���� ��ü�� �ϳ� �̻��� �ؽ��ĸ� ���� �� �ִ�. CTexture�� �ؽ��ĸ� �����ϱ� ���� Ŭ�����̴�.
class CTexture
{
public:
	CTexture(int nTextures = 1, int nSamplers = 1, int nTextureStartSlot = 0, int nSamplerStartSlot = 0);
	virtual ~CTexture();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture);
	void SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState);

	//�ؽ��� ���ҽ��� ���÷� ���� ��ü�� ���� ���̴� ������ �����Ѵ�.
	void UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext);
	//�ؽ��� ���ҽ��� ���� ���̴� ������ �����Ѵ�.
	void UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex = 0, int nSlot = 0);
	//���÷� ���� ��ü�� ���� ���̴� ������ �����Ѵ�.
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
//AABB Ŭ����
class AABB
{
public:
	AABB() { m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX); m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX); }
	AABB(D3DXVECTOR3 d3dxvMinimum, D3DXVECTOR3 d3dxvMaximum) { m_d3dxvMinimum = d3dxvMinimum; m_d3dxvMaximum = d3dxvMaximum; }

	//�� ���� �ٿ�� �ڽ��� ���Ѵ�.
	void Union(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	void Union(AABB *pAABB);
	//�ٿ�� �ڽ��� 8���� �������� ��ķ� ��ȯ�ϰ� �ּ����� �ִ����� �ٽ� ����Ѵ�.
	void Update(D3DXMATRIX *pd3dxmtxTransform);

public:
	D3DXVECTOR3 m_d3dxvMinimum;
	D3DXVECTOR3 m_d3dxvMaximum;
};

//**********************************************************************
//��ŷ Ŭ������ ����
struct MESHINTERSECTINFO {
	DWORD m_dwFaceIndex;
	float m_fU;
	float m_fV;
	float m_fDistance;
};

//**********************************************************************
//Mesh Ŭ�������� ����
class CMesh
{
public:
	CMesh(ID3D11Device *pd3dDevice);
	virtual ~CMesh();

	void AddRef() { m_nReferences++; }			//�޽��� ����ī���� ó��
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
	UINT							m_nSlot;				//���۸� IA�� ������ �� ���� ���� ��ȣ
	UINT							m_nStartVertex;

	UINT							m_nIndices;
	UINT							m_nStartIndex;
	int								m_nBaseVertex;
	DXGI_FORMAT						m_dxgiIndexFormat;
	UINT							m_nIndexOffset;

	UINT*							m_pnVertexStrides;
	UINT*							m_pnVertexOffsets;
	
	UINT*							m_pnIndices;
	
	D3DXVECTOR3*					m_pd3dxvPositions;		//��ŷ�� ���� �����
	D3DXMATRIX						m_d3dxmtxLocal;
	
	D3D11_PRIMITIVE_TOPOLOGY		 m_d3dPrimitiveTopology;

	AABB							 m_bcBoundingCube;
	MATERIAL						 m_meshMaterial;


private:
	int m_nReferences;			//�޽��� ����ī����

};

//**********************************************************************
//�׽�Ʈ�� ť�� �޽�
class CCubeMesh : public CMesh
{
public:
	CCubeMesh(ID3D11Device* pd3dDevice);
	virtual ~CCubeMesh();
};



//**********************************************************************
//MouseRect�� ���� �簢�� �޽�
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

	//���� ��ũ�� ��ǥ�θ� �ߴ� �ڵ��ε�, �׷��ٺ��� ���콺�� �̵��� ��쿡 ���� ó���� ���� ���ߴ�.
	//void UpdateMouseRect(D3DXVECTOR2&, D3DXVECTOR2&, float, float, ID3D11DeviceContext*);

	//return���� nextPos�� ��ġ -> LT�� 1��, RT�� 0��, LB�� 3��, RB�� 2���̴�.
	void UpdateMouseRect(D3DXVECTOR3&, D3DXVECTOR3&, ID3D11DeviceContext*);
};

//**********************************************************************
//���õ� ��ü�� ǥ���ϴ� UI�� ���� �޽�
class CSelectedCircleMesh : public CMesh
{
public:
	CSelectedCircleMesh(ID3D11Device* pd3dDevice);
	virtual ~CSelectedCircleMesh();

	void UpdateSelectedCircle(std::vector<D3DXVECTOR4>& vPosScale, ID3D11DeviceContext* pd3dDeviceContext);
};

//**********************************************************************
//�̴ϸ� ���� ǥ�õǴ� ��ü���� ���� �޽�
class CMiniMapObjectMesh : public CMesh
{
public:
	CMiniMapObjectMesh(ID3D11Device* pd3dDevice);
	virtual ~CMiniMapObjectMesh();

	void UpdateMiniMapObject(std::vector<D3DXVECTOR4>& vPosType, ID3D11DeviceContext* pd3dDeviceContext);
};

//**********************************************************************
//�̴ϸʿ� ���� �����ִ� ��ġ�� ǥ���ϴ� �̴ϸ� �簢��
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
//Scv���� �ڿ��� ����ִ� ��� �׷����� ��
class CNeutralResourceMesh : public CMesh
{
public:
	CNeutralResourceMesh(ID3D11Device* pd3dDevice);
	virtual ~CNeutralResourceMesh();

	void UpdateNeutralResource(std::vector<D3DXVECTOR4>& vPosType, ID3D11DeviceContext* pd3dDeviceContext);
};


//**********************************************************************
//���� ������ UI �޽�
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
//���� ������ ���� �׷��� UI �̹����� ���� Mesh
struct ScreenVertex
{
	D3DXVECTOR2 pos;
	D3DXVECTOR2 tex;
	float fRendered; //0.0f -> ȸ��, 1.0f ����������
};

class CScreenMesh : public CMesh
{
public:
	CScreenMesh(ID3D11Device* pd3dDevice, D3DXVECTOR2& leftBottom, D3DXVECTOR2& rightTop, bool beRendered = true);
	virtual ~CScreenMesh();

	//��ũƮ�� ǥ��
	ScreenVertex	m_VertexData[4];
	void UpdateScreenMeshRendered(ID3D11DeviceContext* pd3dDeviceContext, bool beRendered);
};

//**********************************************************************
//1003 ���� ���� ǥ�ø� ���� Mesh
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
//SkyBoxMesh Ŭ������ ����
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
//���̸� ǥ���� ���� CHeightMapGridMesh Ŭ������ ����
class CHeightMapGridMesh : public CDefaultMesh
{
protected:
	//������ ũ��(����: x-����, ����: z-����)�̴�.
	int m_nWidth;
	int m_nLength;
	/*������ ������(����: x-����, ����: z-����, ����: y-����) �����̴�. ���� ���� �޽��� �� ������ x-��ǥ, y-��ǥ, z-��ǥ�� ������ ������ x-��ǥ, y-��ǥ, z-��ǥ�� ���� ���� ���´�. ��, ���� ������ x-�� ������ ������ 1�� �ƴ϶� ������ ������ x-��ǥ�� �ȴ�. �̷��� �ϸ� ���� ���ڸ� ����ϴ��� ū ���ڸ� ������ �� �ִ�.*/
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

	//������ ����(����)�� ���̸� �����Ѵ�.
	virtual float OnGetHeight(int x, int z, void *pContext);
};

//**********************************************************************
//TerrainMesh Ŭ������ ����
class CTerrainMesh : public CDefaultMesh
{
public:
	CTerrainMesh(ID3D11Device *pd3dDevice, float fWidth = 100.0f, float fHeight = 100.0f);
	virtual ~CTerrainMesh();

	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};

//**********************************************************************
//�׼����̼� ����... Ŭ������ ����
class CTSTerrainMesh : public CMesh
{
private:
	//normal�� �׼����̼��� ���� ���� �� �ִ�.
	ID3D11Buffer* m_pd3dTexCoordBuffer;

public:
	//0 ������ 1 ������ �� 2���ʾƷ� 3�����ʾƷ�
	CTSTerrainMesh(ID3D11Device* pd3dDevice);
	virtual ~CTSTerrainMesh();
};

//**********************************************************************
//�ǹ� �� �ڿ��� ���� ���� ������Ʈ�鿡 ���� Mesh
class CStaticMesh : public CDefaultMesh
{
public:
	CStaticMesh(ID3D11Device* pd3dDevice);
	virtual ~CStaticMesh();

	//Static object... pos, tex, nor
	void LoadStaticObjectFromFBX(const string& fileName, D3DXVECTOR3 vScale, ID3D11Device* pd3dDevice, D3DXVECTOR3 offset, float aabbSize);

	//�ٸ� �ؽ��ĸ� ���� �������ϱ� ���� ���� �޽�������(����)�� �� �޽ÿ� �����ϴ� �Լ�
	CStaticMesh* CreateAndCopyMeshData();
	
	virtual void CreateRasterizerState(ID3D11Device *pd3dDevice);
	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext);
	virtual void RenderInstancedShadow(ID3D11DeviceContext *pd3dDeviceContext, ID3D11Buffer* pd3dInstanceBuffer, int nInstances = 0, int nStartInstance = 0);

protected:
	//1214 �븻��
	ID3D11Buffer*				m_pd3dTangentBuffer{ nullptr };

};

//**********************************************************************
//BoneID, BoneWeight�� ���Ե� �ִϸ��̼� �޽� Ŭ���� ����
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
	//1214 �븻��
	ID3D11Buffer*				m_pd3dTangentBuffer{ nullptr };

public:
	SkinnedData*				m_pSkinnedData{ nullptr };
};
