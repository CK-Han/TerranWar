#include "stdafx.h"
#include "Camera.h"

//**********************************************************************
CCamera::CCamera()
{
	//m_d3dxvPosition = D3DXVECTOR3(0.0f, 114.749702f, 0.0f);
	m_d3dxvPosition = D3DXVECTOR3(0.0f, 120.0f, -60.0f);
	//m_d3dxvPosition = D3DXVECTOR3(0.0f, 60.0f, -30.0f);
	m_d3dxvRight = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_d3dxvLook = *D3DXVec3Normalize(&m_d3dxvLook, &D3DXVECTOR3(0.0f, -1.0f, 1.0f));
	D3DXVec3Cross(&m_d3dxvUp, &m_d3dxvLook, &m_d3dxvRight);
	m_d3dxvOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMatrixIdentity(&m_d3dxmtxView);
	D3DXMatrixIdentity(&m_d3dxmtxProjection);
}

CCamera::~CCamera()
{
	if (m_pd3dcbCamera) m_pd3dcbCamera->Release();
}

//**********************************************************************
//카메라, 투영 변환 행렬 및 뷰포트 생성

void CCamera::GenerateViewMatrix()
{
	D3DXMatrixLookAtLH(&m_d3dxmtxView, &m_d3dxvPosition, &m_d3dxvLookAtWorld, &m_d3dxvUp);
	CalculateFrustumPlanes();
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	D3DXMatrixPerspectiveFovLH(&m_d3dxmtxProjection, (float)D3DXToRadian(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	CalculateFrustumPlanes();
}

void CCamera::SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
	pd3dDeviceContext->RSSetViewports(1, &m_d3dViewport);
}

void CCamera::Move(const D3DXVECTOR3& d3dxvShift)
{
	m_d3dxvPosition += d3dxvShift;
	//m_d3dxvLookAtWorld += d3dxvShift;
}

void CCamera::Move(const D3DXVECTOR3& d3dxvAxis, float fValue)
{
	m_d3dxvPosition += d3dxvAxis * fValue;
	//m_d3dxvLookAtWorld += d3dxvAxis * fValue;
}

void CCamera::SetLookAtWorld(D3DXVECTOR3 vLookAtWorld)
{
	m_d3dxvLookAtWorld = vLookAtWorld;
	D3DXMATRIX mtxLookAt;
	D3DXMatrixLookAtLH(&mtxLookAt, &m_d3dxvPosition, &m_d3dxvLookAtWorld, &m_d3dxvUp);
	m_d3dxvRight = D3DXVECTOR3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_d3dxvUp = D3DXVECTOR3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_d3dxvLook = D3DXVECTOR3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}

//**********************************************************************
//상수 버퍼 생성, 갱신 후 셰이더에 연결
void CCamera::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_CAMERA);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbCamera);
}

void CCamera::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	/*상수 버퍼의 메모리 주소를 가져와서 카메라 변환 행렬과 투영 변환 행렬을 복사한다. 쉐이더에서 행렬의 행과 열이 바뀌는 것에 주의하라.*/
	pd3dDeviceContext->Map(m_pd3dcbCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_CAMERA *pcbViewProjection = (VS_CB_CAMERA *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxView, &m_d3dxmtxView);
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxProjection, &m_d3dxmtxProjection);
	pd3dDeviceContext->Unmap(m_pd3dcbCamera, 0);

	pd3dDeviceContext->VSSetConstantBuffers(CB_SLOT_CAMERA, 1, &m_pd3dcbCamera);
	pd3dDeviceContext->GSSetConstantBuffers(CB_SLOT_CAMERA, 1, &m_pd3dcbCamera); //SelectedCircle
	pd3dDeviceContext->DSSetConstantBuffers(CB_SLOT_CAMERA, 1, &m_pd3dcbCamera); //Tessellation Terrain
}

//그림자 및 안개 텍스쳐를 생성할 때
void CCamera::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX& mtxView, D3DXMATRIX& mtxProjection)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	/*상수 버퍼의 메모리 주소를 가져와서 카메라 변환 행렬과 투영 변환 행렬을 복사한다. 쉐이더에서 행렬의 행과 열이 바뀌는 것에 주의하라.*/
	pd3dDeviceContext->Map(m_pd3dcbCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_CAMERA *pcbViewProjection = (VS_CB_CAMERA *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxView, &mtxView);
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxProjection, &mtxProjection);
	pd3dDeviceContext->Unmap(m_pd3dcbCamera, 0);

	pd3dDeviceContext->VSSetConstantBuffers(CB_SLOT_CAMERA, 1, &m_pd3dcbCamera);
	pd3dDeviceContext->GSSetConstantBuffers(CB_SLOT_CAMERA, 1, &m_pd3dcbCamera); 
	pd3dDeviceContext->DSSetConstantBuffers(CB_SLOT_CAMERA, 1, &m_pd3dcbCamera);
}


