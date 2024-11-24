// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 
 */
class AURA_API FAuraGameplayTags
{
public:
	static FAuraGameplayTags& Get() { return Singleton; };

	void InitializeNativeGameplayTags();

	/**
	 * Attributes tags
	 */
	FGameplayTag Attribute_Primary_Strength;
	FGameplayTag Attribute_Primary_Intelligence;
	FGameplayTag Attribute_Primary_Resilience;
	FGameplayTag Attribute_Primary_Vigor;

	FGameplayTag Attribute_Secondary_Armor;
	FGameplayTag Attribute_Secondary_ArmorPenetration;
	FGameplayTag Attribute_Secondary_BlockChance;
	FGameplayTag Attribute_Secondary_CriticalHitChance;
	FGameplayTag Attribute_Secondary_CriticalHitDamage;
	FGameplayTag Attribute_Secondary_CriticalHitResistance;
	FGameplayTag Attribute_Secondary_HealthRegeneration;
	FGameplayTag Attribute_Secondary_ManaRegeneration;
	FGameplayTag Attribute_Secondary_MaxHealth;
	FGameplayTag Attribute_Secondary_MaxMana;


	/**
	 * Input tags
	 */
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_Key1;
	FGameplayTag InputTag_Key2;
	FGameplayTag InputTag_Key3;
	FGameplayTag InputTag_Key4;


	/**
	 * Damage Tags
	 */
	FGameplayTag Damage;


	/**
	 * Effect Tags
	 */
	FGameplayTag Effect_HitReact;

private:
	static FAuraGameplayTags Singleton;
};
