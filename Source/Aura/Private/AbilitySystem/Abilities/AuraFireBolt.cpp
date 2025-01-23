// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"

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
	//const int32 NumProjectiles = FMath::Min(GetAbilityLevel(), MaxProjectilesNum);
	const int32 NumProjectiles = MaxProjectilesNum;
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
		Projectile->FinishSpawning(SpawnTransform);
	}
}
