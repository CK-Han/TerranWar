#include "stdafx.h"
#include "Timer.h"


//**********************************************************************
CGameTimer::CGameTimer()
{
	using namespace std;
	
	if(QueryPerformanceFrequency((LARGE_INTEGER *)&m_PerformanceFrequency))
	{
		m_bHardwareHasPerformanceCounter = TRUE;
		QueryPerformanceCounter((LARGE_INTEGER *)&m_nLastTime);
		m_fTimeScale = 1.0f / m_PerformanceFrequency;
	}
	else
	{
		m_bHardwareHasPerformanceCounter = FALSE;
		m_nLastTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
		m_fTimeScale = 1.0f;
	}

	m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_FramePerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
}

CGameTimer::~CGameTimer()
{
}


//**********************************************************************
//�� ������ Ÿ�̸Ӹ� �����Ѵ�.
void CGameTimer::Tick(float fLockFPS)
{
	using namespace std;

	if (m_bHardwareHasPerformanceCounter)
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentTime);
	}
	else
	{
		m_nCurrentTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
	}

	//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�.
	float fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;

	
	if (fLockFPS > 0.0f)
	{
		//�� �Լ��� �Ķ����(fLockFPS)�� 0���� ũ�� �� �ð���ŭ ȣ���� �Լ��� ��ٸ��� �Ѵ�.
		while (fTimeElapsed < (1.0f / fLockFPS))
		{
			if (m_bHardwareHasPerformanceCounter)
			{
				QueryPerformanceCounter((LARGE_INTEGER *)&m_nCurrentTime);
			}
			else
			{
				m_nCurrentTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now().time_since_epoch()).count();
			}
			//���������� �� �Լ��� ȣ���� ���� ����� �ð��� ����Ѵ�.
			fTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_fTimeScale;
		}
	}

	//���� �ð��� m_nLastTime�� �����Ѵ�.
	m_nLastTime = m_nCurrentTime;

	/* ������ ������ ó�� �ð��� ���� ������ ó�� �ð��� ���̰� 1�ʺ��� ������
	���� ������ ó�� �ð��� m_fFrameTime[0]�� �����Ѵ�. */
	if (fabsf(fTimeElapsed - m_fTimeElapsed) < 1.0f)
	{
		memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = fTimeElapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT) m_nSampleCount++;
	}

	//�ʴ� ������ ���� 1 ������Ű�� ���� ������ ó�� �ð��� �����Ͽ� �����Ѵ�.
	m_FramePerSecond++;
	m_fFPSTimeElapsed += fTimeElapsed;
	if (m_fFPSTimeElapsed > 1.0f)
	{
		m_nCurrentFrameRate = m_FramePerSecond;
		m_FramePerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	}

	//������ ������ ó�� �ð��� ����� ���Ͽ� ������ ó�� �ð��� ���Ѵ�.
	m_fTimeElapsed = 0.0f;
	for (ULONG i = 0; i < m_nSampleCount; i++) m_fTimeElapsed += m_fFrameTime[i];
	if (m_nSampleCount > 0) m_fTimeElapsed /= m_nSampleCount;
}

float CGameTimer::GetTimeElapsed()
{
	return(m_fTimeElapsed);
}

//**********************************************************************
//������ ������ ����Ʈ�� ���ڿ��� ���������� ��ȯ�Ѵ�.
unsigned long CGameTimer::GetFrameRate(LPTSTR lpszString, int nCharacters)
{
	//���� ������ ����Ʈ�� ���ڿ��� ��ȯ�Ͽ� lpszString ���ۿ� ���� �� FPS���� �����Ѵ�.
	if (lpszString)
	{
		char buf[50];
		_itoa_s(m_nCurrentFrameRate, buf, nCharacters, 10);
		//wcscat_s((WCHAR*)lpszString, nCharacters, (WCHAR*)" FPS)");
		int len = strlen(buf);
		strcat_s(&buf[len], 6, " FPS)");
		memcpy(lpszString, buf, strlen(buf));
	}

	return(m_nCurrentFrameRate);
}