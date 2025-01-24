// There is no fucking Copyright


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

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
