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
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	FDamageEffectParams MakeDamageParamsFromClassDefaults(AActor* TargetActor = nullptr) const;

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
	float DeathImpulseMagnitude = 60.f;
};
