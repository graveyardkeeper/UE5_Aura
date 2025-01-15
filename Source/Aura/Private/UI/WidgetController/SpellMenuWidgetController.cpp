// There is no fucking Copyright


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	GetAuraASC()->OnSpellEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnSpellEquipped);
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

	FString Description;
	FString NextLevelDescription;
	GetAuraASC()->GetDescriptionsByAbilityTag(AbilityTag, Description, NextLevelDescription);

	OnSpellSelectedDelegate.Broadcast(bShouldEnableSpendPoints, bShouldEnableEquipSpell, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpendSpellPoint(const FGameplayTag& AbilityTag)
{
	GetAuraASC()->ServerSpendSpellPoint(AbilityTag);
}

void USpellMenuWidgetController::OnEquipSpellClicked(const FGameplayTag& AbilityTag)
{
	// 点击装备技能按钮，通知选中技能的类型，让蓝图做动画
	WaitForEquipSpellDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
}

void USpellMenuWidgetController::EquipSpell(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	GetAuraASC()->ServerEquipSpell(AbilityTag, InputTag);
}

void USpellMenuWidgetController::OnSpellEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag)
{
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.AbilityTag = FGameplayTag(); // None
	LastSlotInfo.StatusTag = AuraTags.Ability_Status_Unlocked;
	LastSlotInfo.InputTag = PrevInputTag;
	OnAbilityInfo.Broadcast(LastSlotInfo); // 同步上一个槽位的空技能消息，用于清除UI

	FAuraAbilityInfo CurrentSlotInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	CurrentSlotInfo.StatusTag = AuraTags.Ability_Status_Equipped;
	CurrentSlotInfo.InputTag = InputTag;
	OnAbilityInfo.Broadcast(CurrentSlotInfo); // 同步新装备的技能
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
