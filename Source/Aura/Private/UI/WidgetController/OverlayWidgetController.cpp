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
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	// player state上的委托，包括经验、等级、技能点等不在AttributeSet中的属性
	GetAuraPS()->OnPlayerXPChangedDelegate.AddLambda([this](int32 NewValue)
	{
		OnPlayerXPPercentChanged.Broadcast(GetAuraPS()->GetPlayerXPPercent());
	});
	GetAuraPS()->OnPlayerLevelChangedDelegate.AddLambda([this](int32 NewValue)
	{
		OnPlayerLevelChanged.Broadcast(NewValue);
	});


	// broadcast base attributes.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);


	// broadcast gameplay effect messages.
	GetAuraASC()->OnEffectAssetTags.AddLambda(
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
}
