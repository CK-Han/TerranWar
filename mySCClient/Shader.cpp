#include "stdafx.h"
#include "Shader.h"
#include "AnimationComponent.h"

//**********************************************************************
ID3D11Buffer *CShader::m_pd3dcbWorldMatrix = nullptr;
ID3D11Buffer *CShader::m_pd3dcbMaterial = nullptr;

CShader::CShader()
{
	m_pd3dVertexLayout = nullptr;
	m_pd3dVertexShader = nullptr;
	m_pd3dPixelShader = nullptr;
	m_pd3dGeometryShader = nullptr;

	m_pd3dHullShader = nullptr;
	m_pd3dDomainShader = nullptr;

	m_nReferences = 0;

	m_ppObjects = nullptr;
	m_nObjects = 0;

	m_pTexture = nullptr;
	m_pMaterial = nullptr;
}

CShader::~CShader()
{
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
	if (m_pd3dGeometryShader) m_pd3dGeometryShader->Release();
	if (m_pd3dHullShader) m_pd3dHullShader->Release();
	if (m_pd3dDomainShader) m_pd3dDomainShader->Release();

	if (m_pTexture) m_pTexture->Release();
	if (m_pMaterial) m_pMaterial->Release();
	ReleaseShaderVariables();
	ReleaseObjects();
}


//**********************************************************************
//세이더 파일 생성 함수
void CShader::CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, LPCSTR pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 정점-쉐이더를 생성한다. 
		pd3dDevice->CreateVertexShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader);
		//컴파일된 쉐이더 코드의 메모리 주소와 입력 레이아웃에서 정점 레이아웃을 생성한다. 
		pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements, pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), ppd3dVertexLayout);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, LPCSTR pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 픽셀-쉐이더를 생성한다. 
		pd3dDevice->CreatePixelShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dPixelShader);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreateGeometryShaderFromFile(ID3D11Device *pd3dDevice, LPCSTR pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11GeometryShader **ppd3dGeometryShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		pd3dDevice->CreateGeometryShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dGeometryShader);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreateHullShaderFromFile(ID3D11Device *pd3dDevice, LPCSTR pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11HullShader **ppd3dHullShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 헐-쉐이더를 생성한다. 
		pd3dDevice->CreateHullShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dHullShader);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreateDomainShaderFromFile(ID3D11Device *pd3dDevice, LPCSTR pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11DomainShader **ppd3dDomainShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 픽셀-쉐이더를 생성한다. 
		pd3dDevice->CreateDomainShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dDomainShader);
		pd3dShaderBlob->Release();
	}
}

//**********************************************************************
//세이더 생성 호출
void CShader::CreateShader(ID3D11Device *pd3dDevice)
{
	/*IA 단계에 설정할 입력-레이아웃을 정의한다. 정점 버퍼의 한 원소가 CVertex 클래스의 멤버 변수(D3DXVECTOR3 
	즉, 실수 세 개)이므로 입력-레이아웃은 실수(32-비트) 3개로 구성되며 시멘틱이 “POSITION”이고 정점 데이터임을 표현한다.*/
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VS", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PS", "ps_5_0", &m_pd3dPixelShader);
	
	CreateShaderVariables(pd3dDevice);
}

void CShader::ReleaseShaderVariables()
{
	if (m_pd3dcbWorldMatrix)
	{
		m_pd3dcbWorldMatrix->Release();
		m_pd3dcbWorldMatrix = nullptr;
	}
	if (m_pd3dcbMaterial)
	{
		m_pd3dcbMaterial->Release();
		m_pd3dcbMaterial = nullptr;
	}
}

//**********************************************************************
//월드 변환 상수버퍼 설정
void CShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	if (m_pd3dcbWorldMatrix)
		return;	//이미 생성되어 있다면 진행하지 않는다.

	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);

	CreateMaterialConstantBuffer(pd3dDevice);
}


void CShader::CreateMaterialConstantBuffer(ID3D11Device* pd3dDevice)
{
	if (m_pd3dcbMaterial)
		return;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(MATERIAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT hr = pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbMaterial);
}


void CShader::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	//월드 변환 행렬을 상수 버퍼에 복사한다.
	D3DXMATRIX worldInvTranspose;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	D3DXMatrixInverse(&worldInvTranspose, nullptr, pd3dxmtxWorld);
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorldInvTranspose, &worldInvTranspose);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	//상수 버퍼를 디바이스의 슬롯(CB_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(CB_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}

void CShader::UpdateMaterial(ID3D11DeviceContext* pd3dDeviceContext, MATERIAL* pMaterial)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy(pcbMaterial, pMaterial, sizeof(MATERIAL));
	pd3dDeviceContext->Unmap(m_pd3dcbMaterial, 0);

	pd3dDeviceContext->PSSetConstantBuffers(CB_SLOT_MATERIAL, 1, &m_pd3dcbMaterial);
}


//**********************************************************************
//세이더에서 오브젝트들의 피킹 여부를 확인합니다.
CGameObject* CShader::PickObjectByRayIntersection(D3DXVECTOR3 *pd3dxvPickPosition, D3DXMATRIX *pd3dxmtxView, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	int nIntersected = 0;
	float fNearHitDistance = FLT_MAX;
	CGameObject *pSelectedObject = nullptr;
	MESHINTERSECTINFO d3dxIntersectInfo;
	/*쉐이더 객체에 포함되는 모든 객체들에 대하여 픽킹 광선을 생성하고 객체의 바운딩 박스와의 교차를 검사한다. 교차하는 객체들 중에 카메라와 가장 가까운 객체의 정보와 객체를 반환한다.*/
	for (int i = 0; i < m_nObjects; i++)
	{
		nIntersected = m_ppObjects[i]->PickObjectByRayIntersection(pd3dxvPickPosition, pd3dxmtxView, &d3dxIntersectInfo);
		if ((nIntersected > 0) && (d3dxIntersectInfo.m_fDistance < fNearHitDistance))
		{
			fNearHitDistance = d3dxIntersectInfo.m_fDistance;
			pSelectedObject = m_ppObjects[i];
			if (pd3dxIntersectInfo) *pd3dxIntersectInfo = d3dxIntersectInfo;
		}
	}
	return(pSelectedObject);
}

//**********************************************************************
//세이더가 관리하는 오브젝트
void CShader::SetTexture(CTexture *pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CShader::SetMaterial(CMATERIAL* pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	m_pMaterial->AddRef();
}

/*
void CShader::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}
*/



void CShader::BuildObjects(ID3D11Device* pd3dDevice)
{
	//test
	CCubeMesh* pMesh = new CCubeMesh(pd3dDevice);
	m_nObjects = 5;
	m_ppObjects = new CGameObject*[m_nObjects];

	for (auto i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i] = new CGameObject(1);
		m_ppObjects[i]->SetMesh(pMesh, 0);
		m_ppObjects[i]->SetPosition(-20.0f + i * 10.0f, 0.5f, 0.0f);
		//m_ppObjects[i]->SetPosition(10.0f, 20.0f, 0.0f);
	}
}

void CShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (auto i = 0; i < m_nObjects; ++i)
			m_ppObjects[i]->Release();
		delete[] m_ppObjects;
	}
}
//**********************************************************************
//프레임 갱신
void CShader::OnPrepareRender(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	pd3dDeviceContext->GSSetShader(m_pd3dGeometryShader, NULL, 0);
	pd3dDeviceContext->HSSetShader(m_pd3dHullShader, NULL, 0);
	pd3dDeviceContext->DSSetShader(m_pd3dDomainShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
}


void CShader::UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList)
{
	if (m_ppObjects)
	{
		for (auto i = 0; i < m_nObjects; ++i)
			m_ppObjects[i]->UpdateObject(fTimeElapsed, pHeightMap, allyList, enemyList);
	}
}

