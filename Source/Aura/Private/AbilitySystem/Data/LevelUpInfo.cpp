// There is no fucking Copyright


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 InXP)
{
	int32 Level = 0;
	// index = 0的位置是占位的
	for (int32 i = 0; i < LevelUpInformation.Num(); i++)
	{
		if (InXP >= LevelUpInformation[i].LevelRequirementXP)
		{
			Level = i;
		}
		else
		{
			break;
		}
	}
	return Level;
}
