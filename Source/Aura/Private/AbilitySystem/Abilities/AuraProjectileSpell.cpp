// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::SpawnProjectile(const FVector& TargetLocation, const FGameplayTag& SocketTag,
                                           float PitchOverride)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		return;
	}
	// spawn a projectile.
	if (AActor* OwnerActor = GetAvatarActorFromActorInfo(); OwnerActor->Implements<UCombatInterface>())
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(OwnerActor, SocketTag);
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		FRotator Direction = (TargetLocation - SocketLocation).Rotation();
		if (PitchOverride != 0.f)
		{
			Direction.Pitch = PitchOverride;
		}
		SpawnTransform.SetRotation(Direction.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageParamsFromClassDefaults(); // 此时还不知道TargetActor，所以不设置
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