void CShader::Animate(float fTimeElapsed)
{
	if (m_ppObjects)
	{
		for (auto i = 0; i < m_nObjects; ++i)
			m_ppObjects[i]->Animate(fTimeElapsed);
	}
}

void CShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera)
{
	OnPrepareRender(pd3dDeviceContext);

	if (m_pTexture) m_pTexture->UpdateShaderVariable(pd3dDeviceContext);
	if (m_pMaterial) UpdateMaterial(pd3dDeviceContext, &(m_pMaterial->m_material));

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			//카메라의 절두체에 포함되는 객체들만을 렌더링한다. 
			if (m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMATRIX d3dmtxObject  = m_ppObjects[j]->m_d3dxmtxLocal * m_ppObjects[j]->m_d3dxmtxWorld;
				UpdateShaderVariable(pd3dDeviceContext, &d3dmtxObject);
				m_ppObjects[j]->Render(pd3dDeviceContext, pCamera);
			}
		}
	}
}

//**********************************************************************
//스카이박스 렌더링을 위한 셰이더입니다.

CSkyBoxShader::CSkyBoxShader() : CShader()
{
	m_pSkyBoxMesh = nullptr;
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
}
CSkyBoxShader::~CSkyBoxShader()
{
	if (m_pSkyBoxMesh) m_pSkyBoxMesh->Release();
}

void CSkyBoxShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSSkyBox", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSSkyBox", "ps_5_0", &m_pd3dPixelShader);
}

void CSkyBoxShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice);
	m_pSkyBoxMesh->AddRef();
	CShader::CreateShaderVariables(pd3dDevice);
}

void CSkyBoxShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::OnPrepareRender(pd3dDeviceContext);
	//스카이 박스 객체의 위치를 카메라의 위치로 변경한다.
	
	D3DXVECTOR3 d3dxvCameraPos = pCamera->GetPosition();
	m_d3dxmtxWorld._41 = d3dxvCameraPos.x; m_d3dxmtxWorld._42 = d3dxvCameraPos.y; m_d3dxmtxWorld._43 = d3dxvCameraPos.z;
	CShader::UpdateShaderVariable(pd3dDeviceContext, &m_d3dxmtxWorld);

	//스카이 박스 메쉬(6개의 사각형)를 렌더링한다.
	m_pSkyBoxMesh->Render(pd3dDeviceContext);
}


//**********************************************************************
//지형 셰이더
CTerrainShader::CTerrainShader() : CShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

void CTerrainShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSTerrain", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSTerrain", "ps_5_0", &m_pd3dPixelShader);

	CreateShaderVariables(pd3dDevice);
}

void CTerrainShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	/* 원본
	CGameObject* pTerrain = new CGameObject(1);
	CTerrainMesh* pTerrainMesh = new CTerrainMesh(pd3dDevice, MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f);
	pTerrain->SetMesh(pTerrainMesh, 0);
	
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];
	m_ppObjects[0] = pTerrain;
	*/

	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];

	D3DXVECTOR3 d3dxvScale(1.0f, 1.0f, 1.0f);
	m_ppObjects[0] = new CHeightMapTerrain(pd3dDevice, _T("Assets/Image/Terrain/mapama_160160_height_raw_205205.raw"), 205, 205, 6, 6, d3dxvScale);
}

void CTerrainShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::Render(pd3dDeviceContext, pCamera);
}

CHeightMapTerrain *CTerrainShader::GetTerrain()
{
	return((CHeightMapTerrain *)m_ppObjects[0]);
}

//**********************************************************************
//테셀레이션 지형  셰이더
CTSTerrainShader::CTSTerrainShader()
{
	m_pd3dHeightResource = nullptr;
	m_pd3dHeightSampler = nullptr;
}

CTSTerrainShader::~CTSTerrainShader()
{
	if (m_pd3dHeightResource) m_pd3dHeightResource->Release();
	if (m_pd3dHeightSampler) m_pd3dHeightSampler->Release();
}


void CTSTerrainShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nObjects = 1;
	m_ppObjects = new CGameObject*[m_nObjects];
	
	//m_ppObjects[0] = new CGameObject(2); //0번은 피킹용 메시, 1번은 실제 테셀레이션 렌더링 메시
	m_ppObjects[0] = new CGameObject(1);
	//CTerrainMesh* pTerrainMesh = new CTerrainMesh(pd3dDevice, MAP_WIDTH / 2.0f, MAP_HEIGHT / 2.0f);
	CTSTerrainMesh *tsMesh = new CTSTerrainMesh(pd3dDevice);

	//m_ppObjects[0]->SetMesh(pTerrainMesh, 0);
	m_ppObjects[0]->SetMesh(tsMesh, 0);
	m_ppObjects[0]->SetPosition(D3DXVECTOR3(-0.0f, 0.0f, -0.0f));

	//높이 텍스쳐 리소스 생성
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;

	HRESULT h = D3DX11CreateShaderResourceViewFromFile(pd3dDevice, _T("Assets/Image/Terrain/mapama_160160_height.png"), NULL, NULL, &m_pd3dHeightResource, NULL);
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &m_pd3dHeightSampler);

}

void CTSTerrainShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);
	CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSTessellation", "vs_5_0", &m_pd3dVertexShader, d3dInputElements, nElements, &m_pd3dVertexLayout);
	CreateHullShaderFromFile(pd3dDevice, "Effect.fx", "HSTessellation", "hs_5_0", &m_pd3dHullShader);
	CreateDomainShaderFromFile(pd3dDevice, "Effect.fx", "DSTessellation", "ds_5_0", &m_pd3dDomainShader);
	CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSTessellation", "ps_5_0", &m_pd3dPixelShader);
}

void CTSTerrainShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	
	if (m_pd3dHeightResource)
		pd3dDeviceContext->DSSetShaderResources(DS_SLOT_TEXTURE, 1, &m_pd3dHeightResource);
	if (m_pd3dHeightSampler)
		pd3dDeviceContext->DSSetSamplers(DS_SLOT_SAMPLER, 1, &m_pd3dHeightSampler);
	

	pd3dDeviceContext->HSSetShader(m_pd3dHullShader, nullptr, 0);
	pd3dDeviceContext->DSSetShader(m_pd3dDomainShader, nullptr, 0);
	CShader::Render(pd3dDeviceContext, pCamera);
	pd3dDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pd3dDeviceContext->DSSetShader(nullptr, nullptr, 0);
}


//**********************************************************************
//테스트 큐브 셰이더
CCubeShader::CCubeShader() : CShader()
{
	m_pd3dcbDebugCube = nullptr;
}
CCubeShader::~CCubeShader()
{
	if (m_pCubeMesh) m_pCubeMesh->Release();

	m_listPositions.clear();

	if (m_pd3dcbDebugCube) m_pd3dcbDebugCube->Release();
}

void CCubeShader::CreateShader(ID3D11Device *pd3dDevice)
{
	//CShader::CreateShader(pd3dDevice);

	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSDebugCube", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSDebugCube", "ps_5_0", &m_pd3dPixelShader);

}

void CCubeShader::BuildObjects(ID3D11Device* pd3dDevice)
{
	m_pCubeMesh = new CCubeMesh(pd3dDevice);
	m_pCubeMesh->AddRef();

	//디버그큐브용 상수버퍼 생성
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(CB_DEBUGCUBE_OPTION);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbDebugCube);
}

void CCubeShader::PushPosition(D3DXVECTOR3 vPos)
{
	bool isFind = false;
	auto iter = m_listPositions.begin();
	for (iter; iter != m_listPositions.end(); ++iter)
	{
		if (iter->x == vPos.x && iter->z == vPos.z) //이미존재한다면
		{
			iter->w += 1.0f;
			isFind = true;
			break;
		}
	}
	
	if(false == isFind) //찾지 못했다 -> 새로운 위치다.
		m_listPositions.push_back(D3DXVECTOR4(vPos, 1.0f));
}

