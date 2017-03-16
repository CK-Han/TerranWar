#pragma once

#include "HeightMap.h"

struct DebugGridVertex
{
	D3DXVECTOR3 position;
	D3DXCOLOR color;
};

class CDebugPrograms
{
public:
	CDebugPrograms(ID3D11Device *pd3dDevice, CHeightMap *pHeightMap);
	~CDebugPrograms();

	void Render(ID3D11DeviceContext *pd3dDeviceContext);

protected:
	D3D11_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology;	//line
	ID3D11Buffer					*m_pd3dPositionBuffer;

	ID3D11VertexShader				*m_pd3dVertexShader;
	ID3D11InputLayout				*m_pd3dInputLayout;
	ID3D11PixelShader				*m_pd3dPixelShader;

	int								m_nVertices;
};

