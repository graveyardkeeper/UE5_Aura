// There is no fucking Copyright

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

class UMVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeLoadSlots();

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FText& SlotName);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);

private:
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;

	// 下面的声明只是为了被GC管理
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;
};
