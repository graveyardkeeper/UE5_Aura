// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 * GameMode在切换地图时会重新构造，如果想跨地图传递信息，使用GameInstance
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	FString LoadSlotName;

	UPROPERTY()
	int32 LoadSlotIndex;
};
