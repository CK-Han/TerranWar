#pragma once

#include "ParticleObject.h"

#define			MAX_PARTICLES			256

struct VS_SPRITE_PARTICLE_INSTANCE_DATA
{
	D3DXMATRIX		mtxWorld;
	float			elapsedTime;
	float			numSprites;
	float			betweenTime;
	float			particleSize;
};


//포인터를 이용해 캐스팅을 표현할 수도 있겠지만... 너무 많은 new delete가 손해라고 생각한다.
//고로 Particle 셰이더를 두 개 두는 것은 어떨까? 아니면 변수 하나를 두어 생성자에서 대입, 이 변수를 통해
//어떤 파티클 객체 리스트를 순회할지 정하는 방법도 있겠다. 일단 테스트로 투사체를 진행하겠다.
//-> 셰이더를 두 개 두면 중복되는 함수 코드가 너무 많다; 고로 list 2 개를 사용하되 변수를 통해 구분지어 사용해야겠다.
struct PARTICLE_DATA
{
	bool									m_bProjectile;
	ID3D11ShaderResourceView*				m_pd3dparticleTextureSRV{ nullptr };
	ID3D11Buffer*							m_pd3dParticleInstanceBuffer{ nullptr };
	list<CProjectileParticleObject>			m_listProjectileParticleObjects;
	list<CPromptParticleObject>				m_listPromptParticleObjects;
	float									m_fNumSprites{ 0.0f };
	float									m_fBetweenTime{ 0.0f };
	float									m_fParticleSize{ 0.0f };
	float									m_fPromptMaintainTime{ 0.4f };
	float									m_fProjectileSpeed{ 15.0f };

	void SetParticleData(ID3D11Device* pd3dDevice, bool isProjectile, float fNumSprites, float fBetweenTime, float fParticleSize, float fPromptMaintainTime, float fProjectileSpeed, LPCSTR fileName)
	{
		m_bProjectile = isProjectile;
		m_fNumSprites = fNumSprites;
		m_fBetweenTime = fBetweenTime;
		m_fParticleSize = fParticleSize;
	
		if (!isProjectile)
			m_fPromptMaintainTime = fPromptMaintainTime;
		else
			m_fProjectileSpeed = fProjectileSpeed;

		D3DX11CreateShaderResourceViewFromFile(pd3dDevice, fileName, NULL, NULL, &m_pd3dparticleTextureSRV, NULL);
		m_listProjectileParticleObjects.clear();
		m_listPromptParticleObjects.clear();

		//인스턴스 버퍼 생성
		D3D11_BUFFER_DESC d3dBufferDesc;
		ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
		d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		d3dBufferDesc.ByteWidth = sizeof(VS_SPRITE_PARTICLE_INSTANCE_DATA) * MAX_PARTICLES;
		d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pd3dDevice->CreateBuffer(&d3dBufferDesc, nullptr, &m_pd3dParticleInstanceBuffer);
	}

	void DeleteParticleData()
	{
		if (m_pd3dparticleTextureSRV) m_pd3dparticleTextureSRV->Release();
		if (m_pd3dParticleInstanceBuffer) m_pd3dParticleInstanceBuffer->Release();
		m_listProjectileParticleObjects.clear();
		m_listPromptParticleObjects.clear();
	}
};



class CSpriteParticle
{
private:
	static CSpriteParticle*		instance;

	CSpriteParticle();
	~CSpriteParticle();

public:
	static CSpriteParticle* GetInstance()
	{
		if (nullptr == instance) instance = new CSpriteParticle();
		return instance;
	}

	void DeleteInstance() 
	{
		if (CSpriteParticle::GetInstance())
		{
			delete CSpriteParticle::GetInstance();
			CSpriteParticle::instance = nullptr;
		}
	}
	
	virtual void BuildObject(ID3D11Device *pd3dDevice);
	virtual void CreateShader(ID3D11Device *pd3dDevice);

	virtual void AddProjectileParticle(string particleName, CProjectileParticleObject particle);
	virtual void AddPromptParticle(string particleName, CPromptParticleObject particle);
	virtual void AddParticleData(string particleName, PARTICLE_DATA particleData);

	virtual void Update(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);


protected:
	ID3D11VertexShader				*m_pd3dVertexShader{ nullptr };
	ID3D11InputLayout				*m_pd3dVertexLayout{ nullptr };
	ID3D11GeometryShader			*m_pd3dGeometryShader{ nullptr };
	ID3D11PixelShader				*m_pd3dPixelShader{ nullptr };

	ID3D11RasterizerState			*m_pd3dRasterizerState{ nullptr };
	ID3D11SamplerState				*m_pd3dParticleSampler{ nullptr };

	ID3D11Buffer					*m_pd3dParticlePositionBuffer{ nullptr };
	
public:
	map<string, PARTICLE_DATA>		m_mapParticleData;
};

