// There is no fucking Copyright


#include "AbilitySystem/Abilities/Electrocute.h"

FString UElectrocute::GetDescription(int32 Level)
{
	const int32 DamageAmount = Damage.GetValueAtLevel(Level);
	const int32 AdditionalTargets = FMath::Min(Level - 1, MaxNumShockTargets) - 1;
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			"<Title>ELECTROCUTE</>\n\n"

			"<Small>Level: </><Level>%d</>\n"
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			"<Default>Emits a beam of lightning, connecting with the target, "
			"repeatedly causing </><Damage>%d</><Default> "
			"lightning damage with a chance to stun.</>"),
		                       Level,
		                       GetManaCost(Level),
		                       GetCooldown(Level),
		                       DamageAmount
		);
	}
	return FString::Printf(TEXT(
		"<Title>ELECTROCUTE</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Emits a beam of lightning, connecting with the target, "
		"propagating to </><Amount>%d</><Default> additional targets, "
		"repeatedly causing </><Damage>%d</><Default> "
		"lightning damage with a chance to stun.</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       AdditionalTargets,
	                       DamageAmount
	);
}

FString UElectrocute::GetNextLevelDescription(int32 Level)
{
	const int32 DamageAmount = Damage.GetValueAtLevel(Level);
	const int32 AdditionalTargets = FMath::Min(Level - 1, MaxNumShockTargets);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL:</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Emits a beam of lightning, connecting with the target, "
		"propagating to </><Amount>%d</><Default> additional targets, "
		"repeatedly causing </><Damage>%d</><Default> "
		"lightning damage with a chance to stun.</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       AdditionalTargets,
	                       DamageAmount
	);
}
