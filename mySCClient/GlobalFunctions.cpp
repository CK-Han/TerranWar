#include "GlobalFunctions.h"
#include "stdafx.h"


bool isAnimateObject(int type)
{
	if (ANIMATE_OBJECT_START <= type && type <= ANIMATE_OBJECT_END)
		return true;
	else if (STATIC_OBJECT_START <= type && type <= STATIC_OBJECT_END)
		return false;
	else
	{
		cprintf("Invalid Object Type : CScene::isAnimateObject\n");
		return false;
	}
}

bool isEnemyObject(int type)
{
	bool bAnimateObject = isAnimateObject(type);

	int my_type = type - ADD_ENEMY_INDEX;
	if (bAnimateObject)
	{
		if (ANIMATE_OBJECT_START <= my_type)	//1이 scv니까
			return true;
		else //0보다 작다 -> 아군 유닛이었다.
			return false;
	}
	else //static
	{
		if (STATIC_OBJECT_START <= my_type)
			return true;
		else
			return false;
	}
}

bool isAirObject(int type)
{
	bool bAnimateObject = isAnimateObject(type);
	if (bAnimateObject)
	{
		int my_type = type;
		if (isEnemyObject(type))
			my_type -= ADD_ENEMY_INDEX;

		if (AIR_OBJECT_START <= my_type && my_type <= AIR_OBJECT_END)
			return true;
		else
			return false;
	}
	else
		return false;
}

int		getSoundOffsetByType(int type)
{
	int my_type = type;
	if (isEnemyObject(type))
		my_type -= ADD_ENEMY_INDEX;

	int retVal;
	switch ((OBJECT_TYPE)my_type)
	{
	case OBJECT_TYPE::Banshee:
		retVal = start_offset::banshee; break;
	case OBJECT_TYPE::BattleCruiser:
		retVal = start_offset::battlecruiser; break;
	case OBJECT_TYPE::Ghost:
		retVal = start_offset::ghost; break;
	case OBJECT_TYPE::Hellion:
		retVal = start_offset::hellion; break;
	case OBJECT_TYPE::Marauder:
		retVal = start_offset::marauder; break;
	case OBJECT_TYPE::Marine:
		retVal = start_offset::marine; break;
	case OBJECT_TYPE::Medivac:
		retVal = start_offset::medivac; break;
	case OBJECT_TYPE::Mule:
		retVal = start_offset::mule; break;
	case OBJECT_TYPE::Raven:
		retVal = start_offset::raven; break;
	case OBJECT_TYPE::Reaper:
		retVal = start_offset::reaper; break;
	case OBJECT_TYPE::Scv:
		retVal = start_offset::scv; break;
	case OBJECT_TYPE::Siegetank:
		retVal = start_offset::siegetank; break;
	case OBJECT_TYPE::Thor:
		retVal = start_offset::thor; break;
	case OBJECT_TYPE::Viking:
		retVal = start_offset::viking; break;

	case OBJECT_TYPE::Armory:
		retVal = start_offset::armory; break;
	case OBJECT_TYPE::Barrack:
		retVal = start_offset::barrack; break;
	case OBJECT_TYPE::Bunker:
		retVal = start_offset::bunker; break;
	case OBJECT_TYPE::Commandcenter:
		retVal = start_offset::commandcenter; break;
	case OBJECT_TYPE::Engineeringbay:
		retVal = start_offset::engineeringbay; break;
	case OBJECT_TYPE::Factory:
		retVal = start_offset::factory; break;
	case OBJECT_TYPE::Fusioncore:
		retVal = start_offset::fusioncore; break;
	case OBJECT_TYPE::Ghostacademy:
		retVal = start_offset::ghostacademy; break;
	case OBJECT_TYPE::Missileturret:
		retVal = start_offset::missileturret; break;
	case OBJECT_TYPE::Reactor:
		retVal = start_offset::reactor; break;
	case OBJECT_TYPE::Refinery:
		retVal = start_offset::refinery; break;
	case OBJECT_TYPE::Sensortower:
		retVal = start_offset::sensortower; break;
	case OBJECT_TYPE::Starport:
		retVal = start_offset::starport; break;
	case OBJECT_TYPE::Supplydepot:
		retVal = start_offset::supplydepot; break;
	case OBJECT_TYPE::Techlab:
		retVal = start_offset::techlab; break;

	default: retVal = 3;
	}

	return retVal;
}


