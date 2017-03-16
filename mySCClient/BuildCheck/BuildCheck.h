#pragma once
#include "../Shader.h"

#define		INVALIDATE_BUILDCHECK		-1

class CBuildCheck
{
	//��츦 �������ڸ�...
	//1. ������ �׷����� �ϳ� -> ������ ���� ��� SRV�� �µǾ�� �ϴ��� �Ǵ�
	//2. ũ��� ��Ѱ� -> �� ���� ������ ���ؼ� Ȯ��, ������۷� �൵ ������ ���۸� map�ؾ� �� �� ������?
	//3. ���� �׷����� �ϴ°� -> �̴� Scene���� ó���ϵ��� �Ѵ�.
	
	struct CREATE_BUILDING_VERTEX
	{
		D3DXVECTOR4 posSize;
	};


private:
	static CBuildCheck*		instance;

	CBuildCheck();
	~CBuildCheck();

public:
	static CBuildCheck* GetInstance()
	{
		if (nullptr == instance) instance = new CBuildCheck();
		return instance;
	}

	void DeleteInstance()
	{
		if (CBuildCheck::GetInstance())
		{
			delete CBuildCheck::GetInstance();
			CBuildCheck::instance = nullptr;
		}
	}

public:
	void		CreateShader(ID3D11Device* pd3dDevice);
	void		CreateResource(ID3D11Device* pd3dDevice);
	void		Render(ID3D11DeviceContext* pd3dDeviceContext);

	void		SetBuildingImage(string buildingName, D3DXVECTOR3 pos);
	void		InvalidateBuildingImage();
	
	bool		IsInvalidate() { return (m_nCurrentTexture == INVALIDATE_BUILDCHECK); }

	D3DXVECTOR4		m_d3dxvPosSize;

private:
	//				slot, size
	map<string, pair<int, float>>	m_mapBuildingTextureBox;
	ID3D11Buffer*					m_pd3dVertexBuffer{ nullptr };
	
	ID3D11ShaderResourceView**		m_ppBuildingTextures{ nullptr };
	int								m_nTextures{ 0 };
	int								m_nCurrentTexture{ -1 };

	ID3D11VertexShader*				m_pd3dVertexShader{ nullptr };
	ID3D11InputLayout*				m_pd3dVertexLayout{ nullptr };
	ID3D11PixelShader*				m_pd3dPixelShader{ nullptr };
	ID3D11GeometryShader*			m_pd3dGeometryShader{ nullptr };
};

