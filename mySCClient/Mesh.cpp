#include "stdafx.h"
#include "Mesh.h"


#include <fstream>

//**********************************************************************
//텍스처 클래스의 함수들의 나열
CTexture::CTexture(int nTextures, int nSamplers, int nTextureStartSlot, int nSamplerStartSlot)
{
	m_nReferences = 0;

	m_nTextures = nTextures;
	m_ppd3dsrvTextures = new ID3D11ShaderResourceView*[m_nTextures];
	for (int i = 0; i < m_nTextures; i++) m_ppd3dsrvTextures[i] = NULL;
	m_nTextureStartSlot = nTextureStartSlot;
	m_nSamplers = nSamplers;
	m_ppd3dSamplerStates = new ID3D11SamplerState*[m_nSamplers];
	for (int i = 0; i < m_nSamplers; i++) m_ppd3dSamplerStates[i] = NULL;
	m_nSamplerStartSlot = nSamplerStartSlot;
}

CTexture::~CTexture()
{
	for (int i = 0; i < m_nTextures; i++) if (m_ppd3dsrvTextures[i]) m_ppd3dsrvTextures[i]->Release();
	for (int i = 0; i < m_nSamplers; i++) if (m_ppd3dSamplerStates[i]) m_ppd3dSamplerStates[i]->Release();
	if (m_ppd3dsrvTextures) delete[] m_ppd3dsrvTextures;
	if (m_ppd3dSamplerStates) delete[] m_ppd3dSamplerStates;
}

void CTexture::SetTexture(int nIndex, ID3D11ShaderResourceView *pd3dsrvTexture)
{
	if (m_ppd3dsrvTextures[nIndex]) m_ppd3dsrvTextures[nIndex]->Release();
	m_ppd3dsrvTextures[nIndex] = pd3dsrvTexture;
	if (pd3dsrvTexture) pd3dsrvTexture->AddRef();
}

void CTexture::SetSampler(int nIndex, ID3D11SamplerState *pd3dSamplerState)
{
	if (m_ppd3dSamplerStates[nIndex]) m_ppd3dSamplerStates[nIndex]->Release();
	m_ppd3dSamplerStates[nIndex] = pd3dSamplerState;
	if (pd3dSamplerState) pd3dSamplerState->AddRef();
}

void CTexture::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->PSSetShaderResources(m_nTextureStartSlot, m_nTextures, m_ppd3dsrvTextures);
	pd3dDeviceContext->PSSetSamplers(m_nSamplerStartSlot, m_nSamplers, m_ppd3dSamplerStates);
}

void CTexture::UpdateTextureShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex, int nSlot)
{
	pd3dDeviceContext->PSSetShaderResources(nSlot, 1, &m_ppd3dsrvTextures[nIndex]);
}

void CTexture::UpdateSamplerShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, int nIndex, int nSlot)
{
	pd3dDeviceContext->PSSetSamplers(nSlot, 1, &m_ppd3dSamplerStates[nIndex]);
}


//**********************************************************************
//Material 클래스의 함수들의 나열

void CMaterial::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof(MATERIAL);
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dcbMaterial);
}

void CMaterial::UpdateShaderVariable(ID3D11DeviceContext *pd3dDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy(pcbMaterial, &m_Material, sizeof(MATERIAL));
	pd3dDeviceContext->Unmap(m_pd3dcbMaterial, 0);
	pd3dDeviceContext->PSSetConstantBuffers(CB_SLOT_MATERIAL, 1, &m_pd3dcbMaterial);
}


//**********************************************************************
//AABB 충돌 클래스들의 함수

void AABB::Union(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum)
{
	if (d3dxvMinimum.x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = d3dxvMinimum.x;
	if (d3dxvMinimum.y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = d3dxvMinimum.y;
	if (d3dxvMinimum.z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = d3dxvMinimum.z;
	if (d3dxvMaximum.x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = d3dxvMaximum.x;
	if (d3dxvMaximum.y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = d3dxvMaximum.y;
	if (d3dxvMaximum.z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = d3dxvMaximum.z;
}

void AABB::Union(AABB *pAABB)
{
	Union(pAABB->m_d3dxvMinimum, pAABB->m_d3dxvMaximum);
}

void AABB::Update(D3DXMATRIX *pmtxTransform)
{
	/*바운딩 박스의 최소점과 최대점은 회전을 하면 더 이상 최소점과 최대점이 되지 않는다. 
	그러므로 바운딩 박스의 최소점과 최대점에서 8개의 정점을 구하고 변환(회전)을 한 다음
	최소점과 최대점을 다시 계산한다.*/
	D3DXVECTOR3 vVertices[8];
	vVertices[0] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[1] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[2] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMaximum.z);
	vVertices[3] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMinimum.y, m_d3dxvMinimum.z);
	vVertices[4] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	vVertices[5] = D3DXVECTOR3(m_d3dxvMinimum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[6] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMaximum.z);
	vVertices[7] = D3DXVECTOR3(m_d3dxvMaximum.x, m_d3dxvMaximum.y, m_d3dxvMinimum.z);
	m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	//8개의 정점에서 x, y, z 좌표의 최소값과 최대값을 구한다.
	for (int i = 0; i < 8; i++)
	{
		//정점을 변환한다.
		D3DXVec3TransformCoord(&vVertices[i], &vVertices[i], pmtxTransform);
		if (vVertices[i].x < m_d3dxvMinimum.x) m_d3dxvMinimum.x = vVertices[i].x;
		if (vVertices[i].y < m_d3dxvMinimum.y) m_d3dxvMinimum.y = vVertices[i].y;
		if (vVertices[i].z < m_d3dxvMinimum.z) m_d3dxvMinimum.z = vVertices[i].z;
		if (vVertices[i].x > m_d3dxvMaximum.x) m_d3dxvMaximum.x = vVertices[i].x;
		if (vVertices[i].y > m_d3dxvMaximum.y) m_d3dxvMaximum.y = vVertices[i].y;
		if (vVertices[i].z > m_d3dxvMaximum.z) m_d3dxvMaximum.z = vVertices[i].z;
	}
}

//**********************************************************************
//메시 피킹에 대한 처리
int CMesh::CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	//모델 좌표계의 광선의 시작점(pd3dxvRayPosition)과 방향이 주어질 때 메쉬와의 충돌 검사를 한다.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE*)m_pd3dxvPositions + m_pnVertexOffsets[0];
	//Offsets -> 정점의 요소들을 나타내는 버퍼들의 시작 위치(바이트 수)를 나타내는 배열이다

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	//메쉬의 프리미티브(삼각형)들의 개수이다. 삼각형 리스트인 경우 (정점의 개수 / 3) 또는 (인덱스의 개수 / 3), 삼각형 스트립의 경우 (정점의 개수 - 2) 또는 (인덱스의 개수 - 2)이다.
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	D3DXVECTOR3 v0, v1, v2;
	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;
	/*메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다. 충돌하는 모든 삼각형을 찾아 광선의 시작점(실제로는 카메라 좌표계의 원점)에 가장 가까운 삼각형을 찾는다.*/
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_pnVertexStrides[0]);
		v1 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_pnVertexStrides[0]);
		v2 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_pnVertexStrides[0]);
		if (D3DXIntersectTri(&v0, &v1, &v2, pd3dxvRayPosition, pd3dxvRayDirection, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		{	//u,v,dist 값을 삼각형과 광선 시작점,방향을 통해서 얻는다.
			//함수의 리턴이 true -> 삼각형과 광선은 교점이 존재한다.
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = fuHitBaryCentric;
					pd3dxIntersectInfo->m_fV = fvHitBaryCentric;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}
	return(nIntersections);
}

D3DXVECTOR3 CMesh::GetPositionRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	//모델 좌표계의 광선의 시작점(pd3dxvRayPosition)과 방향이 주어질 때 메쉬와의 충돌 검사를 한다.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE*)m_pd3dxvPositions + m_pnVertexOffsets[0];
	//Offsets -> 정점의 요소들을 나타내는 버퍼들의 시작 위치(바이트 수)를 나타내는 배열이다

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	//메쉬의 프리미티브(삼각형)들의 개수이다. 삼각형 리스트인 경우 (정점의 개수 / 3) 또는 (인덱스의 개수 / 3), 삼각형 스트립의 경우 (정점의 개수 - 2) 또는 (인덱스의 개수 - 2)이다.
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	D3DXVECTOR3 v0, v1, v2;
	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;
	/*메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다. 충돌하는 모든 삼각형을 찾아 광선의 시작점(실제로는 카메라 좌표계의 원점)에 가장 가까운 삼각형을 찾는다.*/
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_pnVertexStrides[0]);
		v1 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_pnVertexStrides[0]);
		v2 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_pnVertexStrides[0]);
		if (D3DXIntersectTri(&v0, &v1, &v2, pd3dxvRayPosition, pd3dxvRayDirection, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		{	//u,v,dist 값을 삼각형과 광선 시작점,방향을 통해서 얻는다.
			//함수의 리턴이 true -> 삼각형과 광선은 교점이 존재한다.
			if (fHitDistance < fNearHitDistance)
			{
				fNearHitDistance = fHitDistance;
				if (pd3dxIntersectInfo)
				{
					pd3dxIntersectInfo->m_dwFaceIndex = i;
					pd3dxIntersectInfo->m_fU = fuHitBaryCentric;
					pd3dxIntersectInfo->m_fV = fvHitBaryCentric;
					pd3dxIntersectInfo->m_fDistance = fHitDistance;
				}
			}
			nIntersections++;
		}
	}

	if(nIntersections > 0)
	{
		int Idx = pd3dxIntersectInfo->m_dwFaceIndex;
		//u는 v1, v는 v2, 1 -  (u+v) 는 v0의 무게중심이다.
 		v0 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(Idx*nOffset) + 0]) : ((Idx*nOffset) + 0)) * m_pnVertexStrides[0]);
		v1 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(Idx*nOffset) + 1]) : ((Idx*nOffset) + 1)) * m_pnVertexStrides[0]);
		v2 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(Idx*nOffset) + 2]) : ((Idx*nOffset) + 2)) * m_pnVertexStrides[0]);
		float u, v, w;
		u = pd3dxIntersectInfo->m_fU; v = pd3dxIntersectInfo->m_fV; w = 1.0f - (u + v);

		D3DXVECTOR3 vResult;
		vResult.x = w*v0.x + u*v1.x + v*v2.x;
		vResult.y = w*v0.y + u*v1.y + v*v2.y;
		vResult.z = w*v0.z + u*v1.z + v*v2.z;

		return (vResult);
	}
	else
	{
		return D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	}
}


