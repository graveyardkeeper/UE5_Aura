// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

struct FSavedAbility;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEffectAssetTagsDelegate, const FGameplayTagContainer& /*Asset Tags*/);
DECLARE_MULTICAST_DELEGATE(FOnAbilityGivenDelegate);
DECLARE_DELEGATE_OneParam(FForEachAbilityDelegate, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAbilityStatusChangedDelegate, const FGameplayTag& /*Ability Tag*/, const FGameplayTag& /* Status Tag */, int32 /*Ability Level*/);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnSpellEquippedDelegate, const FGameplayTag& /*Ability Tag*/, const FGameplayTag& /* Input Tag */, const FGameplayTag& /*Prev Input Tag*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPassiveAbilityDeactivatedDelegate, const FGameplayTag& /* Passive Ability Tag*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPassiveEffectActivatedDelegate, const FGameplayTag& /* Passive Ability Tag*/, bool /*bActivate*/);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	FOnEffectAssetTagsDelegate OnEffectAssetTags;
	FOnAbilityGivenDelegate OnAbilityGivenDelegate;
	FOnAbilityStatusChangedDelegate OnAbilityStatusChangedDelegate;
	FOnSpellEquippedDelegate OnSpellEquippedDelegate;
	FOnPassiveAbilityDeactivatedDelegate OnPassiveAbilityDeactivatedDelegate;
	FOnPassiveEffectActivatedDelegate OnPassiveEffectActivatedDelegate;

	bool bStartupAbilitiesGiven = false;

	void AbilityActorInfoSet();

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);
	void AddCharacterAbilitiesFromSaveData(const TArray<FSavedAbility>& SavedAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& PassiveAbilities);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbilityDelegate& Delegate);

	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayAbilitySpec* GetSpecFromInputTag(const FGameplayTag& InputTag);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetAbilityStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetAbilityTypeFromSpec(const FGameplayAbilitySpec& AbilitySpec) const;
	void AssignInputTagToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Client, Reliable)
	void ClientSpellPointSpent(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);

	UFUNCTION(Server, Reliable)
	void ServerEquipSpell(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

	UFUNCTION(Client, Reliable)
	void ClientEquipSpell(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag);

	void UpdateAbilityStatuses(int32 PlayerLevel);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);

	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);

	void ClearAbilityInputTag(FGameplayAbilitySpec* AbilitySpec);
	void ClearAbilitiesOfInputTag(const FGameplayTag& InputTag);
	static bool AbilityHasInputTag(const FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag);

protected:
	/** 能力在Server端可激活状态变化时，用来通知Client*/
	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);
};
