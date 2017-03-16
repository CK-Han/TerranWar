#pragma once

#include "SoundComponent\SoundManager.h"

bool			isAnimateObject(int type);
bool			isEnemyObject(int type);
bool			isAirObject(int type);
int				getSoundOffsetByType(int type);
string			getNameByType(int type);
void			getNeedResourceForCreate(int type, int& mineral, int& gas);



//extern int nPacketCount;