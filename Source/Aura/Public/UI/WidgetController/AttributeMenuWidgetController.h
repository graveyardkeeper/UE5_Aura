// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, AttrInfo);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;

	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerStatChangedSignature OnPlayerAttributePointsChanged;

	UFUNCTION(BlueprintCallable, Category="GAS|Attributes")
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

	void BroadcastAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute) const;
};
