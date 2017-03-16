#include "stdafx.h"
#include "Mesh.h"


#include <fstream>

//**********************************************************************
//�ؽ�ó Ŭ������ �Լ����� ����
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
//Material Ŭ������ �Լ����� ����

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
//AABB �浹 Ŭ�������� �Լ�

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
	/*�ٿ�� �ڽ��� �ּ����� �ִ����� ȸ���� �ϸ� �� �̻� �ּ����� �ִ����� ���� �ʴ´�. 
	�׷��Ƿ� �ٿ�� �ڽ��� �ּ����� �ִ������� 8���� ������ ���ϰ� ��ȯ(ȸ��)�� �� ����
	�ּ����� �ִ����� �ٽ� ����Ѵ�.*/
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
	//8���� �������� x, y, z ��ǥ�� �ּҰ��� �ִ밪�� ���Ѵ�.
	for (int i = 0; i < 8; i++)
	{
		//������ ��ȯ�Ѵ�.
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
//�޽� ��ŷ�� ���� ó��
int CMesh::CheckRayIntersection(D3DXVECTOR3 *pd3dxvRayPosition, D3DXVECTOR3 *pd3dxvRayDirection, MESHINTERSECTINFO *pd3dxIntersectInfo)
{
	//�� ��ǥ���� ������ ������(pd3dxvRayPosition)�� ������ �־��� �� �޽����� �浹 �˻縦 �Ѵ�.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE*)m_pd3dxvPositions + m_pnVertexOffsets[0];
	//Offsets -> ������ ��ҵ��� ��Ÿ���� ���۵��� ���� ��ġ(����Ʈ ��)�� ��Ÿ���� �迭�̴�

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	//�޽��� ������Ƽ��(�ﰢ��)���� �����̴�. �ﰢ�� ����Ʈ�� ��� (������ ���� / 3) �Ǵ� (�ε����� ���� / 3), �ﰢ�� ��Ʈ���� ��� (������ ���� - 2) �Ǵ� (�ε����� ���� - 2)�̴�.
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	D3DXVECTOR3 v0, v1, v2;
	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;
	/*�޽��� ��� ������Ƽ��(�ﰢ��)�鿡 ���Ͽ� ��ŷ �������� �浹�� �˻��Ѵ�. �浹�ϴ� ��� �ﰢ���� ã�� ������ ������(�����δ� ī�޶� ��ǥ���� ����)�� ���� ����� �ﰢ���� ã�´�.*/
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_pnVertexStrides[0]);
		v1 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_pnVertexStrides[0]);
		v2 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_pnVertexStrides[0]);
		if (D3DXIntersectTri(&v0, &v1, &v2, pd3dxvRayPosition, pd3dxvRayDirection, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		{	//u,v,dist ���� �ﰢ���� ���� ������,������ ���ؼ� ��´�.
			//�Լ��� ������ true -> �ﰢ���� ������ ������ �����Ѵ�.
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
	//�� ��ǥ���� ������ ������(pd3dxvRayPosition)�� ������ �־��� �� �޽����� �浹 �˻縦 �Ѵ�.
	int nIntersections = 0;
	BYTE *pbPositions = (BYTE*)m_pd3dxvPositions + m_pnVertexOffsets[0];
	//Offsets -> ������ ��ҵ��� ��Ÿ���� ���۵��� ���� ��ġ(����Ʈ ��)�� ��Ÿ���� �迭�̴�

	int nOffset = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	//�޽��� ������Ƽ��(�ﰢ��)���� �����̴�. �ﰢ�� ����Ʈ�� ��� (������ ���� / 3) �Ǵ� (�ε����� ���� / 3), �ﰢ�� ��Ʈ���� ��� (������ ���� - 2) �Ǵ� (�ε����� ���� - 2)�̴�.
	int nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) nPrimitives = (m_d3dPrimitiveTopology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);

	D3DXVECTOR3 v0, v1, v2;
	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;
	/*�޽��� ��� ������Ƽ��(�ﰢ��)�鿡 ���Ͽ� ��ŷ �������� �浹�� �˻��Ѵ�. �浹�ϴ� ��� �ﰢ���� ã�� ������ ������(�����δ� ī�޶� ��ǥ���� ����)�� ���� ����� �ﰢ���� ã�´�.*/
	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 0]) : ((i*nOffset) + 0)) * m_pnVertexStrides[0]);
		v1 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 1]) : ((i*nOffset) + 1)) * m_pnVertexStrides[0]);
		v2 = *(D3DXVECTOR3 *)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i*nOffset) + 2]) : ((i*nOffset) + 2)) * m_pnVertexStrides[0]);
		if (D3DXIntersectTri(&v0, &v1, &v2, pd3dxvRayPosition, pd3dxvRayDirection, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		{	//u,v,dist ���� �ﰢ���� ���� ������,������ ���ؼ� ��´�.
			//�Լ��� ������ true -> �ﰢ���� ������ ������ �����Ѵ�.
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
		//u�� v1, v�� v2, 1 -  (u+v) �� v0�� �����߽��̴�.
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

	m_nReferences = 0;	//ó�� �޽� ���� �� SetMesh ���� ����
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
//������ �پ��� ������ �ִ� ���, ������ �����ͷ� �����ϱ� ����
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
//�⺻ �θ�Ŭ������ CMesh�� ������
void CMesh::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if (m_ppd3dVertexBuffers)
	{
		pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
		pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
		pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
		//pd3dDeviceContext->RSSetState(m_pd3dRasterizerState); //RS�� ������Ʈ�� �����ϴ°ɷ�... Ȥ�� ���̴��� ����

		if (m_pd3dIndexBuffer)
			pd3dDeviceContext->DrawIndexed(m_nIndices, m_nStartIndex, m_nBaseVertex);
		else
			pd3dDeviceContext->Draw(m_nVertices, m_nStartVertex);
	}
}

void CMesh::RenderInstanced(ID3D11DeviceContext *pd3dDeviceContext, int nInstances, int nStartInstance)
{
	//�ν��Ͻ��� ��� �Է� �����⿡ �޽��� ���� ���ۿ� �ν��Ͻ� ���� ���۰� ����ȴ�.
	pd3dDeviceContext->IASetVertexBuffers(m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets);
	pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer, m_dxgiIndexFormat, m_nIndexOffset);
	pd3dDeviceContext->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dDeviceContext->RSSetState(m_pd3dRasterizerState);

	//��ü���� �ν��Ͻ����� �������Ѵ�. 
	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexedInstanced(m_nIndices, nInstances, m_nStartIndex, m_nBaseVertex, nStartInstance);
	else
		pd3dDeviceContext->DrawInstanced(m_nVertices, nInstances, m_nStartVertex, nStartInstance);
}


//**********************************************************************
//CMouseRectMesh �Լ����̴�.
CMouseRectMesh::CMouseRectMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = 4;
	MouseRectVertex* pVertices = new MouseRectVertex[m_nVertices];
	//���� ���� �־����� �� Update �����̴�.

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
	//old��ġ���� ���� �� or �Ʒ� / ������ �� or �Ʒ� ��츦 ������ ������ ��ġ�� map�Ѵ�.
	//�ε����� �����̹Ƿ� LT�� 1��, RT�� 0��, LB�� 3��, RB�� 2���̴�.
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
		if (oldPosR.y < nextPosR.y)	//���� ��
		{
			vVertices[1].pos = D3DXVECTOR3(nextX, nextY, 0.0f);
			vVertices[0].pos = D3DXVECTOR3(oldX, nextY, 0.0f);
			vVertices[3].pos = D3DXVECTOR3(nextX, oldY, 0.0f);
			vVertices[2].pos = D3DXVECTOR3(oldX, oldY, 0.0f);
		}
		else  //���� �Ʒ�
		{
			vVertices[1].pos = D3DXVECTOR3(nextX, oldY, 0.0f);
			vVertices[0].pos = D3DXVECTOR3(oldX, oldY, 0.0f);
			vVertices[3].pos = D3DXVECTOR3(nextX, nextY, 0.0f);
			vVertices[2].pos = D3DXVECTOR3(oldX, nextY, 0.0f);
		}
	}
	else
	{
		if (oldPosR.y < nextPosR.y) // ������ ��
		{
			vVertices[1].pos = D3DXVECTOR3(oldX, nextY, 0.0f);
			vVertices[0].pos = D3DXVECTOR3(nextX, nextY, 0.0f);
			vVertices[3].pos = D3DXVECTOR3(oldX, oldY, 0.0f);
			vVertices[2].pos = D3DXVECTOR3(nextX, oldY, 0.0f);
		}
		else //������ �Ʒ�
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
	//old��ġ���� ���� �� or �Ʒ� / ������ �� or �Ʒ� ��츦 ������ ������ ��ġ�� map�Ѵ�.
	//�ε����� �����̹Ƿ� LT�� 1��, RT�� 0��, LB�� 3��, RB�� 2���̴�.
	MouseRectVertex vVertices[4];
	vVertices[0].tex = D3DXVECTOR2(1.0f, 0.0f);	vVertices[1].tex = D3DXVECTOR2(0.0f, 0.0f);
	vVertices[2].tex = D3DXVECTOR2(1.0f, 1.0f); vVertices[3].tex = D3DXVECTOR2(0.0f, 1.0f);

	//pos�� ���� ��� �̹� ������ǥ���� ���̴�. ���� ó���� ������ ��ũ�� ó���� �ϸ� �ȴ�. y���� ū �ǹ̰� ������ �ϴ� next�ɷ�

	if (oldPos.x < nextPos.x)
	{
		if (oldPos.z < nextPos.z)	//���� ��
		{
			vVertices[1].pos = D3DXVECTOR3(nextPos.x, nextPos.y , nextPos.z);
			vVertices[0].pos = D3DXVECTOR3(oldPos.x, nextPos.y , nextPos.z);
			vVertices[3].pos = D3DXVECTOR3(nextPos.x, nextPos.y , oldPos.z);
			vVertices[2].pos = D3DXVECTOR3(oldPos.x, nextPos.y , oldPos.z);
		}
		else  //���� �Ʒ�
		{
			vVertices[1].pos = D3DXVECTOR3(nextPos.x, nextPos.y , oldPos.z);
			vVertices[0].pos = D3DXVECTOR3(oldPos.x, nextPos.y , oldPos.z);
			vVertices[3].pos = D3DXVECTOR3(nextPos.x, nextPos.y , nextPos.z);
			vVertices[2].pos = D3DXVECTOR3(oldPos.x, nextPos.y , nextPos.z);
		}
	}
	else
	{
		if (oldPos.z < nextPos.z) // ������ ��
		{
			vVertices[1].pos = D3DXVECTOR3(oldPos.x, nextPos.y , nextPos.z);
			vVertices[0].pos = D3DXVECTOR3(nextPos.x, nextPos.y , nextPos.z);
			vVertices[3].pos = D3DXVECTOR3(oldPos.x, nextPos.y , oldPos.z);
			vVertices[2].pos = D3DXVECTOR3(nextPos.x, nextPos.y , oldPos.z);
		}
		else //������ �Ʒ�
		{
			vVertices[1].pos = D3DXVECTOR3(oldPos.x, nextPos.y , oldPos.z);
			vVertices[0].pos = D3DXVECTOR3(nextPos.x, nextPos.y , oldPos.z);
			vVertices[3].pos = D3DXVECTOR3(oldPos.x, nextPos.y , nextPos.z);
			vVertices[2].pos = D3DXVECTOR3(nextPos.x, nextPos.y , nextPos.z);
		}
	}

	//�̹����� ������ �� �� �� y�� 0.0f�� �������� �ؾ� ���콺�� ��ġ�� �´�. ���� ��ü ��ŷ�ÿ��� y���� ����Ѵ�.
	//for (int i = 0; i < 4; ++i)
	//	vVertices[i].pos.y = 0.0f;

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	MouseRectVertex *pcbMRVertices = (MouseRectVertex *)d3dMappedResource.pData;
	memcpy(pcbMRVertices, &vVertices, sizeof(MouseRectVertex) * 4);
	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}


