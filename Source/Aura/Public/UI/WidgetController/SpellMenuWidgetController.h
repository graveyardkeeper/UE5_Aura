// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilityInfoBasedWidgetController.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSpellSelectedDelegate, bool, bEnableSpendPoints, bool, bEnableEquipSpell, FString, Desc, FString, NextLevelDesc);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSpellDelegate, const FGameplayTag&, AbilityType);

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

	UPROPERTY(BlueprintAssignable, Category="GAS|Abilities")
	FWaitForEquipSpellDelegate WaitForEquipSpellDelegate;

	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	/** 技能图标被选中时触发 */
	UFUNCTION(BlueprintCallable, Category="GAS|Abilities")
	void SpellSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, Category="GAS|Abilities")
	void SpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable, Category="GAS|Abilities")
	void OnEquipSpellClicked(const FGameplayTag& AbilityTag);

	/** 把技能AbilityTag装备到输入槽位InputTag上 */
	UFUNCTION(BlueprintCallable, Category="GAS|Abilities")
	void EquipSpell(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

protected:
	static void ShouldEnableButtons(const FGameplayTag& StatusTag, int32 SpellPoints, bool& bShouldEnableSpendPoints, bool& bShouldEnableEquipSpell);
};
