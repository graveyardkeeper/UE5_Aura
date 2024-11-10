// There is no fucking Copyright


#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::Singleton;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Strength"),
		FString("Increases physical damage")
	);

	Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Intelligence"),
		FString("Increases magical damage")
	);

	Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Resilience"),
		FString("Increases Armor and Armor Penetration")
	);

	Attribute_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Primary.Vigor"),
		FString("Increases Health")
	);

	Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.Armor"),
		FString("Reduces damage taken, improves Block Chance.")
	);

	Attribute_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.ArmorPenetration"),
		FString("Ignored percentage of enemy Armor, increases Critical Hit Chance")
	);

	Attribute_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.BlockChance"),
		FString("Chance to cut incoming damage in half")
	);

	Attribute_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.CriticalHitChance"),
		FString("Chance to double damage plus critical hit bonus")
	);

	Attribute_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.CriticalHitDamage"),
		FString("Bonus damage added when a critical hit scored")
	);

	Attribute_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.CriticalHitResistance"),
		FString("Reduces Critical Hit Chance of attacking enemies")
	);

	Attribute_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.HealthRegeneration"),
		FString("Amount of Health regenerated every 1 second")
	);

	Attribute_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.ManaRegeneration"),
		FString("Amount of Mana regenerated every 1 second")
	);

	Attribute_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.MaxHealth"),
		FString("Maximum amount of Health obtainable")
	);

	Attribute_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attribute.Secondary.MaxMana"),
		FString("Maximum amount if Mana obtainable")
	);
}
