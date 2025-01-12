// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilityInfoBasedWidgetController.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellSelectedDelegate, bool, bEnableSpendPoints, bool, bEnableEquipSpell);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API USpellMenuWidgetController : public UAbilityInfoBasedWidgetController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="GAS|Abilities")
	FOnSpellSelectedDelegate OnSpellSelectedDelegate;

	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	/** 技能图标被选中时触发 */
	UFUNCTION(BlueprintCallable, Category="GAS|Abilities")
	void SpellSelected(const FGameplayTag& AbilityTag);

protected:
	static void ShouldEnableButtons(const FGameplayTag& StatusTag, int32 SpellPoints, bool& bShouldEnableSpendPoints, bool& bShouldEnableEquipSpell);
};
