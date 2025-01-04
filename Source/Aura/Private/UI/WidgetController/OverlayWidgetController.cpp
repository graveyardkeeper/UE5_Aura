// There is no fucking Copyright


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Player/AuraPlayerState.h"
#include "UI/Widget/AuraUserWidget.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AS->GetHealth());
	OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	OnManaChanged.Broadcast(AS->GetMana());
	OnMaxManaChanged.Broadcast(AS->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	// player state上的委托，包括经验、等级、技能点等不在AttributeSet中的属性
	AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState);
	PS->OnPlayerXPChangedDelegate.AddLambda([this, PS](int32 NewValue)
	{
		OnPlayerXPPercentChanged.Broadcast(PS->GetPlayerXPPercent());
	});
	PS->OnPlayerLevelChangedDelegate.AddLambda([this](int32 NewValue)
	{
		OnPlayerLevelChanged.Broadcast(NewValue);
	});


	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	// broadcast base attributes.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);

	// broadcast gameplay effect messages.
	AuraASC->OnEffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& TagContainer)
		{
			for (const FGameplayTag& Tag : TagContainer)
			{
				// only broadcast Message.* tags.
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					if (!Row)
					{
						UE_LOG(LogAbilitySystemComponent, Error,
						       TEXT("Tag '%s' not found in MessageWidgetDataTable, check it out."),
						       *Tag.ToString());
					}
					else
					{
						OnMessageWidgetRow.Broadcast(*Row);
					}
				}
			}
		}
	);

	// on ability given, just an event, no details
	// 考虑到绑定委托前，StartupAbilities可能已经被赋予，需要手动检查一下
	if (AuraASC->bStartupAbilitiesGiven)
	{
		OnInitializeStartupAbilities(AuraASC);
	}
	else
	{
		AuraASC->OnAbilityGivenDelegate.AddUObject(this, &UOverlayWidgetController::OnInitializeStartupAbilities);
	}
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC)
{
	if (!AuraASC->bStartupAbilitiesGiven)
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
	AuraASC->ForEachAbility(ForEachAbility); /** 这里的ForEachAbilityDelegate本质上就是个函数传递，用委托实现，目的是对ASC中的每个Ability都执行同一个操作*/
}