void CCubeShader::RemovePosition(D3DXVECTOR3 vPos)
{
	bool isFind = false;
	auto iter = m_listPositions.begin();
	for (iter; iter != m_listPositions.end(); ++iter)
	{
		if (iter->x == vPos.x && iter->z == vPos.z) //이미존재한다면
		{
			iter->w -= 1.0f;
			isFind = true;
			break;
		}
	}

	if (false == isFind)
		printf("잘못된 노드를 제거하려 했습니다!\n");
	else if (iter->w <= 0.0f)
		m_listPositions.erase(iter);

}

void CCubeShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::OnPrepareRender(pd3dDeviceContext);

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;

	for (auto d : m_listPositions)
	{
		D3DXMATRIX d3dmtxObject = D3DXMATRIX{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, d.x, d.y, d.z, 1.0f};
		D3DXVECTOR4 fColor;
		switch ((int)d.w)
		{ //무지개색 순서대로
		case 1:
			fColor = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case 2:
			fColor = D3DXVECTOR4(1.0f, 0.4f, 0.0f, 1.0f);
			break;
		case 3:
			fColor = D3DXVECTOR4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		case 4:
			fColor = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		case 5:
			fColor = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
			break;
		case 6:
			fColor = D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f);
			break;
		case 7:
			fColor = D3DXVECTOR4(1.0f, 0.0f, 1.0f, 1.0f);
			break;
		default:
			fColor = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		pd3dDeviceContext->Map(m_pd3dcbDebugCube, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		CB_DEBUGCUBE_OPTION *cubeData = (CB_DEBUGCUBE_OPTION *)d3dMappedResource.pData;
		cubeData->mtxWorld = d3dmtxObject;
		cubeData->fColor = fColor;
		pd3dDeviceContext->Unmap(m_pd3dcbDebugCube, 0);

		//상수 버퍼를 디바이스의 슬롯(CB_SLOT_WORLD_MATRIX)에 연결한다.
		pd3dDeviceContext->VSSetConstantBuffers(CB_SLOT_DEBUGCUBE, 1, &m_pd3dcbDebugCube);

		m_pCubeMesh->Render(pd3dDeviceContext);
	}
}


//**********************************************************************
//인스턴싱 셰이더
CInstancingShader::CInstancingShader()
{
	//m_pd3dInstanceBuffer = NULL;

	m_vectorObjects.reserve(MAX_KINDS_OBJECTS);
}

CInstancingShader::~CInstancingShader()
{
	//if (m_pd3dInstanceBuffer) m_pd3dInstanceBuffer->Release();


	for (int i = m_vectorObjects.size() - 1; i >= 0; --i)
	{
		if (m_vectorObjects[i]->m_pMesh) m_vectorObjects[i]->m_pMesh->Release();
		if (m_vectorObjects[i]->m_pd3dInstanceBuffer) m_vectorObjects[i]->m_pd3dInstanceBuffer->Release();
		if (m_vectorObjects[i]->m_pTexture) m_vectorObjects[i]->m_pTexture->Release();
		if (m_vectorObjects[i]->m_pMaterial) m_vectorObjects[i]->m_pMaterial->Release();
		if (m_vectorObjects[i]->m_ppObjects) ReleaseAllObjects(m_vectorObjects[i]->m_ppObjects);
		delete m_vectorObjects[i];
	}

	if (m_pd3dShadowVertexShader) m_pd3dShadowVertexShader->Release();
	if (m_pd3dShadowVertexLayout) m_pd3dShadowVertexLayout->Release();
	if (m_pd3dShadowPixelShader) m_pd3dShadowPixelShader->Release();
}

void CInstancingShader::ReleaseAllObjects(CGameObject** ppObjects)
{
	if (ppObjects)
	{
		for (int i = 0; i < MAX_INSTANCE_OBJECTS; ++i)
			ppObjects[i]->Release();
		delete[] ppObjects;
	}
}

void CInstancingShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEID", 0, DXGI_FORMAT_R32G32B32A32_UINT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 5, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSAnimationInstanced", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSAnimationInstanced", "ps_5_0", &m_pd3dPixelShader);

	CreateShaderVariables(pd3dDevice);
}

void CInstancingShader::BuildObjects(ID3D11Device *pd3dDevice)
{

}

void CInstancingShader::UpdateObject(float fTimeElapsed, CHeightMap* pHeightMap, list<CGameObject*>* allyList, list<CGameObject*>* enemyList)
{
	for (int i = m_vectorObjects.size() - 1; i >= 0; --i)
	{
		for (int j = 0; j < m_vectorObjects[i]->m_nValidObjects; ++j)
			if (m_vectorObjects[i]->m_ppObjects[j]->GetActive()) m_vectorObjects[i]->m_ppObjects[j]->UpdateObject(fTimeElapsed, pHeightMap, allyList, enemyList);
	}
}

void CInstancingShader::Animate(float fTimeElapsed)
{
	for (int i = m_vectorObjects.size() - 1; i >= 0; --i)
	{
		for (int j = 0; j < m_vectorObjects[i]->m_nValidObjects; ++j)
			if (m_vectorObjects[i]->m_ppObjects[j]->GetActive()) m_vectorObjects[i]->m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CInstancingShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	int nValidObjects;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	for (int i = m_vectorObjects.size() - 1; i >= 0; --i)
	{
		nValidObjects = 0;
		if (m_vectorObjects[i]->m_pTexture) m_vectorObjects[i]->m_pTexture->UpdateShaderVariable(pd3dDeviceContext);
		if (m_vectorObjects[i]->m_pMaterial) UpdateMaterial(pd3dDeviceContext, &(m_vectorObjects[i]->m_pMaterial->m_material));

		pd3dDeviceContext->Map(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		D3DXMATRIX *pd3dxmtx = (D3DXMATRIX *)d3dMappedResource.pData;
		for (int j = 0; j < m_vectorObjects[i]->m_nValidObjects; ++j)
		{
			if (m_vectorObjects[i]->m_ppObjects[j]->GetActive() && m_vectorObjects[i]->m_ppObjects[j]->IsInSight() && m_vectorObjects[i]->m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pd3dxmtx[nValidObjects++], &(m_vectorObjects[i]->m_ppObjects[j]->m_d3dxmtxLocal * m_vectorObjects[i]->m_ppObjects[j]->m_d3dxmtxWorld));
			}
		}
		pd3dDeviceContext->Unmap(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0);
		m_vectorObjects[i]->m_pMesh->RenderInstanced(pd3dDeviceContext, nValidObjects, 0);
	}
}

//그림자
void CInstancingShader::RenderObjectShadow(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dShadowVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dShadowVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dShadowPixelShader, NULL, 0);

	pd3dDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pd3dDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pd3dDeviceContext->DSSetShader(nullptr, nullptr, 0);

	int nValidObjects;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	for (int i = m_vectorObjects.size() - 1; i >= 0; --i)
	{
		nValidObjects = 0;

		pd3dDeviceContext->Map(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		D3DXMATRIX *pd3dxmtx = (D3DXMATRIX *)d3dMappedResource.pData;
		for (int j = 0; j < m_vectorObjects[i]->m_nValidObjects; ++j)
		{
			if (m_vectorObjects[i]->m_ppObjects[j]->GetActive() && m_vectorObjects[i]->m_ppObjects[j]->IsInSight() &&m_vectorObjects[i]->m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pd3dxmtx[nValidObjects++], &(m_vectorObjects[i]->m_ppObjects[j]->m_d3dxmtxLocal * m_vectorObjects[i]->m_ppObjects[j]->m_d3dxmtxWorld));
			}
		}
		pd3dDeviceContext->Unmap(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0);
		m_vectorObjects[i]->m_pMesh->RenderInstancedShadow(pd3dDeviceContext, m_vectorObjects[i]->m_pd3dInstanceBuffer, nValidObjects, 0);
	}
}


