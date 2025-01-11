// There is no fucking Copyright


#include "UI/WidgetController/AbilityInfoBasedWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"


void UAbilityInfoBasedWidgetController::BindCallbacksToDependencies()
{
	// on ability given, just an event, no details
	// 考虑到绑定委托前，StartupAbilities可能已经被赋予，需要手动检查一下
	if (GetAuraASC()->bStartupAbilitiesGiven)
	{
		BroadcastAbilityInfo();
	}
	else
	{
		GetAuraASC()->OnAbilityGivenDelegate.AddUObject(
			this, &UAbilityInfoBasedWidgetController::BroadcastAbilityInfo);
	}

	// 能力状态发生变化
	GetAuraASC()->OnAbilityStatusChangedDelegate.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
		Info.StatusTag = StatusTag;
		OnAbilityInfo.Broadcast(Info);
	});

	GetAuraPS()->OnPlayerSpellPointsChangedDelegate.AddLambda([this](int32 Value)
	{
		OnPlayerSpellPointsChanged.Broadcast(Value);
	});
}

void UAbilityInfoBasedWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	OnPlayerSpellPointsChanged.Broadcast(GetAuraPS()->GetPlayerSpellPoints());
}

void UAbilityInfoBasedWidgetController::BroadcastAbilityInfo()
{
	if (!GetAuraASC()->bStartupAbilitiesGiven)
	{
		return;
	}
	FForEachAbilityDelegate ForEachAbility;
	ForEachAbility.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		if (const FGameplayTag AbilityTag = UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec); AbilityTag.
			IsValid())
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.InputTag = UAuraAbilitySystemComponent::GetAbilityInputTagFromSpec(AbilitySpec);
			Info.StatusTag = UAuraAbilitySystemComponent::GetAbilityStatusFromSpec(AbilitySpec);
			OnAbilityInfo.Broadcast(Info);
		}
	});
	/** 这里的ForEachAbilityDelegate本质上就是个函数传递，用委托实现，目的是对ASC中的每个Ability都执行同一个操作*/
	GetAuraASC()->ForEachAbility(ForEachAbility);
}
