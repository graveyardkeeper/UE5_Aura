// There is no fucking Copyright


#include "AbilitySystem/Passive/PassiveSpellNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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
	}
	else if (ICombatInterface* Combat = Cast<ICombatInterface>(GetOwner()))
	{
		Combat->GetOnAscRegisteredDelegate().AddLambda([this](UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				AuraASC->OnPassiveEffectActivatedDelegate.AddUObject(this, &UPassiveSpellNiagaraComponent::OnPassiveActivate);
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
