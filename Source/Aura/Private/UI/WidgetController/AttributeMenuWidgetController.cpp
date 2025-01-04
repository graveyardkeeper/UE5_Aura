// There is no fucking Copyright


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	for (auto& Pair : AS->TagToAttribute)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState);
	PS->OnPlayerAttributePointsChangedDelegate.AddLambda([this](int32 Value)
	{
		OnPlayerAttributePointsChanged.Broadcast(Value);
	});
	PS->OnPlayerSpellPointsChangedDelegate.AddLambda([this](int32 Value)
	{
		OnPlayerSpellPointsChanged.Broadcast(Value);
	});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	check(AttributeInfo);

	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	for (auto& Pair : AS->TagToAttribute)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	AAuraPlayerState* PS = CastChecked<AAuraPlayerState>(PlayerState);
	OnPlayerAttributePointsChanged.Broadcast(PS->GetPlayerAttributePoints());
	OnPlayerSpellPointsChanged.Broadcast(PS->GetPlayerSpellPoints());
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag,
                                                            const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo AttrInfo = AttributeInfo->FindAttributeInfoForTag(Tag, true);
	AttrInfo.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(AttrInfo);
}