//**********************************************************************
//CSelectedCircleMesh �Լ����̴�.
CSelectedCircleMesh::CSelectedCircleMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = MAX_SELECTED_OBJECTS;
	D3DXVECTOR4* pVertices = new D3DXVECTOR4[m_nVertices];
	//���� ���� �־����� �� Update �����̴�.

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	//Map
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR4) * m_nVertices;	//ó�� ũ�� ��� Update���� vector size��ŭ��
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

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;	//���ϼ��̴��ϱ�
	
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
	//������ Scale ���ڰ��� �Ʊ��̸� 0~100 / �����̸� +100 / �߸��̸� +200�� �Ѵ�.
	//���̴��ڵ忡�� 0~100�����̸� �ʷ�, 100���� ũ�� -100 �ϰ� ��������, 200���� ũ�� -200�ϰ� ������� ������

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
//CMiniMapObject �Լ����̴�.
CMiniMapObjectMesh::CMiniMapObjectMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = MAX_KINDS_OBJECTS * MAX_INSTANCE_OBJECTS;	//�ϴ� �ִ�� ��Ƴ��´� -> ���� �����ϱ� ���� / ���� �������� ��ȭ
	//���� ���� �־����� �� Update �����̴�.

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	//Map
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR4) * m_nVertices;	//ó�� ũ�� ��� Update���� vector size��ŭ��
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dPositionBuffer);

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(D3DXVECTOR4) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;	//���ϼ��̴��ϱ�
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

	// �ε������� ��, lt rt lb rb 0 1 2 3
	pcbMinimapRectVertices[0] = vLT;
	pcbMinimapRectVertices[1] = vRT;
	pcbMinimapRectVertices[2] = vLB;
	pcbMinimapRectVertices[3] = vRB;

	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}