ID3D11Buffer *CInstancingShader::CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData)
{
	ID3D11Buffer *pd3dInstanceBuffer = NULL;
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	//Dynamic, Write 사용해야지 당연히
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = nBufferStride * nObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pBufferData;
	d3dBufferData.SysMemPitch = nBufferStride * nObjects;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, (pBufferData) ? &d3dBufferData : NULL, &pd3dInstanceBuffer);
	return(pd3dInstanceBuffer);
}


//**********************************************************************
//정적인 객체들의 셰이더
CStaticShader::CStaticShader() : CInstancingShader()
{
	m_pModelBox = nullptr;
}

CStaticShader::~CStaticShader()
{
	if (m_pModelBox) delete m_pModelBox;
}

void CStaticShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSStatic", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSStatic", "ps_5_0", &m_pd3dPixelShader);

	CreateShaderVariables(pd3dDevice);
}

void CStaticShader::CreateShadowShader(ID3D11Device* pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCEPOS", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSStaticShadow", "vs_5_0", &m_pd3dShadowVertexShader, d3dInputLayout, nElements, &m_pd3dShadowVertexLayout);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSStaticShadow", "ps_5_0", &m_pd3dShadowPixelShader);
}

void CStaticShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	//모든 애니메이션 메시 로드
	m_pModelBox = new CStaticMeshBox(pd3dDevice);
	m_pModelBox->LoadModelMesh(pd3dDevice);

	//기본 값 지정
	m_nInstanceBufferStride = sizeof(D3DXMATRIX);
	m_nInstanceBufferOffset = 0;
	m_nObjects = 0;

	ID3D11Buffer* pd3dInstanceBuffer = nullptr;
	CGameObject** ppObjects = nullptr;
	CGameObject* pObjects = nullptr;
	RenderObjectData* pRenderObjectData = nullptr;
	for (auto d : m_pModelBox->m_mapMeshBox)
	{
		//인스턴스 버퍼 생성
		pd3dInstanceBuffer = CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE_OBJECTS, m_nInstanceBufferStride, nullptr);
		d.second->AssembleToVertexBuffer(1, &pd3dInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

		//오브젝트 미리 생성, 활성화 여부는 false이다.
		ppObjects = new CGameObject*[MAX_INSTANCE_OBJECTS];
		for (int i = 0; i < MAX_INSTANCE_OBJECTS; ++i)
		{
			//ppObjects[i] = new CGameObject(1);
			ppObjects[i] = new CStaticObject();
			ppObjects[i]->SetMesh(d.second, 0);
			ppObjects[i]->SetActive(false);

			D3DXMATRIX mtxRotate;
			D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(0.0f), (float)D3DXToRadian(0.0f), (float)D3DXToRadian(-0.0f));
			ppObjects[i]->m_d3dxmtxLocal = d.second->GetMeshLocalMatrix() * mtxRotate;

			//객체별로 클래스를 만들지 않았으니 여기서 switch문을 진행한다.
			//0904 max HP
			switch ((OBJECT_TYPE)d.first)
			{
			case OBJECT_TYPE::Armory: case OBJECT_TYPE::Armory_Enemy:
				ppObjects[i]->m_listUI.push_back(string("addmecashield"));
				ppObjects[i]->m_listUI.push_back(string("addmecaattack"));

				ppObjects[i]->m_nMaxHP = 750;
				break;
			case OBJECT_TYPE::Barrack: case OBJECT_TYPE::Barrack_Enemy:
				ppObjects[i]->m_listUI.push_back(string("marine"));
				ppObjects[i]->m_listUI.push_back(string("reaper"));
				ppObjects[i]->m_listUI.push_back(string("marauder"));
				//ppObjects[i]->m_listUI.push_back(string("ghost"));

				ppObjects[i]->m_nMaxHP = 1000;
				break;
			case OBJECT_TYPE::Bunker: case OBJECT_TYPE::Bunker_Enemy:
				ppObjects[i]->m_nMaxHP = 400;
				break;
			case OBJECT_TYPE::Commandcenter: case OBJECT_TYPE::Commandcenter_Enemy:
				ppObjects[i]->m_listUI.push_back(string("scv"));

				ppObjects[i]->m_nMaxHP = 1500;
				break;
			case OBJECT_TYPE::Engineeringbay: case OBJECT_TYPE::Engineeringbay_Enemy:
				ppObjects[i]->m_listUI.push_back(string("addattack"));
				ppObjects[i]->m_listUI.push_back(string("addshield"));

				ppObjects[i]->m_nMaxHP = 850;
				break;
			case OBJECT_TYPE::Factory: case OBJECT_TYPE::Factory_Enemy:
				ppObjects[i]->m_listUI.push_back(string("thor"));

				ppObjects[i]->m_nMaxHP = 1250;
				break;
			case OBJECT_TYPE::Fusioncore: case OBJECT_TYPE::Fusioncore_Enemy:
				ppObjects[i]->m_listUI.push_back(string("yamatoenergy"));
				ppObjects[i]->m_listUI.push_back(string("yamatore"));

				ppObjects[i]->m_nMaxHP = 750;
				break;
			case OBJECT_TYPE::Ghostacademy: case OBJECT_TYPE::Ghostacademy_Enemy:
				ppObjects[i]->m_listUI.push_back(string("cloak"));

				ppObjects[i]->m_nMaxHP = 1250;
				break;
			case OBJECT_TYPE::Missileturret: case OBJECT_TYPE::Missileturret_Enemy:
				ppObjects[i]->m_listUI.push_back(string("sensor"));

				ppObjects[i]->m_nMaxHP = 250;
				break;
			case OBJECT_TYPE::Reactor: case OBJECT_TYPE::Reactor_Enemy:

				ppObjects[i]->m_nMaxHP = 400;
				break;
			case OBJECT_TYPE::Refinery: case OBJECT_TYPE::Refinery_Enemy:
				ppObjects[i]->m_nMaxHP = 500;
				break;
			case OBJECT_TYPE::Sensortower: case OBJECT_TYPE::Sensortower_Enemy:
				ppObjects[i]->m_listUI.push_back(string("sensor"));

				ppObjects[i]->m_nMaxHP = 200;
				break;
			case OBJECT_TYPE::Starport: case OBJECT_TYPE::Starport_Enemy:
				ppObjects[i]->m_listUI.push_back(string("banshee"));
				ppObjects[i]->m_listUI.push_back(string("medivac"));
				ppObjects[i]->m_listUI.push_back(string("raven"));
				ppObjects[i]->m_listUI.push_back(string("battlecruiser"));
				ppObjects[i]->m_listUI.push_back(string("viking"));

				ppObjects[i]->m_nMaxHP = 1300;
				break;
			case OBJECT_TYPE::Supplydepot: case OBJECT_TYPE::Supplydepot_Enemy:
				ppObjects[i]->m_nMaxHP = 400;
				break;
			case OBJECT_TYPE::Techlab: case OBJECT_TYPE::Techlab_Enemy:
				ppObjects[i]->m_listUI.push_back(string("bunkerup"));
				ppObjects[i]->m_listUI.push_back(string("addbuildingshield"));

				ppObjects[i]->m_nMaxHP = 400;
				break;

			}
		}

		int idx = d.first;
		auto pTexture = m_pModelBox->m_mapTextureBox.find(idx)->second;
		auto pMaterial = m_pModelBox->m_mapMaterialBox.find(idx)->second;
		pTexture->AddRef(); pMaterial->AddRef(); d.second->AddRef();

		pRenderObjectData = new RenderObjectData(d.first, d.second, 0, pd3dInstanceBuffer, ppObjects,
			pTexture, pMaterial);
		m_vectorObjects.push_back(pRenderObjectData);
	}

	
	/*
	//중립자원 그냥 여기서 부른다.
	ifstream in{ "Assets/ModelFile/neutral_position.txt" };
	string name;
	int xpos, zpos, numMineral, numGas;
	if (in.is_open())
	{
		in >> name; // "mineral";
		in >> numMineral; // 59
		auto p = std::find_if(m_vectorObjects.begin(), m_vectorObjects.end(),
			[](RenderObjectData* rod) -> bool { return rod->m_ObjectType == (int)OBJECT_TYPE::Mineral; });
		
		for (int i = 0; i < numMineral; ++i)
		{
			in >> xpos >> zpos;
			CStaticObject* pObject = (CStaticObject*)((*p)->m_ppObjects[(*p)->m_nValidObjects]);
			pObject->m_fCompleteBuildTime = pObject->m_fBuildTime = 1.0f;

			(*p)->m_ppObjects[(*p)->m_nValidObjects]->SetActive(true);
			(*p)->m_ppObjects[(*p)->m_nValidObjects]->SetPosition(xpos, 0.0f, zpos);
			(*p)->m_ppObjects[(*p)->m_nValidObjects]->m_nObjectID;
			(*p)->m_ppObjects[(*p)->m_nValidObjects]->m_nObjectType = (int)OBJECT_TYPE::Mineral;
			(*p)->m_ppObjects[(*p)->m_nValidObjects]->SetInSight(true);
			(*p)->m_nValidObjects++;
		}

		in >> name; // "gas";
		in >> numGas; // 16
		p = std::find_if(m_vectorObjects.begin(), m_vectorObjects.end(),
			[](RenderObjectData* rod) -> bool { return rod->m_ObjectType == (int)OBJECT_TYPE::Gas; });

		for (int i = 0; i < numGas; ++i)
		{
			in >> xpos >> zpos;
			CStaticObject* pObject = (CStaticObject*)((*p)->m_ppObjects[(*p)->m_nValidObjects]);
			pObject->m_fCompleteBuildTime = pObject->m_fBuildTime = 1.0f;

			(*p)->m_ppObjects[(*p)->m_nValidObjects]->SetActive(true);
			(*p)->m_ppObjects[(*p)->m_nValidObjects]->SetPosition(xpos, 0.0f, zpos);
			(*p)->m_ppObjects[(*p)->m_nValidObjects]->m_nObjectID;
			(*p)->m_ppObjects[(*p)->m_nValidObjects]->m_nObjectType = (int)OBJECT_TYPE::Gas;
			(*p)->m_ppObjects[(*p)->m_nValidObjects]->SetInSight(true);
			(*p)->m_nValidObjects++;
		}

		in.close();
	}
	*/
}

