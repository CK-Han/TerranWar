#pragma once

const ULONG MAX_SAMPLE_COUNT = 60; // 60ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();

	
	void Tick(float fLockFPS = 0.0f); //Ÿ�̸� �ð� ����
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); //������ ����Ʈ ��ȯ
	float GetTimeElapsed(); //�������� ��� ��� �ð��� ��ȯ


private:
	bool m_bHardwareHasPerformanceCounter;				
	float m_fTimeScale;
	float m_fTimeElapsed;
	__int64 m_nCurrentTime;
	__int64 m_nLastTime;
	__int64 m_PerformanceFrequency;

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_FramePerSecond;
	float m_fFPSTimeElapsed;
};
