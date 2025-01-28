// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (GetCurrentActorInfo())
	{
		OwnerPlayerController = GetCurrentActorInfo()->PlayerController.Get();
		OwnerCharacter = OwnerPlayerController->GetCharacter();
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (!OwnerCharacter->Implements<UCombatInterface>())
	{
		return;
	}
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(OwnerCharacter, FAuraGameplayTags::Get().CombatSocket_Weapon);
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerCharacter);

	FHitResult HitResult;
	UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter, SocketLocation, BeamTargetLocation, 10.f, TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::None, HitResult, true);

	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	// 如果首个目标是敌人（非地面），获取距离该敌人最近的N个其他敌人，用来连锁光束
	if (!IsValid(MouseHitActor))
	{
		return;
	}
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetAvatarActorFromActorInfo());
	IgnoreActors.Add(MouseHitActor);
	
	TArray<AActor*> OverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(OwnerCharacter, OverlappingActors, IgnoreActors, 850.f, MouseHitActor->GetActorLocation());

	// const int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	const int32 NumAdditionalTargets = MaxNumShockTargets;
	UAuraAbilitySystemLibrary::GetClosestTargets(OverlappingActors, MouseHitActor->GetActorLocation(), NumAdditionalTargets, OutAdditionalTargets);
}
