// There is no fucking Copyright


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UDebuffNiagaraComponent::OnDebuffTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bool bOwnerValid = IsValid(GetOwner());
	bool bOwnerAlive = GetOwner()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(GetOwner());
	if (NewCount > 0 && bOwnerValid && bOwnerAlive)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	// 此时ASC可能还没完成初始化，分情况讨论
	ICombatInterface* Combat = Cast<ICombatInterface>(GetOwner());
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::OnDebuffTagChanged);
	}
	else if (Combat)
	{
		// Note: 使用WeakLambda，不持有自身的所有权
		Combat->GetOnAscRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* MyASC)
		{
			MyASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UDebuffNiagaraComponent::OnDebuffTagChanged);
		});
	}
	if (Combat)
	{
		Combat->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
