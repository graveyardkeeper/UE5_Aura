// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties()
	{
	}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;

	UPROPERTY()
	TObjectPtr<AActor> SourceAvatarActor;

	UPROPERTY()
	TObjectPtr<ACharacter> SourceCharacter;

	UPROPERTY()
	TObjectPtr<AController> SourceController;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC;

	UPROPERTY()
	TObjectPtr<AActor> TargetAvatarActor;

	UPROPERTY()
	TObjectPtr<ACharacter> TargetCharacter;

	UPROPERTY()
	TObjectPtr<AController> TargetController;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAuraAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/** All tags to attribute static method */
	TMap<FGameplayTag, FGameplayAttribute(*)()> TagToAttribute;

	/**
	 * Vital Attributes.
	 * */
	/** 生命 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health, Category="VitalAttributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);

	/** 魔法 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana, Category="VitalAttributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);


	/**
	 * Primary Attributes.
	 **/
	/** 力量 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Strength, Category="PrimaryAttributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);

	/** 智力 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Intelligence, Category="PrimaryAttributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence);

	/** 恢复力 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Resilience, Category="PrimaryAttributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience);

	/** 活力 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Vigor, Category="PrimaryAttributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor);

	/**
	 * Secondary Attributes.
	 **/
	/**
	 * 护甲: 减少伤害，提高格挡率
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Armor, Category="SecondaryAttributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor);

	/**
	 * 护甲穿透: 忽视敌方护甲，提高暴击率
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ArmorPenetration, Category="SecondaryAttributes")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration);

	/**
	 * 格挡率: 有几率减少即将到来的一半伤害
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BlockChance, Category="SecondaryAttributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance);

	/**
	 * 暴击率: 有机率造成额外伤害
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CriticalHitChance, Category="SecondaryAttributes")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitChance);

	/**
	 * 暴击伤害: 增加暴击时造成的额外伤害
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CriticalHitDamage, Category="SecondaryAttributes")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitDamage);

	/**
	 * 暴击抵抗: 减少敌方对自身造成伤害时的暴击机率
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CriticalHitResistance, Category="SecondaryAttributes")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitResistance);

	/**
	 * 生命回复: 每秒回复的生命值
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_HealthRegeneration, Category="SecondaryAttributes")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration);

	/**
	 * 魔法回复: 每秒回复的魔法值
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ManaRegeneration, Category="SecondaryAttributes")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration);

	/**
	 * 最大生命: 生命的最大值
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth, Category="SecondaryAttributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);

	/**
	 * 最大魔法: 魔法的最大值
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana, Category="SecondaryAttributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

	/**
	 * 伤害抵抗: Fire、Lightning、Arcane、Physical
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_FireResistance, Category="ResistanceAttributes")
	FGameplayAttributeData FireResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, FireResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_LightningResistance, Category="ResistanceAttributes")
	FGameplayAttributeData LightningResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, LightningResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ArcaneResistance, Category="ResistanceAttributes")
	FGameplayAttributeData ArcaneResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArcaneResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PhysicalResistance, Category="ResistanceAttributes")
	FGameplayAttributeData PhysicalResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, PhysicalResistance);


	/**
	 * Meta Attributes, NOTE: not replicated
	 */
	UPROPERTY(BlueprintReadOnly, Category="MetaAttributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage);


	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue) const;

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties) const;
	void ShowFloatingText(const FEffectProperties& Props, float Damage, bool bIsBlockedHit, bool bIsCriticalHit) const;
};
