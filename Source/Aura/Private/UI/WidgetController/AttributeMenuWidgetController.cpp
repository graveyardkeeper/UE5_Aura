// There is no fucking Copyright


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	for (auto& Pair : GetAuraAS()->TagToAttribute)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	GetAuraPS()->OnPlayerAttributePointsChangedDelegate.AddLambda([this](int32 Value)
	{
		OnPlayerAttributePointsChanged.Broadcast(Value);
	});
	GetAuraPS()->OnPlayerSpellPointsChangedDelegate.AddLambda([this](int32 Value)
	{
		OnPlayerSpellPointsChanged.Broadcast(Value);
	});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	for (auto& Pair : GetAuraAS()->TagToAttribute)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	OnPlayerAttributePointsChanged.Broadcast(GetAuraPS()->GetPlayerAttributePoints());
	OnPlayerSpellPointsChanged.Broadcast(GetAuraPS()->GetPlayerSpellPoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraASC()->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag,
                                                            const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo AttrInfo = AttributeInfo->FindAttributeInfoForTag(Tag, true);
	AttrInfo.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(AttrInfo);
}