string		getNameByType(int type)
{
	int my_type = type;
	if (isEnemyObject(type))
		my_type -= ADD_ENEMY_INDEX;

	string retString;
	switch ((OBJECT_TYPE)my_type)
	{
	case OBJECT_TYPE::Banshee:
		retString = string("banshee"); break;
	case OBJECT_TYPE::BattleCruiser:
		retString = string("battlecruiser"); break;
	case OBJECT_TYPE::Ghost:
		retString = string("ghost"); break;
	case OBJECT_TYPE::Hellion:
		retString = string("hellion"); break;
	case OBJECT_TYPE::Marauder:
		retString = string("marauder"); break;
	case OBJECT_TYPE::Marine:
		retString = string("marine"); break;
	case OBJECT_TYPE::Medivac:
		retString = string("medivac"); break;
	case OBJECT_TYPE::Mule:
		retString = string("mule"); break;
	case OBJECT_TYPE::Raven:
		retString = string("raven"); break;
	case OBJECT_TYPE::Reaper:
		retString = string("reaper"); break;
	case OBJECT_TYPE::Scv:
		retString = string("scv"); break;
	case OBJECT_TYPE::Siegetank:
		retString = string("siegetank"); break;
	case OBJECT_TYPE::Thor:
		retString = string("thor"); break;
	case OBJECT_TYPE::Viking:
		retString = string("viking"); break;

	case OBJECT_TYPE::Armory:
		retString = string("armory"); break;
	case OBJECT_TYPE::Barrack:
		retString = string("barrack"); break;
	case OBJECT_TYPE::Bunker:
		retString = string("bunker"); break;
	case OBJECT_TYPE::Commandcenter:
		retString = string("commandcenter"); break;
	case OBJECT_TYPE::Engineeringbay:
		retString = string("engineeringbay"); break;
	case OBJECT_TYPE::Factory:
		retString = string("factory"); break;
	case OBJECT_TYPE::Fusioncore:
		retString = string("fusioncore"); break;
	case OBJECT_TYPE::Ghostacademy:
		retString = string("ghostacademy"); break;
	case OBJECT_TYPE::Missileturret:
		retString = string("missileturret"); break;
	case OBJECT_TYPE::Reactor:
		retString = string("reactor"); break;
	case OBJECT_TYPE::Refinery:
		retString = string("refinery"); break;
	case OBJECT_TYPE::Sensortower:
		retString = string("sensortower"); break;
	case OBJECT_TYPE::Starport:
		retString = string("starport"); break;
	case OBJECT_TYPE::Supplydepot:
		retString = string("supplydepot"); break;
	case OBJECT_TYPE::Techlab:
		retString = string("techlab"); break;

	default: retString = string("");
	}

	return retString;
}

