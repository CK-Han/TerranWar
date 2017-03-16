#pragma once

#include "../Shader.h"

class CProgressImage
{
	struct PROGRESS_VERTEX
	{
		D3DXVECTOR4 posColor;
		D3DXVECTOR2 texCoord;
	};

public:
	CProgressImage();
	~CProgressImage();

	void		CreateShader(ID3D11Device* pd3dDevice);
	void		CreateResource(ID3D11Device* pd3dDevice);
	
	//ProgressImage�� ��ũ�� ��ǥ, HP �̹����� ������ǥ�� �ް� �ȴ�. �̸� �����ϴ� ���� w���� 0.0f ���η� Ȯ���ϵ��� ��.
	void		RenderProgressImage(ID3D11DeviceContext* pd3dDeviceContext, float fProgress);
	void		RenderHPImage(ID3D11DeviceContext* pd3dDeviceContext, CGameObject** ppObjects, int nValidObject);

	D3DXVECTOR3		m_d3dxvLeftBottom;
	D3DXVECTOR3		m_d3dxvRightTop;

protected:
	ID3D11Buffer*					m_pd3dBarBuffer{ nullptr };
	
	//0904
	ID3D11Buffer*					m_pd3dHPBuffer{ nullptr };
	
	ID3D11ShaderResourceView*		m_pd3dSRVProgressBar{ nullptr };	//0~0.5���� ��, 0.5~1.0���� ������
	ID3D11ShaderResourceView*		m_pd3dSRVHPBar{ nullptr };	//0.25f�� ������ �ʷ� ��� ���� ����
	ID3D11SamplerState*				m_pd3dProgressSamplerState{ nullptr };

	ID3D11VertexShader*				m_pd3dProgressVertexShader{ nullptr };
	ID3D11InputLayout*				m_pd3dProgressInputLayout{ nullptr };
	ID3D11PixelShader*				m_pd3dProgressPixelShader{ nullptr };
};

