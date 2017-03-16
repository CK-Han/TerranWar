#pragma once

#include "Mesh.h"

struct VS_CB_CAMERA
{
	D3DXMATRIX		m_d3dxmtxView;
	D3DXMATRIX		m_d3dxmtxProjection;
};


class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle);
	D3DXMATRIX GetViewMatrix() { return m_d3dxmtxView; }
	D3DXMATRIX GetProjectionMatrix() { return m_d3dxmtxProjection; }
	
	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	D3D11_VIEWPORT	 GetViewport() { return(m_d3dViewport); }

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX& mtxView, D3DXMATRIX& mtxProjection);

	virtual void Move(const D3DXVECTOR3& d3dxvShift);
	virtual void Move(const D3DXVECTOR3& d3dxvAxis, float fValue);
	
	void		SetPosition(D3DXVECTOR3 d3dxvPosition) { m_d3dxvPosition = d3dxvPosition; }	
	void		SetLookAtWorld(D3DXVECTOR3 vLookAtWorld);
	void		SetRight(D3DXVECTOR3 d3dxvRight) { m_d3dxvRight = d3dxvRight; }
	void		SetUpVector(D3DXVECTOR3 d3dxvUp) { m_d3dxvUp = d3dxvUp; }
	void		SetLook(D3DXVECTOR3 d3dxvLook) { m_d3dxvLook = d3dxvLook; }
	void		SetOffset(D3DXVECTOR3 d3dxvOffset) { m_d3dxvLookAtWorld = d3dxvOffset; m_d3dxvPosition += d3dxvOffset; }

	CCamera*							  	 GetCamera() { return(this); }
	
	D3DXVECTOR3&							 GetPosition() { return m_d3dxvPosition; }
	D3DXVECTOR3&							 GetLookAtPosition() { return(m_d3dxvLookAtWorld); }
	D3DXVECTOR3&							 GetRightVector() { return m_d3dxvRight; }
	D3DXVECTOR3&							 GetUpVector() { return m_d3dxvUp; }
	D3DXVECTOR3&							 GetLookVector() { return m_d3dxvLook; }
	D3DXVECTOR3&							 GetOffset() { return(m_d3dxvOffset); }

	//이상 Get&Set, 동작함수

	//절두체 컬링
	void		 CalculateFrustumPlanes();
	bool		 IsInFrustum(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	bool		 IsInFrustum(AABB *pAABB);

	
protected:
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;

	D3D11_VIEWPORT m_d3dViewport;

	ID3D11Buffer *m_pd3dcbCamera;

	//Camera Control
	D3DXVECTOR3 m_d3dxvLookAtWorld;
	D3DXVECTOR3 m_d3dxvOffset;

	D3DXVECTOR3 m_d3dxvPosition;
	D3DXVECTOR3 m_d3dxvRight;
	D3DXVECTOR3 m_d3dxvUp;
	D3DXVECTOR3 m_d3dxvLook;

	//절두체 컬링
	D3DXPLANE m_d3dxFrustumPlanes[6];
};