//**********************************************************************
CMesh::CMesh(ID3D11Device *pd3dDevice)
{
	m_ppd3dVertexBuffers = nullptr;
	m_pd3dPositionBuffer = nullptr;
	m_pd3dIndexBuffer = nullptr;

	m_pd3dRasterizerState = nullptr;

	m_nVertices = 0;
	m_nBuffers = 0;
	m_nSlot = 0;
	m_nStartVertex = 0;

	m_nIndices = 0;
	m_nStartIndex = 0;
	m_nBaseVertex = 0;
	m_dxgiIndexFormat = DXGI_FORMAT_R32_UINT;
	m_nIndexOffset = 0;

	m_pnVertexStrides = nullptr;
	m_pnVertexOffsets = nullptr;
	
	m_pd3dxvPositions = nullptr;
	D3DXMatrixIdentity(&m_d3dxmtxLocal);
	//XMStoreFloat4x4(&m_xm4x4Local, XMMatrixIdentity());
	m_pnIndices = nullptr;

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nReferences = 0;	//처음 메시 생성 시 SetMesh 잊지 말자
	m_meshMaterial = MATERIAL();

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

CMesh::~CMesh()
{
	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();

	if (m_pd3dRasterizerState) m_pd3dRasterizerState->Release();

	if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
	if (m_pnVertexStrides) delete[] m_pnVertexStrides;
	if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;

	if (m_pnIndices) delete[] m_pnIndices;
	if (m_pd3dxvPositions) delete[] m_pd3dxvPositions;
}

//**********************************************************************
//정점에 다양한 정보가 있는 경우, 버퍼의 포인터로 관리하기 위함
void CMesh::AssembleToVertexBuffer(int nBuffers, ID3D11Buffer **ppd3dBuffers, UINT *pnBufferStrides, UINT *pnBufferOffsets)
{
	ID3D11Buffer **ppd3dNewVertexBuffers = new ID3D11Buffer*[m_nBuffers + nBuffers];
	UINT *pnNewVertexStrides = new UINT[m_nBuffers + nBuffers];
	UINT *pnNewVertexOffsets = new UINT[m_nBuffers + nBuffers];

	if (m_nBuffers > 0)
	{
		for (int i = 0; i < m_nBuffers; i++)
		{
			ppd3dNewVertexBuffers[i] = m_ppd3dVertexBuffers[i];
			pnNewVertexStrides[i] = m_pnVertexStrides[i];
			pnNewVertexOffsets[i] = m_pnVertexOffsets[i];
		}
		if (m_ppd3dVertexBuffers) delete[] m_ppd3dVertexBuffers;
		if (m_pnVertexStrides) delete[] m_pnVertexStrides;
		if (m_pnVertexOffsets) delete[] m_pnVertexOffsets;
	}

	for (int i = 0; i < nBuffers; i++)
	{
		ppd3dNewVertexBuffers[m_nBuffers + i] = ppd3dBuffers[i];
		pnNewVertexStrides[m_nBuffers + i] = pnBufferStrides[i];
		pnNewVertexOffsets[m_nBuffers + i] = pnBufferOffsets[i];
	}

	m_nBuffers += nBuffers;
	m_ppd3dVertexBuffers = ppd3dNewVertexBuffers;
	m_pnVertexStrides = pnNewVertexStrides;
	m_pnVertexOffsets = pnNewVertexOffsets;
}


ID3D11Buffer *CMesh::CreateBuffer(ID3D11Device *pd3dDevice, UINT nStride, int nElements, void *pBufferData, UINT nBindFlags, D3D11_USAGE d3dUsage, UINT nCPUAccessFlags)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = d3dUsage;
	d3dBufferDesc.ByteWidth = nStride * nElements;
	d3dBufferDesc.BindFlags = nBindFlags;
	d3dBufferDesc.CPUAccessFlags = nCPUAccessFlags;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pBufferData;
	d3dBufferData.SysMemPitch = nStride * nElements;

	ID3D11Buffer *pd3dBuffer = NULL;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &pd3dBuffer);
	return(pd3dBuffer);
}

//**********************************************************************
//기본 부모클래스인 CMesh의 렌더링
void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_ppd3dVertexBuffers)
	{
		pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
		pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
		pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
		//pd3dDeviceContext->RSSetState(m_pd3dRasterizerState); //RS는 오브젝트가 관리하는걸로... 혹은 셰이더도 괜춘

		if (m_pd3dIndexBuffer)
			pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
		else
			pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
	}
}

void CMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	//인스턴싱의 경우 입력 조립기에 메쉬의 정점 버퍼와 인스턴스 정점 버퍼가 연결된다.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	//객체들의 인스턴스들을 렌더링한다. 
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}


//**********************************************************************
//CMouseRectMesh 함수들이다.
CMouseRectMesh::CMouseRectMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 4;
	MouseRectVertex* pVertices = new MouseRectVertex[m_nVertices];
	//실제 값이 넣어지는 건 Update 이후이다.

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	//Map
	d3dBufferDesc.ByteWidth = sizeof(MouseRectVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	d3dBufferData.SysMemPitch = sizeof(MouseRectVertex) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	delete[] pVertices;

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(MouseRectVertex) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);


	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nIndices = 4;
	UINT* pIndices = new UINT[m_nIndices];
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 3;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	d3dBufferData.SysMemPitch = sizeof(UINT)* m_nIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);
	delete[] pIndices;

	CreateRasterizerState(pd3dDevice);
}

CMouseRectMesh::~CMouseRectMesh()
{
}

/*
void CMouseRectMesh::UpdateMouseRect(D3DXVECTOR2& oldPos, D3DXVECTOR2& nextPos, float width, float height, ID3D11DeviceContext* pd3dDeviceContext)
{
	//old위치보다 왼쪽 위 or 아래 / 오른쪽 위 or 아래 경우를 나누어 정점의 위치를 map한다.
	//인덱스는 고정이므로 LT가 1번, RT가 0번, LB가 3번, RB가 2번이다.
	MouseRectVertex vVertices[4];
	vVertices[0].tex = D3DXVECTOR2(1.0f, 0.0f);	vVertices[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	vVertices[2].tex = D3DXVECTOR2(1.0f, 1.0f); vVertices[3].tex = D3DXVECTOR2(0.0f, 1.0f);
	
	D3DXVECTOR2 oldPosR = D3DXVECTOR2(oldPos.x, height - oldPos.y);
	D3DXVECTOR2 nextPosR = D3DXVECTOR2(nextPos.x, height - nextPos.y);

	float oldX = (oldPosR.x * 2.0f - width) / width;
	float oldY = (oldPosR.y * 2.0f - height) / height;
	float nextX = (nextPosR.x * 2.0f - width) / width;
	float nextY = (nextPosR.y * 2.0f - height) / height;

	if (oldPosR.x < nextPosR.x)
	{
		if (oldPosR.y < nextPosR.y)	//왼쪽 위
		{
			vVertices[1].pos = D3DXVECTOR3(nextX, nextY, 0.0f);
			vVertices[0].pos = D3DXVECTOR3(oldX, nextY, 0.0f);
			vVertices[3].pos = D3DXVECTOR3(nextX, oldY, 0.0f);
			vVertices[2].pos = D3DXVECTOR3(oldX, oldY, 0.0f);
		}
		else  //왼쪽 아래
		{
			vVertices[1].pos = D3DXVECTOR3(nextX, oldY, 0.0f);
			vVertices[0].pos = D3DXVECTOR3(oldX, oldY, 0.0f);
			vVertices[3].pos = D3DXVECTOR3(nextX, nextY, 0.0f);
			vVertices[2].pos = D3DXVECTOR3(oldX, nextY, 0.0f);
		}
	}
	else
	{
		if (oldPosR.y < nextPosR.y) // 오른쪽 위
		{
			vVertices[1].pos = D3DXVECTOR3(oldX, nextY, 0.0f);
			vVertices[0].pos = D3DXVECTOR3(nextX, nextY, 0.0f);
			vVertices[3].pos = D3DXVECTOR3(oldX, oldY, 0.0f);
			vVertices[2].pos = D3DXVECTOR3(nextX, oldY, 0.0f);
		}
		else //오른쪽 아래
		{
			vVertices[1].pos = D3DXVECTOR3(oldX, oldY, 0.0f);
			vVertices[0].pos = D3DXVECTOR3(nextX, oldY, 0.0f);
			vVertices[3].pos = D3DXVECTOR3(oldX, nextY, 0.0f);
			vVertices[2].pos = D3DXVECTOR3(nextX, nextY, 0.0f);
		}
	}

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MouseRectVertex *pcbMRVertices = (MouseRectVertex *)d3dMappedResource.pData;
	memcpy(pcbMRVertices, &vVertices, sizeof(MouseRectVertex) * 4);
	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}
*/


void CMouseRectMesh::UpdateMouseRect(D3DXVECTOR3& oldPos, D3DXVECTOR3& nextPos, ID3D11DeviceContext* pd3dDeviceContext)
{
	//old위치보다 왼쪽 위 or 아래 / 오른쪽 위 or 아래 경우를 나누어 정점의 위치를 map한다.
	//인덱스는 고정이므로 LT가 1번, RT가 0번, LB가 3번, RB가 2번이다.
	MouseRectVertex vVertices[4];
	vVertices[0].tex = D3DXVECTOR2(1.0f, 0.0f);	vVertices[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	vVertices[2].tex = D3DXVECTOR2(1.0f, 1.0f); vVertices[3].tex = D3DXVECTOR2(0.0f, 1.0f);

	//pos의 값은 모두 이미 월드좌표계의 값이다. 이후 처리는 투영과 스크린 처리만 하면 된다. y값은 큰 의미가 없지만 일단 next걸로

	if (oldPos.x < nextPos.x)
	{
		if (oldPos.z < nextPos.z)	//왼쪽 위
		{
			vVertices[1].pos = D3DXVECTOR3(nextPos.x, nextPos.y , nextPos.z);
			vVertices[0].pos = D3DXVECTOR3(oldPos.x, nextPos.y , nextPos.z);
			vVertices[3].pos = D3DXVECTOR3(nextPos.x, nextPos.y , oldPos.z);
			vVertices[2].pos = D3DXVECTOR3(oldPos.x, nextPos.y , oldPos.z);
		}
		else  //왼쪽 아래
		{
			vVertices[1].pos = D3DXVECTOR3(nextPos.x, nextPos.y , oldPos.z);
			vVertices[0].pos = D3DXVECTOR3(oldPos.x, nextPos.y , oldPos.z);
			vVertices[3].pos = D3DXVECTOR3(nextPos.x, nextPos.y , nextPos.z);
			vVertices[2].pos = D3DXVECTOR3(oldPos.x, nextPos.y , nextPos.z);
		}
	}
	else
	{
		if (oldPos.z < nextPos.z) // 오른쪽 위
		{
			vVertices[1].pos = D3DXVECTOR3(oldPos.x, nextPos.y , nextPos.z);
			vVertices[0].pos = D3DXVECTOR3(nextPos.x, nextPos.y , nextPos.z);
			vVertices[3].pos = D3DXVECTOR3(oldPos.x, nextPos.y , oldPos.z);
			vVertices[2].pos = D3DXVECTOR3(nextPos.x, nextPos.y , oldPos.z);
		}
		else //오른쪽 아래
		{
			vVertices[1].pos = D3DXVECTOR3(oldPos.x, nextPos.y , oldPos.z);
			vVertices[0].pos = D3DXVECTOR3(nextPos.x, nextPos.y , oldPos.z);
			vVertices[3].pos = D3DXVECTOR3(oldPos.x, nextPos.y , nextPos.z);
			vVertices[2].pos = D3DXVECTOR3(nextPos.x, nextPos.y , nextPos.z);
		}
	}

	//이미지를 렌더링 할 때 는 y가 0.0f를 기준으로 해야 마우스와 위치가 맞다. 실제 객체 피킹시에는 y값을 고려한다.
	//for (int i = 0; i < 4; ++i)
	//	vVertices[i].pos.y = 0.0f;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MouseRectVertex *pcbMRVertices = (MouseRectVertex *)d3dMappedResource.pData;
	memcpy(pcbMRVertices, &vVertices, sizeof(MouseRectVertex) * 4);
	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}


//**********************************************************************
//CSelectedCircleMesh 함수들이다.
CSelectedCircleMesh::CSelectedCircleMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = MAX_SELECTED_OBJECTS;
	D3DXVECTOR4* pVertices = new D3DXVECTOR4[m_nVertices];
	//실제 값이 넣어지는 건 Update 이후이다.

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	//Map
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR4) * m_nVertices;	//처음 크게 잡고 Update에서 vector size만큼만
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR4) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	delete[] pVertices;

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(D3DXVECTOR4) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;	//기하셰이더니까
	
	/*
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory(&d3dRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	pd3dDevice->CreateRasterizerState(&d3dRasterizerDesc, &m_pd3dRasterizerState);
	*/
}

