// There is no fucking Copyright


#include "AbilitySystem/Abilities/ArcaneShards.h"

FString UArcaneShards::GetDescription(int32 Level)
{
	const int32 DamageAmount = Damage.GetValueAtLevel(Level);
	const int32 NumShards = FMath::Min(Level, MaxNumShards);
	return FString::Printf(TEXT(
		"<Title>ARCANE SHARDS</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Summon </><Amount>%d</><Default> shards of arcane energy, "
		"causing radial arcane damage of </><Damage>%d</><Default> at the shard origin.</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       NumShards,
	                       DamageAmount
	);
}

FString UArcaneShards::GetNextLevelDescription(int32 Level)
{
	const int32 DamageAmount = Damage.GetValueAtLevel(Level);
	const int32 NumShards = FMath::Min(Level, MaxNumShards);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL:</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Summon </><Amount>%d</><Default> shards of arcane energy, "
		"causing radial arcane damage of </><Damage>%d</><Default> at the shard origin.</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       NumShards,
	                       DamageAmount
	);
}

int32 UArcaneShards::GetNumShards()
{
	return FMath::Min(GetAbilityLevel(), MaxNumShards);
}
