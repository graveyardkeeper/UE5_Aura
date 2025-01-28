// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AuraGameplayTags.h"
#include "Aura/Aura.h"
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