CSelectedCircleMesh::~CSelectedCircleMesh()
{

}

void CSelectedCircleMesh::UpdateSelectedCircle(std::vector<D3DXVECTOR4>& vPosScale, ID3D11DeviceContext* pd3dDeviceContext)
{
	//마지막 Scale 인자값에 아군이면 0~100 / 적군이면 +100 / 중립이면 +200을 한다.
	//셰이더코드에서 0~100사이이면 초록, 100보다 크면 -100 하고 빨강으로, 200보다 크면 -200하고 노랑으로 렌더링

	m_nVertices = vPosScale.size();	///w = scaleType
	
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	D3DXVECTOR4 *pcbSCVertices = (D3DXVECTOR4 *)d3dMappedResource.pData;
	//memcpy(pcbSCVertices, &(vPosScale[0]), sizeof(D3DXVECTOR4) * m_nVertices);
	for (int i = 0; i < m_nVertices; ++i)
		pcbSCVertices[i] = vPosScale[i];
	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}


//**********************************************************************
//CMiniMapObject 함수들이다.
CMiniMapObjectMesh::CMiniMapObjectMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = MAX_KINDS_OBJECTS * MAX_INSTANCE_OBJECTS;	//일단 최대로 잡아놓는다 -> 버퍼 생성하기 위해 / 추후 동적으로 변화
	//실제 값이 넣어지는 건 Update 이후이다.

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	//Map
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR4) * m_nVertices;	//처음 크게 잡고 Update에서 vector size만큼만
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dPositionBuffer);

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(D3DXVECTOR4) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;	//기하셰이더니까
}

CMiniMapObjectMesh::~CMiniMapObjectMesh()
{

}

void CMiniMapObjectMesh::UpdateMiniMapObject(std::vector<D3DXVECTOR4>& vPosType, ID3D11DeviceContext* pd3dDeviceContext)
{
	m_nVertices = vPosType.size();	///w = scaleType

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	D3DXVECTOR4 *pcbSCVertices = (D3DXVECTOR4 *)d3dMappedResource.pData;
	for (int i = 0; i < m_nVertices; ++i)
		pcbSCVertices[i] = vPosType[i];
	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}

//**********************************************************************
//CMiniMapRectMesh
CMiniMapRectMesh::CMiniMapRectMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 4;	

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	//Map
	d3dBufferDesc.ByteWidth = sizeof(VERTEX_MINIMAP_RECT) * m_nVertices;	
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dPositionBuffer);

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(VERTEX_MINIMAP_RECT) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nIndices = 4;
	UINT* pIndices = new UINT[m_nIndices];
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 3;


	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	d3dBufferData.SysMemPitch = sizeof(UINT)* m_nIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);
	delete[] pIndices;
}

CMiniMapRectMesh::~CMiniMapRectMesh()
{

}

void CMiniMapRectMesh::UpdateMiniMapRect(D3DXVECTOR3 cameraLookPos, ID3D11DeviceContext* pd3dDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VERTEX_MINIMAP_RECT *pcbMinimapRectVertices = (VERTEX_MINIMAP_RECT *)d3dMappedResource.pData;
	
	VERTEX_MINIMAP_RECT vLB, vRB, vLT, vRT;
	vLT.texCoord = D3DXVECTOR2(0.0f, 0.0f); vRT.texCoord = D3DXVECTOR2(1.0f, 0.0f);
	vLB.texCoord = D3DXVECTOR2(0.0f, 1.0f); vRB.texCoord = D3DXVECTOR2(1.0f, 1.0f);

	float xHalf = 15.0f; float zHalf = 12.0f;
	vLT.position = D3DXVECTOR3(cameraLookPos.x - xHalf, 0.0f, cameraLookPos.z + zHalf); vRT.position = D3DXVECTOR3(cameraLookPos.x + xHalf, 0.0f, cameraLookPos.z + zHalf);
	vLB.position = D3DXVECTOR3(cameraLookPos.x - xHalf, 0.0f, cameraLookPos.z - zHalf); vRB.position = D3DXVECTOR3(cameraLookPos.x + xHalf, 0.0f, cameraLookPos.z - zHalf);

	// 인덱스버퍼 씀, lt rt lb rb 0 1 2 3
	pcbMinimapRectVertices[0] = vLT;
	pcbMinimapRectVertices[1] = vRT;
	pcbMinimapRectVertices[2] = vLB;
	pcbMinimapRectVertices[3] = vRB;

	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}


//**********************************************************************
//CNeutralResourceMesh 함수들이다.
CNeutralResourceMesh::CNeutralResourceMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = MAX_KINDS_OBJECTS;	//최대 scv 개수

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	//Map
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR4) * m_nVertices;	//처음 크게 잡고 Update에서 vector size만큼만
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dPositionBuffer);

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(D3DXVECTOR4) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;	//기하셰이더니까
}

CNeutralResourceMesh::~CNeutralResourceMesh()
{

}

void CNeutralResourceMesh::UpdateNeutralResource(std::vector<D3DXVECTOR4>& vPosType, ID3D11DeviceContext* pd3dDeviceContext)
{
	//기하셰이더에서 w값을 통해 텍스쳐좌표를 생성, 0~0.5f와 0.5f~1.0f로 자원 이미지를 얻어낸다.
	m_nVertices = vPosType.size();	///w가 0.0f -> mineral , 1.0f -> gas 

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	D3DXVECTOR4 *pcbNRVertices = (D3DXVECTOR4 *)d3dMappedResource.pData;
	for (int i = 0; i < m_nVertices; ++i)
		pcbNRVertices[i] = vPosType[i];
	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}




//**********************************************************************
//CMainFrameMesh 함수들이다.
CMainFrameMesh::CMainFrameMesh(ID3D11Device* pd3dDevice, D3DXVECTOR2 vScreenLB, D3DXVECTOR2 vScreenRT) : CMesh(pd3dDevice)
{
	//좌상 0 우상 1 좌하 2 우하 3 바로 스크린좌표로 연결될것이다.
	m_nVertices = 4;
	MainFrameVertex* pVertices = new MainFrameVertex[m_nVertices];
	
	/*
	pVertices[0].pos = D3DXVECTOR2(-1.0f, -1.0f + 0.6f);
	pVertices[1].pos = D3DXVECTOR2(+1.0f, -1.0f + 0.6f);
	pVertices[2].pos = D3DXVECTOR2(-1.0f, -1.0f);
	pVertices[3].pos = D3DXVECTOR2(+1.0f, -1.0f);
	*/

	pVertices[0].pos = D3DXVECTOR2(vScreenLB.x, vScreenRT.y);
	pVertices[1].pos = D3DXVECTOR2(vScreenRT.x, vScreenRT.y);
	pVertices[2].pos = D3DXVECTOR2(vScreenLB.x, vScreenLB.y);
	pVertices[3].pos = D3DXVECTOR2(vScreenRT.x, vScreenLB.y);

	pVertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVertices[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVertices[3].tex = D3DXVECTOR2(1.0f, 1.0f);
	

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(MainFrameVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	d3dBufferData.SysMemPitch = sizeof(MainFrameVertex) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	delete[] pVertices;

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(MainFrameVertex) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);


	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nIndices = 4;
	UINT* pIndices = new UINT[m_nIndices];
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 3;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	d3dBufferData.SysMemPitch = sizeof(UINT)* m_nIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);
	delete[] pIndices;

	/*
	//블렌딩 스테이트
	ID3D11BlendState *pd3dBlendState = NULL;
	D3D11_BLEND_DESC BD;
	ZeroMemory(&BD, sizeof(D3D11_BLEND_DESC));
	BD.AlphaToCoverageEnable = false;
	BD.IndependentBlendEnable = false; //한개의 렌더타겟만 쓰겠다.
	ZeroMemory(&(BD.RenderTarget[0]), sizeof(D3D11_RENDER_TARGET_BLEND_DESC));

	BD.RenderTarget[0].BlendEnable = true;

	BD.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;		//텍스쳐 색상 그대로
	BD.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;	//렌더타겟 색상은 0이 곱해진다.
	BD.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;

	BD.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BD.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BD.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	BD.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//BD.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_BLUE;
	pd3dDevice->CreateBlendState(&BD, &pd3dBlendState);
	m_pd3dBlendState = pd3dBlendState;
	*/
}

CMainFrameMesh::~CMainFrameMesh()
{
	if (m_pd3dBlendState) m_pd3dBlendState->Release();
}

void CMainFrameMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	/*
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	UINT prevSampleMask;
	ID3D11BlendState *prevBlendState;
	pd3dDeviceContext->OMGetBlendState(&prevBlendState, blendFactor, &prevSampleMask);
	pd3dDeviceContext->OMSetBlendState(m_pd3dBlendState, blendFactor, sampleMask);
	*/
	CMesh::Render(pd3dDeviceContext);

	//pd3dDeviceContext->OMSetBlendState(prevBlendState, blendFactor, prevSampleMask);
}

