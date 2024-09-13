// Copyright Jordan McWilliams


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(const int32 XP)
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		//LevelInfo[1] = Level 1 Information
		//LevelInfo[2] = Level 2 Information
		if (LevelInfo.Num() - 1 <= Level) return Level;

		if (XP >= LevelInfo[Level].LevelUpRequirement)
		{
			++Level;
		} else
		{
			bSearching = false;
		}
	}
	return Level;
}

bool ULevelUpInfo::CanLevelUp(const int32 Level, const int32 Experience)
{
	return LevelInfo[Level].LevelUpRequirement <= Experience;
}

FLevelInfo ULevelUpInfo::GetLevelInfo(const int32 Level)
{
	if (Level <= 0 || Level > LevelInfo.Num() - 1) return FLevelInfo();
	return LevelInfo[Level];
}
