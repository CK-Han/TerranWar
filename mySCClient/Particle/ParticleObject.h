#pragma once

class CGameObject;

class CParticleObject
{
public:
	CParticleObject() : m_fParticleElapsedTime{ 0.0f }, m_pMyObject{ nullptr }, m_pTargetObject { nullptr } { D3DXMatrixIdentity(&m_d3dxmtxWorld); }
	virtual ~CParticleObject() {}

public:
	D3DXMATRIX			m_d3dxmtxWorld;
	float				m_fParticleElapsedTime{ 0.0f };

	CGameObject*		m_pMyObject{ nullptr };
	CGameObject*		m_pTargetObject{ nullptr };

	virtual bool UpdateAndGetValid(float fElapsedTime) { return true; }
};



//투사체 파티클 객체
class CProjectileParticleObject : public CParticleObject
{
public:
	CProjectileParticleObject();
	virtual ~CProjectileParticleObject();

	virtual bool UpdateAndGetValid(float fElapsedTime);

public:
	float			m_fProjectileSpeed{ 15.0f };
};



//즉시 발생하는 파티클 객체
class CPromptParticleObject : public CParticleObject
{
public:
	CPromptParticleObject();
	virtual ~CPromptParticleObject();

	virtual bool UpdateAndGetValid(float fElapsedTime);

public:
	float			m_fMaxParticleTime{ 0.4f };
	
	//0901
	float			m_fRandXOffset{ 0.0f };
	float			m_fRandYOffset{ 0.0f };
	float			m_fRandZOffset{ 0.0f };
};