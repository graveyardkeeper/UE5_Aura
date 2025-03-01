// There is no fucking Copyright


#include "AbilitySystem/Passive/PassiveSpellNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveSpellNiagaraComponent::UPassiveSpellNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveSpellNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->OnPassiveEffectActivatedDelegate.AddUObject(this, &UPassiveSpellNiagaraComponent::OnPassiveActivate);
		ActiveIfAlreadyEquipped(AuraASC);
	}
	else if (ICombatInterface* Combat = Cast<ICombatInterface>(GetOwner()))
	{
		Combat->GetOnAscRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->OnPassiveEffectActivatedDelegate.AddUObject(this, &UPassiveSpellNiagaraComponent::OnPassiveActivate);
				ActiveIfAlreadyEquipped(AuraASC);
			}
		});
	}
}

void UPassiveSpellNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag != PassiveSpellTag)
	{
		return;
	}
	if (bActivate)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UPassiveSpellNiagaraComponent::ActiveIfAlreadyEquipped(UAuraAbilitySystemComponent* AuraASC)
{
	if (AuraASC->bStartupAbilitiesGiven)
	{
		// 如果初始技能已经赋予过了，表示已经错过了监听(一般是Loading场景)，需要手动check下技能状态，手动触发粒子效果
		if (AuraASC->GetAbilityStatusFromAbilityTag(PassiveSpellTag) == FAuraGameplayTags::Get().Ability_Status_Equipped)
		{
			Activate();
		}
	}
}
