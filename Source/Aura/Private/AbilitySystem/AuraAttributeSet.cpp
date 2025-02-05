// There is no fucking Copyright


#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	/** Tag到获取Attribute方法的映射，目前主要用在UI监听属性变化事件 */
	TagToAttribute.Add(GameplayTags.Attribute_Primary_Strength, GetStrengthAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Primary_Resilience, GetResilienceAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Primary_Vigor, GetVigorAttribute);

	TagToAttribute.Add(GameplayTags.Attribute_Secondary_Armor, GetArmorAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Secondary_BlockChance, GetBlockChanceAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Secondary_MaxMana, GetMaxManaAttribute);

	TagToAttribute.Add(GameplayTags.Attribute_Resistance_Fire, GetFireResistanceAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Resistance_Lightning, GetLightningResistanceAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagToAttribute.Add(GameplayTags.Attribute_Resistance_Physical, GetPhysicalResistanceAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	/** Clamp our attributes. */
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties EffectProperties;
	SetEffectProperties(Data, EffectProperties);

	if (EffectProperties.TargetAvatarActor->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(EffectProperties.TargetAvatarActor))
	{
		return;
	}

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(EffectProperties);
	}
	if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		HandleIncomingXP(EffectProperties);
	}
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& Props)
{
	if (Props.TargetAvatarActor->Implements<UCombatInterface>())
	{
		const int32 XPReward = UAuraAbilitySystemLibrary::GetXPRewardForCharacterAndLevel(
			Props.TargetAvatarActor,
			ICombatInterface::Execute_GetCharacterClass(Props.TargetAvatarActor),
			ICombatInterface::Execute_GetCharacterLevel(Props.TargetAvatarActor));

		FGameplayEventData Payload;
		Payload.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceAvatarActor,
		                                                         FAuraGameplayTags::Get().Attribute_Meta_IncomingXP,
		                                                         Payload);
	}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f); // meta attribute需要被消耗，清空

	if (LocalIncomingDamage <= 0.f)
	{
		return;
	}

	// 造成了伤害
	const float NewHealth = GetHealth() - LocalIncomingDamage;
	SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

	// 是否致命伤害
	const bool bFatal = NewHealth <= 0.f;
	if (!bFatal)
	{
		// 非致命伤，尝试激活拥有受击Tag的能力
		if (Props.TargetAvatarActor->Implements<UCombatInterface>() && !ICombatInterface::Execute_GetIsBeingShocked(Props.TargetAvatarActor))
		{
			FGameplayTagContainer TagContainer;
			TagContainer.AddTag(FAuraGameplayTags::Get().Effect_HitReact);
			Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
		}

		if (const FVector KnockbackForce = UAuraAbilitySystemLibrary::GetKnockbackForce(Props.EffectContextHandle); !KnockbackForce.IsZero())
		{
			// 击退效果
			// TODO 目前有Bug，只会往Z轴移动，XY平面不会移动，怀疑是地面摩擦力原因，但不解决
			// DrawDebugLine(GetWorld(), Props.TargetCharacter->GetActorLocation(), Props.TargetCharacter->GetActorLocation() + KnockbackForce, FColor::Green, true);
			Props.TargetCharacter->LaunchCharacter(KnockbackForce, true, true);
		}
	}
	else
	{
		// 致命伤，call Die()
		if (ICombatInterface* Combat = Cast<ICombatInterface>(Props.TargetAvatarActor))
		{
			Combat->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
		}
		// 发送获得经验事件
		// ！！！这里很重要，不仅关系到经验的获得，还关系到升级后最大生命和最大魔法的更新
		// 如果只是单纯等级变更，不会触发最大生命的重新计算（MMC里面只捕获了PrimaryAttribute，而Level不在AttributeSet中），
		// 关键就在这里，发送经验变更事件后，监听端会同时应用一个SetByCaller的Effect（GE_EventBasedEffect），该Effect中配置了
		// 会影响最大生命的PrimaryAttribute的更新（虽然Add了一个0，但足以触发MMC的重新计算）
		// 也就是说，如果MMC没有捕获任何其他属性，那么永远不会重新计算
		SendXPEvent(Props);
	}

	// 显示伤害数字
	ShowFloatingText(Props, LocalIncomingDamage, UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle), UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle));

	// 处理Debuff
	HandleDebuff(Props);
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float LocalIncomingXP = GetIncomingXP();
	SetIncomingXP(0.f);

	if (Props.SourceAvatarActor->Implements<UPlayerInterface>())
	{
		const int32 DeltaLevel = IPlayerInterface::Execute_AddToXP(Props.SourceAvatarActor, LocalIncomingXP);
		if (DeltaLevel > 0)
		{
			// 升级，补满生命和魔法，不在此处直接设置，因为最大生命在次函数结束后才会更新
			// 在PostAttributeChange中进行
			bTopOffHealth = true;
			bTopOffMana = true;

			IPlayerInterface::Execute_LevelUp(Props.SourceAvatarActor);
		}
	}
}

