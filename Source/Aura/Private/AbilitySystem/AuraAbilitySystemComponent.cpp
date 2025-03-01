// There is no fucking Copyright


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Game/LoadScreenSaveGame.h"
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

void UAuraAbilitySystemComponent::AddCharacterAbilitiesFromSaveData(const TArray<FSavedAbility>& SavedAbilities)
{
	for (const FSavedAbility& Data : SavedAbilities)
	{
		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(Data.GameplayAbility, Data.AbilityLevel);
		LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilityInputTag);
		LoadedAbilitySpec.DynamicAbilityTags.AddTag(Data.AbilityStatus);

		if (Data.AbilityType == FAuraGameplayTags::Get().Ability_Type_Offensive)
		{
			GiveAbility(LoadedAbilitySpec);
		}
		else if (Data.AbilityType == FAuraGameplayTags::Get().Ability_Type_Passive)
		{
			if (Data.AbilityStatus == FAuraGameplayTags::Get().Ability_Status_Equipped)
			{
				GiveAbilityAndActivateOnce(LoadedAbilitySpec);
			}
			else
			{
				GiveAbility(LoadedAbilitySpec);
			}
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
		AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Ability_Status_Equipped);
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

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	FScopedAbilityListLock Lock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				// 向GA发送按键按下的事件，蓝图中WaitInputPress异步任务会监听
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey()); // 触发向服务器同步按键松开的事件
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	FScopedAbilityListLock Lock(*this);
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
	FScopedAbilityListLock Lock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			// 向GA发送按键松开的事件，蓝图中WaitInputRelease异步任务会监听
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
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

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetAbilityInputTagFromSpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetAbilityStatusFromSpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTypeFromSpec(const FGameplayAbilitySpec& AbilitySpec) const
{
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(AbilitySpec);
	return UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor())->FindAbilityInfoForTag(AbilityTag).AbilityType;
}

void UAuraAbilitySystemComponent::AssignInputTagToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& InputTag)
{
	ClearAbilityInputTag(&AbilitySpec);
	AbilitySpec.DynamicAbilityTags.AddTag(InputTag);
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

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock Lock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasInputTag(&AbilitySpec, InputTag))
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

void UAuraAbilitySystemComponent::ClientSpellPointSpent_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	OnAbilityStatusChangedDelegate.Broadcast(AbilityTag, StatusTag, AbilityLevel);
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

		ClientSpellPointSpent(AbilityTag, StatusTag, Spec->Level);
		MarkAbilitySpecDirty(*Spec);
	}
}

void UAuraAbilitySystemComponent::ServerEquipSpell_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	/** 装备技能，如果目标槽位上已有装备技能，会被顶替 */
	FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag);
	if (Spec == nullptr)
	{
		return;
	}
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	// 1. 确保技能状态正常，仅Unlocked和Equipped状态的技能可以装备
	const FGameplayTag AbilityStatus = GetAbilityStatusFromSpec(*Spec);
	if (!(AbilityStatus.MatchesTagExact(AuraTags.Ability_Status_Equipped) || AbilityStatus.MatchesTagExact(AuraTags.Ability_Status_Unlocked)))
	{
		return;
	}
	// 2. 确保技能类型和槽位类型匹配（主动/被动）
	const FGameplayTag AbilityType = GetAbilityTypeFromSpec(*Spec);
	const FGameplayTag SlotType = InputTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag.Passive"))) ? AuraTags.Ability_Type_Passive : AuraTags.Ability_Type_Offensive;
	if (!AbilityType.MatchesTagExact(SlotType))
	{
		return;
	}
	// 3. 装备技能
	const FGameplayTag PrevSlot = GetAbilityInputTagFromSpec(*Spec);
	if (FGameplayAbilitySpec* SpecWithSlot = GetSpecFromInputTag(InputTag); SpecWithSlot != nullptr)
	{
		// 目标槽位已经装备了其他技能，卸载该技能，如果是被动技能还需要取消激活
		const FGameplayTag AbilityTagWithSlot = GetAbilityTagFromSpec(*SpecWithSlot);
		if (AbilityTag.MatchesTagExact(AbilityTagWithSlot))
		{
			// 相同技能重复装备
			ClientEquipSpell(AbilityTag, InputTag, PrevSlot);
			return;
		}
		if (GetAbilityTypeFromSpec(*SpecWithSlot).MatchesTagExact(AuraTags.Ability_Type_Passive))
		{
			// 是顶替掉的是被动技能，取消激活
			OnPassiveAbilityDeactivatedDelegate.Broadcast(AbilityTagWithSlot);
			MulticastActivatePassiveEffect(AbilityTagWithSlot, false);
		}
		ClearAbilityInputTag(SpecWithSlot);

		// 更新Equipped状态为Unlocked
		SpecWithSlot->DynamicAbilityTags.RemoveTag(AuraTags.Ability_Status_Equipped);
		SpecWithSlot->DynamicAbilityTags.AddTag(AuraTags.Ability_Status_Unlocked);
	}

	if (AbilityStatus == AuraTags.Ability_Status_Unlocked)
	{
		// 待装备技能是未装备状态，装备该技能，如果该技能是被动技能，还需要激活
		if (AbilityType == AuraTags.Ability_Type_Passive)
		{
			TryActivateAbility(Spec->Handle);
			MulticastActivatePassiveEffect(AbilityTag, true);
		}
		// Unlocked状态，更新状态为Equipped
		Spec->DynamicAbilityTags.RemoveTag(AuraTags.Ability_Status_Unlocked);
		Spec->DynamicAbilityTags.AddTag(AuraTags.Ability_Status_Equipped);
	}
	AssignInputTagToAbility(*Spec, InputTag); // 赋予目标技能新的槽位

	MarkAbilitySpecDirty(*Spec);
	ClientEquipSpell(AbilityTag, InputTag, PrevSlot);
}

void UAuraAbilitySystemComponent::ClientEquipSpell_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag)
{
	OnSpellEquippedDelegate.Broadcast(AbilityTag, InputTag, PrevInputTag);
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

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag, bool bActivate)
{
	OnPassiveEffectActivatedDelegate.Broadcast(AbilityTag, bActivate);
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(Spec->Ability))
		{
			OutDescription = AuraAbility->GetDescription(Spec->Level);
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(Spec->Level + 1);
			return true;
		}
	}
	// 未获得的技能，返回锁定文案
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::ClearAbilityInputTag(FGameplayAbilitySpec* AbilitySpec)
{
	const FGameplayTag InputTag = GetAbilityInputTagFromSpec(*AbilitySpec);
	AbilitySpec->DynamicAbilityTags.RemoveTag(InputTag);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock Lock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasInputTag(&Spec, InputTag))
		{
			ClearAbilityInputTag(&Spec);
		}
	}
}

bool UAuraAbilitySystemComponent::AbilityHasInputTag(const FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag)
{
	return AbilitySpec->DynamicAbilityTags.HasTagExact(InputTag);
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