void CStaticShader::Animate(float fTimeElapsed)
{
	CInstancingShader::Animate(fTimeElapsed);
}

void CStaticShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera)
{
	//CInstancingShader::Render(pd3dDeviceContext, pCamera);

	OnPrepareRender(pd3dDeviceContext);
	int nValidObjects;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	for (int i = m_vectorObjects.size() - 1; i >= 0; --i)
	{
		nValidObjects = 0;
		if (m_vectorObjects[i]->m_pTexture) m_vectorObjects[i]->m_pTexture->UpdateShaderVariable(pd3dDeviceContext);
		if (m_vectorObjects[i]->m_pMaterial) UpdateMaterial(pd3dDeviceContext, &(m_vectorObjects[i]->m_pMaterial->m_material));
		
		//1213 스펙큘러 리소스 설정
		auto iterSpecular = m_pModelBox->m_mapSpecularMapBox.find(m_vectorObjects[i]->m_ObjectType);
		if (iterSpecular != m_pModelBox->m_mapSpecularMapBox.end()) iterSpecular->second->UpdateShaderVariable(pd3dDeviceContext);

		//1214 노말 리소스 설정
		auto iterNormal = m_pModelBox->m_mapNormalMapBox.find(m_vectorObjects[i]->m_ObjectType);
		if (iterNormal != m_pModelBox->m_mapNormalMapBox.end()) iterNormal->second->UpdateShaderVariable(pd3dDeviceContext);

		pd3dDeviceContext->Map(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		D3DXMATRIX *pd3dxmtx = (D3DXMATRIX *)d3dMappedResource.pData;
		for (int j = 0; j < m_vectorObjects[i]->m_nValidObjects; ++j)
		{
			if (m_vectorObjects[i]->m_ppObjects[j]->GetActive() && m_vectorObjects[i]->m_ppObjects[j]->IsInSight() && m_vectorObjects[i]->m_ppObjects[j]->IsVisible(pCamera))
			{
				D3DXMatrixTranspose(&pd3dxmtx[nValidObjects++], &(m_vectorObjects[i]->m_ppObjects[j]->m_d3dxmtxLocal * m_vectorObjects[i]->m_ppObjects[j]->m_d3dxmtxWorld));
			}
		}
		pd3dDeviceContext->Unmap(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0);
		m_vectorObjects[i]->m_pMesh->RenderInstanced(pd3dDeviceContext, nValidObjects, 0);
	}
}

void CStaticShader::RenderObjectShadow(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	//CInstancingShader::RenderObjectShadow(pd3dDeviceContext, pCamera);

	pd3dDeviceContext->IASetInputLayout(m_pd3dShadowVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dShadowVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dShadowPixelShader, NULL, 0);

	pd3dDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pd3dDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pd3dDeviceContext->DSSetShader(nullptr, nullptr, 0);

	int nValidObjects;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	for (int i = m_vectorObjects.size() - 1; i >= 0; --i)
	{
		nValidObjects = 0;

		pd3dDeviceContext->Map(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
		D3DXMATRIX *pd3dxmtx = (D3DXMATRIX *)d3dMappedResource.pData;
		for (int j = 0; j < m_vectorObjects[i]->m_nValidObjects; ++j)
		{
			if (m_vectorObjects[i]->m_ppObjects[j]->GetActive() && m_vectorObjects[i]->m_ppObjects[j]->IsInSight() && m_vectorObjects[i]->m_ppObjects[j]->IsVisible(pCamera))
			{
				//1213 추가, 50% 이상  완성된 경우부터 그림자 있게 하겠다.
				CStaticObject* pObject = reinterpret_cast<CStaticObject*>(m_vectorObjects[i]->m_ppObjects[j]);
				if(pObject->m_fBuildTime >= (pObject->m_fCompleteBuildTime / 2.0f))
					D3DXMatrixTranspose(&pd3dxmtx[nValidObjects++], &(m_vectorObjects[i]->m_ppObjects[j]->m_d3dxmtxLocal * m_vectorObjects[i]->m_ppObjects[j]->m_d3dxmtxWorld));
			}
		}
		pd3dDeviceContext->Unmap(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0);
		m_vectorObjects[i]->m_pMesh->RenderInstancedShadow(pd3dDeviceContext, m_vectorObjects[i]->m_pd3dInstanceBuffer, nValidObjects, 0);
	}
}


//**********************************************************************
//애니메이션 셰이더
CAnimationShader::CAnimationShader() : CInstancingShader()
{
	m_pModelBox = nullptr;
}

CAnimationShader::~CAnimationShader()
{
	if (m_pModelBox) delete m_pModelBox;
	if (m_pd3dSRVAnimationBuffer) m_pd3dSRVAnimationBuffer->Release();
	if (m_pd3dSRVAnimation) m_pd3dSRVAnimation->Release();
}

void CAnimationShader::CreateShader(ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEID", 0, DXGI_FORMAT_R32G32B32A32_UINT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCESLOT", 0, DXGI_FORMAT_R32_UINT, 6, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSAnimation", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSAnimation", "ps_5_0", &m_pd3dPixelShader);

	CreateShaderVariables(pd3dDevice);

	CreateAnimationShaderResourceBuffer(pd3dDevice);
}

void CAnimationShader::CreateShadowShader(ID3D11Device* pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEID", 0, DXGI_FORMAT_R32G32B32A32_UINT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCESLOT", 0, DXGI_FORMAT_R32_UINT, 3, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 } 
	};

	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CShader::CreateVertexShaderFromFile(pd3dDevice, "Effect.fx", "VSAnimationShadow", "vs_5_0", &m_pd3dShadowVertexShader, d3dInputLayout, nElements, &m_pd3dShadowVertexLayout);
	CShader::CreatePixelShaderFromFile(pd3dDevice, "Effect.fx", "PSAnimationShadow", "ps_5_0", &m_pd3dShadowPixelShader);
}