void CMainFrameMesh::UpdateScreenPosition(ID3D11DeviceContext* pd3dDeviceContext, float xPosition, float yPosition)
{
	float width = 0.06f;
	D3DXVECTOR2 vScreenLB{ xPosition, yPosition - width};
	D3DXVECTOR2 vScreenRT{ xPosition + width, yPosition};
	

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MainFrameVertex *pVertices = (MainFrameVertex *)d3dMappedResource.pData;
	
	pVertices[0].pos = D3DXVECTOR2(vScreenLB.x, vScreenRT.y);
	pVertices[1].pos = D3DXVECTOR2(vScreenRT.x, vScreenRT.y);
	pVertices[2].pos = D3DXVECTOR2(vScreenLB.x, vScreenLB.y);
	pVertices[3].pos = D3DXVECTOR2(vScreenRT.x, vScreenLB.y);

	pVertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVertices[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVertices[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}


//**********************************************************************
//CScreenMesh 함수들이다.
CScreenMesh::CScreenMesh(ID3D11Device* pd3dDevice, D3DXVECTOR2& leftBottom, D3DXVECTOR2& rightTop, bool beRendered) :CMesh(pd3dDevice)
{
	float fRendered = 0.0f;
	if (beRendered)
		fRendered = 1.0f;

	//좌상 0 우상 1 좌하 2 우하 3 바로 스크린좌표로 연결될것이다.
	m_nVertices = 4;
	ScreenVertex* pVertices = new ScreenVertex[m_nVertices];

	pVertices[0].pos = D3DXVECTOR2(leftBottom.x, rightTop.y);
	pVertices[1].pos = D3DXVECTOR2(rightTop.x, rightTop.y);
	pVertices[2].pos = D3DXVECTOR2(leftBottom.x, leftBottom.y);
	pVertices[3].pos = D3DXVECTOR2(rightTop.x, leftBottom.y);

	pVertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVertices[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVertices[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	for (int i = 0; i < 4; ++i)
		pVertices[i].fRendered = fRendered;
	
	//테크트리 표현을 위해 위치 및 텍스쳐 정보를 저장한다.
	m_VertexData[0].pos = pVertices[0].pos;
	m_VertexData[1].pos = pVertices[1].pos;
	m_VertexData[2].pos = pVertices[2].pos;
	m_VertexData[3].pos = pVertices[3].pos;

	m_VertexData[0].tex = pVertices[0].tex;
	m_VertexData[1].tex = pVertices[1].tex;
	m_VertexData[2].tex = pVertices[2].tex;
	m_VertexData[3].tex = pVertices[3].tex;

	for(int i = 0; i < 4; ++i)
		m_VertexData[i].fRendered = fRendered;
	/////////////////////////////////////////////////


	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	//d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT; //테크트리 표현
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(ScreenVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//d3dBufferDesc.CPUAccessFlags = 0; //테크트리 표현
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	d3dBufferData.SysMemPitch = sizeof(ScreenVertex) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	delete[] pVertices;

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(ScreenVertex) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nIndices = 4;
	UINT* pIndices = new UINT[m_nIndices];
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 3;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	d3dBufferData.SysMemPitch = sizeof(UINT)* m_nIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);
	delete[] pIndices;
}

CScreenMesh::~CScreenMesh()
{

}

void CScreenMesh::UpdateScreenMeshRendered(ID3D11DeviceContext* pd3dDeviceContext, bool beRendered)
{
	float fRendered = 0.0f;
	if (beRendered)
		fRendered = 1.0f;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	ScreenVertex *pVertices = (ScreenVertex *)d3dMappedResource.pData;

	pVertices[0].pos = m_VertexData[0].pos;
	pVertices[1].pos = m_VertexData[1].pos;
	pVertices[2].pos = m_VertexData[2].pos;
	pVertices[3].pos = m_VertexData[3].pos;

	pVertices[0].tex = m_VertexData[0].tex;
	pVertices[1].tex = m_VertexData[1].tex;
	pVertices[2].tex = m_VertexData[2].tex;
	pVertices[3].tex = m_VertexData[3].tex;

	for (int i = 0; i < 4; ++i)
		pVertices[i].fRendered = fRendered;

	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}



//**********************************************************************
//CCrowdMesh 함수들이다.
CCrowdMesh::CCrowdMesh(ID3D11Device* pd3dDevice, D3DXVECTOR2& leftBottom, D3DXVECTOR2& rightTop) :CMesh(pd3dDevice)
{
	vLB = leftBottom;
	vRT = rightTop;

	//좌상 0 우상 1 좌하 2 우하 3 바로 스크린좌표로 연결될것이다.
	m_nVertices = 4;
	CrowdVertex* pVertices = new CrowdVertex[m_nVertices];

	pVertices[0].pos = D3DXVECTOR2(leftBottom.x, rightTop.y);
	pVertices[1].pos = D3DXVECTOR2(rightTop.x, rightTop.y);
	pVertices[2].pos = D3DXVECTOR2(leftBottom.x, leftBottom.y);
	pVertices[3].pos = D3DXVECTOR2(rightTop.x, leftBottom.y);

	pVertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVertices[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVertices[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVertices[0].color = 0.0f;
	pVertices[1].color = 0.0f;
	pVertices[2].color = 0.0f;
	pVertices[3].color = 0.0f;

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(CrowdVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pVertices;
	d3dBufferData.SysMemPitch = sizeof(CrowdVertex) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	delete[] pVertices;

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(CrowdVertex) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_nIndices = 4;
	UINT* pIndices = new UINT[m_nIndices];
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 3;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pIndices;
	d3dBufferData.SysMemPitch = sizeof(UINT)* m_nIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);
	delete[] pIndices;
}

CCrowdMesh::~CCrowdMesh()
{
}

void CCrowdMesh::UpdateCrowdColor(ID3D11DeviceContext* pd3dDeviceContext, float fColor)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	CrowdVertex *pVertices = (CrowdVertex *)d3dMappedResource.pData;
	
	pVertices[0].pos = D3DXVECTOR2(vLB.x, vRT.y);
	pVertices[1].pos = D3DXVECTOR2(vRT.x, vRT.y);
	pVertices[2].pos = D3DXVECTOR2(vLB.x, vLB.y);
	pVertices[3].pos = D3DXVECTOR2(vRT.x, vLB.y);

	pVertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVertices[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVertices[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVertices[0].color = fColor;
	pVertices[1].color = fColor;
	pVertices[2].color = fColor;
	pVertices[3].color = fColor;
	
	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);

}

//**********************************************************************
//CSkyBoxMesh 함수들이다.
CSkyBoxMesh::CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice)
{
	//스카이 박스는 6개의 면(사각형), 사각형은 정점 4개, 그러므로 24개의 정점이 필요하다.
	m_nVertices = 24;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];

	int i = 0;
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;
	// Front Quad 
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Back Quad
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Left Quad
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Right Quad
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Top Quad
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	// Bottom Quad
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, +fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fx, -fx);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	::ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	::ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR3) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR2) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	delete[] pd3dxvTexCoords;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	//삼각형 스트립으로 사각형 1개를 그리기 위해 인덱스는 4개가 필요하다.
	m_nIndices = 4;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 0;
	m_pnIndices[1] = 1;
	m_pnIndices[2] = 3;
	m_pnIndices[3] = 2;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	d3dBufferData.SysMemPitch = sizeof(UINT) * m_nIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	D3D11_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	//스카이 박스 사각형들은 깊이 버퍼 알고리즘을 적용하지 않고 깊이 버퍼를 변경하지 않는다.
	d3dDepthStencilDesc.DepthEnable = false;
	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	d3dDepthStencilDesc.StencilEnable = false;
	d3dDepthStencilDesc.StencilReadMask = 0xFF;
	d3dDepthStencilDesc.StencilWriteMask = 0xFF;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	pd3dDevice->CreateDepthStencilState(&d3dDepthStencilDesc, &m_pd3dDepthStencilState);

	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory(&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState(&d3dSamplerDesc, &pd3dSamplerState);

	m_pSkyboxTexture = new CTexture(6, 1, PS_SLOT_TEXTURE, PS_SLOT_TEXTURE);
	m_pSkyboxTexture->SetSampler(0, pd3dSamplerState);
	pd3dSamplerState->Release();
	m_pSkyboxTexture->AddRef();

	OnChangeSkyBoxTextures(pd3dDevice, 0);
}

CSkyBoxMesh::~CSkyBoxMesh()
{
	if (m_pd3dDepthStencilState) m_pd3dDepthStencilState->Release();
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
	if (m_pSkyboxTexture) m_pSkyboxTexture->Release();
}

void CSkyBoxMesh::OnChangeSkyBoxTextures(ID3D11Device *pd3dDevice, int nIndex)
{
	//6개의 스카이 박스 텍스쳐를 생성하여 CTexture 객체에 연결한다.
	_TCHAR pstrTextureName[80];
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	/*
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/SkyBox_Front_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/SkyBox_Back_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(1, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/SkyBox_Left_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(2, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/SkyBox_Right_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(3, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/SkyBox_Top_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(4, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/SkyBox_Bottom_%d.jpg"), nIndex, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(5, pd3dsrvTexture);
	*/
	int n = 0;
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/space_%d.png"), n++, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(0, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/space_%d.png"), n++, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(1, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/space_%d.png"), n++, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(2, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/space_%d.png"), n++, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(3, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/space_%d.png"), n++, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(4, pd3dsrvTexture);
	pd3dsrvTexture->Release();
	_stprintf_s(pstrTextureName, _T("Assets/Image/SkyBox/space_%d.png"), n++, 80);
	D3DX11CreateShaderResourceViewFromFile(pd3dDevice, pstrTextureName, NULL, NULL, &pd3dsrvTexture, NULL);
	m_pSkyboxTexture->SetTexture(5, pd3dsrvTexture);
	pd3dsrvTexture->Release();
}

void CSkyBoxMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);

	//스카이 박스를 그리기 위한 샘플러 상태 객체와 깊이 스텐실 상태 객체를 설정한다.
	m_pSkyboxTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, PS_SLOT_SAMPLER);
	pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState, 0);

	//스카이 박스의 6개 면(사각형)을 순서대로 그린다.
	for (int i = 0; i < 6; i++)
	{
		//스카이 박스의 각 면(사각형)을 그릴 때 사용할 텍스쳐를 설정한다.
		m_pSkyboxTexture->UpdateTextureShaderVariable(pd3dDeviceContext, i, PS_SLOT_TEXTURE);
		pd3dDeviceContext->DrawIndexed(4, 0, i * 4);
	}

	pd3dDeviceContext->OMSetDepthStencilState(nullptr, 1);
}

