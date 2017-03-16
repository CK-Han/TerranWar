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
	
	//ProgressImage는 스크린 좌표, HP 이미지는 월드좌표를 받게 된다. 이를 구분하는 것은 w값이 0.0f 여부로 확인하도록 함.
	void		RenderProgressImage(ID3D11DeviceContext* pd3dDeviceContext, float fProgress);
	void		RenderHPImage(ID3D11DeviceContext* pd3dDeviceContext, CGameObject** ppObjects, int nValidObject);

	D3DXVECTOR3		m_d3dxvLeftBottom;
	D3DXVECTOR3		m_d3dxvRightTop;

protected:
	ID3D11Buffer*					m_pd3dBarBuffer{ nullptr };
	
	//0904
	ID3D11Buffer*					m_pd3dHPBuffer{ nullptr };
	
	ID3D11ShaderResourceView*		m_pd3dSRVProgressBar{ nullptr };	//0~0.5까지 바, 0.5~1.0까지 프레임
	ID3D11ShaderResourceView*		m_pd3dSRVHPBar{ nullptr };	//0.25f를 범위로 초록 노랑 빨강 검정
	ID3D11SamplerState*				m_pd3dProgressSamplerState{ nullptr };

	ID3D11VertexShader*				m_pd3dProgressVertexShader{ nullptr };
	ID3D11InputLayout*				m_pd3dProgressInputLayout{ nullptr };
	ID3D11PixelShader*				m_pd3dProgressPixelShader{ nullptr };
};