//**********************************************************************
//CNeutralResourceMesh �Լ����̴�.
CNeutralResourceMesh::CNeutralResourceMesh(ID3D11Device* pd3dDevice) : CMesh(pd3dDevice)
{
	m_nVertices = MAX_KINDS_OBJECTS;	//�ִ� scv ����

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	//Map
	d3dBufferDesc.ByteWidth = sizeof(D3DXVECTOR4) * m_nVertices;	//ó�� ũ�� ��� Update���� vector size��ŭ��
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dPositionBuffer);

	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(D3DXVECTOR4) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;	//���ϼ��̴��ϱ�
}

CNeutralResourceMesh::~CNeutralResourceMesh()
{

}

void CNeutralResourceMesh::UpdateNeutralResource(std::vector<D3DXVECTOR4>& vPosType, ID3D11DeviceContext* pd3dDeviceContext)
{
	//���ϼ��̴����� w���� ���� �ؽ�����ǥ�� ����, 0~0.5f�� 0.5f~1.0f�� �ڿ� �̹����� ����.
	m_nVertices = vPosType.size();	///w�� 0.0f -> mineral , 1.0f -> gas 

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	D3DXVECTOR4 *pcbNRVertices = (D3DXVECTOR4 *)d3dMappedResource.pData;
	for (int i = 0; i < m_nVertices; ++i)
		pcbNRVertices[i] = vPosType[i];
	pd3dDeviceContext->Unmap(m_pd3dPositionBuffer, 0);
}




