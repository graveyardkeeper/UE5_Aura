// There is no fucking Copyright


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"


UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,
                                                                const FGameplayTag& CooldownTag)
{
	UWaitCooldownChange* Instance = NewObject<UWaitCooldownChange>();
	Instance->ASC = AbilitySystemComponent;
	Instance->CooldownTag = CooldownTag;

	if (!IsValid(AbilitySystemComponent) || !CooldownTag.IsValid())
	{
		Instance->EndTask();
		return nullptr;
	}

	// 用来获取冷却效果何时结束
	AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(
		Instance, &UWaitCooldownChange::OnCooldownTagChanged);

	// 用来获取冷却效果何时被赋予
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
		Instance, &UWaitCooldownChange::OnActiveEffectAdded);

	return Instance;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(ASC))
	{
		return;
	}
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	/** Mark this! */
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UWaitCooldownChange::OnCooldownTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC,
                                              const FGameplayEffectSpec& SpecApplied,
                                              FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);

	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag)) // Cooldown Tag可以是资产标签或赋予的标签
	{
		// 在目标ASC中查询拥有Cooldown标签的Effect的剩余持续时间
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(
			CooldownTag.GetSingleTagContainer());

		TArray<float> TimeRemaining = ASC->GetActiveEffectsTimeRemaining(Query);
		if (!TimeRemaining.IsEmpty()) // 一般只有一个，如果有多个，取最长的冷却时间
		{
			float HighestTime = TimeRemaining[0];
			for (int32 i = 0; i < TimeRemaining.Num(); i++)
			{
				if (TimeRemaining[i] > HighestTime)
				{
					HighestTime = TimeRemaining[i];
				}
			}
			CooldownStart.Broadcast(HighestTime);
		}
	}
}
