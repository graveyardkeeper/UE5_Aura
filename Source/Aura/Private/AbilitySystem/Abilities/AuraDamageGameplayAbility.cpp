// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor, bool bIgnoreDebuff, FVector RadialDamageOrigin)
{
	if (!IsValid(TargetActor))
	{
		return;
	}
	FDamageEffectParams Params = MakeDamageParamsFromClassDefaults(TargetActor, RadialDamageOrigin);
	if (bIgnoreDebuff)
	{
		Params.DebuffChance = 0.f;
	}
	UAuraAbilitySystemLibrary::ApplyDamageEffect(Params);
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageParamsFromClassDefaults(AActor* TargetActor, FVector RadialDamageOrigin) const
{
	FDamageEffectParams Params;

	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DebuffDuration = DebuffDuration;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackChance = KnockbackChance;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		Params.RadialDamageOrigin = RadialDamageOrigin;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
	}
	if (IsValid(TargetActor))
	{
		if (const bool bKnockback = FMath::RandRange(0.f, 100.f) <= KnockbackChance)
		{
			// 范围技能从中心向敌人方向击退，非范围技能从主角向敌人方向击退
			const FVector KnockbackOrigin = bIsRadialDamage ? RadialDamageOrigin : GetAvatarActorFromActorInfo()->GetActorLocation();
			FRotator Direction = (TargetActor->GetActorLocation() - KnockbackOrigin).Rotation();
			Direction.Pitch = 30.0f;
			Params.KnockbackForce = Direction.Vector() * KnockbackForceMagnitude;
			Params.DeathImpulse = Direction.Vector() * DeathImpulseMagnitude;
		}
	}

	return Params;
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(
	const TArray<FTaggedMontage>& TaggedMontages) const
{
	if (TaggedMontages.IsEmpty())
	{
		return FTaggedMontage();
	}
	return TaggedMontages[FMath::RandRange(0, TaggedMontages.Num() - 1)];
}
