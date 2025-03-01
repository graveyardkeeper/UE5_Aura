// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist,
	Warrior,
	Ranger,
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	// 只有primary attribute每个角色都不同，其余vital attribute和secondary attribute都是基于primary attribute的公式，在各个角色间共享
	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	FScalableFloat XPReward;
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInfo;

	/** 从磁盘加载属性时，使用SetByCaller方式 */
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults|Attributes")
	TSubclassOf<UGameplayEffect> PrimaryAttributes_SetByCaller;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults|Attributes")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults|Attributes")
	TSubclassOf<UGameplayEffect> SecondaryAttributes_Infinite;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults|Attributes")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;

	const FCharacterClassDefaultInfo& GetClassDefaultInfo(ECharacterClass CharacterClass) const;
};
