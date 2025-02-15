// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const int32 DamageAmount = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>FIRE BLAST</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Launches </><Amount>%d</><Default> fire balls in all directions, "
		"each coming back and exploding upon return, causing </><Damage>%d</><Default> "
		"fire damage with a chance to burn.</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       NumFireBalls,
	                       DamageAmount
	);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 DamageAmount = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL:</>\n\n"

		"<Small>Level: </><Level>%d</>\n"
		"<Small>ManaCost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		"<Default>Launches </><Amount>%d</><Default> fire balls in all directions, "
		"each coming back and exploding upon return, causing </><Damage>%d</><Default> "
		"fire damage with a chance to burn.</>"),
	                       Level,
	                       GetManaCost(Level),
	                       GetCooldown(Level),
	                       NumFireBalls,
	                       DamageAmount
	);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	TArray<FVector> SpawnDirections = UAuraAbilitySystemLibrary::EvenlyRotatedVectors(Forward, FVector::UpVector, 360.f, NumFireBalls);

	TArray<AAuraFireBall*> FireBallsSpawned;
	for (const FVector& Direction : SpawnDirections)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetAvatarActorFromActorInfo()->GetActorLocation());
		SpawnTransform.SetRotation(Direction.Rotation().Quaternion());
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,
			SpawnTransform,
			GetAvatarActorFromActorInfo(),
			GetCurrentActorInfo()->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		FireBall->DamageEffectParams = MakeDamageParamsFromClassDefaults();

		FireBall->FinishSpawning(SpawnTransform);
		FireBallsSpawned.Add(FireBall);
	}
	return FireBallsSpawned;
}
