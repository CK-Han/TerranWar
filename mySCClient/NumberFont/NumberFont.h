#pragma once

#define		MAX_NUMBER_DIGIT		10
#define		MAX_NUM_FONT			30
#define		INVALIDATE_FONT			-1

#include "../Shader.h"


class CNumberFont
{
	struct FONT_DATA
	{
		D3DXVECTOR4	posColor;
		D3DXVECTOR2	texCoord;
	};

private:
	static CNumberFont*		instance;

	CNumberFont();
	~CNumberFont();

public:
	static CNumberFont* GetInstance()
	{
		if (nullptr == instance) instance = new CNumberFont();
		return instance;
	}

	void DeleteInstance()
	{
		if (CNumberFont::GetInstance())
		{
			delete CNumberFont::GetInstance();
			CNumberFont::instance = nullptr;
		}
	}

public:
	void		CreateShader(ID3D11Device* pd3dDevice);
	void		CreateResource(ID3D11Device* pd3dDevice);
	void		AddNumber(string& str, D3DXVECTOR4& screenPos, int number);
	void		DelNumber(string& str);
	void		InvalidateNumber(string& str);
	void		InvalidateAll();
	void		UpdateNumber(string& str, int number);
	
	void		Render(ID3D11DeviceContext* pd3dDeviceContext, float clientWidth, float clientHeight);

private:
	//pos, color(w) / number
	map<string, pair<D3DXVECTOR4, int>>	m_mapNumberData;

	ID3D11Buffer*					m_pd3dNumberBuffer{ nullptr };
	//ID3D11Buffer*					m_pd3dNumberIndexBuffer{ nullptr };
	ID3D11ShaderResourceView*		m_pd3dSRVNumber{ nullptr };
	ID3D11SamplerState*				m_pd3dSamplerState{ nullptr };

	ID3D11VertexShader*				m_pd3dVertexShader{ nullptr };
	ID3D11InputLayout*				m_pd3dVertexLayout{ nullptr };
	ID3D11PixelShader*				m_pd3dPixelShader{ nullptr };

	//test
	ID3D11RasterizerState*			m_pd3dRS{ nullptr };

};