void CAnimationShader::CreateAnimationShaderResourceBuffer(ID3D11Device* pd3dDevice)
{
	//******************************************
	//애니메이션을 위한 StructuredBuffer 생성
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_SKINNED_MATRIX) * MAX_INSTANCE_OBJECTS;	// 64 * 32 * 32 = 64KB
	bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.StructureByteStride = sizeof(VS_SKINNED_MATRIX);
	HRESULT h = pd3dDevice->CreateBuffer(&bd, nullptr, &m_pd3dSRVAnimationBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_UNKNOWN;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvd.BufferEx.NumElements = MAX_INSTANCE_OBJECTS;
	pd3dDevice->CreateShaderResourceView(m_pd3dSRVAnimationBuffer, &srvd, &m_pd3dSRVAnimation);
}

void CAnimationShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	//모든 애니메이션 메시 로드
	m_pModelBox = new CAnimationMeshBox(pd3dDevice);
	m_pModelBox->LoadModelMesh(pd3dDevice);
	
	//기본 값 지정
	m_nInstanceBufferStride = sizeof(VS_INSTANCE_WITH_SLOT);
	m_nInstanceBufferOffset = 0;
	m_nObjects = 0;
	
	ID3D11Buffer* pd3dInstanceBuffer = nullptr;
	CGameObject** ppObjects = nullptr;
	CGameObject* pObjects = nullptr;
	RenderObjectData* pRenderObjectData = nullptr;

	for (auto d : m_pModelBox->m_mapMeshBox)
	{
		//인스턴스 버퍼 생성
		pd3dInstanceBuffer = CreateInstanceBuffer(pd3dDevice, MAX_INSTANCE_OBJECTS, m_nInstanceBufferStride, nullptr);
		d.second->AssembleToVertexBuffer(1, &pd3dInstanceBuffer, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

		//오브젝트 미리 생성, 활성화 여부는 false이다.
		ppObjects = new CGameObject*[MAX_INSTANCE_OBJECTS];
		for (int i = 0; i < MAX_INSTANCE_OBJECTS; ++i)
		{
			if(isAirObject(d.first))
				ppObjects[i] = new CAirObject();
			else
				ppObjects[i] = new CGroundObject();

			CAnimationObject* pAnimationObject = (CAnimationObject*)ppObjects[i];

			pAnimationObject->SetMesh(d.second);
			pAnimationObject->SetActive(false);
			
			D3DXMATRIX mtxRotate;
			D3DXMatrixRotationYawPitchRoll(&mtxRotate, (float)D3DXToRadian(-180.0f), (float)D3DXToRadian(-0.0f), (float)D3DXToRadian(-0.0f));
			pAnimationObject->m_d3dxmtxLocal = d.second->GetMeshLocalMatrix() * mtxRotate;
			
			//애니메이션 Set
			pAnimationObject->m_mapAnimationSet.insert(make_pair("Idle", make_pair(0, 50)));
			pAnimationObject->m_mapAnimationSet.insert(make_pair("Attack", make_pair(60, 110)));
			pAnimationObject->m_mapAnimationSet.insert(make_pair("Move", make_pair(120, 170)));
			pAnimationObject->m_mapAnimationSet.insert(make_pair("Die", make_pair(180, 230)));
			
			string defaultAnim = "Idle";
			pAnimationObject->SetAnimation(defaultAnim);

			//공통적인 UI
			pAnimationObject->m_listUI.push_back(string("attack"));
			pAnimationObject->m_listUI.push_back(string("move"));
			pAnimationObject->m_listUI.push_back(string("stop"));
			pAnimationObject->m_listUI.push_back(string("patrol"));
			pAnimationObject->m_listUI.push_back(string("hold"));

			switch ((OBJECT_TYPE)d.first)
			{
			case OBJECT_TYPE::Banshee: case OBJECT_TYPE::Banshee_Enemy:
			{
				pAnimationObject->m_listUI.push_back(string("cloak"));

				pAnimationObject->m_ProjectileParticle.m_pMyObject = pAnimationObject;
				pAnimationObject->m_bProjectileParticle = true;
				pAnimationObject->m_strParticle = "viking_fire";
				
				pAnimationObject->m_bAirUnit = true;
				pAnimationObject->m_bAirAttack = false;
				pAnimationObject->m_fObjectSightRange = 16.0f;
				pAnimationObject->m_nNeedMineral = 200;
				pAnimationObject->m_nNeedGas = 150;
				pAnimationObject->m_fObjectAttackRange = 6.0f + 2.0f;
				

				pAnimationObject->m_nMaxHP = 140;
				pAnimationObject->m_fAttackSpeed = 1.666f / 1.0f;
			}
			break;
			case OBJECT_TYPE::BattleCruiser: case OBJECT_TYPE::BattleCruiser_Enemy:
			{	
				pAnimationObject->m_listUI.push_back(string("yamatore"));
				pAnimationObject->m_listUI.push_back(string("tacticaljump"));

				pAnimationObject->m_ProjectileParticle.m_pMyObject = pAnimationObject;
				pAnimationObject->m_bProjectileParticle = true;
				pAnimationObject->m_strParticle = "battlecruiser_fire";

				pAnimationObject->m_bAirUnit = true;
				pAnimationObject->m_bAirAttack = true;
				pAnimationObject->m_fObjectSightRange = 18.0f;
				pAnimationObject->m_nNeedMineral = 400;
				pAnimationObject->m_nNeedGas = 300;
				pAnimationObject->m_fObjectAttackRange = 7.0f + 2.0f;

				pAnimationObject->m_nMaxHP = 550;
				pAnimationObject->m_fAttackSpeed = 1.666f / 0.16f /  2.0f; //너무 빨라서 2배로 함
			}
			break;
			case OBJECT_TYPE::Marauder: case OBJECT_TYPE::Marauder_Enemy:
			{
				pAnimationObject->m_listUI.push_back(string("steampack"));
				pAnimationObject->m_listUI.push_back(string("grin"));
				pAnimationObject->m_listUI.push_back(string("quake"));

				pAnimationObject->m_ProjectileParticle.m_pMyObject = pAnimationObject;
				pAnimationObject->m_bProjectileParticle = true;
				pAnimationObject->m_strParticle = "marauder_fire";

				pAnimationObject->m_bAirAttack = false;
				pAnimationObject->m_fObjectSightRange = 16.0f;
				pAnimationObject->m_nNeedMineral = 150;
				pAnimationObject->m_nNeedGas = 50;
				pAnimationObject->m_fObjectAttackRange = 5.0f + 2.0f;

				pAnimationObject->m_nMaxHP = 125;
				pAnimationObject->m_fAttackSpeed = 1.666f / 1.07f;
			}
			break;
			case OBJECT_TYPE::Marine: case OBJECT_TYPE::Marine_Enemy:
			{
				pAnimationObject->m_listUI.push_back(string("steampack"));

				pAnimationObject->m_PromptParticle.m_pMyObject = pAnimationObject;
				pAnimationObject->m_bProjectileParticle = false;
				pAnimationObject->m_strParticle = "marine_fire";

				pAnimationObject->m_bAirAttack = true;
				pAnimationObject->m_fObjectSightRange = 15.0f;
				pAnimationObject->m_nNeedMineral = 50;
				pAnimationObject->m_nNeedGas = 0;
				pAnimationObject->m_fObjectAttackRange = 4.0f + 2.0f;

				pAnimationObject->m_nMaxHP = 45;
				pAnimationObject->m_fAttackSpeed = 1.666f / 0.61f;

				//마린의 경우 24번째 본이 오른손이다. 숫자로는 23
				pAnimationObject->m_bAttackOffset = true;
				pAnimationObject->m_nAttackOffsetIndex = 23;
			}
			break;
			case OBJECT_TYPE::Medivac: case OBJECT_TYPE::Medivac_Enemy:
			{
				pAnimationObject->m_listUI.push_back(string("heal"));

				pAnimationObject->m_ProjectileParticle.m_pMyObject = pAnimationObject;
				pAnimationObject->m_bProjectileParticle = true;
				//pAnimationObject->m_strParticle = "missilefire2";

				pAnimationObject->m_bAirUnit = true;
				pAnimationObject->m_bAirAttack = true;
				pAnimationObject->m_fObjectSightRange = 18.0f;
				pAnimationObject->m_nNeedMineral = 200;
				pAnimationObject->m_nNeedGas = 150;
				pAnimationObject->m_fObjectAttackRange = 0.0f;

				pAnimationObject->m_nMaxHP = 150;
				pAnimationObject->m_fAttackSpeed = 1.0f;
			}
			break;
			case OBJECT_TYPE::Mule: case OBJECT_TYPE::Mule_Enemy:
			{
				pAnimationObject->m_bAirAttack = false;
				pAnimationObject->m_fObjectSightRange = 14.0f;
				pAnimationObject->m_nNeedMineral = 100;
				pAnimationObject->m_nNeedGas = 0;
				pAnimationObject->m_fObjectAttackRange = 0.0f;

				pAnimationObject->m_nMaxHP = 60;
			}
			break;
			case OBJECT_TYPE::Raven: case OBJECT_TYPE::Raven_Enemy:
			{
				pAnimationObject->m_listUI.push_back(string("sensor"));

				pAnimationObject->m_bAirUnit = true;
				pAnimationObject->m_bAirAttack = true;
				pAnimationObject->m_fObjectSightRange = 20.0f;
				pAnimationObject->m_nNeedMineral = 150;
				pAnimationObject->m_nNeedGas = 200;
				pAnimationObject->m_fObjectAttackRange = 0.0f;

				pAnimationObject->m_nMaxHP = 140;
				pAnimationObject->m_fAttackSpeed = 1.0f;
			}
			break;
			case OBJECT_TYPE::Reaper: case OBJECT_TYPE::Reaper_Enemy:
			{
				pAnimationObject->m_listUI.push_back(string("jump"));

				pAnimationObject->m_ProjectileParticle.m_pMyObject = pAnimationObject;
				pAnimationObject->m_bProjectileParticle = false;
				pAnimationObject->m_strParticle = "reaper_fire";

				pAnimationObject->m_bAirAttack = false;
				pAnimationObject->m_fObjectSightRange = 14.0f;
				pAnimationObject->m_nNeedMineral = 100;
				pAnimationObject->m_nNeedGas = 50;
				pAnimationObject->m_fObjectAttackRange = 4.0f + 2.0f;

				pAnimationObject->m_nMaxHP = 60;
				pAnimationObject->m_fAttackSpeed = 1.666f / 0.79f;

				//사신의 경우 10 14번째 본이 왼손 오른손이다. 인덱스는 9, 13
				pAnimationObject->m_bAttackOffset = true;
				pAnimationObject->m_nAttackOffsetIndex = 9;
			}
			break;
			case OBJECT_TYPE::Scv: case OBJECT_TYPE::Scv_Enemy:
			{
				pAnimationObject->m_listUI.push_back(string("repair"));
				pAnimationObject->m_listUI.push_back(string("build"));
				pAnimationObject->m_listUI.push_back(string("buildadv"));

				pAnimationObject->m_bAirAttack = false;
				pAnimationObject->m_fObjectSightRange = 12.0f;
				pAnimationObject->m_nNeedMineral = 50;
				pAnimationObject->m_nNeedGas = 0;
				pAnimationObject->m_fObjectAttackRange = 0.0f;

				pAnimationObject->m_nMaxHP = 45;
				pAnimationObject->m_fAttackSpeed = 1.0f;
			}
			break;
			case OBJECT_TYPE::Thor: case OBJECT_TYPE::Thor_Enemy:
			{
				pAnimationObject->m_listUI.push_back(string("burnner"));
				pAnimationObject->m_listUI.push_back(string("emp"));

				pAnimationObject->m_ProjectileParticle.m_pMyObject = pAnimationObject;
				pAnimationObject->m_bProjectileParticle = false;
				pAnimationObject->m_strParticle = "thor_fire";

				pAnimationObject->m_bAirAttack = true;
				pAnimationObject->m_fObjectSightRange = 20.0f;
				pAnimationObject->m_nNeedMineral = 400;
				pAnimationObject->m_nNeedGas = 300;
				pAnimationObject->m_fObjectAttackRange = 8.0f + 2.0f;

				pAnimationObject->m_nMaxHP = 400;
				pAnimationObject->m_fAttackSpeed = 1.666f / 0.91f / 1.5f;

				//토르의 경우 4 6번째 본이 왼손 오른손이다. 인덱스는 3, 5
				pAnimationObject->m_bAttackOffset = true;
				pAnimationObject->m_nAttackOffsetIndex = 3;
			}
			break;
			case OBJECT_TYPE::Viking: case OBJECT_TYPE::Viking_Enemy:
			{
				pAnimationObject->m_ProjectileParticle.m_pMyObject = pAnimationObject;
				pAnimationObject->m_bProjectileParticle = true;
				pAnimationObject->m_strParticle = "viking_fire";

				pAnimationObject->m_bAirUnit = true;
				pAnimationObject->m_bAirAttack = true;
				pAnimationObject->m_fObjectSightRange = 18.0f;
				pAnimationObject->m_nNeedMineral = 300;
				pAnimationObject->m_nNeedGas = 200;
				pAnimationObject->m_fObjectAttackRange = 8.0f + 2.0f;

				pAnimationObject->m_nMaxHP = 125;
				pAnimationObject->m_fAttackSpeed = 1.666f / 1.43f;
			}
			break;
			}

		}

		int idx = d.first;
		auto pTexture = m_pModelBox->m_mapTextureBox.find(idx)->second;
		auto pMaterial = m_pModelBox->m_mapMaterialBox.find(idx)->second;
		pTexture->AddRef(); pMaterial->AddRef(); d.second->AddRef();
		
		pRenderObjectData = new RenderObjectData(d.first, d.second, 0, pd3dInstanceBuffer, ppObjects, 
			pTexture, pMaterial);
			
		m_vectorObjects.push_back(pRenderObjectData);
	}
}

