// There is no fucking Copyright


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> EffectClass)
{
	checkf(EffectClass, TEXT("Effect class not set, fill it."));

	if (TargetActor->ActorHasTag("Enemy") && !bApplyEffectToEnemies)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(EffectClass, EffectLevel, EffectContext);
	FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

	bool bIsInfiniteEffect = EffectSpec.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfiniteEffect && InfiniteEffectRemovalPolicy != EEffectRemovalPolicy::DoNotRemove)
	{
		// store EffectHandle and Target AbilitySystemComponent of it, in order to remove the effect in the future.
		// only infinite effect need to remove.
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	if (!bIsInfiniteEffect && bDestroyOnEffectApplication)
	{
		// 只有instant和duration效果才需要被销毁
		Destroy();
	}
}

void AAuraEffectActor::OnEffectOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag("Enemy") && !bApplyEffectToEnemies)
	{
		return;
	}

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEffectEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag("Enemy") && !bApplyEffectToEnemies)
	{
		return;
	}

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		// remove infinite effect. only infinite effect need to remove.
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC))
		{
			return;
		}

		TArray<FActiveGameplayEffectHandle> EffectToRemove;
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> Pair : ActiveEffectHandles)
		{
			if (Pair.Value == TargetASC)
			{
				EffectToRemove.Add(Pair.Key);
			}
		}
		for (const FActiveGameplayEffectHandle& EffectHandle : EffectToRemove)
		{
			TargetASC->RemoveActiveGameplayEffect(EffectHandle, 1);
			ActiveEffectHandles.FindAndRemoveChecked(EffectHandle);
		}
	}
}
