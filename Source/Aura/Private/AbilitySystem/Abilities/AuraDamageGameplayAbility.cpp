// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle SpecHandle = MakeDamageEffectSpecHandle();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

FGameplayEffectSpecHandle UAuraDamageGameplayAbility::MakeDamageEffectSpecHandle()
{
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	// 对每个不同伤害类型，向SetByCaller的修饰符添加对应的tag和伤害值
	for (auto& DamageType : DamageTypes)
	{
		const float ScaledDamage = DamageType.Value.AsInteger(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType.Key, ScaledDamage);
	}
	return SpecHandle;
}

FGameplayEffectSpecHandle UAuraDamageGameplayAbility::MakeDamageEffectSpecHandle(
	const FGameplayEffectContextHandle& ContextHandle)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(),
	                                                                         ContextHandle);
	// 对每个不同伤害类型，向SetByCaller的修饰符添加对应的tag和伤害值
	for (auto& DamageType : DamageTypes)
	{
		const float ScaledDamage = DamageType.Value.AsInteger(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType.Key, ScaledDamage);
	}
	return SpecHandle;
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