void CAnimationShader::Animate(float fTimeElapsed)
{
	CInstancingShader::Animate(fTimeElapsed);
}

void CAnimationShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera* pCamera)
{
	OnPrepareRender(pd3dDeviceContext);
	int nValidObjects;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResourceWorld;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResourceAnimation;

	pd3dDeviceContext->VSSetShaderResources(VS_SLOT_ANIMATION_STRUCTURED, 1, &m_pd3dSRVAnimation);

	for (int i = m_vectorObjects.size() - 1; i >= 0; --i)
	{
		//재질 및 텍스쳐 리소스 Set
		if (m_vectorObjects[i]->m_pTexture) m_vectorObjects[i]->m_pTexture->UpdateShaderVariable(pd3dDeviceContext);
		if (m_vectorObjects[i]->m_pMaterial) UpdateMaterial(pd3dDeviceContext, &(m_vectorObjects[i]->m_pMaterial->m_material));

		//1213 스펙큘러 리소스 설정
		auto iterSpecular = m_pModelBox->m_mapSpecularMapBox.find(m_vectorObjects[i]->m_ObjectType);
		if (iterSpecular != m_pModelBox->m_mapSpecularMapBox.end()) iterSpecular->second->UpdateShaderVariable(pd3dDeviceContext);

		//1214 노말 리소스 설정
		auto iterNormal = m_pModelBox->m_mapNormalMapBox.find(m_vectorObjects[i]->m_ObjectType);
		if (iterNormal != m_pModelBox->m_mapNormalMapBox.end()) iterNormal->second->UpdateShaderVariable(pd3dDeviceContext);


		//월드변환행렬 및 구조화 된 버퍼 Map
		pd3dDeviceContext->Map(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResourceWorld);
		pd3dDeviceContext->Map(m_pd3dSRVAnimationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResourceAnimation);
		VS_INSTANCE_WITH_SLOT *pd3dxmtxWorld = (VS_INSTANCE_WITH_SLOT*)d3dMappedResourceWorld.pData;
		VS_SKINNED_MATRIX *pd3dxmtxAnimation = (VS_SKINNED_MATRIX*)d3dMappedResourceAnimation.pData;

		nValidObjects = 0;
		//로컬 좌표계에 대해 생각해보았는데, 프로그램 축을 맞추기 위한 Mesh의 로컬 좌표계, 그 외 일반적으로 쓰이는 Object의 로컬 좌표계
		for (int j = 0; j < m_vectorObjects[i]->m_nValidObjects; ++j)
		{
			if (m_vectorObjects[i]->m_ppObjects[j]->GetActive() && 
				m_vectorObjects[i]->m_ppObjects[j]->IsInSight() && 
				m_vectorObjects[i]->m_ppObjects[j]->IsVisible(pCamera))
			{
				CAnimationObject* pAnimationObject = (CAnimationObject*)(m_vectorObjects[i]->m_ppObjects[j]);

				//월드변환행렬 인스턴스 버퍼
				D3DXMATRIX mtxTransform = pAnimationObject->m_d3dxmtxLocal * pAnimationObject->m_d3dxmtxWorld;
				D3DXMatrixTranspose(&(pd3dxmtxWorld[nValidObjects].m_d3dxmtxWorld), &mtxTransform);
				pd3dxmtxWorld[nValidObjects].nSlot = nValidObjects;
				
				//애니메이션 구조화 버퍼
				for (int k = 0; k < MAX_BONE_COUNT - 1; ++k)
				{
					D3DXMatrixTranspose(&(pd3dxmtxAnimation[nValidObjects].m_d3dxmtxAnimation[k]), &(pAnimationObject->m_vecFinalTransforms[k]));
				}
				
				++nValidObjects;
			}
		}
		pd3dDeviceContext->Unmap(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0);
		pd3dDeviceContext->Unmap(m_pd3dSRVAnimationBuffer, 0);
		
		m_vectorObjects[i]->m_pMesh->RenderInstanced(pd3dDeviceContext, nValidObjects, 0);
	}
}

