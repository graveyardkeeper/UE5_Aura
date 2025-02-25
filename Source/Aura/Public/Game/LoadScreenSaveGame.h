// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

UENUM(BlueprintType)
enum class ESaveSlotStatus : uint8
{
	Vacant,
	EnterName,
	Taken,
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
	FName PlayerStartTag;
};