//**********************************************************************
//CTerrainMesh를 정의합니다.
CHeightMapGridMesh::CHeightMapGridMesh(ID3D11Device *pd3dDevice, int xStart, int zStart, int nWidth, int nLength, D3DXVECTOR3 d3dxvScale, void *pContext) : CDefaultMesh(pd3dDevice)
{
	m_nVertices = nWidth * nLength;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR3 *pd3dxvNormals = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 *pd3dxvTexCoords = new D3DXVECTOR2[m_nVertices];
	//D3DXVECTOR2 *pd3dxvDetailTexCoords = new D3DXVECTOR2[m_nVertices];

	m_nWidth = nWidth;
	m_nLength = nLength;
	m_d3dxvScale = d3dxvScale;

	CHeightMap *pHeightMap = (CHeightMap *)pContext;
	float fHeight = 0.0f, fMinHeight = +FLT_MAX, fMaxHeight = -FLT_MAX;
	int cxHeightMap = pHeightMap->GetHeightMapWidth();
	int czHeightMap = pHeightMap->GetHeightMapLength();

	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			//fHeight = OnGetHeight(x, z, pContext);
			fHeight = pHeightMap->GetHeight(x, z, false);
			m_pd3dxvPositions[i] = D3DXVECTOR3((x*m_d3dxvScale.x), fHeight, (z*m_d3dxvScale.z));
			pd3dxvNormals[i] = pHeightMap->GetHeightMapNormal(x, z);
			pd3dxvTexCoords[i] = D3DXVECTOR2(float(x + (MAP_WIDTH / 2.0f)) / float(cxHeightMap - 1), float(czHeightMap - 1 - z + (MAP_HEIGHT / 2.0f)) / float(czHeightMap - 1));
			//pd3dxvDetailTexCoords[i] = D3DXVECTOR2(float(x) / float(m_d3dxvScale.x*0.5f), float(z) / float(m_d3dxvScale.z*0.5f));

			if (fHeight < fMinHeight) fMinHeight = fHeight;
			if (fHeight > fMaxHeight) fMaxHeight = fHeight;
		}
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR3)* m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR3)* m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2)* m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR2)* m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	
	if (pd3dxvTexCoords) delete[] pd3dxvTexCoords;
	//if (pd3dxvDetailTexCoords) delete[] pd3dxvDetailTexCoords;

	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[3] = { 0, 0, 0 };
	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	/*격자는 사각형들의 집합이고 사각형은 두 개의 삼각형으로 구성되므로 격자는 <그림 18>과 같이 삼각형들의 집합이라고 할 수 있다. 격자를 표현하기 위하여 격자의 삼각형들을 인덱스로 표현해야 한다. 삼각형들은 사각형의 줄 단위로 아래에서 위쪽 방향으로(z-축) 나열한다. 첫 번째 사각형 줄의 삼각형들은 왼쪽에서 오른쪽으로(x-축) 나열한다. 두 번째 줄의 삼각형들은 오른쪽에서 왼쪽 방향으로 나열한다. 즉, 사각형의 줄이 바뀔 때마다 나열 순서가 바뀌도록 한다. <그림 18>의 격자에 대하여 삼각형 스트립을 사용하여 삼각형들을 표현하기 위한 인덱스의 나열은 다음과 같이 격자의 m번째 줄과 (m+1)번째 줄의 정점 번호를 사각형의 나열 방향에 따라 번갈아 아래, 위, 아래, 위, ... 순서로 나열하면 된다.
	0, 6, 1, 7, 2, 8, 3, 9, 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12
	이렇게 인덱스를 나열하면 삼각형 스트립을 사용할 것이므로 실제 그려지는 삼각형들의 인덱스는 다음과 같다.
	(0, 6, 1), (1, 6, 7), (1, 7, 2), (2, 7, 8), (2, 8, 3), (3, 8, 9), ...
	그러나 이러한 인덱스를 사용하면 첫 번째 줄을 제외하고 삼각형들이 제대로 그려지지 않는다. 왜냐하면 삼각형 스트립에서는 마지막 2개의 정점과 새로운 하나의 정점을 사용하여 새로운 삼각형을 그린다. 그리고 홀수 번째 삼각형의 정점 나열 순서(와인딩 순서)는 시계방향이고 짝수 번째 삼각형의 와인딩 순서는 반시계방향이어야 한다. 격자의 사각형이 한 줄에서 몇 개가 있던지 상관없이 한 줄의 마지막 삼각형은 짝수 번째 삼각형이고 와인딩 순서는 반시계 방향이다. 왜냐하면 사각형은 두 개의 삼각형으로 나누어지기 때문이다. 첫 번째 줄에서 두 번째 줄의 인덱스 나열과 실제 그려지는 삼각형들의 인덱스를 살펴보자.
	..., 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
	..., (4, 10, 5), (5, 10, 11), (5, 11, 11), (11, 11, 17), (11, 17, 10), ...
	삼각형 (5, 10, 11)은 첫 번째 줄의 마지막 삼각형이고 짝수 번째이다. 삼각형 (11, 17, 10)은 두 번째 줄의 첫 번째 삼각형이고 홀수 번째이다. 홀수 번째이므로 와인딩 순서가 시계방향이어야 하는데 실제 와인딩 순서는 반시계방향이므로 그려지지 않을 것이다. 당연히 다음 삼각형도 와인딩 순서가 맞지 않으므로 그려지지 않을 것이다. 삼각형 (11, 17, 10)의 와인딩 순서가 반시계방향이므로 그려지도록 하려면 이 삼각형이 짝수 번째 삼각형이 되도록 해야 한다. 이를 위해서 줄이 바뀔 때마다 마지막 정점의 인덱스를 추가하도록 하자. 그러면 줄이 바뀐 첫 번째 삼각형은 짝수 번째 삼각형이 된다. 다음의 예에서는 11이 추가된 마지막 정점의 인덱스이다. 이렇게 하면 삼각형을 구성할 수 없어서 그려지지 않는 삼각형이 각 줄마다 3개씩 생기게 된다.
	..., 4, 10, 5, 11, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
	..., (5, 10, 11), (5, 11, 11), (11, 11, 11), (11, 11, 17), (11, 17, 10), ...
	세 개의 삼각형 (5, 11, 11), (11, 11, 11), (11, 11, 17)은 삼각형을 구성할 수 없으므로 실제로 그려지지 않는다.
	이렇게 인덱스를 나열하면 인덱스 버퍼는 ((nWidth*2)*(nLength-1))+((nLength-1)-1)개의 인덱스를 갖는다. 사각형 줄의 개수는 (nLength-1)이고 한 줄에서 (nWidth*2)개의 인덱스를 갖는다. 그리고 줄이 바뀔 때마다 인덱스를 하나 추가하므로 (nLength-1)-1개의 인덱스가 추가로 필요하다.*/
	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	m_pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//홀수 번째 줄이므로(z = 0, 2, 4, ...) 인덱스의 나열 순서는 왼쪽에서 오른쪽 방향이다.
			for (int x = 0; x < nWidth; x++)
			{
				//첫 번째 줄을 제외하고 줄이 바뀔 때마다(x == 0) 첫 번째 인덱스를 추가한다.
				if ((x == 0) && (z > 0)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래, 위의 순서로 인덱스를 추가한다.
				m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			//짝수 번째 줄이므로(z = 1, 3, 5, ...) 인덱스의 나열 순서는 오른쪽에서 왼쪽 방향이다.
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//줄이 바뀔 때마다(x == (nWidth-1)) 첫 번째 인덱스를 추가한다.
				if (x == (nWidth - 1)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				//아래, 위의 순서로 인덱스를 추가한다.
				m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT)* m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	d3dBufferData.SysMemPitch = sizeof(UINT)* m_nIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(xStart*m_d3dxvScale.x, fMinHeight, zStart*m_d3dxvScale.z);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3((xStart + nWidth)*m_d3dxvScale.x, fMaxHeight, (zStart + nLength)*m_d3dxvScale.z);
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}


float CHeightMapGridMesh::OnGetHeight(int x, int z, void *pContext)
{
	//높이 맵 객체의 높이 맵 이미지의 픽셀 값을 지형의 높이로 반환한다. 
	CHeightMap *pHeightMap = (CHeightMap *)pContext;
	BYTE *pHeightMapImage = pHeightMap->GetHeightMapImage();
	D3DXVECTOR3 d3dxvScale = pHeightMap->GetScale();
	int cxTerrain = pHeightMap->GetHeightMapWidth();
	float fHeight = pHeightMapImage[x + (z*cxTerrain)] * d3dxvScale.y;
	return(fHeight);
}


//**********************************************************************
//CTerrainMesh를 정의합니다.
CTerrainMesh::CTerrainMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight) : CDefaultMesh(pd3dDevice)
{
	m_nVertices = 4;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2* vTexCoord = new D3DXVECTOR2[m_nVertices];
	D3DXVECTOR3* vNormal = new D3DXVECTOR3[m_nVertices];

	m_pd3dxvPositions[0] = D3DXVECTOR3(-fWidth, 0.0f, -fHeight);
	m_pd3dxvPositions[1] = D3DXVECTOR3(-fWidth, 0.0f, +fHeight);
	m_pd3dxvPositions[2] = D3DXVECTOR3(+fWidth, 0.0f, +fHeight);
	m_pd3dxvPositions[3] = D3DXVECTOR3(+fWidth, 0.0f, -fHeight);

	vTexCoord[0] = D3DXVECTOR2(1.0f, 0.0f);
	vTexCoord[1] = D3DXVECTOR2(0.0f, 0.0f);
	vTexCoord[2] = D3DXVECTOR2(0.0f, 1.0f);
	vTexCoord[3] = D3DXVECTOR2(1.0f, 1.0f);

	vNormal[0] = vNormal[1] = vNormal[2] = vNormal[3] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	::ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	::ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR3) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = vTexCoord;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR2) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);
	delete[] vTexCoord;

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3) * m_nVertices;
	d3dBufferData.pSysMem = vNormal;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR3) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);
	delete[] vNormal;


	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[3] = { 0, 0, 0 };
	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	//삼각형 스트립으로 사각형 1개를 그리기 위해 인덱스는 4개가 필요하다.
	m_nIndices = 4;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 2;
	m_pnIndices[1] = 3;
	m_pnIndices[2] = 1;
	m_pnIndices[3] = 0;

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_nIndices;
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pnIndices;
	d3dBufferData.SysMemPitch = sizeof(UINT) * m_nIndices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);


	//항상 보이는 상태이다.
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

}

CTerrainMesh::~CTerrainMesh()
{

}

void CTerrainMesh::Render(ID3D11DeviceContext* pd3dDeviceContext)
{
	CMesh::Render(pd3dDeviceContext);
}


//**********************************************************************
//테셀레이션 지형 CTSTerrainMesh를 정의합니다.