//그림자
void CAnimationShader::RenderObjectShadow(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	pd3dDeviceContext->IASetInputLayout(m_pd3dShadowVertexLayout);
	pd3dDeviceContext->VSSetShader(m_pd3dShadowVertexShader, NULL, 0);
	pd3dDeviceContext->PSSetShader(m_pd3dShadowPixelShader, NULL, 0);

	pd3dDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pd3dDeviceContext->HSSetShader(nullptr, nullptr, 0);
	pd3dDeviceContext->DSSetShader(nullptr, nullptr, 0);

	int nValidObjects;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResourceWorld;
	D3D11_MAPPED_SUBRESOURCE d3dMappedResourceAnimation;

	pd3dDeviceContext->VSSetShaderResources(VS_SLOT_ANIMATION_STRUCTURED, 1, &m_pd3dSRVAnimation);

	for (int i = m_vectorObjects.size() - 1; i >= 0; --i)
	{
		//월드변환행렬 및 구조화 된 버퍼 Map
		pd3dDeviceContext->Map(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResourceWorld);
		pd3dDeviceContext->Map(m_pd3dSRVAnimationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResourceAnimation);
		VS_INSTANCE_WITH_SLOT *pd3dxmtxWorld = (VS_INSTANCE_WITH_SLOT*)d3dMappedResourceWorld.pData;
		VS_SKINNED_MATRIX *pd3dxmtxAnimation = (VS_SKINNED_MATRIX*)d3dMappedResourceAnimation.pData;

		nValidObjects = 0;
		//로컬 좌표계에 대해 생각해보았는데, 프로그램 축을 맞추기 위한 Mesh의 로컬 좌표계, 그 외 일반적으로 쓰이는 Object의 로컬 좌표계
		for (int j = 0; j < m_vectorObjects[i]->m_nValidObjects; ++j)
		{
			if (m_vectorObjects[i]->m_ppObjects[j]->GetActive() && m_vectorObjects[i]->m_ppObjects[j]->IsInSight() && m_vectorObjects[i]->m_ppObjects[j]->IsVisible(pCamera))
			{
				CAnimationObject* pAnimationObject = (CAnimationObject*)(m_vectorObjects[i]->m_ppObjects[j]);

				//월드변환행렬 인스턴스 버퍼
				D3DXMatrixTranspose(&(pd3dxmtxWorld[nValidObjects].m_d3dxmtxWorld), &((pAnimationObject->m_d3dxmtxLocal) * (pAnimationObject->m_d3dxmtxWorld)));
				pd3dxmtxWorld[nValidObjects].nSlot = nValidObjects;

				//애니메이션 구조화 버퍼
				for (int k = 0; k < MAX_BONE_COUNT; ++k)
				{
					//XMStoreFloat4x4(&(pd3dxmtxAnimation[nValidObjects].m_d3dxmtxAnimation[k]), XMMatrixTranspose(XMLoadFloat4x4(&(pAnimationObject->m_vecFinalTransforms[k]))));
					D3DXMatrixTranspose(&(pd3dxmtxAnimation[nValidObjects].m_d3dxmtxAnimation[k]), &(pAnimationObject->m_vecFinalTransforms[k]));
				}

				/*
				//배열의 마지막에 메시 로컬 변환행렬을 담는다.
				//D3DXMATRIX xm4x4Matrix = pAnimationObject->GetMesh(0)->GetMeshLocalMatrixXM();
				//D3DXMATRIX mtxLocal = XMLoadFloat4x4(&(pAnimationObject->GetMesh()->GetMeshLocalMatrixXM()));
				//XMStoreFloat4x4(&(pd3dxmtxAnimation[nValidObjects].m_d3dxmtxAnimation[MAX_BONE_COUNT - 1]), XMMatrixTranspose(mtxLocal));

				D3DXMATRIX xm4x4Matrix = pAnimationObject->GetMesh(0)->GetMeshLocalMatrixD3DX();
				D3DXMATRIX mtxLocal = ((pAnimationObject->GetMesh()->GetMeshLocalMatrixD3DX()));
				D3DXMatrixTranspose(&(pd3dxmtxAnimation[nValidObjects].m_d3dxmtxAnimation[MAX_BONE_COUNT - 1]), &mtxLocal);
				*/

				++nValidObjects;
			}
		}
		pd3dDeviceContext->Unmap(m_vectorObjects[i]->m_pd3dInstanceBuffer, 0);
		pd3dDeviceContext->Unmap(m_pd3dSRVAnimationBuffer, 0);

		m_vectorObjects[i]->m_pMesh->RenderInstancedShadow(pd3dDeviceContext, m_vectorObjects[i]->m_pd3dInstanceBuffer, nValidObjects, 0);
	}
}
