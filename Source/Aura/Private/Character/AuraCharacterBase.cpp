// There is no fucking Copyright


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
	// do nothing.
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level) const
{
	const FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(
		EffectClass, Level, ContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*EffectSpec.Data.Get(), AbilitySystemComponent);
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	if (DefaultPrimaryAttributes != nullptr)
	{
		ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	}
	else
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("DefaultPrimaryAttributes not set on character '%s', fill it!"),
		       *GetName());
	}

	if (DefaultSecondaryAttributes != nullptr)
	{
		ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	}
	else
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("DefaultSecondaryAttributes not set on character '%s', fill it!"),
		       *GetName()
		)
	}
}