CTSTerrainMesh::CTSTerrainMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 4;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 pd3dxvTexCoords[4];

	float MapSize = 205.0f;//256.0f * 8.0f;
	int i = 0;

	//0	//triangle_cw
	m_pd3dxvPositions[i] = D3DXVECTOR3(-MapSize / 2.0f, 0.0f, +MapSize / 2.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	//1
	m_pd3dxvPositions[i] = D3DXVECTOR3(+MapSize / 2.0f, 0.0f, +MapSize / 2.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	//2
	m_pd3dxvPositions[i] = D3DXVECTOR3(+MapSize / 2.0f, 0.0f, -MapSize / 2.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	//3
	m_pd3dxvPositions[i] = D3DXVECTOR3(-MapSize / 2.0f, 0.0f, -MapSize / 2.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);



	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR3)* m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2) * m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR2) * m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer };
	UINT pnBufferStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);


	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-MapSize / 2.0f, -MapSize / 2.0f, -MapSize / 2.0f);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+MapSize / 2.0f, +MapSize / 2.0f, +MapSize / 2.0f);


}


CTSTerrainMesh::~CTSTerrainMesh()
{
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
}


//**********************************************************************
//CDefaultMesh    -> 일단 normal, texCoord 추가
CDefaultMesh::CDefaultMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_pd3dNormalBuffer = nullptr;
	m_pd3dTexCoordBuffer = nullptr;
}

CDefaultMesh::~CDefaultMesh()
{
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
	if (m_pd3dTexCoordBuffer) m_pd3dTexCoordBuffer->Release();
}

