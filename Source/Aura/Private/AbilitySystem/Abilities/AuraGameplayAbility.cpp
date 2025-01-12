// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

float UAuraGameplayAbility::GetManaCost(int32 Level) const
{
	const UGameplayEffect* CostEffect = GetCostGameplayEffect();
	if (!CostEffect)
	{
		return 0.f;
	}
	float ManaCost = 0.f;
	for (const FGameplayModifierInfo& Modifier : CostEffect->Modifiers)
	{
		if (Modifier.Attribute == UAuraAttributeSet::GetManaAttribute())
		{
			Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(Level, ManaCost);
			break;
		}
	}
	return FMath::Abs(ManaCost);
}

float UAuraGameplayAbility::GetCooldown(int32 Level) const
{
	const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	if (!CooldownEffect)
	{
		return 0.f;
	}
	float Cooldown = 0.f;
	CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(Level, Cooldown);
	return Cooldown;
}

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s</>\n\n<Small>Level: </><Level>%d</>"), L"{{Spell Name}}", Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Next Level: </><Level>%d</>\n<Default>Causes much more damage</>"), Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level </><Level>%d</>"), Level);
}
