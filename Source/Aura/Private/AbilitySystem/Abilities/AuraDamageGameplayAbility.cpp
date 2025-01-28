// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return;
	}
	UAuraAbilitySystemLibrary::ApplyDamageEffect(MakeDamageParamsFromClassDefaults(TargetActor));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageParamsFromClassDefaults(AActor* TargetActor) const
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
	if (IsValid(TargetActor))
	{
		if (const bool bKnockback = FMath::RandRange(0.f, 100.f) <= KnockbackChance)
		{
			FRotator Direction = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
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