void UAuraAttributeSet::HandleDebuff(const FEffectProperties& Props)
{
	if (!UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Props.EffectContextHandle))
	{
		return;
	}
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	// 创建一个动态的Effect，并应用
	FGameplayEffectContextHandle EffectContext = Props.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Props.SourceAvatarActor);

	const FGameplayTag DamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);

	const FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));

	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
	Effect->Period = DebuffFrequency;

	const FGameplayTag DebuffTag = AuraTags.DamageTypes2Debuffs[DamageType];

	// AddGrantTags 新写法 相当于：Effect->InheritableOwnedTagsContainer.AddTag(AuraTags.DamageTypes2Debuffs[DamageType]);
	UTargetTagsGameplayEffectComponent& TargetTagsComponent = Effect->AddComponent<UTargetTagsGameplayEffectComponent>();
	FInheritedTagContainer TagContainer = TargetTagsComponent.GetConfiguredTargetTagChanges();
	TagContainer.AddTag(DebuffTag); // 赋予Debuff标签到目标身上，如Debuff.Fire
	// 同时，如果是眩晕Debuff，阻止角色输入
	if (DebuffTag.MatchesTagExact(AuraTags.Debuff_Stun))
	{
		TagContainer.AddTag(AuraTags.Player_Block_CursorTrace);
		TagContainer.AddTag(AuraTags.Player_Block_InputHeld);
		TagContainer.AddTag(AuraTags.Player_Block_InputPressed);
		TagContainer.AddTag(AuraTags.Player_Block_InputReleased);
	}
	TargetTagsComponent.SetAndApplyTargetTagChanges(TagContainer);

	Effect->StackingType = EGameplayEffectStackingType::AggregateBySource;
	Effect->StackLimitCount = 1;

	FGameplayModifierInfo Modifier;
	Modifier.Attribute = GetIncomingDamageAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FScalableFloat(DebuffDamage);
	Effect->Modifiers.Add(Modifier);

	const FGameplayEffectSpec Spec = FGameplayEffectSpec(Effect, EffectContext, 1.f);
	FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(Spec.GetContext().Get());
	AuraContext->SetDamageType(DamageType);

	Props.TargetASC->ApplyGameplayEffectSpecToSelf(Spec);
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bIsBlockedHit,
                                         bool bIsCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		// ShowDamageNumber是一个client method，必须使用对应端的controller才能正确展示在对应客户端
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.SourceController))
		{
			// SourceController为Player，如玩家对敌人造成伤害
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bIsBlockedHit, bIsCriticalHit);
			return;
		}
		if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(Props.TargetController))
		{
			// TargetController为Player，如敌人对玩家造成伤害
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bIsBlockedHit, bIsCriticalHit);
		}
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
                                            FEffectProperties& Properties) const
{
	Properties.EffectContextHandle = Data.EffectSpec.GetContext();

	// source
	Properties.SourceASC = Properties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(Properties.SourceASC) && Properties.SourceASC->AbilityActorInfo.IsValid() &&
		Properties.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Properties.SourceAvatarActor = Properties.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Properties.SourceController = Properties.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Properties.SourceController == nullptr && Properties.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Properties.SourceAvatarActor))
			{
				Properties.SourceController = Pawn->GetController();
			}
		}
		if (Properties.SourceController != nullptr)
		{
			Properties.SourceCharacter = Cast<ACharacter>(Properties.SourceController->GetPawn());
		}
	}

	// target
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Properties.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Properties.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Properties.TargetCharacter = Cast<ACharacter>(Properties.TargetAvatarActor);
		Properties.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Properties.TargetAvatarActor);
	}
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldValue);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldValue);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldValue);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldValue);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldValue);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldValue);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldValue);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldValue);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldValue);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldValue);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldValue);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldValue);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldValue);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldValue);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldValue);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldValue);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldValue);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldValue);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldValue);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldValue);
}
