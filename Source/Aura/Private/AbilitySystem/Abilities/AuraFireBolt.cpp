// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
	const int32 ProjectileNum = FMath::Min(Level, MaxProjectilesNum);
	return FString::Printf(TEXT(
		"<Title>FIRE BOLT</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Launches </><Amount>%d</><Default> bolt of fire, "
		"exploding on impact and dealing </><Damage>%d</><Default> "
		"fire damage with a chance to burn</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       ProjectileNum,
	                       Damage
	);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
	const int32 ProjectileNum = FMath::Min(Level, MaxProjectilesNum);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL:</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Launches </><Amount>%d</><Default> bolt of fire, "
		"exploding on impact and dealing </><Damage>%d</><Default> "
		"fire damage with a chance to burn</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       ProjectileNum,
	                       Damage
	);
}
