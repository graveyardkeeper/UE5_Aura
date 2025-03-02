// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum class ESaveSlotStatus : uint8
{
	Vacant,
	EnterName,
	Taken,
};

/**
 * Structure for Saving World Objects
 */
USTRUCT(BlueprintType)
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TArray<uint8> Bytes;

	bool operator==(const FSavedActor& Other) const
	{
		return ActorName == Other.ActorName;
	}
};

/**
 * Structure for Saving a Map
 */
USTRUCT(BlueprintType)
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName;

	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

/**
 * Structure for Saving Abilities
 */
USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityInputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityStatus;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel;
};

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ESaveSlotStatus SaveSlotStatus = ESaveSlotStatus::Vacant;

	UPROPERTY()
	FString SlotName;

	UPROPERTY()
	int32 SlotIndex;

	UPROPERTY()
	FText PlayerName;

	UPROPERTY()
	FString MapName;

	UPROPERTY()
	FString MapAssetName;

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	bool bFirstTimeLoadIn = true;

	/** Player */
	UPROPERTY()
	int32 PlayerLevel;

	UPROPERTY()
	int32 PlayerXP;

	UPROPERTY()
	int32 AttributePoints;

	UPROPERTY()
	int32 SpellPoints;

	/** Attributes */
	UPROPERTY()
	float Strength;

	UPROPERTY()
	float Intelligence;

	UPROPERTY()
	float Resilience;

	UPROPERTY()
	float Vigor;

	/** Abilities */
	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	/** Maps */
	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	bool HasMap(const FString& InMapName) const;
	FSavedMap GetSavedMapWithMapName(const FString& InMapName) const;
};
