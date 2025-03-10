// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);

/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;

	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButton;

	void InitializeSlot();

	void SetSlotStatus(ESaveSlotStatus InSlotStatus) { SlotStatus = InSlotStatus; };
	void SetSlotName(const FString& InSlotName);
	void SetPlayerName(const FText& InPlayerName);
	void SetPlayerLevel(int32 InPlayerLevel);
	void SetSlotIndex(int32 InSlotIndex);
	void SetMapName(const FString& InMapName);

	ESaveSlotStatus GetSlotStatus() const { return SlotStatus; };
	FString GetSlotName() const { return SlotName; };
	FText GetPlayerName() const { return PlayerName; };
	int32 GetPlayerLevel() const { return PlayerLevel; };
	int32 GetSlotIndex() const { return SlotIndex; };
	FString GetMapName() const { return MapName; };

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	FString MapAssetName;
private:
	UPROPERTY()
	ESaveSlotStatus SlotStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess="true"))
	FString SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess="true"))
	int32 SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess="true"))
	FText PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess="true"))
	int32 PlayerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta=(AllowPrivateAccess="true"))
	FString MapName;
};
