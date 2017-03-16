#pragma once

#define			MAX_SOUND_CHANNEL						8
#define			BACKGROUND_CHANNEL						0
#define			UNIT_WHAT_CHANNEL						1

#define			UNIT_READY_OFFSET						0				
#define			UNIT_DEATH_OFFSET						1				
#define			UNIT_ATTACK_OFFSET						2				
#define			UNIT_WHAT_OFFSET						3

enum start_offset {
	not_enough_food = 2,
	not_enough_mineral = 3,
	not_enough_gas = 4,
	cannot_build = 5,
	job_finish = 6,

	armory = 7,
	barrack = 9,
	bunker = 11,
	commandcenter = 13,
	engineeringbay = 15,
	factory = 17,
	fusioncore = 19,
	ghostacademy = 21,
	missileturret = 23,
	reactor = 25,
	refinery = 27,
	sensortower = 29,
	starport = 31,
	supplydepot = 33,
	techlab = 35,

	banshee = 37,
	battlecruiser = 43,
	ghost = 49,
	hellion = 55,
	marauder = 61,
	marine = 67,
	medivac = 73,
	mule = 79,
	raven = 85,
	reaper = 91,
	scv = 97,
	siegetank = 103,
	thor = 109,
	viking = 115
};


#include "FMOD\inc\fmod.hpp"
#include <fstream>
#include <string>
#include <map>

#pragma comment(lib, "fmodex_vc.lib")
#pragma comment(lib, "fmodex64_vc.lib")
using namespace std;

class CSoundManager
{

private:
	static CSoundManager		*instance;

	CSoundManager();
	~CSoundManager();

public:
	static CSoundManager*	GetInstance()
	{
		if (nullptr == instance)	instance = new CSoundManager();
		return instance;
	}

	void DeleteInstance() { delete CSoundManager::GetInstance(); }

	void		CreateSound(string& fileList);
	void		PlayBackgroundSound();
	void		PlayEffectSound(string str);

	//str은 항상 what0를 입력하도록 한다.
	void		PlayUnitWhatSound(int nIndex);
	void		StopSound(string str);
	void		PlayEffectSound(int nIndex);
	void		StopSound(int nIndex);

	void		SetVolume(int channelIndex, float volume);

	void		ReleaseSound();
	void		UpdateSound();

	
public:
	map<string, int>		m_mapSound;

private:
	FMOD::System*			m_pSystem;
	FMOD::Sound**			m_ppSound;
	FMOD::Channel**			m_ppChannel;

	int						m_nSoundCount;
	int						m_nBGMIndex;
};

