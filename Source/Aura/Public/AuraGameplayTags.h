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
	 * Damage Type Resistance
	 */
	FGameplayTag Attribute_Resistance_Fire;
	FGameplayTag Attribute_Resistance_Lightning;
	FGameplayTag Attribute_Resistance_Arcane;
	FGameplayTag Attribute_Resistance_Physical;

	/**
	 * Meta Attribute
	 */
	FGameplayTag Attribute_Meta_IncomingXP;

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
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	TMap<FGameplayTag, FGameplayTag> DamageTypes2Resistances;


	/**
	 * Effect Tags
	 */
	FGameplayTag Effect_HitReact;


	/**
	 * Ability Tags
	 */
	FGameplayTag Ability_Attack;
	FGameplayTag Ability_Summon;
	FGameplayTag Ability_Fire_FireBolt;


	/**
	 * Ability Tags
	 */
	FGameplayTag Cooldown_Fire_FireBolt;


	/**
	 * Combat Socket Tags
	 */
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_Tail;

	/**
	 * Montage Tags
	 */
	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

private:
	static FAuraGameplayTags Singleton;
};
