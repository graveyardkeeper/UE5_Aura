// There is no fucking Copyright


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
}

void USpellMenuWidgetController::SpellSelected(const FGameplayTag& AbilityTag)
{
	FGameplayTag StatusTag = FAuraGameplayTags::Get().Ability_Status_Locked;

	if (AbilityTag.IsValid())
	{
		if (const FGameplayAbilitySpec* Spec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag))
		{
			StatusTag = GetAuraASC()->GetAbilityStatusFromSpec(*Spec);
		}
	}

	bool bShouldEnableSpendPoints = false;
	bool bShouldEnableEquipSpell = false;
	ShouldEnableButtons(StatusTag, GetAuraPS()->GetPlayerSpellPoints(), bShouldEnableSpendPoints, bShouldEnableEquipSpell);

	OnSpellSelectedDelegate.Broadcast(bShouldEnableSpendPoints, bShouldEnableEquipSpell);
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& StatusTag, int32 SpellPoints, bool& bShouldEnableSpendPoints, bool& bShouldEnableEquipSpell)
{
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();

	if (StatusTag.MatchesTagExact(AuraTags.Ability_Status_Locked))
	{
		bShouldEnableSpendPoints = false;
		bShouldEnableEquipSpell = false;
	}
	else if (StatusTag.MatchesTagExact(AuraTags.Ability_Status_Eligible))
	{
		bShouldEnableSpendPoints = SpellPoints > 0;
		bShouldEnableEquipSpell = false;
	}
	else if (StatusTag.MatchesTagExact(AuraTags.Ability_Status_Unlocked))
	{
		bShouldEnableSpendPoints = SpellPoints > 0;
		bShouldEnableEquipSpell = true;
	}
	else if (StatusTag.MatchesTagExact(AuraTags.Ability_Status_Equipped))
	{
		bShouldEnableSpendPoints = SpellPoints > 0;
		bShouldEnableEquipSpell = true;
	}
}