//**********************************************************************
//월드좌표계 절두체 컬링 구현

void CCamera::CalculateFrustumPlanes()
{
	/*카메라 변환 행렬과 원근 투영 변환 행렬을 곱한 행렬을 사용하여 절두체 평면들을 구한다.
	즉 월드 좌표계에서 절두체 컬링을 한다.*/
	D3DXMATRIX mtxViewProject = m_d3dxmtxView * m_d3dxmtxProjection;

	//절두체의 왼쪽 평면
	m_d3dxFrustumPlanes[0].a = -(mtxViewProject._14 + mtxViewProject._11);
	m_d3dxFrustumPlanes[0].b = -(mtxViewProject._24 + mtxViewProject._21);
	m_d3dxFrustumPlanes[0].c = -(mtxViewProject._34 + mtxViewProject._31);
	m_d3dxFrustumPlanes[0].d = -(mtxViewProject._44 + mtxViewProject._41);

	//절두체의 오른쪽 평면
	m_d3dxFrustumPlanes[1].a = -(mtxViewProject._14 - mtxViewProject._11);
	m_d3dxFrustumPlanes[1].b = -(mtxViewProject._24 - mtxViewProject._21);
	m_d3dxFrustumPlanes[1].c = -(mtxViewProject._34 - mtxViewProject._31);
	m_d3dxFrustumPlanes[1].d = -(mtxViewProject._44 - mtxViewProject._41);

	//절두체의 위쪽 평면
	m_d3dxFrustumPlanes[2].a = -(mtxViewProject._14 - mtxViewProject._12);
	m_d3dxFrustumPlanes[2].b = -(mtxViewProject._24 - mtxViewProject._22);
	m_d3dxFrustumPlanes[2].c = -(mtxViewProject._34 - mtxViewProject._32);
	m_d3dxFrustumPlanes[2].d = -(mtxViewProject._44 - mtxViewProject._42);

	//절두체의 아래쪽 평면
	m_d3dxFrustumPlanes[3].a = -(mtxViewProject._14 + mtxViewProject._12);
	m_d3dxFrustumPlanes[3].b = -(mtxViewProject._24 + mtxViewProject._22);
	m_d3dxFrustumPlanes[3].c = -(mtxViewProject._34 + mtxViewProject._32);
	m_d3dxFrustumPlanes[3].d = -(mtxViewProject._44 + mtxViewProject._42);

	//절두체의 근평면
	m_d3dxFrustumPlanes[4].a = -(mtxViewProject._13);
	m_d3dxFrustumPlanes[4].b = -(mtxViewProject._23);
	m_d3dxFrustumPlanes[4].c = -(mtxViewProject._33);
	m_d3dxFrustumPlanes[4].d = -(mtxViewProject._43);

	//절두체의 원평면
	m_d3dxFrustumPlanes[5].a = -(mtxViewProject._14 - mtxViewProject._13);
	m_d3dxFrustumPlanes[5].b = -(mtxViewProject._24 - mtxViewProject._23);
	m_d3dxFrustumPlanes[5].c = -(mtxViewProject._34 - mtxViewProject._33);
	m_d3dxFrustumPlanes[5].d = -(mtxViewProject._44 - mtxViewProject._43);

	/*절두체의 각 평면의 법선 벡터 (a, b. c)의 크기로 a, b, c, d를 나눈다.
	즉, 법선 벡터를 정규화하고 원점에서 평면까지의 거리를 계산한다.*/
	for (int i = 0; i < 6; i++) D3DXPlaneNormalize(&m_d3dxFrustumPlanes[i], &m_d3dxFrustumPlanes[i]);
}

