// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraFireBlast.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 DamageAmount = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>FIRE BLAST</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Launches </><Amount>%d</><Default> fire balls in all directions, "
		"each coming back and exploding upon return, causing </><Damage>%d</><Default> "
		"fire damage with a chance to burn.</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       NumFireBalls,
	                       DamageAmount
	);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 DamageAmount = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL:</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Launches </><Amount>%d</><Default> fire balls in all directions, "
		"each coming back and exploding upon return, causing </><Damage>%d</><Default> "
		"fire damage with a chance to burn.</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       NumFireBalls,
	                       DamageAmount
	);
}