//**********************************************************************
//CCubeMesh -> 테스트용 큐브
CCubeMesh::CCubeMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 36;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = 0.5f, fy = 0.5f, fz = 0.5f;
	/*
	m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	D3DXVECTOR2 pd3dxvTexCoords[36];
	D3DXVECTOR3 pd3dxvNormals[36];
	int i = 0;

	//직육면체의 각 면(삼각형 2개)에 하나의 텍스쳐 이미지 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f); 
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, +fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(-fx, -fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);

	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, +fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 0.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, +fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(1.0f, 1.0f);
	m_pd3dxvPositions[i] = D3DXVECTOR3(+fx, -fy, -fz);
	pd3dxvNormals[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pd3dxvTexCoords[i++] = D3DXVECTOR2(0.0f, 1.0f);

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pd3dxvPositions;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR3)* m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR2)* m_nVertices;
	d3dBufferData.pSysMem = pd3dxvTexCoords;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR2)* m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTexCoordBuffer);

	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR3)* m_nVertices;
	d3dBufferData.pSysMem = pd3dxvNormals;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR3)* m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	ID3D11Buffer *pd3dBuffers[3] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[3] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[3] = { 0, 0, 0 };
	AssembleToVertexBuffer(3, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
	*/
	
	D3DXVECTOR4 position[36];
	//m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];

	int i = 0;

	//직육면체의 각 면(삼각형 2개)에 하나의 텍스쳐 이미지 전체가 맵핑되도록 텍스쳐 좌표를 설정한다.
	position[i++] = D3DXVECTOR4(-fx, +fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, +fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, -fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(-fx, +fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, -fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(-fx, -fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(-fx, +fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, +fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, +fy, -fz, 1.0f);	
	position[i++] = D3DXVECTOR4(-fx, +fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, +fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(-fx, +fy, -fz, 1.0f);	
	position[i++] = D3DXVECTOR4(-fx, -fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, -fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, +fy, +fz, 1.0f);	
	position[i++] = D3DXVECTOR4(-fx, -fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, +fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(-fx, +fy, +fz, 1.0f);	
	position[i++] = D3DXVECTOR4(-fx, -fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, -fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, -fy, +fz, 1.0f);	
	position[i++] = D3DXVECTOR4(-fx, -fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, -fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(-fx, -fy, +fz, 1.0f);	
	position[i++] = D3DXVECTOR4(-fx, +fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(-fx, +fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(-fx, -fy, -fz, 1.0f);	
	position[i++] = D3DXVECTOR4(-fx, +fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(-fx, -fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(-fx, -fy, +fz, 1.0f);	
	position[i++] = D3DXVECTOR4(+fx, +fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, +fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, -fy, +fz, 1.0f);	
	position[i++] = D3DXVECTOR4(+fx, +fy, -fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, -fy, +fz, 1.0f);
	position[i++] = D3DXVECTOR4(+fx, -fy, -fz, 1.0f);
	
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR4)* m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = position;
	d3dBufferData.SysMemPitch = sizeof(D3DXVECTOR4)* m_nVertices;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);
	

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(D3DXVECTOR4) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-fx, -fy, -fz);
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3(+fx, +fy, +fz);
}

CCubeMesh::~CCubeMesh()
{

}

//**********************************************************************
//CStaticMesh
CStaticMesh::CStaticMesh(ID3D11Device* pd3dDevice) : CDefaultMesh(pd3dDevice)
{
	m_pd3dTangentBuffer = nullptr;
}

CStaticMesh::~CStaticMesh()
{
	if (m_pd3dTangentBuffer) m_pd3dTangentBuffer->Release();
}

void CStaticMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	CDefaultMesh::CreateRasterizerState(pd3dDevice);

	/*
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;

	pd3dDevice->CreateRasterizerState(&rd, &m_pd3dRasterizerState);
	*/
}

void CStaticMesh::Render(ID3D11DeviceContext* pd3dDeviceContext)
{
	CDefaultMesh::Render(pd3dDeviceContext);
}

//그림자 그려내기 위한
void CStaticMesh::RenderInstancedShadow(ID3D11DeviceContext *pd3dDeviceContext, ID3D11Buffer* pd3dInstanceBuffer, int nInstances, int nStartInstance)
{
	ID3D11Buffer*	ppVertexBuffers[2];
	UINT			pnVertexStrides[2] = { sizeof(D3DXVECTOR3), sizeof(D3DXMATRIX) };
	UINT			pnVertexOffsets[2] = { 0, 0 };

	ppVertexBuffers[0] = m_pd3dPositionBuffer;
	ppVertexBuffers[1] = pd3dInstanceBuffer;

	pd3dDeviceContext->IASetVertexBuffers(0, 2, ppVertexBuffers, pnVertexStrides, pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	//객체들의 인스턴스들을 렌더링한다. 
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}


void CStaticMesh::LoadStaticObjectFromFBX(const string& fileName, D3DXVECTOR3 vScale, ID3D11Device* pd3dDevice, D3DXVECTOR3 offset, float aabbSize)
{
	ifstream fin(fileName);

	if (!fin.is_open()) return;

	int meshCount;
	int vertexCount, indexCount, boneCount, animationCount;

	D3DXVECTOR3 pos, normal;
	D3DXVECTOR4 boneWeight;
	UINT4 boneIndex;
	D3DXVECTOR2 uv;
	UINT index;

	vector<D3DXVECTOR3> vPos;
	vector<D3DXVECTOR2> vTexCoord;
	vector<D3DXVECTOR3> vNormal;
	vector<UINT4> vBoneIndex;
	vector<D3DXVECTOR4> vBoneWeight;
	vector<UINT> vIndex;

	/*
	D3DXMATRIX mtxScale;
	D3DXMATRIX mtxRotate;
	mtxScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	mtxRotate = XMMatrixRotationRollPitchYaw((float)XMConvertToRadians(-0.0f), (float)XMConvertToRadians(-0.0f), (float)XMConvertToRadians(-0.0f));
	XMStoreFloat4x4(&m_xm4x4Local, mtxScale * mtxRotate);
	*/

	D3DXMATRIX mtxScaleDx;
	D3DXMATRIX mtxRotateDx;
	D3DXMatrixScaling(&mtxScaleDx, vScale.x, vScale.y, vScale.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRotateDx, (float)D3DXToRadian(+30.0f), (float)D3DXToRadian(-0.0f), (float)D3DXToRadian(-0.0f));
	m_d3dxmtxLocal = mtxScaleDx * mtxRotateDx;


	string buf;

	while (!fin.eof()) {
		fin >> buf;		//[FBX_META_DATA]
						// FBX Meta Data
		if (buf == "MeshCount:")
			fin >> meshCount;
		// Mesh Data
		else if (buf == "VertexCount:") {
			fin >> vertexCount;
			vPos.reserve(vertexCount);
			vNormal.reserve(vertexCount);
			vTexCoord.reserve(vertexCount);
			vBoneIndex.reserve(vertexCount);
			vBoneWeight.reserve(vertexCount);
		}
		else if (buf == "IndexCount:") {
			fin >> indexCount;
			vIndex.reserve(indexCount);
		}

		// Vertex Data
		else if (buf == "[VERTEX_DATA]") {
			for (int i = 0; i < vertexCount; ++i) {
				fin >> buf;      // Position:
				fin >> pos.x >> pos.y >> pos.z;
				fin >> buf;      // Normal: 
				fin >> normal.x >> normal.y >> normal.z;
				fin >> buf;      // UV: 
				fin >> uv.x >> uv.y;
				
				pos.x *= vScale.x; pos.y *= vScale.y; pos.z *= vScale.z;
				vPos.push_back(pos);
				vNormal.push_back(normal);
				vTexCoord.push_back(uv);
			}
		}
		// Index Data
		else if (buf == "[INDEX_DATA]") {
			for (int i = 0; i < indexCount; ++i) {
				fin >> index;
				vIndex.push_back(index);
			}
			break;
		}
	}

	
	fin.close();

	//원래는 피킹처리를 위해 정점과 인덱스의 정보를 가지고 있지만
	//지형이 아닌 경우는 위치와 크기를 안다면 바운딩박스를 통해 피킹이 가능하므로 굳이 저장하지 않는다.
	m_nVertices = vertexCount;
	//m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	//for (auto i = 0; i < m_nVertices; ++i)
	//	m_pd3dxvPositions[i] = D3DXVECTOR3(vPos[i].x, vPos[i].y, vPos[i].z);

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nIndices = indexCount;
	//m_pnIndices = new UINT[m_nIndices];
	//for (auto i = 0; i < m_nIndices; ++i)
	//	m_pnIndices[i] = vIndex[i];

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, &(vPos[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, &(vTexCoord[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, &(vNormal[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	
	m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, &(vIndex[0]), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	///////////////////////
	//1214 노말매핑
	//여기서부터 탄젠트 값 얻는 코드
	int idx0, idx1, idx2;
	vector<D3DXVECTOR3> vTangent;
	D3DXVECTOR3 edge0, edge1;
	D3DXVECTOR2 tex0, tex1, tex2;
	float dU0, dU1, dV0, dV1;
	vTangent.resize(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
		vTangent[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	int denoZero = 0;

	for (int i = 0; i < vertexCount; ++i)
	{
		for (int j = 0; j < indexCount; j += 3)
		{
			idx0 = vIndex[j + 0]; idx1 = vIndex[j + 1]; idx2 = vIndex[j + 2];

			if (i == idx0 || i == idx1 || i == idx2) //프리미티브에 속하는 정점이라면
			{
				//이 세 개의 정점으로부터 T 벡터를 구하고 더해놓는다.
				if (i == idx0)
				{
					edge0 = vPos[idx1] - vPos[idx0];
					edge1 = vPos[idx2] - vPos[idx0];

					tex0 = vTexCoord[idx0];
					tex1 = vTexCoord[idx1];
					tex2 = vTexCoord[idx2];
				}
				else if (i == idx1)
				{
					edge0 = vPos[idx2] - vPos[idx1];
					edge1 = vPos[idx0] - vPos[idx1];

					tex0 = vTexCoord[idx1];
					tex1 = vTexCoord[idx2];
					tex2 = vTexCoord[idx0];
				}
				else if (i == idx2)
				{
					edge0 = vPos[idx0] - vPos[idx2];
					edge1 = vPos[idx1] - vPos[idx2];

					tex0 = vTexCoord[idx2];
					tex1 = vTexCoord[idx0];
					tex2 = vTexCoord[idx1];
				}

				dU0 = tex1.x - tex0.x;
				dV0 = tex1.y - tex0.y;
				dU1 = tex2.x - tex0.x;
				dV1 = tex2.y - tex0.y;

				float denominator = abs(dU0*dV1 - dU1*dV0);
				if (denominator == 0.0f)
				{
					denominator = 1.0f;
					//cout << "Zero Denominator!!\n";
				}
				float adj = 1.0f / denominator;


				D3DXVECTOR3 tangent = adj * ((dV1*edge0) + (-dV0 * edge1));
				vTangent[i] += tangent;
			}
		}
	}
	for (int i = 0; i < vertexCount; ++i)
		D3DXVec3Normalize(&vTangent[i], &vTangent[i]);

	m_pd3dTangentBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, &(vTangent[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	if (nullptr == m_pd3dTangentBuffer)
		printf("탄젠트 버퍼 안만들어짐!\n");
	//노말매핑
	////////////

	ID3D11Buffer *pd3dBuffers[4] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer, m_pd3dNormalBuffer, m_pd3dTangentBuffer};
	UINT pnBufferStrides[4] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[4] = { 0, 0, 0, 0 };
	AssembleToVertexBuffer(4, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	D3DXCOLOR matDif = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	D3DXCOLOR matAmb = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	D3DXCOLOR matSpe = D3DXCOLOR(0.6f, 0.6f, 0.6f, 2.0f);
	D3DXCOLOR matEmi = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

	MATERIAL mat{ matAmb, matDif, matEmi, matSpe };
	m_meshMaterial = mat;

	//충돌 체크를 위한 바운딩 박스이다.
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-(aabbSize / 2.0f), -(aabbSize / 2.0f), -(aabbSize / 2.0f));
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3((aabbSize / 2.0f), (aabbSize / 2.0f), (aabbSize / 2.0f));
}


CStaticMesh* CStaticMesh::CreateAndCopyMeshData()
{
	CStaticMesh* pMesh = new CStaticMesh(nullptr);	//굳이 디바이스 안줘도 됌

	pMesh->m_d3dxmtxLocal = m_d3dxmtxLocal;

	pMesh->m_nVertices = m_nVertices;
	pMesh->m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pMesh->m_nIndices = m_nIndices;

	pMesh->m_pd3dPositionBuffer = m_pd3dPositionBuffer; m_pd3dPositionBuffer->AddRef();
	pMesh->m_pd3dTexCoordBuffer = m_pd3dTexCoordBuffer; m_pd3dTexCoordBuffer->AddRef();
	pMesh->m_pd3dNormalBuffer = m_pd3dNormalBuffer; m_pd3dNormalBuffer->AddRef();

	pMesh->m_pd3dIndexBuffer = m_pd3dIndexBuffer; m_pd3dIndexBuffer->AddRef();

	//1214 노말매핑을위한 탄젠트
	pMesh->m_pd3dTangentBuffer = m_pd3dTangentBuffer; m_pd3dTangentBuffer->AddRef();

	ID3D11Buffer *pd3dBuffers[4] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer, m_pd3dNormalBuffer, m_pd3dTangentBuffer };
	UINT pnBufferStrides[4] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[4] = { 0, 0, 0, 0 };
	pMesh->AssembleToVertexBuffer(4, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	pMesh->m_meshMaterial = m_meshMaterial;

	//충돌 체크를 위한 바운딩 박스이다.
	pMesh->m_bcBoundingCube.m_d3dxvMinimum = m_bcBoundingCube.m_d3dxvMinimum;
	pMesh->m_bcBoundingCube.m_d3dxvMaximum = m_bcBoundingCube.m_d3dxvMaximum;

	pMesh->AddRef();
	return pMesh;
}

//**********************************************************************
//CAnimationMesh		->		BoneID, BoneWeight
CAnimationMesh::CAnimationMesh(ID3D11Device* pd3dDevice) : CDefaultMesh(pd3dDevice)
{
	m_pSkinnedData = nullptr;
	m_pd3dBoneIDBuffer = nullptr;
	m_pd3dWeightBuffer = nullptr;
	m_pd3dTangentBuffer = nullptr;
}

CAnimationMesh::~CAnimationMesh()
{
	if (m_pd3dBoneIDBuffer) m_pd3dBoneIDBuffer->Release();
	if (m_pd3dWeightBuffer) m_pd3dWeightBuffer->Release();
	if (m_pd3dTangentBuffer) m_pd3dTangentBuffer->Release();

	if (m_pSkinnedData) m_pSkinnedData->Release();
}

void CAnimationMesh::CreateRasterizerState(ID3D11Device *pd3dDevice)
{
	CDefaultMesh::CreateRasterizerState(pd3dDevice);
}

void CAnimationMesh::Render(ID3D11DeviceContext* pd3dDeviceContext)
{
	CDefaultMesh::Render(pd3dDeviceContext);
}

//그림자 그려내기 위한
void CAnimationMesh::RenderInstancedShadow(ID3D11DeviceContext *pd3dDeviceContext, ID3D11Buffer* pd3dInstanceBuffer, int nInstances, int nStartInstance)
{
	ID3D11Buffer*	ppVertexBuffers[4];
	UINT			pnVertexStrides[4] = { sizeof(D3DXVECTOR3), sizeof(UINT) * 4, sizeof(D3DXVECTOR4), sizeof(D3DXMATRIX) + sizeof(UINT) };
	UINT			pnVertexOffsets[4] = { 0, 0, 0, 0 };

	ppVertexBuffers[0] = m_pd3dPositionBuffer;
	ppVertexBuffers[1] = m_pd3dBoneIDBuffer;
	ppVertexBuffers[2] = m_pd3dWeightBuffer;
	ppVertexBuffers[3] = pd3dInstanceBuffer;

	pd3dDeviceContext->IASetVertexBuffers(0, 4, ppVertexBuffers, pnVertexStrides, pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	//객체들의 인스턴스들을 렌더링한다. 
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}

void CAnimationMesh::LoadAnimateObjectFromFBX(const string& fileName, D3DXVECTOR3 vScale, ID3D11Device* pd3dDevice, D3DXVECTOR3 offset, float aabbSize)
{
	ifstream fin(fileName);

	if (!fin.is_open()) return;

	int meshCount;
	int vertexCount, indexCount, boneCount, animationCount;

	D3DXVECTOR3 pos, normal;
	D3DXVECTOR4 boneWeight;
	UINT4 boneIndex;
	D3DXVECTOR2 uv;
	UINT index;
	int parentIndex;
	D3DXMATRIX offsetMatrix;

	string clipName;
	AnimationClip clip;

	vector<D3DXVECTOR3> vPos;
	vector<D3DXVECTOR2> vTexCoord;
	vector<D3DXVECTOR3> vNormal;
	vector<UINT4> vBoneIndex;
	vector<D3DXVECTOR4> vBoneWeight;
	vector<UINT> vIndex;
	vector<int> vBoneHierarchy;
	vector<D3DXMATRIX> vOffsetMatrix;
	map<string, AnimationClip> mapAnimationClip;	//4개일것이다... 한 클립에 4 동작 넣어놓은 상태임

	
	/*
	D3DXMATRIX mtxScale;
	D3DXMATRIX mtxRotate;
	D3DXMATRIX mtxTranslate;
	mtxScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	mtxRotate = XMMatrixRotationRollPitchYaw((float)XMConvertToRadians(-0.0f), (float)XMConvertToRadians(-0.0f), (float)XMConvertToRadians(-0.0f));
	mtxTranslate = XMMatrixTranslation(offset.x, offset.y, offset.z);
	XMStoreFloat4x4(&m_xm4x4Local, mtxScale * mtxRotate * mtxTranslate);
	*/

	D3DXMATRIX mtxScaleDx;
	D3DXMATRIX mtxRotateDx;
	D3DXMATRIX mtxTranslateDx;
	D3DXMatrixScaling(&mtxScaleDx, vScale.x, vScale.y, vScale.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRotateDx, (float)D3DXToRadian(-0.0f), (float)D3DXToRadian(-0.0f), (float)D3DXToRadian(-0.0f));
	D3DXMatrixTranslation(&mtxTranslateDx, offset.x, offset.y, offset.z);
	m_d3dxmtxLocal = mtxScaleDx * mtxRotateDx * mtxTranslateDx;

	string buf;

	while (!fin.eof()) {
		fin >> buf;		//[FBX_META_DATA]
						// FBX Meta Data
		if (buf == "MeshCount:")
			fin >> meshCount;
		// Mesh Data
		else if (buf == "VertexCount:") {
			fin >> vertexCount;
			vPos.reserve(vertexCount);
			vNormal.reserve(vertexCount);
			vTexCoord.reserve(vertexCount);
			vBoneIndex.reserve(vertexCount);
			vBoneWeight.reserve(vertexCount);
		}
		else if (buf == "IndexCount:") {
			fin >> indexCount;
			vIndex.reserve(indexCount);
		}
		else if (buf == "BoneCount:")
		{
			fin >> boneCount;
			vBoneHierarchy.reserve(boneCount);
			vOffsetMatrix.reserve(boneCount);
		}
		else if (buf == "AnimationClips:")
			fin >> animationCount;

		// Vertex Data
		else if (buf == "[VERTEX_DATA]") {
			for (int i = 0; i < vertexCount; ++i) {
				fin >> buf;      // Position:
				fin >> pos.x >> pos.y >> pos.z;
				//fin >> pos.x >> pos.z >> pos.y;
				fin >> buf;      // Normal: 
				fin >> normal.x >> normal.y >> normal.z;
				fin >> buf;      // UV: 
				fin >> uv.x >> uv.y;
				fin >> buf;		// BoneIndices:
				fin >> boneIndex.x >> boneIndex.y >> boneIndex.z >> boneIndex.w;
				fin >> buf;		// BoneWeights:
				fin >> boneWeight.x >> boneWeight.y >> boneWeight.z >> boneWeight.w;

				vPos.push_back(pos);
				vNormal.push_back(normal);
				vTexCoord.push_back(uv);
				vBoneIndex.push_back(boneIndex);
				vBoneWeight.push_back(boneWeight);
			}
		}
		// Index Data
		else if (buf == "[INDEX_DATA]") {
			for (int i = 0; i < indexCount; ++i) {
				fin >> index;
				vIndex.push_back(index);
			}
		}

		else if (buf == "[BONE_HIERARCHY]")
		{
			for (int i = 0; i < boneCount; ++i)
			{
				fin >> buf;	// "ParentIndexOfBoneN: "
				fin >> parentIndex;
				vBoneHierarchy.push_back(parentIndex);
			}
		}

		else if (buf == "[OFFSET_MATRIX]")
		{
			for (int i = 0; i < boneCount; ++i)
			{
				fin >> buf; // "BoneOffSetN: "
				fin >> offsetMatrix._11 >> offsetMatrix._12 >> offsetMatrix._13 >> offsetMatrix._14 >>
					offsetMatrix._21 >> offsetMatrix._22 >> offsetMatrix._23 >> offsetMatrix._24 >>
					offsetMatrix._31 >> offsetMatrix._32 >> offsetMatrix._33 >> offsetMatrix._34 >>
					offsetMatrix._41 >> offsetMatrix._42 >> offsetMatrix._43 >> offsetMatrix._44;
				vOffsetMatrix.push_back(offsetMatrix);
			}
		}
		else if (buf == "[ANIMATION_CLIPS]")
		{

			for (int nClip = 0; nClip < animationCount; ++nClip)
			{
				fin >> buf; // "AnimationClip "
				fin >> clipName; //애니메이션 이름 들어감
				fin >> buf; // "{ " clip 시작 중괄호

				int nKeyFrames = 0;
				for (int i = 0; i < boneCount; ++i)
				{
					fin >> buf; // "Bone "
					fin >> buf; // "N: " 본 번호가 있는데 애초에 순서대로 읽으므로 쓸 필요 X
					fin >> nKeyFrames;

					BoneAnimation boneAnim;
					boneAnim.Keyframes.resize(nKeyFrames);

					fin >> buf; // "{ " KeyFrame 나열 시작 괄호
					for (int j = 0; j < nKeyFrames; ++j)
					{
						fin >> buf; // "Time: "
						fin >> boneAnim.Keyframes[j].TimePos;
						fin >> buf; // "T: "
						fin >> boneAnim.Keyframes[j].Translation.x >> boneAnim.Keyframes[j].Translation.y >> boneAnim.Keyframes[j].Translation.z;
						fin >> buf; // "S: "
						fin >> boneAnim.Keyframes[j].Scale.x >> boneAnim.Keyframes[j].Scale.y >> boneAnim.Keyframes[j].Scale.z;
						fin >> buf; // "Q: "
						fin >> boneAnim.Keyframes[j].RotationQuat.x >> boneAnim.Keyframes[j].RotationQuat.y >> boneAnim.Keyframes[j].RotationQuat.z >> boneAnim.Keyframes[j].RotationQuat.w;
					}
					fin >> buf; // "} " KeyFrame 나열 끝 괄호	
					clip.BoneAnimations.push_back(boneAnim);
				}
				fin >> buf; // "} " clip 끝 중괄호

				mapAnimationClip.insert(make_pair(clipName, clip));
			}
			fin.close(); //로드 끝!

			m_pSkinnedData = new SkinnedData();
			m_pSkinnedData->AddRef();
			m_pSkinnedData->Set(vBoneHierarchy, vOffsetMatrix, mapAnimationClip);
		}
	}


	//원래는 피킹처리를 위해 정점과 인덱스의 정보를 가지고 있지만
	//지형이 아닌 경우는 위치와 크기를 안다면 바운딩박스를 통해 피킹이 가능하므로 굳이 저장하지 않는다.
	m_nVertices = vertexCount;
	//m_pd3dxvPositions = new D3DXVECTOR3[m_nVertices];
	//for (auto i = 0; i < m_nVertices; ++i)
	//	m_pd3dxvPositions[i] = D3DXVECTOR3(vPos[i].x, vPos[i].y, vPos[i].z);

	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_nIndices = indexCount;
	//m_pnIndices = new UINT[m_nIndices];
	//for (auto i = 0; i < m_nIndices; ++i)
	//	m_pnIndices[i] = vIndex[i];

	m_pd3dPositionBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, &(vPos[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dTexCoordBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR2), m_nVertices, &(vTexCoord[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dNormalBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, &(vNormal[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dBoneIDBuffer = CreateBuffer(pd3dDevice, sizeof(UINT4), m_nVertices, &(vBoneIndex[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	m_pd3dWeightBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR4), m_nVertices, &(vBoneWeight[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	m_pd3dIndexBuffer = CreateBuffer(pd3dDevice, sizeof(UINT), m_nIndices, &(vIndex[0]), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT, 0);

	///////////////////////
	//1214 노말매핑
	//여기서부터 탄젠트 값 얻는 코드
	int idx0, idx1, idx2;
	vector<D3DXVECTOR3> vTangent;
	D3DXVECTOR3 edge0, edge1;
	D3DXVECTOR2 tex0, tex1, tex2;
	float dU0, dU1, dV0, dV1;
	vTangent.resize(vertexCount);
	for (int i = 0; i < vertexCount; ++i)
		vTangent[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	int denoZero = 0;

	for (int i = 0; i < vertexCount; ++i)
	{
		for (int j = 0; j < indexCount; j += 3)
		{
			idx0 = vIndex[j + 0]; idx1 = vIndex[j + 1]; idx2 = vIndex[j + 2];

			if (i == idx0 || i == idx1 || i == idx2) //프리미티브에 속하는 정점이라면
			{
				//이 세 개의 정점으로부터 T 벡터를 구하고 더해놓는다.
				if (i == idx0)
				{
					edge0 = vPos[idx1] - vPos[idx0];
					edge1 = vPos[idx2] - vPos[idx0];

					tex0 = vTexCoord[idx0];
					tex1 = vTexCoord[idx1];
					tex2 = vTexCoord[idx2];
				}
				else if (i == idx1)
				{
					edge0 = vPos[idx2] - vPos[idx1];
					edge1 = vPos[idx0] - vPos[idx1];

					tex0 = vTexCoord[idx1];
					tex1 = vTexCoord[idx2];
					tex2 = vTexCoord[idx0];
				}
				else if (i == idx2)
				{
					edge0 = vPos[idx0] - vPos[idx2];
					edge1 = vPos[idx1] - vPos[idx2];

					tex0 = vTexCoord[idx2];
					tex1 = vTexCoord[idx0];
					tex2 = vTexCoord[idx1];
				}

				dU0 = tex1.x - tex0.x;
				dV0 = tex1.y - tex0.y;
				dU1 = tex2.x - tex0.x;
				dV1 = tex2.y - tex0.y;

				float denominator = abs(dU0*dV1 - dU1*dV0);
				if (denominator == 0.0f)
				{
					denominator = 1.0f;
					//cout << "Zero Denominator!!\n";
				}
				float adj = 1.0f / denominator;


				D3DXVECTOR3 tangent = adj * ((dV1*edge0) + (-dV0 * edge1));
				vTangent[i] += tangent;
			}
		}
	}
	for(int i = 0; i < vertexCount; ++i)
		D3DXVec3Normalize(&vTangent[i], &vTangent[i]);

	m_pd3dTangentBuffer = CreateBuffer(pd3dDevice, sizeof(D3DXVECTOR3), m_nVertices, &(vTangent[0]), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	if (nullptr == m_pd3dTangentBuffer)
		printf("탄젠트 버퍼 안만들어짐!\n");
	//노말매핑
	////////////

	
	ID3D11Buffer *pd3dBuffers[6] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer, m_pd3dNormalBuffer, m_pd3dBoneIDBuffer, m_pd3dWeightBuffer, m_pd3dTangentBuffer };
	UINT pnBufferStrides[6] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR3), sizeof(UINT4), sizeof(D3DXVECTOR4) ,sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[6] = { 0, 0, 0, 0, 0, 0 };
	AssembleToVertexBuffer(6, pd3dBuffers, pnBufferStrides, pnBufferOffsets);


	D3DXCOLOR matDif = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	D3DXCOLOR matAmb = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	D3DXCOLOR matSpe = D3DXCOLOR(0.6f, 0.6f, 0.6f, 2.0f);
	D3DXCOLOR matEmi = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

	MATERIAL mat{ matAmb, matDif, matEmi, matSpe };
	m_meshMaterial = mat;

	//좀 더 제대로된 값으로...
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-(aabbSize / 2.0f), -(aabbSize / 2.0f), -(aabbSize / 2.0f));
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3((aabbSize / 2.0f), (aabbSize / 2.0f), (aabbSize / 2.0f));
}

CAnimationMesh* CAnimationMesh::CreateAndCopyMeshData()
{
	CAnimationMesh* pMesh = new CAnimationMesh(nullptr);	//굳이 디바이스 안줘도 됌

	pMesh->m_d3dxmtxLocal = m_d3dxmtxLocal;

	pMesh->m_nVertices = m_nVertices;
	pMesh->m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pMesh->m_nIndices = m_nIndices;

	pMesh->m_pSkinnedData = m_pSkinnedData;
	pMesh->m_pSkinnedData->AddRef();

	pMesh->m_pd3dPositionBuffer = m_pd3dPositionBuffer; m_pd3dPositionBuffer->AddRef();
	pMesh->m_pd3dTexCoordBuffer = m_pd3dTexCoordBuffer; m_pd3dTexCoordBuffer->AddRef();
	pMesh->m_pd3dNormalBuffer = m_pd3dNormalBuffer; m_pd3dNormalBuffer->AddRef();
	pMesh->m_pd3dBoneIDBuffer = m_pd3dBoneIDBuffer; m_pd3dBoneIDBuffer->AddRef();
	pMesh->m_pd3dWeightBuffer = m_pd3dWeightBuffer;  m_pd3dWeightBuffer->AddRef();

	pMesh->m_pd3dIndexBuffer = m_pd3dIndexBuffer; m_pd3dIndexBuffer->AddRef();

	//1214 노말매핑을위한 탄젠트
	pMesh->m_pd3dTangentBuffer = m_pd3dTangentBuffer; m_pd3dTangentBuffer->AddRef();

	ID3D11Buffer *pd3dBuffers[6] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer, m_pd3dNormalBuffer, m_pd3dBoneIDBuffer, m_pd3dWeightBuffer, m_pd3dTangentBuffer };
	UINT pnBufferStrides[6] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR3), sizeof(UINT4), sizeof(D3DXVECTOR4), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[6] = { 0, 0, 0, 0, 0, 0 };
	pMesh->AssembleToVertexBuffer(6, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	pMesh->m_meshMaterial = m_meshMaterial;

	//충돌 체크를 위한 바운딩 박스이다.
	pMesh->m_bcBoundingCube.m_d3dxvMinimum = m_bcBoundingCube.m_d3dxvMinimum;
	pMesh->m_bcBoundingCube.m_d3dxvMaximum = m_bcBoundingCube.m_d3dxvMaximum;

	//pMesh->AddRef();
	return pMesh;
}