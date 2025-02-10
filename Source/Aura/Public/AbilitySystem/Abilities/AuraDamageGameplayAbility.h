// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void CauseDamage(AActor* TargetActor, bool bIgnoreDebuff = false, FVector RadialDamageOrigin = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	FDamageEffectParams MakeDamageParamsFromClassDefaults(AActor* TargetActor = nullptr, FVector RadialDamageOrigin = FVector::ZeroVector) const;

protected:
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/** 伤害类型 Damage.*标签 */
	UPROPERTY(EditDefaultsOnly, Category="Damage", meta=(Categories="Damage"))
	FGameplayTag DamageType;

	/** 伤害量 */
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FScalableFloat Damage;

	/** Debuff概率 */
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffChance = 0.f;

	/** Debuff伤害 */
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffDamage = 0.f;

	/** Debuff频率 */
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffFrequency = 0.f;

	/** Debuff持续时间 */
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DebuffDuration = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float DeathImpulseMagnitude = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockbackChance = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	float KnockbackForceMagnitude = 500.f;

	UPROPERTY(EditDefaultsOnly, Category="Damage")
	bool bIsRadialDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float RadialDamageInnerRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	float RadialDamageOuterRadius = 0.f;
};