//**********************************************************************
//CMainFrameMesh �Լ����̴�.
CMainFrameMesh::CMainFrameMesh(ID3D11Device* pd3dDevice, D3DXVECTOR2 vScreenLB, D3DXVECTOR2 vScreenRT) : CMesh(pd3dDevice)
{
	//�»� 0 ��� 1 ���� 2 ���� 3 �ٷ� ��ũ����ǥ�� ����ɰ��̴�.
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
	//���� ������Ʈ
	ID3D11BlendState *pd3dBlendState = NULL;
	D3D11_BLEND_DESC BD;
	ZeroMemory(&BD, sizeof(D3D11_BLEND_DESC));
	BD.AlphaToCoverageEnable = false;
	BD.IndependentBlendEnable = false; //�Ѱ��� ����Ÿ�ٸ� ���ڴ�.
	ZeroMemory(&(BD.RenderTarget[0]), sizeof(D3D11_RENDER_TARGET_BLEND_DESC));

	BD.RenderTarget[0].BlendEnable = true;

	BD.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;		//�ؽ��� ���� �״��
	BD.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;	//����Ÿ�� ������ 0�� ��������.
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
//CScreenMesh �Լ����̴�.
CScreenMesh::CScreenMesh(ID3D11Device* pd3dDevice, D3DXVECTOR2& leftBottom, D3DXVECTOR2& rightTop, bool beRendered) :CMesh(pd3dDevice)
{
	float fRendered = 0.0f;
	if (beRendered)
		fRendered = 1.0f;

	//�»� 0 ��� 1 ���� 2 ���� 3 �ٷ� ��ũ����ǥ�� ����ɰ��̴�.
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
	
	//��ũƮ�� ǥ���� ���� ��ġ �� �ؽ��� ������ �����Ѵ�.
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
	//d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT; //��ũƮ�� ǥ��
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(ScreenVertex) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//d3dBufferDesc.CPUAccessFlags = 0; //��ũƮ�� ǥ��
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
//CCrowdMesh �Լ����̴�.
CCrowdMesh::CCrowdMesh(ID3D11Device* pd3dDevice, D3DXVECTOR2& leftBottom, D3DXVECTOR2& rightTop) :CMesh(pd3dDevice)
{
	vLB = leftBottom;
	vRT = rightTop;

	//�»� 0 ��� 1 ���� 2 ���� 3 �ٷ� ��ũ����ǥ�� ����ɰ��̴�.
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
//CSkyBoxMesh �Լ����̴�.
CSkyBoxMesh::CSkyBoxMesh(ID3D11Device *pd3dDevice, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice)
{
	//��ī�� �ڽ��� 6���� ��(�簢��), �簢���� ���� 4��, �׷��Ƿ� 24���� ������ �ʿ��ϴ�.
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

	//�ﰢ�� ��Ʈ������ �簢�� 1���� �׸��� ���� �ε����� 4���� �ʿ��ϴ�.
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
	//��ī�� �ڽ� �簢������ ���� ���� �˰����� �������� �ʰ� ���� ���۸� �������� �ʴ´�.
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
	//6���� ��ī�� �ڽ� �ؽ��ĸ� �����Ͽ� CTexture ��ü�� �����Ѵ�.
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

	//��ī�� �ڽ��� �׸��� ���� ���÷� ���� ��ü�� ���� ���ٽ� ���� ��ü�� �����Ѵ�.
	m_pSkyboxTexture->UpdateSamplerShaderVariable(pd3dDeviceContext, 0, PS_SLOT_SAMPLER);
	pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState, 0);

	//��ī�� �ڽ��� 6�� ��(�簢��)�� ������� �׸���.
	for (int i = 0; i < 6; i++)
	{
		//��ī�� �ڽ��� �� ��(�簢��)�� �׸� �� ����� �ؽ��ĸ� �����Ѵ�.
		m_pSkyboxTexture->UpdateTextureShaderVariable(pd3dDeviceContext, i, PS_SLOT_TEXTURE);
		pd3dDeviceContext->DrawIndexed(4, 0, i * 4);
	}

	pd3dDeviceContext->OMSetDepthStencilState(nullptr, 1);
}

//**********************************************************************
//CTerrainMesh�� �����մϴ�.
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

	/*���ڴ� �簢������ �����̰� �簢���� �� ���� �ﰢ������ �����ǹǷ� ���ڴ� <�׸� 18>�� ���� �ﰢ������ �����̶�� �� �� �ִ�. ���ڸ� ǥ���ϱ� ���Ͽ� ������ �ﰢ������ �ε����� ǥ���ؾ� �Ѵ�. �ﰢ������ �簢���� �� ������ �Ʒ����� ���� ��������(z-��) �����Ѵ�. ù ��° �簢�� ���� �ﰢ������ ���ʿ��� ����������(x-��) �����Ѵ�. �� ��° ���� �ﰢ������ �����ʿ��� ���� �������� �����Ѵ�. ��, �簢���� ���� �ٲ� ������ ���� ������ �ٲ�� �Ѵ�. <�׸� 18>�� ���ڿ� ���Ͽ� �ﰢ�� ��Ʈ���� ����Ͽ� �ﰢ������ ǥ���ϱ� ���� �ε����� ������ ������ ���� ������ m��° �ٰ� (m+1)��° ���� ���� ��ȣ�� �簢���� ���� ���⿡ ���� ������ �Ʒ�, ��, �Ʒ�, ��, ... ������ �����ϸ� �ȴ�.
	0, 6, 1, 7, 2, 8, 3, 9, 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12
	�̷��� �ε����� �����ϸ� �ﰢ�� ��Ʈ���� ����� ���̹Ƿ� ���� �׷����� �ﰢ������ �ε����� ������ ����.
	(0, 6, 1), (1, 6, 7), (1, 7, 2), (2, 7, 8), (2, 8, 3), (3, 8, 9), ...
	�׷��� �̷��� �ε����� ����ϸ� ù ��° ���� �����ϰ� �ﰢ������ ����� �׷����� �ʴ´�. �ֳ��ϸ� �ﰢ�� ��Ʈ�������� ������ 2���� ������ ���ο� �ϳ��� ������ ����Ͽ� ���ο� �ﰢ���� �׸���. �׸��� Ȧ�� ��° �ﰢ���� ���� ���� ����(���ε� ����)�� �ð�����̰� ¦�� ��° �ﰢ���� ���ε� ������ �ݽð�����̾�� �Ѵ�. ������ �簢���� �� �ٿ��� �� ���� �ִ��� ������� �� ���� ������ �ﰢ���� ¦�� ��° �ﰢ���̰� ���ε� ������ �ݽð� �����̴�. �ֳ��ϸ� �簢���� �� ���� �ﰢ������ ���������� �����̴�. ù ��° �ٿ��� �� ��° ���� �ε��� ������ ���� �׷����� �ﰢ������ �ε����� ���캸��.
	..., 4, 10, 5, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
	..., (4, 10, 5), (5, 10, 11), (5, 11, 11), (11, 11, 17), (11, 17, 10), ...
	�ﰢ�� (5, 10, 11)�� ù ��° ���� ������ �ﰢ���̰� ¦�� ��°�̴�. �ﰢ�� (11, 17, 10)�� �� ��° ���� ù ��° �ﰢ���̰� Ȧ�� ��°�̴�. Ȧ�� ��°�̹Ƿ� ���ε� ������ �ð�����̾�� �ϴµ� ���� ���ε� ������ �ݽð�����̹Ƿ� �׷����� ���� ���̴�. �翬�� ���� �ﰢ���� ���ε� ������ ���� �����Ƿ� �׷����� ���� ���̴�. �ﰢ�� (11, 17, 10)�� ���ε� ������ �ݽð�����̹Ƿ� �׷������� �Ϸ��� �� �ﰢ���� ¦�� ��° �ﰢ���� �ǵ��� �ؾ� �Ѵ�. �̸� ���ؼ� ���� �ٲ� ������ ������ ������ �ε����� �߰��ϵ��� ����. �׷��� ���� �ٲ� ù ��° �ﰢ���� ¦�� ��° �ﰢ���� �ȴ�. ������ �������� 11�� �߰��� ������ ������ �ε����̴�. �̷��� �ϸ� �ﰢ���� ������ �� ��� �׷����� �ʴ� �ﰢ���� �� �ٸ��� 3���� ����� �ȴ�.
	..., 4, 10, 5, 11, 11, 11, 17, 10, 16, 9, 15, 8, 14, 7, 13, 6, 12, ...
	..., (5, 10, 11), (5, 11, 11), (11, 11, 11), (11, 11, 17), (11, 17, 10), ...
	�� ���� �ﰢ�� (5, 11, 11), (11, 11, 11), (11, 11, 17)�� �ﰢ���� ������ �� �����Ƿ� ������ �׷����� �ʴ´�.
	�̷��� �ε����� �����ϸ� �ε��� ���۴� ((nWidth*2)*(nLength-1))+((nLength-1)-1)���� �ε����� ���´�. �簢�� ���� ������ (nLength-1)�̰� �� �ٿ��� (nWidth*2)���� �ε����� ���´�. �׸��� ���� �ٲ� ������ �ε����� �ϳ� �߰��ϹǷ� (nLength-1)-1���� �ε����� �߰��� �ʿ��ϴ�.*/
	m_nIndices = ((nWidth * 2)*(nLength - 1)) + ((nLength - 1) - 1);
	m_pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//Ȧ�� ��° ���̹Ƿ�(z = 0, 2, 4, ...) �ε����� ���� ������ ���ʿ��� ������ �����̴�.
			for (int x = 0; x < nWidth; x++)
			{
				//ù ��° ���� �����ϰ� ���� �ٲ� ������(x == 0) ù ��° �ε����� �߰��Ѵ�.
				if ((x == 0) && (z > 0)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				//�Ʒ�, ���� ������ �ε����� �߰��Ѵ�.
				m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				m_pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
		else
		{
			//¦�� ��° ���̹Ƿ�(z = 1, 3, 5, ...) �ε����� ���� ������ �����ʿ��� ���� �����̴�.
			for (int x = nWidth - 1; x >= 0; x--)
			{
				//���� �ٲ� ������(x == (nWidth-1)) ù ��° �ε����� �߰��Ѵ�.
				if (x == (nWidth - 1)) m_pnIndices[j++] = (UINT)(x + (z * nWidth));
				//�Ʒ�, ���� ������ �ε����� �߰��Ѵ�.
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
	//���� �� ��ü�� ���� �� �̹����� �ȼ� ���� ������ ���̷� ��ȯ�Ѵ�. 
	CHeightMap *pHeightMap = (CHeightMap *)pContext;
	BYTE *pHeightMapImage = pHeightMap->GetHeightMapImage();
	D3DXVECTOR3 d3dxvScale = pHeightMap->GetScale();
	int cxTerrain = pHeightMap->GetHeightMapWidth();
	float fHeight = pHeightMapImage[x + (z*cxTerrain)] * d3dxvScale.y;
	return(fHeight);
}


//**********************************************************************
//CTerrainMesh�� �����մϴ�.
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

	//�ﰢ�� ��Ʈ������ �簢�� 1���� �׸��� ���� �ε����� 4���� �ʿ��ϴ�.
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


	//�׻� ���̴� �����̴�.
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
//�׼����̼� ���� CTSTerrainMesh�� �����մϴ�.

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
//CDefaultMesh    -> �ϴ� normal, texCoord �߰�
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
//CCubeMesh -> �׽�Ʈ�� ť��
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

	//������ü�� �� ��(�ﰢ�� 2��)�� �ϳ��� �ؽ��� �̹��� ��ü�� ���εǵ��� �ؽ��� ��ǥ�� �����Ѵ�.
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

	//������ü�� �� ��(�ﰢ�� 2��)�� �ϳ��� �ؽ��� �̹��� ��ü�� ���εǵ��� �ؽ��� ��ǥ�� �����Ѵ�.
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

//�׸��� �׷����� ����
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

	//��ü���� �ν��Ͻ����� �������Ѵ�. 
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

	//������ ��ŷó���� ���� ������ �ε����� ������ ������ ������
	//������ �ƴ� ���� ��ġ�� ũ�⸦ �ȴٸ� �ٿ���ڽ��� ���� ��ŷ�� �����ϹǷ� ���� �������� �ʴ´�.
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
	//1214 �븻����
	//���⼭���� ź��Ʈ �� ��� �ڵ�
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

			if (i == idx0 || i == idx1 || i == idx2) //������Ƽ�꿡 ���ϴ� �����̶��
			{
				//�� �� ���� �������κ��� T ���͸� ���ϰ� ���س��´�.
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
		printf("ź��Ʈ ���� �ȸ������!\n");
	//�븻����
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

	//�浹 üũ�� ���� �ٿ�� �ڽ��̴�.
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-(aabbSize / 2.0f), -(aabbSize / 2.0f), -(aabbSize / 2.0f));
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3((aabbSize / 2.0f), (aabbSize / 2.0f), (aabbSize / 2.0f));
}


CStaticMesh* CStaticMesh::CreateAndCopyMeshData()
{
	CStaticMesh* pMesh = new CStaticMesh(nullptr);	//���� ����̽� ���൵ ��

	pMesh->m_d3dxmtxLocal = m_d3dxmtxLocal;

	pMesh->m_nVertices = m_nVertices;
	pMesh->m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pMesh->m_nIndices = m_nIndices;

	pMesh->m_pd3dPositionBuffer = m_pd3dPositionBuffer; m_pd3dPositionBuffer->AddRef();
	pMesh->m_pd3dTexCoordBuffer = m_pd3dTexCoordBuffer; m_pd3dTexCoordBuffer->AddRef();
	pMesh->m_pd3dNormalBuffer = m_pd3dNormalBuffer; m_pd3dNormalBuffer->AddRef();

	pMesh->m_pd3dIndexBuffer = m_pd3dIndexBuffer; m_pd3dIndexBuffer->AddRef();

	//1214 �븻���������� ź��Ʈ
	pMesh->m_pd3dTangentBuffer = m_pd3dTangentBuffer; m_pd3dTangentBuffer->AddRef();

	ID3D11Buffer *pd3dBuffers[4] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer, m_pd3dNormalBuffer, m_pd3dTangentBuffer };
	UINT pnBufferStrides[4] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[4] = { 0, 0, 0, 0 };
	pMesh->AssembleToVertexBuffer(4, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	pMesh->m_meshMaterial = m_meshMaterial;

	//�浹 üũ�� ���� �ٿ�� �ڽ��̴�.
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

//�׸��� �׷����� ����
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

	//��ü���� �ν��Ͻ����� �������Ѵ�. 
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
	map<string, AnimationClip> mapAnimationClip;	//4���ϰ��̴�... �� Ŭ���� 4 ���� �־���� ������

	
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
				fin >> clipName; //�ִϸ��̼� �̸� ��
				fin >> buf; // "{ " clip ���� �߰�ȣ

				int nKeyFrames = 0;
				for (int i = 0; i < boneCount; ++i)
				{
					fin >> buf; // "Bone "
					fin >> buf; // "N: " �� ��ȣ�� �ִµ� ���ʿ� ������� �����Ƿ� �� �ʿ� X
					fin >> nKeyFrames;

					BoneAnimation boneAnim;
					boneAnim.Keyframes.resize(nKeyFrames);

					fin >> buf; // "{ " KeyFrame ���� ���� ��ȣ
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
					fin >> buf; // "} " KeyFrame ���� �� ��ȣ	
					clip.BoneAnimations.push_back(boneAnim);
				}
				fin >> buf; // "} " clip �� �߰�ȣ

				mapAnimationClip.insert(make_pair(clipName, clip));
			}
			fin.close(); //�ε� ��!

			m_pSkinnedData = new SkinnedData();
			m_pSkinnedData->AddRef();
			m_pSkinnedData->Set(vBoneHierarchy, vOffsetMatrix, mapAnimationClip);
		}
	}


	//������ ��ŷó���� ���� ������ �ε����� ������ ������ ������
	//������ �ƴ� ���� ��ġ�� ũ�⸦ �ȴٸ� �ٿ���ڽ��� ���� ��ŷ�� �����ϹǷ� ���� �������� �ʴ´�.
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
	//1214 �븻����
	//���⼭���� ź��Ʈ �� ��� �ڵ�
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

			if (i == idx0 || i == idx1 || i == idx2) //������Ƽ�꿡 ���ϴ� �����̶��
			{
				//�� �� ���� �������κ��� T ���͸� ���ϰ� ���س��´�.
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
		printf("ź��Ʈ ���� �ȸ������!\n");
	//�븻����
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

	//�� �� ����ε� ������...
	m_bcBoundingCube.m_d3dxvMinimum = D3DXVECTOR3(-(aabbSize / 2.0f), -(aabbSize / 2.0f), -(aabbSize / 2.0f));
	m_bcBoundingCube.m_d3dxvMaximum = D3DXVECTOR3((aabbSize / 2.0f), (aabbSize / 2.0f), (aabbSize / 2.0f));
}

CAnimationMesh* CAnimationMesh::CreateAndCopyMeshData()
{
	CAnimationMesh* pMesh = new CAnimationMesh(nullptr);	//���� ����̽� ���൵ ��

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

	//1214 �븻���������� ź��Ʈ
	pMesh->m_pd3dTangentBuffer = m_pd3dTangentBuffer; m_pd3dTangentBuffer->AddRef();

	ID3D11Buffer *pd3dBuffers[6] = { m_pd3dPositionBuffer, m_pd3dTexCoordBuffer, m_pd3dNormalBuffer, m_pd3dBoneIDBuffer, m_pd3dWeightBuffer, m_pd3dTangentBuffer };
	UINT pnBufferStrides[6] = { sizeof(D3DXVECTOR3), sizeof(D3DXVECTOR2), sizeof(D3DXVECTOR3), sizeof(UINT4), sizeof(D3DXVECTOR4), sizeof(D3DXVECTOR3) };
	UINT pnBufferOffsets[6] = { 0, 0, 0, 0, 0, 0 };
	pMesh->AssembleToVertexBuffer(6, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	pMesh->m_meshMaterial = m_meshMaterial;

	//�浹 üũ�� ���� �ٿ�� �ڽ��̴�.
	pMesh->m_bcBoundingCube.m_d3dxvMinimum = m_bcBoundingCube.m_d3dxvMinimum;
	pMesh->m_bcBoundingCube.m_d3dxvMaximum = m_bcBoundingCube.m_d3dxvMaximum;

	//pMesh->AddRef();
	return pMesh;
}