// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& TargetLocation, const FGameplayTag& SocketTag, float PitchOverride = 0.f, AActor* HomingTarget = nullptr);

protected:
	/** 扇出角度 */
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	bool bIsHomingProjectile = true;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float HomingAccelerationMin = 1600.f;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float HomingAccelerationMax = 3200.f;
};