bool CCamera::IsInFrustum(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum)
{
	D3DXVECTOR3 d3dxvNearPoint, d3dxvFarPoint, d3dxvNormal;
	for (int i = 0; i < 6; i++)
	{
		/*절두체의 각 평면에 대하여 바운딩 박스의 근접점을 계산한다. 근접점의 x, y, z 좌표는 법선 벡터의 각 요소가 음수이면 바운딩 박스의 최대점의 좌표가 되고 그렇지 않으면 바운딩 박스의 최소점의 좌표가 된다.*/
		d3dxvNormal = D3DXVECTOR3(m_d3dxFrustumPlanes[i].a, m_d3dxFrustumPlanes[i].b, m_d3dxFrustumPlanes[i].c);
		if (d3dxvNormal.x >= 0.0f)
		{
			if (d3dxvNormal.y >= 0.0f)
			{
				if (d3dxvNormal.z >= 0.0f)
				{
					//법선 벡터의 x, y, z 좌표의 부호가 모두 양수이므로 근접점은 바운딩 박스의 최소점이다.
					d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMinimum.z;
				}
				else
				{
					/*법선 벡터의 x, y 좌표의 부호가 모두 양수이므로 근접점의 x, y 좌표는 바운딩 박스의 최소점의 x, y 좌표이고 법선 벡터의 z 좌표가 움수이므로 근접점의 z 좌표는 바운딩 박스의 최대점의 z 좌표이다.*/
					d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMaximum.z;
				}
			}
			else
			{
				if (d3dxvNormal.z >= 0.0f)
				{
					/*법선 벡터의 x, z 좌표의 부호가 모두 양수이므로 근접점의 x, z 좌표는 바운딩 박스의 최소점의 x, z 좌표이고 법선 벡터의 y 좌표가 움수이므로 근접점의 y 좌표는 바운딩 박스의 최대점의 y 좌표이다.*/
					d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMinimum.z;
				}
				else
				{
					/*법선 벡터의 y, z 좌표의 부호가 모두 음수이므로 근접점의 y, z 좌표는 바운딩 박스의 최대점의 y, z 좌표이고 법선 벡터의 x 좌표가 양수이므로 근접점의 x 좌표는 바운딩 박스의 최소점의 x 좌표이다.*/
					d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMaximum.z;
				}
			}
		}
		else
		{
			if (d3dxvNormal.y >= 0.0f)
			{
				if (d3dxvNormal.z >= 0.0f)
				{
					/*법선 벡터의 y, z 좌표의 부호가 모두 양수이므로 근접점의 y, z 좌표는 바운딩 박스의 최소점의 y, z 좌표이고 법선 벡터의 x 좌표가 음수이므로 근접점의 x 좌표는 바운딩 박스의 최대점의 x 좌표이다.*/
					d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMinimum.z;
				}
				else
				{
					/*법선 벡터의 x, z 좌표의 부호가 모두 음수이므로 근접점의 x, z 좌표는 바운딩 박스의 최대점의 x, z 좌표이고 법선 벡터의 y 좌표가 양수이므로 근접점의 y 좌표는 바운딩 박스의 최소점의 y 좌표이다.*/
					d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMaximum.z;
				}
			}
			else
			{
				if (d3dxvNormal.z >= 0.0f)
				{
					/*법선 벡터의 x, y 좌표의 부호가 모두 음수이므로 근접점의 x, y 좌표는 바운딩 박스의 최대점의 x, y 좌표이고 법선 벡터의 z 좌표가 양수이므로 근접점의 z 좌표는 바운딩 박스의 최소점의 z 좌표이다.*/
					d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMinimum.z;
				}
				else
				{
					//법선 벡터의 x, y, z 좌표의 부호가 모두 음수이므로 근접점은 바운딩 박스의 최대점이다.
					d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMaximum.z;
				}
			}
		}
		/*근접점이 절두체 평면 중 하나의 평면의 바깥(앞)쪽에 있으면 근접점은 절두체에 포함되지 않는다. 
		근접점이 어떤 평면의 바깥(앞)쪽에 있으면 근접점을 평면의 방정식에 대입하면 부호가 양수가 된다.
		각 평면의 방정식에 근접점을 대입하는 것은 근접점 (x, y, z)과 평면의 법선벡터 (a, b, c)의 
		내적에 원점에서 평면까지의 거리를 더한 것과 같다.*/
		if ((D3DXVec3Dot(&d3dxvNormal, &d3dxvNearPoint) + m_d3dxFrustumPlanes[i].d) > 0.0f) return(false);
	}
	return(true);
}

bool CCamera::IsInFrustum(AABB *pAABB)
{
	return(IsInFrustum(pAABB->m_d3dxvMinimum, pAABB->m_d3dxvMaximum));
}