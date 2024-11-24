// There is no fucking Copyright


#include "AbilitySystem/Data/CharacterClassInfo.h"

const FCharacterClassDefaultInfo& UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass) const
{
	return CharacterClassInfo.FindChecked(CharacterClass);
}
