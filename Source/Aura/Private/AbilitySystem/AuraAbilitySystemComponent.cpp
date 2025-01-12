// There is no fucking Copyright


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Ability_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	OnAbilityGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& PassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : PassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec); // 被动能力在赋予时就激活
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec,
                                                                     FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	if (!TagContainer.IsEmpty())
	{
		OnEffectAssetTags.Broadcast(TagContainer);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbilityDelegate& Delegate)
{
	/** 遍历时Ability可能会被其他异步任务修改，这里加锁 */
	FScopedAbilityListLock Lock(*this);

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability"))))
			{
				return Tag;
			}
		}
		UE_LOG(LogAura, Error, TEXT("Can't get Ability Tag from ability spec [%s]"),
		       *GetNameSafe(AbilitySpec.Ability.Get()));
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	UE_LOG(LogAura, Error, TEXT("Can't get Input Tag from ability spec [%s]"),
	       *GetNameSafe(AbilitySpec.Ability.Get()));
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Status"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock Lock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.Ability->AbilityTags.HasTagExact(AbilityTag))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	// 加点需要RPC到Server
	if (GetAvatarActor()->Implements<UPlayerInterface>() && IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
	{
		ServerUpgradeAttribute(AttributeTag);
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (!GetAvatarActor()->Implements<UPlayerInterface>())
	{
		return;
	}
	if (IPlayerInterface::Execute_GetSpellPoints(GetAvatarActor()) <= 0)
	{
		return;
	}
	if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		// 消耗技能点
		IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);

		const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
		FGameplayTag StatusTag = GetAbilityStatusFromSpec(*Spec);
		if (StatusTag.MatchesTagExact(AuraTags.Ability_Status_Eligible))
		{
			Spec->DynamicAbilityTags.RemoveTag(AuraTags.Ability_Status_Eligible);

			StatusTag = AuraTags.Ability_Status_Unlocked;
			Spec->DynamicAbilityTags.AddTag(StatusTag);
		}
		else if (StatusTag.MatchesTagExact(AuraTags.Ability_Status_Unlocked) || StatusTag.MatchesTagExact(AuraTags.Ability_Status_Equipped))
		{
			++Spec->Level;
		}

		OnAbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, Spec->Level);

		MarkAbilitySpecDirty(*Spec);
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 PlayerLevel)
{
	// 通过等级的变更驱动技能状态的变更。这里会导致技能从Locked状态（没有赋予角色的）转换为Eligible状态（角色可解锁的）。
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (!Info.AbilityTag.IsValid())
		{
			continue;
		}
		if (PlayerLevel < Info.LevelRequirement)
		{
			continue;
		}
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			// 角色等级满足要求但没有拥有该技能，说明该技能时可解锁的Eligible状态，赋予角色该技能，但不指定InputTag，也就是说还无法施放该技能
			FGameplayAbilitySpec Spec = FGameplayAbilitySpec(Info.Ability, 1);
			Spec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Ability_Status_Eligible);
			GiveAbility(Spec);
			MarkAbilitySpecDirty(Spec); //////** 强制立即复制此Ability到客户端 *///////

			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Ability_Status_Eligible, Spec.Level);
		}
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	// 升级属性
	FGameplayEventData Payload;
	Payload.EventMagnitude = 1.f;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	// 消耗1个属性点
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		OnAbilityGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	OnAbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}