void getNeedResourceForCreate(int type, int& mineral, int& gas)
{
	int my_type = type;
	if (isEnemyObject(type))
		my_type -= ADD_ENEMY_INDEX;

	switch ((OBJECT_TYPE)my_type)
	{
	case OBJECT_TYPE::Banshee:
		mineral = 150; gas = 100; break;
	case OBJECT_TYPE::BattleCruiser:
		mineral = 400; gas = 300; break;
	case OBJECT_TYPE::Ghost:
		 break;
	case OBJECT_TYPE::Hellion:
		 break;
	case OBJECT_TYPE::Marauder:
		mineral = 100; gas = 25; break;
	case OBJECT_TYPE::Marine:
		mineral = 50; gas = 0; break;
	case OBJECT_TYPE::Medivac:
		mineral = 100; gas = 100; break;
	case OBJECT_TYPE::Mule:
		 break;
	case OBJECT_TYPE::Raven:
		mineral = 100; gas = 200; break;
	case OBJECT_TYPE::Reaper:
		mineral = 50; gas = 50; break;
	case OBJECT_TYPE::Scv:
		mineral = 50; gas = 0; break;
	case OBJECT_TYPE::Siegetank:
		 break;
	case OBJECT_TYPE::Thor:
		mineral = 300; gas = 200; break;
	case OBJECT_TYPE::Viking:
		mineral = 150; gas = 75; break;

	case OBJECT_TYPE::Armory:
		mineral = 150; gas = 100; break;
	case OBJECT_TYPE::Barrack:
		mineral = 150; gas = 0; break;
	case OBJECT_TYPE::Bunker:
		mineral = 100; gas = 0; break;
	case OBJECT_TYPE::Commandcenter:
		mineral = 400; gas = 0; break;
	case OBJECT_TYPE::Engineeringbay:
		mineral = 125; gas = 0;	break;
	case OBJECT_TYPE::Factory:
		mineral = 200; gas = 50; break;
	case OBJECT_TYPE::Fusioncore:
		mineral = 200; gas = 200; break;
	case OBJECT_TYPE::Ghostacademy:
		mineral = 150; gas = 50; break;
	case OBJECT_TYPE::Missileturret:
		mineral = 100; gas = 0; break;
	case OBJECT_TYPE::Reactor:
		 break;
	case OBJECT_TYPE::Refinery:
		mineral = 75; gas = 0; break;
	case OBJECT_TYPE::Sensortower:
		mineral = 100; gas = 50; break;
	case OBJECT_TYPE::Starport:
		mineral = 150; gas = 100; break;
	case OBJECT_TYPE::Supplydepot:
		mineral = 100; gas = 0; break;
	case OBJECT_TYPE::Techlab:
		 break;
	}
}

int		gGameMineral = 0;
int		gGameGas = 0;
int		nPacketCount = 0;
void*   gGameCamera = nullptr;



/*
switch ((OBJECT_TYPE)my_type)
{
case OBJECT_TYPE::Banshee:
break;
case OBJECT_TYPE::BattleCruiser:
break;
case OBJECT_TYPE::Ghost:
break;
case OBJECT_TYPE::Hellion:
break;
case OBJECT_TYPE::Marauder:
break;
case OBJECT_TYPE::Marine:
break;
case OBJECT_TYPE::Medivac:
break;
case OBJECT_TYPE::Mule:
break;
case OBJECT_TYPE::Raven:
break;
case OBJECT_TYPE::Reaper:
break;
case OBJECT_TYPE::Scv:
break;
case OBJECT_TYPE::Siegetank:
break;
case OBJECT_TYPE::Thor:
break;
case OBJECT_TYPE::Viking:
break;

case OBJECT_TYPE::Armory:
break;
case OBJECT_TYPE::Barrack:
break;
case OBJECT_TYPE::Bunker:
break;
case OBJECT_TYPE::Commandcenter:
break;
case OBJECT_TYPE::Engineeringbay:
break;
case OBJECT_TYPE::Factory:
break;
case OBJECT_TYPE::Fusioncore:
break;
case OBJECT_TYPE::Ghostacademy:
break;
case OBJECT_TYPE::Missileturret:
break;
case OBJECT_TYPE::Reactor:
break;
case OBJECT_TYPE::Refinery:
break;
case OBJECT_TYPE::Sensortower:
break;
case OBJECT_TYPE::Starport:
break;
case OBJECT_TYPE::Supplydepot:
break;
case OBJECT_TYPE::Techlab:
break;
}
*/