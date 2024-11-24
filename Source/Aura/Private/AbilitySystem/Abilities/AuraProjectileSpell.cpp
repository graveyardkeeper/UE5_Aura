// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Aura/Aura.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		return;
	}
	// spawn a projectile.
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		FRotator Direction = (TargetLocation - SocketLocation).Rotation();
		SpawnTransform.SetRotation(Direction.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// give projectile a damage effect
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			GetAvatarActorFromActorInfo());
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(),
			SourceASC->MakeEffectContext());

		// 向SetByCaller的修饰符添加一个tag和幅度
		const float ScaledDamage = Damage.AsInteger(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FAuraGameplayTags::Get().Damage,
		                                                              ScaledDamage);
#ifdef AURA_DEBUG
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
		                                 FString::Printf(TEXT("FireBolt Damage: %f"), ScaledDamage));
#endif

		Projectile->DamageEffectSpecHandle = SpecHandle;

		Projectile->FinishSpawning(SpawnTransform);
	}
}

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
