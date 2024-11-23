// There is no fucking Copyright


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

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
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(
		EffectClass, Level, ContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*EffectSpec.Data.Get(), AbilitySystemComponent);
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	// so, the order of initialization is significant
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

	if (DefaultVitalAttributes != nullptr)
	{
		ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
	}
	else
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("DefaultVitalAttributes not set on character '%s', fill it!"),
		       *GetName()
		)
	}
}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	if (!HasAuthority())
	{
		return;
	}
	UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	ASC->AddCharacterAbilities(StartupAbilities);
}
