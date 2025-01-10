// There is no fucking Copyright


#include "UI/WidgetController/AbilityInfoBasedWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"


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
}

void UAbilityInfoBasedWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
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
			OnAbilityInfo.Broadcast(Info);
		}
	});
	/** 这里的ForEachAbilityDelegate本质上就是个函数传递，用委托实现，目的是对ASC中的每个Ability都执行同一个操作*/
	GetAuraASC()->ForEachAbility(ForEachAbility);
}
