#include "SoundManager.h"

CSoundManager* CSoundManager::instance = nullptr;

CSoundManager::CSoundManager()
{
	FMOD::System_Create(&m_pSystem);
	m_pSystem->init(MAX_SOUND_CHANNEL, FMOD_DEFAULT, nullptr);
}


CSoundManager::~CSoundManager()
{
	m_pSystem->release();
	m_pSystem->close();

	m_mapSound.clear();
}

void CSoundManager::CreateSound(string& fileList)
{
	int nCount;
	string name;
	string filename;
	bool isLoop;
	FMOD_MODE loopMode;

	string prefix = "SoundComponent/";
	string fullFileName;

	ifstream file{ fileList };
	if (file.is_open())
	{
		file >> nCount;
		m_nSoundCount = nCount;
		m_ppSound = new FMOD::Sound*[nCount];
		m_ppChannel = new FMOD::Channel*[nCount];

		for (int i = 0; i < nCount; ++i)
		{
			file >> name >> filename >> isLoop;
			loopMode = isLoop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT;
			fullFileName = prefix + filename;
			m_pSystem->createSound(fullFileName.data(), loopMode, nullptr, &m_ppSound[i]);
			m_mapSound.insert(make_pair(name, i));

			//cout << name << endl;
		}
	}
}

void CSoundManager::PlayBackgroundSound()
{
	int nBGM = rand() & 0x0001;
	m_nBGMIndex = nBGM;
	m_pSystem->playSound(FMOD_CHANNELINDEX::FMOD_CHANNEL_REUSE, m_ppSound[m_nBGMIndex], 0, &m_ppChannel[BACKGROUND_CHANNEL]);
}

void CSoundManager::PlayEffectSound(string str)
{
	auto p = m_mapSound.find(str);
	if (p != m_mapSound.end())
		PlayEffectSound(p->second);
}

void CSoundManager::PlayUnitWhatSound(int nIndex)
{
	bool isPlayed = false;
	m_ppChannel[UNIT_WHAT_CHANNEL]->isPlaying(&isPlayed);
	if (!isPlayed)
	{
		int seed = rand() % 3;
		int index = nIndex + seed;

		m_pSystem->playSound(FMOD_CHANNELINDEX::FMOD_CHANNEL_REUSE, m_ppSound[index], 0, &m_ppChannel[UNIT_WHAT_CHANNEL]);
	}
}

void CSoundManager::PlayEffectSound(int nIndex)
{
	if (nIndex < m_nSoundCount)
	{
		//m_pSystem->playSound(FMOD_CHANNEL_FREE, m_ppSound[nIndex], 0, &m_ppChannel[nIndex]);
		int randIndex = rand() % (MAX_SOUND_CHANNEL - 2) + 2; // bgm, unit_what
		m_pSystem->playSound(FMOD_CHANNEL_REUSE, m_ppSound[nIndex], 0, &m_ppChannel[randIndex]);
	}
}

void CSoundManager::StopSound(string str)
{
	auto p = m_mapSound.find(str);
	if (p != m_mapSound.end())
		StopSound(p->second);
}

void CSoundManager::StopSound(int nIndex)
{
	if (nIndex < m_nSoundCount)
	{
		m_ppChannel[nIndex]->stop();
	}
}

void CSoundManager::SetVolume(int channelIndex, float volume)
{
	float lVolume;
	if (volume > 1.0f) lVolume = 1.0f;
	else if (volume < 0.0f) lVolume = 0.0f;
	else lVolume = volume;

	if (channelIndex < MAX_SOUND_CHANNEL)
	{
		m_ppChannel[channelIndex]->setVolume(lVolume);
	}
}

void  CSoundManager::ReleaseSound()
{
	delete[] m_ppChannel;

	for (int i = 0; i < m_nSoundCount; ++i)
		m_ppSound[i]->release();
	delete[] m_ppSound;
}

void CSoundManager::UpdateSound()
{
	if (m_pSystem)	m_pSystem->update();
	
	/*
	bool bBGMPlayed;
	m_ppChannel[BACKGROUND_CHANNEL]->isPlaying(&bBGMPlayed);
	if (!bBGMPlayed)
	{
		m_pSystem->playSound(FMOD_CHANNELINDEX::FMOD_CHANNEL_REUSE, m_ppSound[m_nBGMIndex], 0, &m_ppChannel[BACKGROUND_CHANNEL]);
	}
	*/
}
