// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 DamageAmount = Damage.GetValueAtLevel(Level);
	const int32 ProjectileNum = FMath::Min(Level, MaxProjectilesNum);
	return FString::Printf(TEXT(
		"<Title>FIRE BOLT</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Launches </><Amount>%d</><Default> bolt of fire, "
		"exploding on impact and dealing </><Damage>%d</><Default> "
		"fire damage with a chance to burn</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       ProjectileNum,
	                       DamageAmount
	);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 DamageAmount = Damage.GetValueAtLevel(Level);
	const int32 ProjectileNum = FMath::Min(Level, MaxProjectilesNum);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL:</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Launches </><Amount>%d</><Default> bolt of fire, "
		"exploding on impact and dealing </><Damage>%d</><Default> "
		"fire damage with a chance to burn</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       ProjectileNum,
	                       DamageAmount
	);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag, float PitchOverride, AActor* HomingTarget)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		return;
	}
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	if (!OwnerActor->Implements<UCombatInterface>())
	{
		return;
	}
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(OwnerActor, SocketTag);
	FRotator Rotation = (TargetLocation - SocketLocation).Rotation();
	if (PitchOverride != 0.f)
	{
		Rotation.Pitch = PitchOverride;
	}

	const FVector ForwardVector = Rotation.Vector();
	const int32 NumProjectiles = FMath::Min(MaxProjectilesNum, GetAbilityLevel());
	const TArray<FRotator> ProjectileDirections = UAuraAbilitySystemLibrary::EvenlySpacedRotators(ForwardVector, FVector::UpVector, ProjectileSpread, NumProjectiles);
	for (const FRotator& Direction : ProjectileDirections)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Direction.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageParamsFromClassDefaults(); // 此时还不知道TargetActor，所以不设置

		// 追踪目标
		USceneComponent* HomingTargetSceneComp = nullptr;
		if (IsValid(HomingTarget) && HomingTarget->Implements<UCombatInterface>())
		{
			HomingTargetSceneComp = HomingTarget->GetRootComponent();
		}
		else
		{
			// 目标不是敌人，如地面等，生成一个SceneComponent
			// ProjectileMovement中的HomingTargetComponent是个WeakPtr，所以需要有一个类进行维护这个SceneComponent的引用
			HomingTargetSceneComp = NewObject<USceneComponent>(USceneComponent::StaticClass());
			HomingTargetSceneComp->SetWorldLocation(TargetLocation);
		}
		Projectile->HomingTargetSceneComponent = HomingTargetSceneComp;
		Projectile->ProjectileMovement->HomingTargetComponent = HomingTargetSceneComp;
		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bIsHomingProjectile;

		Projectile->FinishSpawning(SpawnTransform);
	}
}
