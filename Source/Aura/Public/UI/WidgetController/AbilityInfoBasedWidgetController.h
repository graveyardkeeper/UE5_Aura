// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilityInfoBasedWidgetController.generated.h"

class UAuraAbilitySystemComponent;
class UAbilityInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityInfoSignature, const FAuraAbilityInfo&, AbilityInfo);

/**
 * 能力相关的WidgetController的基类，持有一个能力Info的委托，避免重复编码
 * 禁止直接实例化
 */
UCLASS()
class AURA_API UAbilityInfoBasedWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Abilities")
	FOnAbilityInfoSignature OnAbilityInfo;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="WidgetData")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	void OnInitializeStartupAbilities();
};
